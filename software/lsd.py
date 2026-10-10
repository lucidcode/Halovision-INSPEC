import utime
import time
import os
from version import version

class lucid_scribe_data:
    def __init__(self, config):
        self.config = config
        self.session_directory = None

        self.recording = False
        self.frames_failed = False
        self.config_changed = False
        self.frame_index = 0
        self.saved_index = -1
        self.frame_row = None
        self.frame_rows = []
        self.frame_directory = None

        if self.config.get('CreateLogs'):
            self.create_lsd()

    def create_lsd(self):
        self.lsd_hour = 0
        self.lsd_minute = 0
        self.lsd_start = utime.ticks_ms()
        self.lsd_minute_start = self.lsd_start
        self.session_start = self.lsd_start

        root_dir_exists = False
        entries = os.listdir()
        for entry in entries:
            if entry == "sessions":
                root_dir_exists = True

        if not root_dir_exists:
            print("creating sessions")
            os.mkdir("sessions")

        vision_index = 0
        entries = os.listdir("sessions")
        for entry in entries:
            if ("session_" in entry):
                directory_index = entry.replace("session_", "")
                if int(directory_index) >= vision_index:
                    vision_index = int(directory_index) + 1

        self.session_directory = "sessions/" + "session_" + str(vision_index)
        print("creating session " + str(vision_index))
        os.mkdir(self.session_directory)

        self.write_config(self.session_directory + '/config.txt')

        self.session_file = self.session_directory + "/session_" + str(vision_index) + ".LSD"
        self.lsd_file = open(self.session_file, 'w')
        self.lsd_file.write("INSPEC")
        self.lsd_file.write("\r\n" + "Researcher:" + self.config.get('Researcher'))
        self.lsd_file.close()
        self.lsd_values = "0"
        self.rem_values = "0"
        self.sqi_values = "0"
        self.eye_values = "0"
        self.face_values = "0"
        self.lag_values = "0"

        self.sample_count = 1

    def write_config(self, file_name):
        config_file = open(file_name, 'w')
        config_file.write(self.config.format())
        config_file.close()

    def log(self, variance, rem, sqi, regional=0, face=0, lag=0):
        if self.config.get('CreateLogs') != 1:
            return

        if self.session_directory is None:
            self.create_lsd()

        now = utime.ticks_ms()

        if (now - self.lsd_start > 1000 * 60 * 60):
            self.write_log()
            self.lsd_start = now
            self.lsd_minute_start = now
            self.lsd_hour = self.lsd_hour + 1
            self.lsd_minute = 0
            self.lsd_values = str(variance)
            self.rem_values = str(rem)
            self.sqi_values = str(sqi)
            self.eye_values = str(regional)
            self.face_values = str(face)
            self.lag_values = str(lag)
            self.sample_count = 1
            return

        if (now - self.lsd_minute_start >= 1000 * 60):
            self.write_log()
            self.lsd_minute_start = now
            self.lsd_minute = self.lsd_minute + 1
            self.lsd_values = str(variance)
            self.rem_values = str(rem)
            self.sqi_values = str(sqi)
            self.eye_values = str(regional)
            self.face_values = str(face)
            self.lag_values = str(lag)
            self.sample_count = 1
            return

        self.lsd_values = f'{self.lsd_values},{str(variance)}'
        self.rem_values = f'{self.rem_values},{str(rem)}'
        self.sqi_values = f'{self.sqi_values},{str(sqi)}'
        self.eye_values = f'{self.eye_values},{str(regional)}'
        self.face_values = f'{self.face_values},{str(face)}'
        self.lag_values = f'{self.lag_values},{str(lag)}'
        self.sample_count = self.sample_count + 1

    def write_log(self):
        self.lsd_file = open(self.session_file, 'a')
        formatted_time = self.format_time()
        self.lsd_file.write(f'\r\n{formatted_time}:lsd - {self.lsd_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:rem - {self.rem_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:sqi - {self.sqi_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:eye - {self.eye_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:face - {self.face_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:lag - {self.lag_values}')
        self.lsd_file.close()

    def add_image(self, image, eye_movements):
        if self.config.get('CreateLogs') != 1:
            return

        if self.session_directory is None:
            self.create_lsd()

        now = utime.ticks_ms()
        second = time.localtime()[5]
        formatted_time = f'{self.format_time()}:{second}:{eye_movements}'
        formatted_time = formatted_time.replace(":", "-")
        file_name = self.session_directory + "/image_" + formatted_time + ".JPG"
        image.save(file_name)

    def start_frames(self):
        if self.session_directory is None:
            self.create_lsd()

        self.frames_directory = self.session_directory + "/frames"
        self.make_directory(self.frames_directory)
        self.frames_file = self.frames_directory + "/frames.csv"

        self.frame_extension = "pgm" if self.config.get('PixelFormat') == 'Grayscale' else "bmp"

        if not self.file_exists(self.frames_file):
            frames_file = open(self.frames_file, 'w')
            frames_file.write(f'# INSPEC {version}\r\n')
            frames_file.write('frame,ms,global,regional,face,detector,x,y,w,h,angle,ipd,right_x,right_y,left_x,left_y,'
                              'r1x,r1y,r1w,r1h,r2x,r2y,r2w,r2h,lag,lag_ms,bx,by,s1x,s1y,s1w,s1h,s2x,s2y,s2w,s2h,'
                              'rem,minute,sample\r\n')
            frames_file.close()

        self.write_config(self.frames_directory + '/config.txt')
        self.config_changed = False

        self.recording = True
        self.saved_index = -1
        self.frame_row = None
        self.last_flush = utime.ticks_ms()

    def record_frame(self, previous, current, global_variance, regional_variance, face, regions, eyes=None, lag_variance=None):
        if not self.recording:
            self.start_frames()

        self.frame_index = self.frame_index + 1
        self.frame_row = None
        lag = 0 if lag_variance is None else lag_variance

        if self.config.get('RecordAllFrames') != 2 and global_variance <= 0 and regional_variance <= 0 and lag <= 0:
            return

        if self.saved_index != self.frame_index - 1:
            self.save_frame(previous, self.frame_index - 1)
        self.save_frame(current, self.frame_index)
        self.saved_index = self.frame_index

        lag_ms, bx, by, squares = 0, 0, 0, [(0, 0, 0, 0), (0, 0, 0, 0)]
        if eyes is not None and eyes.lagged is not None:
            self.save_frame(eyes.lagged, f'{self.frame_index}L', self.frame_index)
            lag_ms = eyes.lag_ms
            bx, by = eyes.box[0], eyes.box[1]
            squares = eyes.squares

        ms = utime.ticks_diff(utime.ticks_ms(), self.session_start)
        has_face = 1 if face.has_face else 0
        x, y, w, h = face.face_object
        (right_x, right_y), (left_x, left_y) = face.eye_centers
        angle = round(face.face_angle, 1)

        r1 = regions[0] if len(regions) > 0 else (0, 0, 0, 0)
        r2 = regions[1] if len(regions) > 1 else (0, 0, 0, 0)
        counted = f'{r1[0]},{r1[1]},{r1[2]},{r1[3]},{r2[0]},{r2[1]},{r2[2]},{r2[3]}'

        s1 = squares[0]
        s2 = squares[1] if len(squares) > 1 else (0, 0, 0, 0)
        lagged = f'{lag},{lag_ms},{bx},{by},{s1[0]},{s1[1]},{s1[2]},{s1[3]},{s2[0]},{s2[1]},{s2[2]},{s2[3]}'

        self.frame_row = f'{self.frame_index},{ms},{global_variance},{regional_variance},{has_face},{face.detector},{x},{y},{w},{h},{angle},{face.ipd},{right_x},{right_y},{left_x},{left_y},{counted},{lagged}'

    def end_frame(self, eye_movements):
        if self.frame_row is None:
            return

        self.frame_rows.append(f'{self.frame_row},{eye_movements},{self.format_time()},{self.sample_count - 1}')
        self.frame_row = None

        if len(self.frame_rows) >= 50 or utime.ticks_diff(utime.ticks_ms(), self.last_flush) > 1000 * 60:
            self.flush_frames()

    def stop_frames(self):
        self.recording = False
        self.saved_index = -1

        self.frame_index = self.frame_index + 1
        self.frame_row = None
        self.flush_frames()

    def flush_frames(self):
        self.last_flush = utime.ticks_ms()

        if self.config_changed:
            self.config_changed = False
            self.write_config(self.frames_directory + '/config.txt')

        if not self.frame_rows:
            return

        rows = self.frame_rows
        self.frame_rows = []
        frames_file = open(self.frames_file, 'a')
        frames_file.write('\r\n'.join(rows) + '\r\n')
        frames_file.close()

    def save_frame(self, image, name, index=None):
        if index is None:
            index = name
        directory = f'{self.frames_directory}/{index // 1000}'
        if directory != self.frame_directory:
            self.make_directory(directory)
            self.frame_directory = directory

        image.save(f'{directory}/{name}.{self.frame_extension}')

    def make_directory(self, directory):
        try:
            os.mkdir(directory)
        except OSError:
            pass

    def file_exists(self, file_name):
        try:
            os.stat(file_name)
            return True
        except OSError:
            return False

    def list_directories(self):
        entries = os.listdir("sessions")
        directories = ""
        for entry in entries:
            if ("session_" in entry):
                if directories == "":
                    directories = entry.replace("session_", "")
                else:
                    directories = f'{directories},{entry.replace("session_", "")}'

        return directories

    def get_config(self, vision):
        file_name = "sessions/session_" + vision + "/config.txt"

        with open(file_name, mode='rb') as file:
            content = bytearray(file.read())
            return content

    def format_time(self):
        hour_string = str(self.lsd_hour)
        if self.lsd_hour < 10:
            hour_string = "0" + hour_string

        minute_string = str(self.lsd_minute)
        if self.lsd_minute < 10:
            minute_string = "0" + minute_string

        return hour_string + ":" + minute_string
