import gc
import image
import utime
from ulab import numpy as np

class eye_history:

    def __init__(self, config, pixformat):
        self.config = config
        self.pixformat = pixformat
        self.size = 12
        self.buffers = []
        self.times = []
        self.allocated = (0, 0)
        self.box = None
        self.squares = None
        self.index = 0
        self.lagged = None
        self.lag_ms = 0
        self.shift_fit = -1.0
        self.shifted = False

    def set_pixformat(self, pixformat):
        self.pixformat = pixformat
        self.allocated = (0, 0)
        self.box = None

    def compare(self, img, regions, has_face):
        self.lagged = None
        self.lag_ms = 0
        self.shift_fit = -1.0
        self.shifted = False

        lag = self.config.get('EyeLag')
        if not lag or not has_face or not regions:
            self.box = None
            return None

        box = self.bounding_box(regions)
        if self.moved(box) or len(regions) != len(self.squares):
            self.reset(box, regions)

        now = utime.ticks_ms()
        current = self.buffers[self.index]
        current.draw_image(img, 0, 0, roi=self.box)
        self.times[self.index] = now

        oldest = None
        for i in range(self.size):
            if i == self.index or self.times[i] is None:
                continue
            age = utime.ticks_diff(now, self.times[i])
            if age >= lag and (oldest is None or age < oldest[1]):
                oldest = (i, age)

        self.index = (self.index + 1) % self.size
        if oldest is None:
            return None

        self.lagged = self.buffers[oldest[0]]
        self.lag_ms = oldest[1]
        global_variance, variance = current.variation(self.lagged, self.config.get('PixelThreshold'), self.config.get('PixelRange'), *self.squares)

        # A head movement shifts the whole crop, while an eye movement changes the lids locally.
        shift_filter = self.config.get('EyeShiftFilter')
        if shift_filter and variance >= self.config.get('TriggerThreshold'):
            try:
                self.shift_fit = self.explained_by_shift(current, self.lagged)
            except Exception as e:
                print("explained_by_shift", e)
                self.shift_fit = 0.0
            self.shifted = self.shift_fit > shift_filter

        return variance

    def explained_by_shift(self, current, lagged):
        """Fits one translation between the crops and returns the share of the change it explains."""
        w = self.box[2]
        h = self.box[3]
        if w < 8 or h < 8 or current.format() != image.GRAYSCALE:
            return 0.0

        a = self.smooth(current.to_ndarray('f')[:h, :w])
        b = self.smooth(lagged.to_ndarray('f')[:h, :w])

        mean = (a + b) / 2
        gx = (mean[1:-1, 2:] - mean[1:-1, :-2]) / 2
        gy = (mean[2:, 1:-1] - mean[:-2, 1:-1]) / 2
        change = a[1:-1, 1:-1] - b[1:-1, 1:-1]

        sxx = np.sum(gx * gx)
        syy = np.sum(gy * gy)
        sxy = np.sum(gx * gy)
        sxt = np.sum(gx * change)
        syt = np.sum(gy * change)
        stt = np.sum(change * change)

        det = sxx * syy - sxy * sxy
        if det <= 0 or stt <= 0:
            return 0.0

        # Solve the least squares shift
        dx = (sxy * syt - syy * sxt) / det
        dy = (sxy * sxt - sxx * syt) / det
        return -(dx * sxt + dy * syt) / stt

    def smooth(self, a):
        rows = a[:-2, :] + a[1:-1, :] + a[2:, :]
        return (rows[:, :-2] + rows[:, 1:-1] + rows[:, 2:]) / 9

    def bounding_box(self, regions):
        x0 = min(r[0] for r in regions)
        y0 = min(r[1] for r in regions)
        x1 = max(r[0] + r[2] for r in regions)
        y1 = max(r[1] + r[3] for r in regions)
        return (x0, y0, x1 - x0, y1 - y0)

    def moved(self, box):
        if self.box is None:
            return True
        tolerance = max(2, self.box[2] // 10)
        return any(abs(box[i] - self.box[i]) > tolerance for i in range(4))

    def reset(self, box, regions):
        self.box = box
        self.squares = [(r[0] - box[0], r[1] - box[1], r[2], r[3]) for r in regions]

        w, h = self.allocated
        if box[2] > w or box[3] > h:
            w = box[2] + 16
            h = box[3] + 16
            self.buffers = []
            gc.collect()
            self.buffers = [image.Image(w, h, self.pixformat) for i in range(self.size)]
            self.allocated = (w, h)
        for buffer in self.buffers:
            buffer.clear()
        self.times = [None] * self.size
        self.index = 0
