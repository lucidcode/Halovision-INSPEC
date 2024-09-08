:mod:`image` --- machine vision
===============================

.. module:: image
   :synopsis: machine vision

The ``image`` module is used for machine vision.

Functions
---------

.. function:: binary_to_grayscale(binary_image_value:Union[0,1]) -> int

   Returns a converted binary value (0-1) to a grayscale value (0-255).

.. function:: binary_to_rgb(binary_image_value:Union[0,1]) -> Tuple[int, int, int]

   Returns a converted binary value (0-1) to a 3 value RGB888 tuple.

.. function:: binary_to_lab(binary_image_value:Union[0,1]) -> Tuple[int, int, int]

   Returns a converted binary value (0-1) to a 3 value LAB tuple.

   L goes between 0 and 100 and A/B go from -128 to 128.

.. function:: binary_to_yuv(binary_image_value:Union[0,1]) -> Tuple[int, int, int]

   Returns a converted binary value (0-1) to a 3 value YUV tuple.

   Y goes between 0 and 255 and U/V go from -128 to 128.

.. function:: grayscale_to_binary(grayscale_value:int) -> Union[0,1]

   Returns a converted grayscale value (0-255) to a binary value (0-1).

.. function:: grayscale_to_rgb(grayscale_value:int) -> Tuple[int, int, int]

   Returns a converted grayscale value to a 3 value RGB888 tuple.

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB565->RGB888 process
      so this method won't return the exact values as a pure RGB888 system would.
      However, it's true to how the image lib works internally.

.. function:: grayscale_to_lab(grayscale_value:int) -> Tuple[int, int, int]

   Returns a converted grayscale value to a 3 value LAB tuple.

   L goes between 0 and 100 and A/B go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB565->LAB process
      so this method won't return the exact values as a pure LAB system would.
      However, it's true to how the image lib works internally.

.. function:: grayscale_to_yuv(grayscale_value:int) -> Tuple[int, int, int]

   Returns a converted grayscale value to a 3 value YUV tuple.

   Y goes between 0 and 255 and U/V go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB565->YUV process
      so this method won't return the exact values as a pure YUV system would.
      However, it's true to how the image lib works internally.

.. function:: rgb_to_binary(rgb_tuple:Tuple[int, int, int]) -> Union[0,1]

   Returns a converted 3 value RGB888 tuple to a center range thresholded binary value (0-1).

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB888->RGB565 process
      so this method won't return the exact values as a pure RGB888 system would.
      However, it's true to how the image lib works internally.

.. function:: rgb_to_grayscale(rgb_tuple:Tuple[int, int, int]) -> int

   Returns a converted 3 value RGB888 tuple to a grayscale value (0-255).

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB888->RGB565 process
      so this method won't return the exact values as a pure RGB888 system would.
      However, it's true to how the image lib works internally.

.. function:: rgb_to_lab(rgb_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value RGB888 tuple to a 3 value LAB tuple.

   L goes between 0 and 100 and A/B go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB888->RGB565 process
      so this method won't return the exact values as a pure RGB888 system would.
      However, it's true to how the image lib works internally.

.. function:: rgb_to_yuv(rgb_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value RGB888 tuple to a 3 value YUV tuple.

   Y goes between 0 and 255 and U/V go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a RGB888->RGB565 process
      so this method won't return the exact values as a pure RGB888 system would.
      However, it's true to how the image lib works internally.

.. function:: lab_to_binary(lab_tuple:Tuple[int, int, int]) -> Union[0,1]

   Returns a converted 3 value LAB tuple to a center range thresholded binary value (0-1).

   .. note::

      The OpenMV Cam firmware does the conversion using a LAB->RGB565 process
      so this method won't return the exact values as a pure LAB system would.
      However, it's true to how the image lib works internally.

.. function:: lab_to_grayscale(lab_tuple:Tuple[int, int, int]) -> int

   Returns a converted 3 value LAB tuple to a grayscale value (0-255).

   .. note::

      The OpenMV Cam firmware does the conversion using a LAB->RGB565 process
      so this method won't return the exact values as a pure LAB system would.
      However, it's true to how the image lib works internally.

.. function:: lab_to_rgb(lab_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value LAB tuple to a 3 value RGB888 tuple.

   .. note::

      The OpenMV Cam firmware does the conversion using a LAB->RGB565 process
      so this method won't return the exact values as a pure LAB system would.
      However, it's true to how the image lib works internally.

.. function:: lab_to_yuv(lab_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value LAB tuple to a 3 value YUV tuple.

   Y goes between 0 and 255 and U/V go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a LAB->RGB565 process
      so this method won't return the exact values as a pure LAB system would.
      However, it's true to how the image lib works internally.

.. function:: yuv_to_binary(yuv_tuple:Tuple[int, int, int]) -> Union[0,1]

   Returns a converted 3 value YUV tuple to a center range thresholded binary value (0-1).

   .. note::

      The OpenMV Cam firmware does the conversion using a YUV->RGB565 process
      so this method won't return the exact values as a pure YUV system would.
      However, it's true to how the image lib works internally.

.. function:: yuv_to_grayscale(yuv_tuple:Tuple[int, int, int]) -> int

   Returns a converted 3 value YUV tuple to a grayscale value (0-255).

   .. note::

      The OpenMV Cam firmware does the conversion using a YUV->RGB565 process
      so this method won't return the exact values as a pure YUV system would.
      However, it's true to how the image lib works internally.

.. function:: yuv_to_rgb(lab_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value YUV tuple to a 3 value RGB888 tuple.

   .. note::

      The OpenMV Cam firmware does the conversion using a YUV->RGB565 process
      so this method won't return the exact values as a pure YUV system would.
      However, it's true to how the image lib works internally.

.. function:: yuv_to_lab(yuv_tuple:Tuple[int, int, int]) -> Tuple[int, int, int]

   Returns a converted 3 value YUV tuple to a 3 value LAB tuple.

   L goes between 0 and 100 and A/B go from -128 to 128.

   .. note::

      The OpenMV Cam firmware does the conversion using a YUV->RGB565 process
      so this method won't return the exact values as a pure YUV system would.
      However, it's true to how the image lib works internally.

.. function:: load_decriptor(path:str)

   Loads a descriptor object from disk.

   ``path`` is the path to the descriptor file to load.

.. function:: save_descriptor(path:str, descriptor)

   Saves the descriptor object ``descriptor`` to disk.

   ``path`` is the path to the descriptor file to save.

.. function:: match_descriptor(descritor0, descriptor1, threshold=70, filter_outliers=False)

   For LBP descriptors this function returns an integer representing the
   difference between the two descriptors. You may then threshold/compare this
   distance metric as necessary. The distance is a measure of similarity. The
   closer it is to zero the better the LBP keypoint match.

   For ORB descriptors this function returns the ``kptmatch`` object. See above.

   ``threshold`` is used for ORB keypoints to filter ambiguous matches. A lower
   ``threshold`` value tightens the keypoint matching algorithm. ``threshold``
   may be between 0-100 (int). Defaults to 70.

   ``filter_outliers`` is used for ORB keypoints to filter out outlier
   keypoints allow you to raise the ``threshold``. Defaults to False.

class HaarCascade -- Feature Descriptor
---------------------------------------

The Haar Cascade feature descriptor is used for the `Image.find_features()`
method. It doesn't have any methods itself for you to call.

.. class:: HaarCascade(path:str, stages:Optional[int]=None)

   Loads a Haar Cascade into memory from a Haar Cascade binary file formatted
   for your OpenMV Cam. If you pass "frontalface" instead of a path then this
   constructor will load the built-in frontal face Haar Cascade into memory.
   Additionally, you can also pass "eye" to load a Haar Cascade for eyes into
   memory. Finally, this method returns the loaded Haar Cascade object for use
   with `Image.find_features()`.

   ``stages`` defaults to the number of stages in the Haar Cascade. However,
   you can specify a lower number of stages to speed up processing the feature
   detector at the cost of a higher rate of false positives.

   .. note::
      You can make your own Haar Cascades to use with your OpenMV Cam.
      First, Google for "<thing> Haar Cascade" to see if someone
      already made an OpenCV Haar Cascade for an object you want to
      detect. If not... then you'll have to generate your own (which is
      a lot of work). See `here <http://coding-robin.de/2013/07/22/train-your-own-opencv-haar-classifier.html>`_
      for how to make your own Haar Cascade. Then see this `script <https://github.com/openmv/openmv/blob/master/usr/openmv-cascade.py>`_
      for converting OpenCV Haar Cascades into a format your OpenMV Cam
      can read.

   Q: What is a Haar Cascade?

   A: A Haar Cascade is a series of contrast checks that are used to determine
   if an object is present in the image. The contrast checks are split of into
   stages where a stage is only run if previous stages have already passed.
   The contrast checks are simple things like checking if the center vertical
   of the image is lighter than the edges. Large area checks are performed
   first in the earlier stages followed by more numerous and smaller area
   checks in later stages.

   Q: How are Haar Cascades made?

   A: Haar Cascades are made by training the generator algorithm against
   positive and negative labeled images. For example, you'd train the
   generator algorithm against hundreds of pictures with cats in them that
   have been labeled as images with cats and against hundreds of images with
   not cat like things labeled differently. The generator algorithm will then
   produce a Haar Cascade that detects cats.

class Similarity -- Similarity Object
-------------------------------------

The similarity object is returned by `Image.get_similarity()`.

.. class:: Similarity()

   Please call `Image.get_similarity()` to create this object.

   .. method:: mean() -> float

      Returns the mean of the similarity values computed across the image (float).

      You may also get this value doing ``[0]`` on the object.

   .. method:: stdev() -> float

      Returns the standard deviation of the similarity values computed across the image ( (float).

      You may also get this value doing ``[1]`` on the object.

   .. method:: min() -> float

      Returns the min of the similarity values computed across the image ( (float).

      Generally, for the SSIM you want to threshold the min value to determine if two images
      are different.

      You may also get this value doing ``[2]`` on the object.

   .. method:: max() -> float

      Returns the max of the similarity values computed across the image ( (float).

      Generally, for the DSIM you want to threshold the max value to determine if two images
      are different.

      You may also get this value doing ``[3]`` on the object.

class Histogram -- Histogram Object
-----------------------------------

The histogram object is returned by `Image.get_histogram()`.

Grayscale histograms have one channel with some number of bins. All bins are
normalized so that all bins sum to 1.

RGB565 histograms have three channels with some number of bins each. All bins
are normalized so that all bins in a channel sum to 1.

.. class:: histogram()

   Please call `Image.get_histogram()` to create this object.

   .. method:: bins() -> List[float]

      Returns a list of floats for the grayscale histogram.

      You may also get this value doing ``[0]`` on the object.

   .. method:: l_bins() -> List[float]

      Returns a list of floats for the RGB565 histogram LAB L channel.

      You may also get this value doing ``[0]`` on the object.

   .. method:: a_bins() -> List[float]

      Returns a list of floats for the RGB565 histogram LAB A channel.

      You may also get this value doing ``[1]`` on the object.

   .. method:: b_bins() -> List[float]

      Returns a list of floats for the RGB565 histogram LAB B channel.

      You may also get this value doing ``[2]`` on the object.

   .. method:: get_percentile(percentile) -> percentile

      Computes the CDF of the histogram channels and returns a `image.percentile`
      object with the values of the histogram at the passed in ``percentile`` (0.0
      - 1.0) (float). So, if you pass in 0.1 this method will tell you (going from
      left-to-right in the histogram) what bin when summed into an accumulator
      caused the accumulator to cross 0.1. This is useful to determine min (with
      0.1) and max (with 0.9) of a color distribution without outlier effects
      ruining your results for adaptive color tracking.

   .. method:: get_threshold() -> threshold

      Uses Otsu's Method to compute the optimal threshold values that split the
      histogram into two halves for each channel of the histogram. This method
      returns a `image.threshold` object. This method is particularly useful for
      determining optimal `Image.binary()` thresholds.

   .. method:: get_statistics() -> statistics

      Computes the mean, median, mode, standard deviation, min, max, lower
      quartile, and upper quartile of each color channel in the histogram and
      returns a `statistics` object.

      You may also use ``histogram.statistics()`` and ``histogram.get_stats()``
      as aliases for this method.

class Percentile -- Percentile Object
-------------------------------------

The percentile object is returned by `histogram.get_percentile()`.

Grayscale percentiles have one channel. Use the non ``l_*``, ``a_*``, and
``b_*`` method.

RGB565 percentiles have three channels. Use the ``l_*``, ``a_*``, and ``b_*``
methods.

.. class:: percentile()

   Please call `histogram.get_percentile()` to create this object.

   .. method:: value() -> int

      Return the grayscale percentile value (between 0 and 255).

      You may also get this value doing ``[0]`` on the object.

   .. method:: l_value() -> int

      Return the RGB565 LAB L channel percentile value (between 0 and 100).

      You may also get this value doing ``[0]`` on the object.

   .. method:: a_value() -> int

      Return the RGB565 LAB A channel percentile value (between -128 and 127).

      You may also get this value doing ``[1]`` on the object.

   .. method:: b_value() -> int

      Return the RGB565 LAB B channel percentile value (between -128 and 127).

      You may also get this value doing ``[2]`` on the object.

class Threshold -- Threshold Object
-----------------------------------

The threshold object is returned by `histogram.get_threshold()`.

Grayscale thresholds have one channel. Use the non ``l_*``, ``a_*``, and
``b_*`` method.

RGB565 thresholds have three channels. Use the ``l_*``, ``a_*``, and ``b_*``
methods.

.. class:: threshold()

   Please call `histogram.get_threshold()` to create this object.

   .. method:: value() -> int

      Return the grayscale threshold value (between 0 and 255).

      You may also get this value doing ``[0]`` on the object.

   .. method:: l_value() -> int

      Return the RGB565 LAB L channel threshold value (between 0 and 100).

      You may also get this value doing ``[0]`` on the object.

   .. method:: a_value() -> int

      Return the RGB565 LAB A channel threshold value (between -128 and 127).

      You may also get this value doing ``[1]`` on the object.

   .. method:: b_value() -> int

      Return the RGB565 LAB B channel threshold value (between -128 and 127).

      You may also get this value doing ``[2]`` on the object.

class Statistics -- Statistics Object
-------------------------------------

The percentile object is returned by `histogram.get_statistics()` or
`Image.get_statistics()`.

Grayscale statistics have one channel. Use the non ``l_*``, ``a_*``, and
``b_*`` method.

RGB565 statistics have three channels. Use the ``l_*``, ``a_*``, and ``b_*``
methods.

.. class:: statistics()

   Please call `histogram.get_statistics()` or `Image.get_statistics()` to create this object.

   .. method:: mean() -> int

      Returns the grayscale mean (0-255) (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: median() -> int

      Returns the grayscale median (0-255) (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: mode() -> int

      Returns the grayscale mode (0-255) (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: stdev() -> int

      Returns the grayscale standard deviation (0-255) (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: min() -> int

      Returns the grayscale min (0-255) (int).

      You may also get this value doing ``[4]`` on the object.

   .. method:: max() -> int

      Returns the grayscale max (0-255) (int).

      You may also get this value doing ``[5]`` on the object.

   .. method:: lq() -> int

      Returns the grayscale lower quartile (0-255) (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: uq() -> int

      Returns the grayscale upper quartile (0-255) (int).

      You may also get this value doing ``[7]`` on the object.

   .. method:: l_mean() -> int

      Returns the RGB565 LAB L mean (0-255) (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: l_median() -> int

      Returns the RGB565 LAB L median (0-255) (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: l_mode() -> int

      Returns the RGB565 LAB L mode (0-255) (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: l_stdev() -> int

      Returns the RGB565 LAB L standard deviation (0-255) (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: l_min() -> int

      Returns the RGB565 LAB L min (0-255) (int).

      You may also get this value doing ``[4]`` on the object.

   .. method:: l_max() -> int

      Returns the RGB565 LAB L max (0-255) (int).

      You may also get this value doing ``[5]`` on the object.

   .. method:: l_lq() -> int

      Returns the RGB565 LAB L lower quartile (0-255) (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: l_uq() -> int

      Returns the RGB565 LAB L upper quartile (0-255) (int).

      You may also get this value doing ``[7]`` on the object.

   .. method:: a_mean() -> int

      Returns the RGB565 LAB A mean (0-255) (int).

      You may also get this value doing ``[8]`` on the object.

   .. method:: a_median() -> int

      Returns the RGB565 LAB A median (0-255) (int).

      You may also get this value doing ``[9]`` on the object.

   .. method:: a_mode() -> int

      Returns the RGB565 LAB A mode (0-255) (int).

      You may also get this value doing ``[10]`` on the object.

   .. method:: a_stdev() -> int

      Returns the RGB565 LAB A standard deviation (0-255) (int).

      You may also get this value doing ``[11]`` on the object.

   .. method:: a_min() -> int

      Returns the RGB565 LAB A min (0-255) (int).

      You may also get this value doing ``[12]`` on the object.

   .. method:: a_max() -> int

      Returns the RGB565 LAB A max (0-255) (int).

      You may also get this value doing ``[13]`` on the object.

   .. method:: a_lq() -> int

      Returns the RGB565 LAB A lower quartile (0-255) (int).

      You may also get this value doing ``[14]`` on the object.

   .. method:: a_uq() -> int

      Returns the RGB565 LAB A upper quartile (0-255) (int).

      You may also get this value doing ``[15]`` on the object.

   .. method:: b_mean() -> int

      Returns the RGB565 LAB B mean (0-255) (int).

      You may also get this value doing ``[16]`` on the object.

   .. method:: b_median() -> int

      Returns the RGB565 LAB B median (0-255) (int).

      You may also get this value doing ``[17]`` on the object.

   .. method:: b_mode() -> int

      Returns the RGB565 LAB B mode (0-255) (int).

      You may also get this value doing ``[18]`` on the object.

   .. method:: b_stdev() -> int

      Returns the RGB565 LAB B standard deviation (0-255) (int).

      You may also get this value doing ``[19]`` on the object.

   .. method:: b_min() -> int

      Returns the RGB565 LAB B min (0-255) (int).

      You may also get this value doing ``[20]`` on the object.

   .. method:: b_max() -> int

      Returns the RGB565 LAB B max (0-255) (int).

      You may also get this value doing ``[21]`` on the object.

   .. method:: b_lq() -> int

      Returns the RGB565 LAB B lower quartile (0-255) (int).

      You may also get this value doing ``[22]`` on the object.

   .. method:: b_uq() -> int

      Returns the RGB565 LAB B upper quartile (0-255) (int).

      You may also get this value doing ``[23]`` on the object.

class Blob -- Blob object
-------------------------

The blob object is returned by `Image.find_blobs()`.

.. class:: blob()

   Please call `Image.find_blobs()` to create this object.

   .. method:: corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. method:: min_corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners than bound the min area
      rectangle of the blob. Unlike `blob.corners()` the min area rectangle corners
      do not necessarily lie on the blob.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the blob's bounding box.

   .. method:: x() -> int

      Returns the blob's bounding box x coordinate (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the blob's bounding box y coordinate (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: w() -> int

      Returns the blob's bounding box w coordinate (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: h() -> int

      Returns the blob's bounding box h coordinate (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: pixels() -> int

      Returns the number of pixels that are part of this blob (int).

      You may also get this value doing ``[4]`` on the object.

   .. method:: cx() -> int

      Returns the centroid x position of the blob (int).

      You may also get this value doing ``[5]`` on the object.

   .. method:: cxf() -> int

      Returns the centroid x position of the blob (float).

   .. method:: cy() -> int

      Returns the centroid y position of the blob (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: cyf() -> int

      Returns the centroid y position of the blob (float).

   .. method:: rotation() -> float

      Returns the rotation of the blob in radians (float). If the blob is like
      a pencil or pen this value will be unique for 0-180 degrees. If the blob
      is round this value is not useful.

      You may also get this value doing ``[7]`` on the object.

   .. method:: rotation_deg() -> float

      Returns the rotation of the blob in degrees.

   .. method:: rotation_rad() -> float

      Returns the rotation of the blob in radians. This method is more descriptive
      than just `blob.rotation()`.

   .. method:: code() -> int

      Returns a 32-bit binary number with a bit set in it for each color threshold
      that's part of this blob. For example, if you passed `Image.find_blobs()`
      three color thresholds to look for then bits 0/1/2 may be set for this blob.
      Note that only one bit will be set for each blob unless `Image.find_blobs()`
      was called with ``merge=True``. Then its possible for multiple blobs with
      different color thresholds to be merged together. You can use this method
      along with multiple thresholds to implement color code tracking.

      You may also get this value doing ``[8]`` on the object.

   .. method:: count() -> int

      Returns the number of blobs merged into this blob. This is 1 unless you
      called `Image.find_blobs()` with ``merge=True``.

      You may also get this value doing ``[9]`` on the object.

   .. method:: perimeter() -> int

      Returns the number of pixels on this blob's perimeter.

   .. method:: roundness() -> float

      Returns a value between 0 and 1 representing how round the object is. A circle would be a 1.

   .. method:: elongation() -> float

      Returns a value between 0 and 1 representing how long (not round) the object is. A line would be a 1.

   .. method:: area()  -> int

      Returns the area of the bounding box around the blob. (w * h).

   .. method:: density() -> float

      Returns the density ratio of the blob. This is the number of pixels in the
      blob over its bounding box area. A low density ratio means in general that
      the lock on the object isn't very good. The result is between 0 and 1.

   .. method:: extent() -> float

      Alias for `blob.density()`.

   .. method:: compactness() -> float

      Like `blob.density()`, but, uses the perimeter of the blob instead to measure
      the objects density and is thus more accurate. The result is between 0 and 1.

   .. method:: solidity() -> float

      Like `blob.density()` but, uses the minimum area rotated rectangle versus the
      bounding rectangle to measure density. The result is between 0 and 1.

   .. method:: convexity() -> float

      Returns a value between 0 and 1 representing how convex the object is. A square would be 1.

   .. method:: x_hist_bins() -> List[float]

      Returns a histogram of the x axis of all columns in a blob. Bin values are
      scaled between 0 and 1.

   .. method:: y_hist_bins() -> List[float]

      Returns a histogram of the y axis of all the rows in a blob. Bin values are
      scaled between 0 and 1.

   .. method:: major_axis_line() -> Tuple[int, int, int, int]

      Returns a line tuple (x1, y1, x2, y2) that can be drawn with `Image.draw_line()` of the major
      axis of the blob (the line going through the longest side of the min area rectangle).

   .. method:: minor_axis_line() -> Tuple[int, int, int, int]

      Returns a line tuple (x1, y1, x2, y2) that can be drawn with `Image.draw_line()` of the minor
      axis of the blob (the line going through the shortest side of the min area rectangle).

   .. method:: enclosing_circle() -> Tuple[int, int, int]

      Returns a circle tuple (x, y, r) that can be drawn with `Image.draw_circle()` of
      the circle that encloses the min area rectangle of a blob.

   .. method:: enclosed_ellipse() -> Tuple[int, int, int, int, float]

      Returns an ellipse tuple (x, y, rx, ry, rotation) that can be drawn with `Image.draw_ellipse()`
      of the ellipse that fits inside of the min area rectangle of a blob.

class Line -- Line object
-------------------------

The line object is returned by `Image.find_lines()`, `Image.find_line_segments()`, or `Image.get_regression()`.

.. class:: line()

   Please call `Image.find_lines()`, `Image.find_line_segments()`, or `Image.get_regression()` to create this object.

   .. method:: line() -> Tuple[int, int, int, int]

      Returns a line tuple (x1, y1, x2, y2) for use with other `image` methods
      like `Image.draw_line()`.

   .. method:: x1() -> int

      Returns the line's p1 x component.

      You may also get this value doing ``[0]`` on the object.

   .. method:: y1() -> int

      Returns the line's p1 y component.

      You may also get this value doing ``[1]`` on the object.

   .. method:: x2() -> int

      Returns the line's p2 x component.

      You may also get this value doing ``[2]`` on the object.

   .. method:: y2() -> int

      Returns the line's p2 y component.

      You may also get this value doing ``[3]`` on the object.

   .. method:: length() -> int

      Returns the line's length: sqrt(((x2-x1)^2) + ((y2-y1)^2).

      You may also get this value doing ``[4]`` on the object.

   .. method:: magnitude() -> int

      Returns the magnitude of the line from the hough transform.

      You may also get this value doing ``[5]`` on the object.

   .. method:: theta() -> int

      Returns the angle of the line from the hough transform - (0 - 179) degrees.

      You may also get this value doing ``[7]`` on the object.

   .. method:: rho() -> int

      Returns the the rho value for the line from the hough transform.

      You may also get this value doing ``[8]`` on the object.

class Circle -- Circle object
-----------------------------

The circle object is returned by `Image.find_circles()`.

.. class:: circle()

   Please call `Image.find_circles()` to create this object.

   .. method:: x() -> int

      Returns the circle's x position.

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the circle's y position.

      You may also get this value doing ``[1]`` on the object.

   .. method:: r() -> int

      Returns the circle's radius.

      You may also get this value doing ``[2]`` on the object.

   .. method:: magnitude() -> int

      Returns the circle's magnitude.

      You may also get this value doing ``[3]`` on the object.

class Rect -- Rectangle Object
------------------------------

The rect object is returned by `Image.find_rects()`.

.. class:: rect()

   Please call `Image.find_rects()` to create this object.

   .. method:: corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the rect's bounding box.

   .. method:: x() -> int

      Returns the rectangle's top left corner's x position.

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the rectangle's top left corner's y position.

      You may also get this value doing ``[1]`` on the object.

   .. method:: w() -> int

      Returns the rectangle's width.

      You may also get this value doing ``[2]`` on the object.

   .. method:: h() -> int

      Returns the rectangle's height.

      You may also get this value doing ``[3]`` on the object.

   .. method:: magnitude() -> int

      Returns the rectangle's magnitude.

      You may also get this value doing ``[4]`` on the object.

class QRCode -- QRCode object
-----------------------------

The qrcode object is returned by `Image.find_qrcodes()`.

.. class:: qrcode()

   Please call `Image.find_qrcodes()` to create this object.

   .. method:: corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the qrcode's bounding box.

   .. method:: x() -> int

      Returns the qrcode's bounding box x coordinate (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the qrcode's bounding box y coordinate (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: w() -> int

      Returns the qrcode's bounding box w coordinate (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: h() -> int

      Returns the qrcode's bounding box h coordinate (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: payload() -> str

      Returns the payload string of the qrcode. E.g. the URL.

      You may also get this value doing ``[4]`` on the object.

   .. method:: version() -> int

      Returns the version number of the qrcode (int).

      You may also get this value doing ``[5]`` on the object.

   .. method:: ecc_level() -> int

      Returns the ecc_level of the qrcode (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: mask() -> int

      Returns the mask of the qrcode (int).

      You may also get this value doing ``[7]`` on the object.

   .. method:: data_type() -> int

      Returns the data type of the qrcode (int).

      You may also get this value doing ``[8]`` on the object.

   .. method:: eci() -> int

      Returns the eci of the qrcode (int). The eci stores the encoding of data
      bytes in the QR Code. If you plan to handling QR Codes that contain more
      than just standard ASCII text you will need to look at this value.

      You may also get this value doing ``[9]`` on the object.

   .. method:: is_numeric() -> bool

      Returns True if the data_type of the qrcode is numeric.

   .. method:: is_alphanumeric() -> bool

      Returns True if the data_type of the qrcode is alpha numeric.

   .. method:: is_binary() -> bool

      Returns True if the data_type of the qrcode is binary. If you are serious
      about handling all types of text you need to check the eci if this is True
      to determine the text encoding of the data. Usually, it's just standard
      ASCII, but, it could be UTF8 that has some 2-byte characters in it.

   .. method:: is_kanji() -> bool

      Returns True if the data_type of the qrcode is alpha Kanji. If this is True
      then you'll need to decode the string yourself as Kanji symbols are 10-bits
      per character and MicroPython has no support to parse this kind of text. The
      payload in this case must be treated as just a large byte array.

class AprilTag -- AprilTag object
---------------------------------

The apriltag object is returned by `Image.find_apriltags()`.

.. class:: apriltag()

   Please call `Image.find_apriltags()` to create this object.

   Attributes
   ~~~~~~~~~~

   .. attribute:: corners
      :type: List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. attribute:: rect
      :type: Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the apriltag's bounding box.

   .. attribute:: x
      :type: int

      Returns the apriltag's bounding box x coordinate (int).

   .. attribute:: y
      :type: int

      Returns the apriltag's bounding box y coordinate (int).

   .. attribute:: w
      :type: int

      Returns the apriltag's bounding box w coordinate (int).

   .. attribute:: h
      :type: int

      Returns the apriltag's bounding box h coordinate (int).

   .. attribute:: area
      :type: int

      Returns the area (w * h) of the apriltag (int).

   .. attribute:: id
      :type: int

      Returns the numeric id of the apriltag.

        * TAG16H5 -> 0 to 29
        * TAG25H7 -> 0 to 241
        * TAG25H9 -> 0 to 34
        * TAG36H10 -> 0 to 2319
        * TAG36H11 -> 0 to 586
        * ARTOOLKIT -> 0 to 511

   .. attribute:: family
      :type: int

      Returns the numeric family of the apriltag.

        * image.TAG16H5
        * image.TAG25H7
        * image.TAG25H9
        * image.TAG36H10
        * image.TAG36H11
        * image.ARTOOLKIT

   .. attribute:: name
      :type: str

      Returns the family of the apriltag.

        * "TAG16H5"
        * "TAG25H7"
        * "TAG25H9"
        * "TAG36H10"
        * "TAG36H11"
        * "ARTOOLKIT"

   .. attribute:: cx
      :type: int

      Returns the centroid x position of the apriltag (int).

   .. attribute:: cxf
      :type: float

      Returns the centroid x position of the apriltag (float).

   .. attribute:: cy
      :type: int

      Returns the centroid y position of the apriltag (int).

   .. attribute:: cyf
      :type: float

      Returns the centroid y position of the apriltag (float).

   .. attribute:: rotation
      :type: float

      Returns the rotation of the apriltag in radians (float).

   .. attribute:: decision_margin
      :type: float

      Returns the quality of the apriltag match (0.0 - 1.0) where 1.0 is the best.

   .. attribute:: hamming
      :type: int

      Returns the number of accepted bit errors for this tag.

        * TAG16H5 -> 0 bit errors will be accepted
        * TAG25H7 -> up to 1 bit error may be accepted
        * TAG25H9 -> up to 3 bit errors may be accepted
        * TAG36H10 -> up to 3 bit errors may be accepted
        * TAG36H11 -> up to 4 bit errors may be accepted
        * ARTOOLKIT -> 0 bit errors will be accepted

   .. attribute:: goodness
      :type: float

      Returns the quality of the apriltag image (0.0 - 1.0) where 1.0 is the best.

      .. note::

         This value is always 0.0 for now. We may enable a feature called "tag
         refinement" in the future which will allow detection of small apriltags.
         However, this feature currently drops the frame rate to less than 1 FPS.

   .. attribute:: x_translation
      :type: float

      Returns the translation in unknown units from the camera in the X direction.

      This method is useful for determining the apriltag's location away from the
      camera. However, the size of the apriltag, the lens you are using, etc. all
      come into play as to actually determining what the X units are in. For ease
      of use we recommend you use a lookup table to convert the output of this
      method to something useful for your application.

      Note that this is the left-to-right direction.

   .. attribute:: y_translation
      :type: float

      Returns the translation in unknown units from the camera in the Y direction.

      This method is useful for determining the apriltag's location away from the
      camera. However, the size of the apriltag, the lens you are using, etc. all
      come into play as to actually determining what the Y units are in. For ease
      of use we recommend you use a lookup table to convert the output of this
      method to something useful for your application.

      Note that this is the up-to-down direction.

   .. attribute:: z_translation
      :type: float

      Returns the translation in unknown units from the camera in the Z direction.

      This method is useful for determining the apriltag's location away from the
      camera. However, the size of the apriltag, the lens you are using, etc. all
      come into play as to actually determining what the Z units are in. For ease
      of use we recommend you use a lookup table to convert the output of this
      method to something useful for your application.

      Note that this is the front-to-back direction.

   .. attribute:: x_rotation
      :type: float

      Returns the rotation in radians of the apriltag in the X plane. E.g. moving
      the camera left-to-right while looking at the tag.

   .. attribute:: y_rotation
      :type: float

      Returns the rotation in radians of the apriltag in the Y plane. E.g. moving
      the camera up-to-down while looking at the tag.

   .. attribute:: z_rotation
      :type: float

      Returns the rotation in radians of the apriltag in the Z plane. E.g.
      rotating the camera while looking directly at the tag.

      Note that this is just a renamed version of `apriltag.rotation`.

class DataMatrix -- DataMatrix object
-------------------------------------

The datamatrix object is returned by `Image.find_datamatrices()`.

.. class:: datamatrix()

   Please call `Image.find_datamatrices()` to create this object.

   .. method:: corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the datamatrix's bounding box.

   .. method:: x() -> int

      Returns the datamatrix's bounding box x coordinate (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the datamatrix's bounding box y coordinate (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: w() -> int

      Returns the datamatrix's bounding box w coordinate (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: h() -> int

      Returns the datamatrix's bounding box h coordinate (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: payload() -> str

      Returns the payload string of the datamatrix. E.g. The string.

      You may also get this value doing ``[4]`` on the object.

   .. method:: rotation() -> float

      Returns the rotation of the datamatrix in radians (float).

      You may also get this value doing ``[5]`` on the object.

   .. method:: rows() -> int

      Returns the number of rows in the data matrix (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: columns() -> int

      Returns the number of columns in the data matrix (int).

      You may also get this value doing ``[7]`` on the object.

   .. method:: capacity() -> int

      Returns how many characters could fit in this data matrix.

      You may also get this value doing ``[8]`` on the object.

   .. method:: padding() -> int

      Returns how many unused characters are in this data matrix.

      You may also get this value doing ``[9]`` on the object.

class BarCode -- BarCode object
-------------------------------

The barcode object is returned by `Image.find_barcodes()`.

.. class:: barcode()

   Please call `Image.find_barcodes()` to create this object.

   .. method:: corners() -> List[Tuple[int, int]]

      Returns a list of 4 (x,y) tuples of the 4 corners of the object. Corners are
      always returned in sorted clock-wise order starting from the top left.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the barcode's bounding box.

   .. method:: x() -> int

      Returns the barcode's bounding box x coordinate (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: y() -> int

      Returns the barcode's bounding box y coordinate (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: w() -> int

      Returns the barcode's bounding box w coordinate (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: h() -> int

      Returns the barcode's bounding box h coordinate (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: payload() -> str

      Returns the payload string of the barcode. E.g. The number.

      You may also get this value doing ``[4]`` on the object.

   .. method:: type() -> int

      Returns the type enumeration of the barcode (int).

      You may also get this value doing ``[5]`` on the object.

        * image.EAN2
        * image.EAN5
        * image.EAN8
        * image.UPCE
        * image.ISBN10
        * image.UPCA
        * image.EAN13
        * image.ISBN13
        * image.I25
        * image.DATABAR
        * image.DATABAR_EXP
        * image.CODABAR
        * image.CODE39
        * image.PDF417 - Future (e.g. doesn't work right now).
        * image.CODE93
        * image.CODE128

   .. method:: rotation() -> float

      Returns the rotation of the barcode in radians (float).

      You may also get this value doing ``[6]`` on the object.

   .. method:: quality() -> int

      Returns the number of times this barcode was detected in the image (int).

      When scanning a barcode each new scanline can decode the same barcode. This
      value increments for a barcode each time that happens...

      You may also get this value doing ``[7]`` on the object.

class Displacement -- Displacement object
-----------------------------------------

The displacement object is returned by `Image.find_displacement()`.

.. class:: displacement()

   Please call `Image.find_displacement()` to create this object.

   .. method:: x_translation() -> float

      Returns the x translation in pixels between two images. This is sub pixel
      accurate so it's a float.

      You may also get this value doing ``[0]`` on the object.

   .. method:: y_translation() -> float

      Returns the y translation in pixels between two images. This is sub pixel
      accurate so it's a float.

      You may also get this value doing ``[1]`` on the object.

   .. method:: rotation() -> float

      Returns the rotation in radians between two images.

      You may also get this value doing ``[2]`` on the object.

   .. method:: scale() -> float

      Returns the scale change between two images.

      You may also get this value doing ``[3]`` on the object.

   .. method:: response() -> float

      Returns the quality of the results of displacement matching between two images.
      Between 0-1. A ``displacement`` object with a response less than 0.1 is likely noise.

      You may also get this value doing ``[4]`` on the object.

class kptmatch -- Keypoint Object
---------------------------------

The kptmatch object is returned by `image.match_descriptor()` for keypoint matches.

.. class:: kptmatch()

   Please call `image.match_descriptor()` to create this object.

   .. method:: rect() -> Tuple[int, int, int, int]

      Returns a rectangle tuple (x, y, w, h) for use with other `image` methods
      like `Image.draw_rectangle()` of the kptmatch's bounding box.

   .. method:: cx() -> int

      Returns the centroid x position of the kptmatch (int).

      You may also get this value doing ``[0]`` on the object.

   .. method:: cy() -> int

      Returns the centroid y position of the kptmatch (int).

      You may also get this value doing ``[1]`` on the object.

   .. method:: x() -> int

      Returns the kptmatch's bounding box x coordinate (int).

      You may also get this value doing ``[2]`` on the object.

   .. method:: y() -> int

      Returns the kptmatch's bounding box y coordinate (int).

      You may also get this value doing ``[3]`` on the object.

   .. method:: w() -> int

      Returns the kptmatch's bounding box w coordinate (int).

      You may also get this value doing ``[4]`` on the object.

   .. method:: h() -> int

      Returns the kptmatch's bounding box h coordinate (int).

      You may also get this value doing ``[5]`` on the object.

   .. method:: count() -> int

      Returns the number of keypoints matched (int).

      You may also get this value doing ``[6]`` on the object.

   .. method:: theta() -> int

      Returns the estimated angle of rotation for the keypoint (int).

      You may also get this value doing ``[7]`` on the object.

   .. method:: match() -> List[Tuple[int, int]]

      Returns the list of (x,y) tuples of matching keypoints.

      You may also get this value doing ``[8]`` on the object.

class ImageIO  -- ImageIO Object
--------------------------------

The ImageIO object allows you to read/write OpenMV Image objects in their native form to disk
or to memory. This class provides fast read/write random access for loading/storing images.

.. class:: ImageIO(path:str, mode)

   Creates an ImageIO object.

   If ``path`` is a file name on disk then that file will be opened for reading if ``mode`` is ``'r'``
   or writing if ``mode`` is ``'w'``.

   ``path`` may also be a 3-value tuple (w, h, bpp) for in-memory storage of images. ``mode`` in
   this case is then the number of image buffers to store in memory. Note that the in-memory
   storage buffer is not allowed to grow in size after being allocated. Use a ``bpp`` value of
   0 for binary images, 1 for grayscale images, and 2 for rgb565 images.

   .. method:: type() -> int

      Returns if the `ImageIO` object is a `FILE_STREAM` or `MEMORY_STREAM`.

   .. method:: is_closed() -> bool

      Returns if the `ImageIO` object is closed and can no longer be used.

   .. method:: count() -> int

      Returns the number of frames stored.

   .. method:: offset() -> int

      Returns the image index offset.

   .. method:: version() -> Optional[int]

      Returns the version of the object if it's `FILE_STREAM`.
      `MEMORY_STREAM` versions are ``none``.

   .. method:: buffer_size()  -> int

      Returns the size allocated by the object for a frame in a single buffer.

      ``buffer_size() * count() == size()``

   .. method:: size() -> int

      Returns the number of bytes on disk or memory used by the ImageIO object.

   .. method:: write(img:Image) -> ImageIO

      Writes a new image ``img`` to the ImageIO object. For on disk ImageIO objects the file will
      grow as new images are added. For in-memory ImageIO objects this just writes an image to the
      current pre-allocated slot before advancing to the next slot.

      Returns the ImageIO object.

   .. method:: read(copy_to_fb=True, loop=True, pause=True) -> Image

      Returns an image object from the ImageIO object. If ``copy_to_fb`` is False then
      the new image is allocated on the MicroPython heap. However, the MicroPython heap is limited
      and may not have space to store the new image if exhausted. Instead, set ``copy_to_fb`` to
      True to set the frame buffer to the new image making this function work just like `sensor.snapshot()`.

      ``loop`` if True automatically causes the ImageIO object to seek to the beginning at the end
      of the stream of images.

      ``pause`` if True causes this method to pause for a previously recorded number of milliseconds
      by write in-order to match the original frame rate that captured the image data.

   .. method:: seek(offset) -> None

      Seeks to the image slot number ``offset`` in the ImageIO object.

      Works for on disk or in-memory objects.

   .. method:: sync() -> None

      Writes out all data pending for on-disk ImageIO objects.

   .. method:: close() -> None

      Closes the ImageIO object. For in-memory objects this free's the allocated space and for
      on-disk files this closes the file and writes out all meta-data.

   .. data:: FILE_STREAM
      :type: int

      ImageIO object was opened on a file.

   .. data:: MEMORY_STREAM
      :type: int

      ImageIO object was opened in memory.

class Image -- Image object
---------------------------

The image object is the basic object for machine vision operations.

.. class:: Image(arg, buffer:Optional[bytes, bytearray, memoryview]=None, copy_to_fb:bool=False)

   If ``arg`` is a string then this creates a new image object from a file at ``arg`` path.
   Supports loading bmp/pgm/ppm/jpg/jpeg/png image files from disk. If ``copy_to_fb`` is true
   the image is copied to the frame buffer verus being allocated on the heap.

   If ``arg`` is an ``ndarray`` then this creates a new image object from the ``ndarray``.
   ``ndarray`` objects with a shape of ``(w, h)`` are treated as grayscale images, ``(w, h, 3)`` are treated
   as RGB565 images. Only float32 point ``ndarrays`` are supported at this time. When creating
   an image this way if you pass a ``buffer`` argument it will be used to store the image data
   versus allocating space on the heap. If ``copy_to_fb`` is true the image is copied to the
   frame buffer verus being allocated on the heap or using the ``buffer``.

   If ``arg`` is an ``int`` it is then considered the width of a new image and a ``height`` value
   and a ``format`` value must follow to create a new blank image object. ``format`` can be
   be any image pixformat value like `image.GRAYSCALE`. The image will be initialized
   to all zeros. Note that a ``buffer`` value is expected for compressed image formats.
   ``buffer`` is considered as the source of image data for creating images this way. If used with
   ``copy_to_fb`` the data from ``buffer`` is copied to the frame buffer. If you'd like to create a
   JPEG image from a JPEG `bytes()` or `bytearray()` object you can pass the ``width``,
   ``height``, ``image.JPEG`` for the JPEG along with setting ``buffer`` to the JPEG byte stream
   to create a JPEG image.

   Images support "[]" notation. Do ``image[index] = 8/16-bit value`` to assign
   an image pixel or ``image[index]`` to get an image pixel which will be
   either an 8-bit value for grayscale/bayer images of a 16-bit value for RGB565/YUV
   images. Binary images return a 1-bit value.

   For JPEG images the "[]" allows you to access the compressed JPEG image blob
   as a byte-array. Reading and writing to the data array is opaque however as
   JPEG images are compressed byte streams.

   Images also support read buffer operations. You can pass images to all sorts
   of MicroPython functions like as if the image were a byte-array object. In
   particular, if you'd like to transmit an image you can just pass it to the
   UART/SPI/I2C write functions to be transmitted automatically.

   Basic Methods
   ~~~~~~~~~~~~~

   .. method:: width() -> int

      Returns the image width in pixels.

   .. method:: height() -> int

      Returns the image height in pixels.

   .. method:: format() -> int

      Returns `image.GRAYSCALE` for grayscale images, `image.RGB565` for RGB565
      images, `image.BAYER` for bayer pattern images, and `image.JPEG` for JPEG
      images.

   .. method:: size() -> int

      Returns the image size in bytes.

   .. method:: bytearray() -> bytearray

      Returns a `bytearray` object that points to the image data for byte-level read/write access.

      .. note::

         Image objects are automatically cast as `bytes` objects when passed to MicroPython driver
         that requires a `bytes` like object. This is read-only access.
         Call `bytearray()` to get read/write access.

   .. method:: get_pixel(x:int, y:int, rgbtuple:Optional[bool]=None) -> Union[int, Tuple[int, int, int]]

      For grayscale images: Returns the grayscale pixel value at location (x, y).
      For RGB565 images: Returns the RGB888 pixel tuple (r, g, b) at location (x, y).
      For bayer pattern images: Returns the the pixel value at the location (x, y).

      Returns None if ``x`` or ``y`` is outside of the image.

      ``x`` and ``y`` may either be passed independently or as a tuple.

      ``rgbtuple`` if True causes this method to return an RGB888 tuple. Otherwise,
      this method returns the integer value of the underlying pixel. I.e. for RGB565
      images this method returns a RGB565 value. Defaults to True
      for RGB565 images and False otherwise.

      Not supported on compressed images.

      .. note::

         `Image.get_pixel()` and `Image.set_pixel()` are the only methods that allow
         you to manipulate bayer pattern images. Bayer pattern images are literal images
         where pixels in the image are R/G/R/G/etc. for even rows and G/B/G/B/etc. for
         odd rows. Each pixel is 8-bits. If you call this method with ``rgbtuple`` set then `Image.get_pixel()`
         will debayer the source image at that pixel location and return a valid RGB888 tuple for the pixel location.

   .. method:: set_pixel(x:int, y:int, pixel:Union[int, Tuple[int, int, int]]) -> Image

      For grayscale images: Sets the pixel at location (x, y) to the grayscale value ``pixel``.
      For RGB565 images: Sets the pixel at location (x, y) to the RGB888 tuple (r, g, b) ``pixel``.
      For bayer pattern images: Sets the pixel value at the location (x, y) to the value ``pixel``.

      Returns the image object so you can call another method using ``.`` notation.

      ``x`` and ``y`` may either be passed independently or as a tuple.

      ``pixel`` may either be an RGB888 tuple (r, g, b) or the underlying pixel
      value (i.e. a RGB565 value for RGB565 images or an 8-bit value
      for grayscale images.

      Not supported on compressed images.

      .. note::

         `Image.get_pixel()` and `Image.set_pixel()` are the only methods that allow
         you to manipulate bayer pattern images. Bayer pattern images are literal images
         where pixels in the image are R/G/R/G/etc. for even rows and G/B/G/B/etc. for
         odd rows. Each pixel is 8-bits. If you call this method with an RGB888 tuple the grayscale
         value of that RGB888 tuple is extracted and set to the pixel location.

   Conversion Methods
   ~~~~~~~~~~~~~~~~~~

   .. method:: to_ndarray(dtype:str, buffer:Optional[bytes, bytearray, memoryview]=None) -> ndarray

      Returns a ``ndarray`` object created from the image.
      This only works for GRAYSCALE or RGB565 images currently.

      ``dtype`` can be ``b``, ``B``, or ``f`` for creating a signed 8-bit, unsigned 8-bit, or 32-bit floating point ``ndarray``.
      GRAYSCALE images are directly converted to unsigned 8-bit ``ndarray`` objects. For signed 8-bit ``ndarray``
      objects the values (0:255) are mapped to (-127:128). For float 32-bit ``ndarray`` objects the values are
      mapped to (0.0:255.0). RGB565 images are converted to 3-channel ``ndarray`` objects and the same
      process described above for GRAYSCALE images is applied to each channel depending on ``dtype``. Note that
      ``dtype`` also accepts the integer values (e.g. `ord()`) of ``b``, ``B``, and ``f`` respectively.

      ``buffer`` if not ``None`` is a ``bytearray`` object to use as the buffer for the ``ndarray``.
      If ``None`` a new buffer is allocated on the heap to store the ``ndarray`` image data. You can
      use the ``buffer`` argument to directly allocate the ``ndarray`` in a pre-allocated buffer saving
      a heap allocation and a copy operation.

      The ``ndarray`` returned has the shape of ``(height, width)`` for GRAYSCALE images and
      ``(height, width, 3)`` for RGB565 images.

   .. method:: to_bitmap(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to a bitmap image (1 bit per pixel).

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      .. note::

         Bitmap images are like grayscale images with only two pixels values - 0
         and 1. Additionally, bitmap images are packed such that they only store
         1 bit per pixel making them very small. The OpenMV image library allows
         bitmap images to be used in all places `sensor.GRAYSCALE` and `sensor.RGB565` images
         can be used. However, many operations when applied on bitmap images don't
         make any sense because bitmap images only have 2 values. OpenMV recommends
         using bitmap images for ``mask`` values in operations and such as they
         fit on the MicroPython heap quite easily. Finally, bitmap image pixel values
         0 and 1 are interpreted as black and white when being applied to `sensor.GRAYSCALE`
         or `sensor.RGB565` images. The library automatically handles conversion.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_grayscale(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to a grayscale image (8-bits per pixel).

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_rgb565(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to an RGB565 image (16-bits per pixel).

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_rainbow(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=PALETTE_RAINBOW, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to an RGB565 rainbow image (16-bits per pixel).

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_ironbow(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=PALETTE_IRONBOW, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to an RGB565 ironbow image (16-bits per pixel).

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_jpeg(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False, quality:int=90, encode_for_ide:bool=False, subsampling:int=0) -> Image

      Converts an image to a JPEG image.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      ``quality`` controls the jpeg image compression quality. The value can be between 0 and 100.

      ``encode_for_ide`` if True the image is encoded in a way that the IDE can display it if
      printed by doing ``print(image)``. This is useful for debugging purposes over UARTs via
      Open Terminal in the IDE.

      ``subsampling`` can be:

         * `image.JPEG_SUBSAMPLING_AUTO`: Use the best subsampling for the image based on the quality.
         * `image.JPEG_SUBSAMPLING_444`: Use 4:4:4 subsampling.
         * `image.JPEG_SUBSAMPLING_422`: Use 4:2:2 subsampling.
         * `image.JPEG_SUBSAMPLING_420`: Use 4:2:0 subsampling.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: to_png(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Converts an image to a PNG image.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: compress(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False, quality:int=90, encode_for_ide:bool=False, subsampling:int=0) -> Image

      Converts an image to a JPEG image.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      ``quality`` controls the jpeg image compression quality. The value can be between 0 and 100.

      ``encode_for_ide`` if True the image is encoded in a way that the IDE can display it if
      printed by doing ``print(image)``. This is useful for debugging purposes over UARTs via
      Open Terminal in the IDE.

      ``subsampling`` can be:

         * `image.JPEG_SUBSAMPLING_AUTO`: Use the best subsampling for the image based on the quality.
         * `image.JPEG_SUBSAMPLING_444`: Use 4:4:4 subsampling.
         * `image.JPEG_SUBSAMPLING_422`: Use 4:2:2 subsampling.
         * `image.JPEG_SUBSAMPLING_420`: Use 4:2:0 subsampling.

      Returns the image object so you can call another method using ``.`` notation.

      .. note::

         `Image.compress` is an alias for `Image.to_jpeg`.

   .. method:: copy(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy_to_fb:float=False) -> Image

      Creates a deep copy of the image object.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      This has no special effect if the image is already in the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: crop(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Modifies an image in-place without changing the underlying image type.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: scale(x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, copy:bool=False, copy_to_fb:bool=False) -> Image

      Modifies an image in-place without changing the underlying image type.

      ``x_scale`` controls how much the image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the image to extract. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the final image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      final image.

      ``alpha`` controls how much of the source image to blend into the final image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the original
      and final image. 0 results in no modification to the final image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the image.
         * `image.VFLIP`: Vertically flip the image.
         * `image.TRANSPOSE`: Transpose the image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.ROTATE_90`: Rotate the image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the image by 270 degrees (this is just HMIRROR | TRANSPOSE).

      ``copy`` if True create a deep-copy on the heap of the image that's been converted versus converting the
      original image in-place.

      ``copy_to_fb`` if True the image is loaded directly into the frame buffer.
      ``copy_to_fb`` has priority over ``copy``. This has no special effect if the image is already in
      the frame buffer.

      Returns the image object so you can call another method using ``.`` notation.

      .. note::

         `Image.scale` is an alias for `Image.crop`.

   .. method:: save(path:str, roi:Optional[Tuple[int,int,int,int]]=None, quality=50) -> Image

      Saves a copy of the image to the filesystem at ``path``.

      Supports bmp/pgm/ppm/jpg/jpeg image files. Note that you cannot save jpeg
      compressed images to an uncompressed format.

      ``roi`` is the region-of-interest rectangle (x, y, w, h) to save from.
      If not specified, it is equal to the image rectangle which copies the entire
      image. This argument is not applicable for JPEG images.

      ``quality`` is the jpeg compression quality to use to save the image to jpeg
      format if the image is not already compressed (0-100) (int).

      Returns the image object so you can call another method using ``.`` notation.

   .. method:: flush() -> None

      Updates the frame buffer in the IDE with the image in the frame buffer on the camera.

   Drawing Methods
   ~~~~~~~~~~~~~~~

   .. method:: clear(mask:Optional[Image]=None) -> Image

      Sets all pixels in the image to zero (very fast).

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images.

   .. method:: draw_line(x0:int, y0:int, x1:int, y1:int, color:Optional[int,Tuple[int,int,int]]=None, thickness=1) -> Image

      Draws a line from (x0, y0) to (x1, y1) on the image. You may either
      pass x0, y0, x1, y1 separately or as a tuple (x0, y0, x1, y1).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``thickness`` controls how thick the line is in pixels.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_rectangle(x:int, y:int, w:int, h:int, color:Optional[int,Tuple[int,int,int]]=None, thickness=1, fill=False) -> Image

      Draws a rectangle on the image. You may either pass x, y, w, h separately
      or as a tuple (x, y, w, h).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``thickness`` controls how thick the lines are in pixels.

      Pass ``fill`` set to True to fill the rectangle.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_circle(x:int, y:int, radius:int, color:Optional[int,Tuple[int,int,int]]=None, thickness=1, fill=False) -> Image

      Draws a circle on the image. You may either pass x, y, radius separately or
      as a tuple (x, y, radius).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``thickness`` controls how thick the edges are in pixels.

      Pass ``fill`` set to True to fill the circle.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_ellipse(cx:int, cy:int, rx:int, ry:int, rotation:int, color:Optional[int,Tuple[int,int,int]]=None, thickness=1, fill=False) -> Image

      Draws an ellipse on the image. You may either pass cx, cy, rx, ry, and the
      rotation (in degrees) separately or as a tuple (cx, yc, rx, ry, rotation).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``thickness`` controls how thick the edges are in pixels.

      Pass ``fill`` set to True to fill the ellipse.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_string(x:int, y:int, text:str, color:Optional[int,Tuple[int,int,int]]=None, scale=1, x_spacing=0, y_spacing=0, mono_space=True, char_rotation=0, char_hmirror=False, char_vflip=False, string_rotation=0, string_hmirror=False, string_vflip=False) -> Image

      Draws 8x10 text starting at location (x, y) in the image. You may either pass
      x, y separately or as a tuple (x, y).

      ``text`` is a string to write to the image. ``\n``, ``\r``, and ``\r\n``
      line endings move the cursor to the next line.

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``scale`` may be increased to increase/decrease the size of the text on the
      image. You can pass greater than 0 integer or floating point values.

      ``x_spacing`` allows you to add (if positive) or subtract (if negative) x
      pixels between characters.

      ``y_spacing`` allows you to add (if positive) or subtract (if negative) y
      pixels between characters (for multi-line text).

      ``mono_space`` defaults to True which forces text to be fixed spaced. For
      large text scales this looks terrible. Set the False to get non-fixed width
      character spacing which looks A LOT better.

      ``char_rotation`` may be 0, 90, 180, 270 to rotate each character in the
      string by this amount.

      ``char_hmirror`` if True horizontally mirrors all characters in the string.

      ``char_vflip`` if True vertically flips all characters in the string.

      ``string_rotation`` may be 0, 90, 180, 270 to rotate the string by this
      amount.

      ``string_hmirror`` if True horizontally mirrors the string.

      ``string_vflip`` if True vertically flips the string.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_cross(x:int, y:int, color:Optional[int,Tuple[int,int,int]]=None, size=5, thickness=1) -> Image

      Draws a cross at location x, y. You may either pass x, y separately or as a
      tuple (x, y).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``size`` controls how long the lines of the cross extend.

      ``thickness`` controls how thick the edges are in pixels.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_arrow(x0:int, y0:int, x1:int, y1:int, color:Optional[int,Tuple[int,int,int]]=None, thickness=1) -> Image

      Draws an arrow from (x0, y0) to (x1, y1) on the image. You may
      either pass x0, y0, x1, y1 separately or as a tuple (x0, y0, x1, y1).

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``thickness`` controls how thick the line is in pixels.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_edges(image:Image, corners, color:Optional[int,Tuple[int,int,int]]=None, size=0, thickness=1, fill=False) -> Image

      Draws line edges between a corner list returned by methods like `blob.corners`. Corners is
      a four valued tuple of two valued x/y tuples. E.g. [(x1,y1),(x2,y2),(x3,y3),(x4,y4)].

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``size`` if greater than 0 causes the corners to be drawn as circles of radius ``size``.

      ``thickness`` controls how thick the line is in pixels.

      Pass ``fill`` set to True to fill the corner circles if drawn.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: draw_image(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Draws an ``image`` whose top-left corner starts at location ``x``, ``y``. This method automatically
      handles rendering the image passed into the correct pixel format for the destination image while
      also handling clipping seamlessly. ``image`` may also be a RGB888 tuple to draw a color instead
      of an image. You may also pass a path instead of an image object for this method to automatically
      load the image from disk and use it in one step. E.g. ``draw_image("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: draw_keypoints(keypoints, color:Optional[int,Tuple[int,int,int]]=None, size=10, thickness=1, fill=False) -> Image

      Draws the keypoints of a keypoints object on the image. You may also pass a
      list of three value tuples containing the (x, y, rotation_angle_in_degrees) to
      reuse this method for drawing keypoint glyphs which are a circle with a line
      pointing in a particular direction.

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      ``size`` controls how large the keypoints are.

      ``thickness`` controls how thick the line is in pixels.

      Pass ``fill`` set to True to fill the keypoints.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: flood_fill(x:int, y:int, seed_threshold=0.05, floating_threshold=0.05, color:Optional[int,Tuple[int,int,int]]=None, invert=False, clear_background=False, mask:Optional[Image]=None) -> Image

      Flood fills a region of the image starting from location x, y. You may either
      pass x, y separately or as a tuple (x, y).

      ``seed_threshold`` controls how different any pixel in the fill area may be
      from the original starting pixel.

      ``floating_threshold`` controls how different any pixel in the fill area may
      be from any neighbor pixels.

      ``color`` is an RGB888 tuple for Grayscale or RGB565 images. Defaults to
      white. However, you may also pass the underlying pixel value (0-255) for
      grayscale images or a RGB565 value for RGB565 images.

      Pass ``invert`` as True to re-color everything outside of the flood-fill
      connected area.

      Pass ``clear_background`` as True to zero the rest of the pixels that
      flood-fill did not re-color.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      evaluated when flood filling.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   Masking Methods
   ~~~~~~~~~~~~~~~

   .. method:: mask_rectange(x:int, y:int, w:int, h:int) -> Image

      Zeros a rectangular part of the image. If no arguments are supplied this
      method zeros the center of the image.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: mask_circle(x:int, y:int, radius:int) -> Image

      Zeros a circular part of the image. If no arguments are supplied this
      method zeros the center of the image.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: mask_ellipse(x:int, y:int, radius_x:int, radius_y:int, rotation_angle_in_degrees:int) -> Image

      Zeros an ellipsed shaped part of the image. If no arguments are supplied this
      method zeros the center of the image.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   Binary Methods
   ~~~~~~~~~~~~~~

   .. method:: binary(thresholds:List[Tuple[int,int]], invert=False, zero=False, mask:Optional[Image]=None, to_bitmap=False, copy=False) -> Image

      Sets all pixels in the image to black or white depending on if the pixel
      is inside of a threshold in the threshold list ``thresholds`` or not.

      ``thresholds`` must be a list of tuples ``[(lo, hi), (lo, hi), ..., (lo, hi)]``
      defining the ranges of color you want to track. For grayscale images each tuple
      needs to contain two values - a min grayscale value and a max grayscale value.
      Only pixel regions that fall between these thresholds will be considered.
      For RGB565 images each tuple needs to have six values (l_lo, l_hi, a_lo, a_hi, b_lo, b_hi)
      - which are minimums and maximums for the LAB L, A, and B channels respectively.
      For easy usage this function will automatically fix swapped min and max values.
      Additionally, if a tuple is larger than six values the rest are ignored.
      Conversely, if the tuple is too short the rest of the thresholds are assumed
      to be at maximum range.

      .. note::

         To get the thresholds for the object you want to track just select (click
         and drag) on the object you want to track in the IDE frame buffer. The
         histogram will then update to just be in that area. Then just write down
         where the color distribution starts and falls off in each histogram channel.
         These will be your low and high values for ``thresholds``. It's best to
         manually determine the thresholds versus using the upper and lower
         quartile statistics because they are too tight.

         You may also determine color thresholds by going into
         ``Tools->Machine Vision->Threshold Editor`` in OpenMV IDE and selecting
         thresholds from the GUI slider window.

      ``invert`` inverts the thresholding operation such that instead of matching
      pixels inside of some known color bounds pixels are matched that are outside
      of the known color bounds.

      Set ``zero`` to True to instead zero thresholded pixels and leave pixels
      not in the threshold list untouched.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      ``to_bitmap`` turns the image data into a binary bitmap image where each
      pixel is stored in 1 bit. For very small images the new bitmap image may
      not fit inside of the original image requiring an out-of-place operation
      using ``copy``.

      ``copy`` if True creates a copy of the binarized image on the heap versus
      modifying the source image.

      .. note::

         Bitmap images are like grayscale images with only two pixels values - 0
         and 1. Additionally, bitmap images are packed such that they only store
         1 bit per pixel making them very small. The OpenMV image library allows
         bitmap images to be used in all places `sensor.GRAYSCALE` and `sensor.RGB565` images
         can be used. However, many operations when applied on bitmap images don't
         make any sense because bitmap images only have 2 values. OpenMV recommends
         using bitmap images for ``mask`` values in operations and such as they
         fit on the MicroPython heap quite easily. Finally, bitmap image pixel values
         0 and 1 are interpreted as black and white when being applied to `sensor.GRAYSCALE`
         or `sensor.RGB565` images. The library automatically handles conversion.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: invert() -> Image

      Flips (binary inverts) all pixels values in the image. Note that binary
      inversion is the same as numerical inversion for images because:

      ``(255 - pixel) % 256 == (255 + ~pixel + 1) % 256 == (~pixel + 256) % 256 == ~pixel`` and
      this holds for any value that's in a range of ``(0-2^n-1)`` which is true for all mutable image datatypes.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_and(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``AND`` of ``image`` and this image (e.g. ``a & b``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_and("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_nand(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``NAND`` of ``image`` and this image (e.g. ``~(a & b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_nand("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_or(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``OR`` of ``image`` and this image (e.g. ``(a | b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_or("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_nor(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``NOR`` of ``image`` and this image (e.g. ``~(a | b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_nor("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_xor(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``XOR`` of ``image`` and this image (e.g. ``(a ^ b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_xor("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: b_xnor(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Finds the logical ``XNOR`` of ``image`` and this image (e.g. ``~(a ^ b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``b_xnor("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   ISP Methods
   ~~~~~~~~~~~

   .. method:: awb(max:bool=False) -> Image

      Performs automatic white balance on the image using the gray-world algorithm. This method
      operates on RAW Bayer Images so that you can improve image quality before converting
      to RGB565 or passing the RAW Bayer Image to an image processing function. You may also
      call this on a RGB565. This method has no affect on binary/grayscale images.

      ``max`` if True uses the white-patch algorithm instead.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed or yuv images.

   .. method:: ccm(matrix) -> Image

      Multiples the passed floating-point color-correction-matrix with the image. Matrices may be in the form of::

          [[rr, rg, rb], [gr, gg, gb], [br, bg, bb]]
          [[rr, rg, rb], [gr, gg, gb], [br, bg, bb], [xx, xx, xx]]
          [[rr, rg, rb, ro], [gr, gg, gb, go], [br, bg, bb, bo]]
          [[rr, rg, rb, ro], [gr, gg, gb, go], [br, bg, bb, bo], [xx, xx, xx, xx]]

          [rr, rg, rb, ro, gr, gg, gb, go, br, bg, bb, bo]
          [rr, rg, rb, ro, gr, gg, gb, go, br, bg, bb, bo, xx, xx, xx, xx]

      The CCM Method does::

          |R'|                |R|      |R'|                |R|
          |G'| = 3x3 Matrix * |G|  or  |G'| = 3x4 Matrix * |G|
          |B'|                |B|      |B'|                |B|
                                                           |1|

      Note that the sum of each row in the 3x3 matrix should generally be -1, +1, or 0.
      Weights may either be positive or negative.

      You may want to use this method to eliminate systemic cross talk between color channels.
      Or alternatively, to do color correction on the whole image.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: gamma(gamma:float=1.0, contrast:float=1.0, brightness:float=0.0) -> Image

      Quickly changes the image gamma, contrast, and brightness.

      ``gamma`` with values greater than 1.0 makes the image darker in a non-linear
      manner while less than 1.0 makes the image brighter. The gamma value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pow(pixel, 1/gamma)`` on all pixels before scaling back.

      ``contrast`` with values greater than 1.0 makes the image brighter in a linear
      manner while less than 1.0 makes the image darker. The contrast value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pixel * contrast`` on all pixels before scaling back.

      ``brightness`` with values greater than 0.0 makes the image brighter in a constant
      manner while less than 0.0 makes the image darker. The brightness value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pixel + brightness`` on all pixels before scaling back.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed or bayer/yuv images.

   .. method:: gamma_corr(gamma:float=1.0, contrast:float=1.0, brightness:float=0.0) -> Image

      Quickly changes the image gamma, contrast, and brightness.

      ``gamma`` with values greater than 1.0 makes the image darker in a non-linear
      manner while less than 1.0 makes the image brighter. The gamma value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pow(pixel, 1/gamma)`` on all pixels before scaling back.

      ``contrast`` with values greater than 1.0 makes the image brighter in a linear
      manner while less than 1.0 makes the image darker. The contrast value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pixel * contrast`` on all pixels before scaling back.

      ``brightness`` with values greater than 0.0 makes the image brighter in a constant
      manner while less than 0.0 makes the image darker. The brightness value is applied
      to the image by scaling all pixel color channels to be between [0:1) and then
      doing a remapping of ``pixel + brightness`` on all pixels before scaling back.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed or bayer/yuv images.

      .. note::

         `Image.gamma_corr` is an alias for `Image.gamma`.

   Math Methods
   ~~~~~~~~~~~~

   .. method:: negate() -> Image

      Flips (binary inverts) all pixels values in the image. Note that binary
      inversion is the same as numerical inversion for images because:

      ``(255 - pixel) % 256 == (255 + ~pixel + 1) % 256 == (~pixel + 256) % 256 == ~pixel`` and
      this holds for any value that's in a range of ``(0-2^n-1)`` which is true for all mutable image datatypes.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

      .. note::

         `Image.negate` is an alias for `Image.invert`.

   .. method:: replace(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Draws an ``image`` whose top-left corner starts at location ``x``, ``y``. This method automatically
      handles rendering the image passed into the correct pixel format for the destination image while
      also handling clipping seamlessly. ``image`` may also be a RGB888 tuple to draw a color instead
      of an image. You may also pass a path instead of an image object for this method to automatically
      load the image from disk and use it in one step. E.g. ``replace("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

      .. note::

         `Image.replace` is an alias for `Image.draw_image`.

   .. method:: assign(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Draws an ``image`` whose top-left corner starts at location ``x``, ``y``. This method automatically
      handles rendering the image passed into the correct pixel format for the destination image while
      also handling clipping seamlessly. ``image`` may also be a RGB888 tuple to draw a color instead
      of an image. You may also pass a path instead of an image object for this method to automatically
      load the image from disk and use it in one step. E.g. ``assign("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

      .. note::

         `Image.assign` is an alias for `Image.draw_image`.

   .. method:: set(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Draws an ``image`` whose top-left corner starts at location ``x``, ``y``. This method automatically
      handles rendering the image passed into the correct pixel format for the destination image while
      also handling clipping seamlessly. ``image`` may also be a RGB888 tuple to draw a color instead
      of an image. You may also pass a path instead of an image object for this method to automatically
      load the image from disk and use it in one step. E.g. ``set("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

      .. note::

         `Image.set` is an alias for `Image.draw_image`.

   .. method:: add(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically adds ``image`` and this image (e.g. ``min(a + b, 255)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``add("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: sub(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically subtracts ``image`` and this image (e.g. ``max(a - b, 0)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``sub("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: rsub(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically reverse subtracts ``image`` and this image (e.g. ``max(b - a, 0)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``rsub("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: min(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically finds the minimum of ``image`` and this image (e.g. ``min(a, b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``min("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: max(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically finds the maximum of ``image`` and this image (e.g. ``max(a, b)``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``max("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: difference(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Numerically finds the absolute difference of ``image`` and this image (e.g. ``|a - b|``),
      color channel by color channel, from the top-left corner at location ``x``, ``y``.
      This method automatically handles rendering the image passed into the correct pixel format for
      the destination image while also handling clipping seamlessly. ``image`` may also be a RGB888
      tuple to draw a color instead of an image. You may also pass a path instead of an image object
      for this method to automatically load the image from disk and use it in one step. E.g. ``difference("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: blend(image:Image, x:int=0, y:int=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, mask:Optional[Image]=None) -> Image

      Draws an ``image`` whose top-left corner starts at location ``x``, ``y``. This method automatically
      handles rendering the image passed into the correct pixel format for the destination image while
      also handling clipping seamlessly. ``image`` may also be a RGB888 tuple to draw a color instead
      of an image. You may also pass a path instead of an image object for this method to automatically
      load the image from disk and use it in one step. E.g. ``blend("test.jpg")``.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image to draw. This
      allows you to extract just the pixels in the ROI to scale and draw on the destination image.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed)
      and to apply onto the destination image. For example, if you pass ``rgb_channel=1`` this will
      extract the green channel of the source RGB565 image and apply that in grayscale on the
      destination image.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in no modification to the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being drawn at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being drawn on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being drawn to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being drawn to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being drawn to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being drawn on is black speeding up blending.

      ``mask`` is another image to use as a pixel level mask for the operation.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

      .. note::

         `Image.blend` is an alias for `Image.draw_image`.

   .. method:: histeq(adaptive=False, clip_limit=-1, mask:Optional[Image]=None) -> Image

      Runs the histogram equalization algorithm on the image. Histogram
      equalization normalizes the contrast and brightness in the image.

      If you pass ``adaptive`` as True then an adaptive histogram equalization
      method will be run on the image instead which as generally better results
      than non-adaptive histogram qualization but a longer run time.

      ``clip_limit`` provides a way to limit the contrast of the adaptive histogram
      qualization. Use a small value for this, like 10, to produce good histogram
      equalized contrast limited images.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   Filtering Methods
   ~~~~~~~~~~~~~~~~~

   .. method:: erode(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Removes pixels from the edges of segmented areas.

      This method works by convolving a kernel of ``((size*2)+1)x((size*2)+1)`` pixels
      across the image and zeroing the center pixel of the kernel if the sum of
      the neighbour pixels clear is greater than ``threshold``.

      This method works like the standard erode method if threshold is not set. If
      ``threshold`` is set then you can specify erode to only erode pixels that
      have, for example, more than 2 pixels clear in the kernel region with a
      threshold of 2.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: dilate(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Adds pixels to the edges of segmented areas.

      This method works by convolving a kernel of ``((size*2)+1)x((size*2)+1)`` pixels
      across the image and setting the center pixel of the kernel if the sum of
      the neighbour pixels set is greater than ``threshold``.

      This method works like the standard dilate method if threshold is not set.
      If ``threshold`` is set then you can specify dilate to only dilate pixels
      that have, for example, more than 2 pixels set in the kernel region with a
      threshold of 2.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: open(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Performs erosion and dilation on an image in order. Please see `Image.erode()`
      and `Image.dilate()` for more information.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: close(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Performs dilation and erosion on an image in order. Please see `Image.dilate()`
      and `Image.erode()` for more information.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: top_hat(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Returns the image difference of the image and `Image.open()`'ed image.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Not supported on compressed images or bayer/yuv images.

   .. method:: black_hat(size:int, threshold:Optional[int]=None, mask:Optional[Image]=None) -> Image

      Returns the image difference of the image and `Image.close()`'ed image.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Not supported on compressed images or bayer/yuv images.

   .. method:: mean(size:int, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Standard mean blurring filter using a box filter.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: median(size:int, percentile:Optional[float]=0.5, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Runs the median filter on the image. The median filter is the best filter
      for smoothing surfaces while preserving edges but it is very slow.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      ``percentile`` controls the percentile of the value used in the kernel. By
      default each pixel is replaced with the 50th percentile (center) of its
      neighbors. You can set this to 0 for a min filter, 0.25 for a lower quartile
      filter, 0.75 for an upper quartile filter, and 1.0 for a max filter.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: mode(size:int, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=Nonee) -> Image

      Runs the mode filter on the image by replacing each pixel with the mode of
      their neighbors. This method works great on grayscale images. However, on
      RGB images it creates a lot of artifacts on edges because of the non-linear
      nature of the operation.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: midpoint(size:int, bias:Optional[float]=0.5, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Runs the midpoint filter on the image. This filter finds the midpoint
      ((max-min)/2) of each pixel neighborhood in the image.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      ``bias`` controls the min/max mixing. 0 for min filtering only, 1.0 for max
      filtering only. By using the ``bias`` you can min/max filter the image.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: morph(size:int, kernel:list, mul:Optional[float]=1.0, add:Optional[float]=0.0, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Convolves the image by a filter kernel. This allows you to do general purpose
      convolutions on an image.

      ``size`` controls the size of the kernel which must be
      ``((size*2)+1)x((size*2)+1)`` elements big.

      ``kernel`` is the kernel to convolve the image by. The kernel can either be
      a 1D tuple or list or a 2D tuple or list. For 1D kernels the tuple/list
      must be ``((size*2)+1)x((size*2)+1)`` elements big. For 2D tuples/lists each
      row must be ``((size*2)+1)`` elements big and there must be ``((size*2)+1)`` rows.

      ``mul`` allows you to do a global contrast adjustment. It's value should be greater than
      0.0. The default value is 1.0 which does nothing.

      ``add`` allows you to do a global brightness adjustment. It's value should be between
      0.0 and 1.0. The default value is 0.0 which does nothing.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: gaussian(size:int, unsharp:Optional[bool]=False, mul:Optional[float]=1.0, add:Optional[float]=0.0, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Convolves the image by a smoothing gaussian kernel.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      If ``unsharp`` is set to the True then instead of doing just a gaussian
      filtering operation this method will perform an unsharp mask operation which
      improves image sharpness on edges.

      ``mul`` allows you to do a global contrast adjustment. It's value should be greater than
      0.0. The default value is 1.0 which does nothing.

      ``add`` allows you to do a global brightness adjustment. It's value should be between
      0.0 and 1.0. The default value is 0.0 which does nothing.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   .. method:: laplacian(size:int, sharpen:Optional[bool]=False, mul:Optional[float]=1.0, add:Optional[float]=0.0, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Convolves the image by a edge detecting laplacian kernel.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      If ``sharpen`` is set to the True then instead of just outputting an
      unthresholded edge detection image this method will instead sharpen the
      image. Increase the kernel size then to increase the image sharpness.

      ``mul`` allows you to do a global contrast adjustment. It's value should be greater than
      0.0. The default value is 1.0 which does nothing.

      ``add`` allows you to do a global brightness adjustment. It's value should be between
      0.0 and 1.0. The default value is 0.0 which does nothing.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

     Not supported on compressed images or bayer/yuv images.

   .. method:: bilateral(size:int, color_sigma:Optional[float]=0.1, space_sigma:Optional[float]=1.0, threshold:Optional[bool]=False, offset:Optional[int]=0, invert:Optional[bool]=False, mask:Optional[Image]=None) -> Image

      Convolves the image by a bilateral filter. The bilateral filter smooths the
      image while keeping edges in the image.

      ``size`` is the kernel size. Use 1 (3x3 kernel), 2 (5x5 kernel), etc.

      ``color_sigma`` controls how closely colors are matched using the bilateral
      filter. Increase this to increase color blurring.

      ``space_sigma`` controls how closely pixels space-wise are blurred with
      each other. Increase this to increase pixel blurring.

      If you'd like to adaptive threshold the image on the output of the filter
      you can pass ``threshold=True`` which will enable adaptive thresholding of the
      image which sets pixels to one or zero based on a pixel's brightness in relation
      to the brightness of the kernel of pixels around them. A negative ``offset``
      value sets more pixels to 1 as you make it more negative while a positive
      value only sets the sharpest contrast changes to 1. Set ``invert`` to invert
      the binary image resulting output.

      ``mask`` is another image to use as a pixel level mask for the operation.
      The mask should be an image with just black or white pixels and should be the
      same size as the image being operated on. Only pixels set in the mask are
      modified.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer/yuv images.

   Geometric Methods
   ~~~~~~~~~~~~~~~~~

   .. method:: linpolar(reverse:bool=False) -> Image

      Re-project's and image from cartessian coordinates to linear polar coordinates.

      Set ``reverse=True`` to re-project in the opposite direction.

      Linear polar re-projection turns rotation of an image into x-translation.

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   .. method:: logpolar(reverse:bool=False) -> Image

      Re-project's and image from cartessian coordinates to log polar coordinates.

      Set ``reverse=True`` to re-project in the opposite direction.

      Log polar re-projection turns rotation of an image into x-translation
      and scaling/zooming into y-translation.

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   .. method:: lens_corr(strength:float=1.8, zoom:float=1.0, x_corr:float=0.0, y_corr:float=0.0) -> Image

      Performs lens correction to un-fisheye the image due to the lens distortion.

      ``strength`` is a float defining how much to un-fisheye the image. Try 1.8
      out by default and then increase or decrease from there until the image
      looks good.

      ``zoom`` is the amount to zoom in on the image by. 1.0 by default.

      ``x_corr`` floating point pixel offset from center. Can be negative or positive.

      ``y_corr`` floating point pixel offset from center. Can be negative or positive.

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

   .. method:: rotation_corr(x_rotation=0.0, y_rotation=0.0, z_rotation=0.0, x_translation=0.0, y_translation=0.0, zoom=1.0, fov=60.0, corners:Optional[List[Tuple[int,int]]]=None) -> Image

      Corrects perspective issues in the image by doing a 3D rotation of the frame buffer.

      ``x_rotation`` is the number of degrees to rotation the image in the frame buffer
      around the x axis (i.e. this spins the image up and down).

      ``y_rotation`` is the number of degrees to rotation the image in the frame buffer
      around the y axis (i.e. this spins the image left and right).

      ``z_rotation`` is the number of degrees to rotation the image in the frame buffer
      around the z axis (i.e. this spins the image in place).

      ``x_translation`` is the number of units to move the image to the left or right
      after rotation. Because this translation is applied in 3D space the units aren't pixels...

      ``y_translation`` is the number of units to move the image to the up or down
      after rotation. Because this translation is applied in 3D space the units aren't pixels...

      ``zoom`` is the amount to zoom in on the image by. 1.0 by default.

      ``fov`` is the field-of-view to use internally when doing 2D->3D projection before
      rotating the image in 3D space. As this value approaches 0 the image is placed at infinity away
      from the viewport. As this value approaches 180 the image is placed within the viewport. Typically,
      you should not change this value but you can modify it to change the 2D->3D mapping effect.

      ``corners`` is a list of four (x,y) tuples representing four corners used to create a 4-point
      correspondence homography that will map the first corner to (0, 0), the second corner to
      (image_width-1, 0), the third corner to (image_width-1, image_height-1), and the fourth corner
      to (0, image_height-1). The 3D rotation is then applied after the image is re-mapped. This
      argument lets you use `rotation_corr` to do things like birds-eye-view transforms. E.g::

          top_tilt = 10 # if the difference between top/bottom_tilt become to large this method will stop working
          bottom_tilt = 0

          points = [(tilt, 0), (img.width()-tilt, 0), (img.width()-1-bottom_tilt, img.height()-1), (bottom_tilt, img.height()-1)]

          img.rotation_corr(corners=points)

      Returns the image object so you can call another method using ``.`` notation.

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   Get Methods
   ~~~~~~~~~~~

   .. method:: get_similarity(image:Image, x:Optional[int]=0, y:Optional[int]=0, x_scale:float=1.0, y_scale:float=1.0, roi:Optional[Tuple[int,int,int,int]]=None, rgb_channel:int=-1, alpha:int=256, color_palette=None, alpha_palette=None, hint:int=0, dssim:bool=False) -> Similarity

      Computes the similarity between two images. The similarity is computed by
      using the structural similarity index (SSIM). The SSIM is a metric that
      compares the structural similarity between two images. The SSIM is a value
      between -1 and 1. A value of 1 means the images are identical, a value of
      0 means the images are not similar, and a value of -1 means the images are
      perfectly the opposite of each other. Typically, if you want to check
      if two images are different you should look to see how negative the SSIM
      value is.

      ``image`` is the image to compare to.

      You may also pass a path instead of an image object for this method to automatically load the image
      from disk and use it in one step. E.g. ``get_similarity("test.jpg")``.

      ``x`` is the x offset to start comparing the image at.

      ``y`` is the y offset to start comparing the image at.

      ``x_scale`` controls how much the source image is scaled by in the x direction (float). If this
      value is negative the image will be flipped horizontally. Note that if ``y_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``y_scale`` controls how much the source image is scaled by in the y direction (float). If this
      value is negative the image will be flipped vertically. Note that if ``x_scale`` is not specified
      then it will match ``x_scale`` to maintain the aspect ratio.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h) of the source image. This
      allows you to extract just the pixels in the ROI.

      ``rgb_channel`` is the RGB channel (0=R, G=1, B=2) to extract from an RGB565 image (if passed).
      For example, if you pass ``rgb_channel=1`` this will extract the green channel of the source
      RGB565 image and apply that in grayscale.

      ``alpha`` controls how much of the source image to blend into the destination image. A value of
      256 draws an opaque source image while a value lower than 256 produces a blend between the source
      and destination image. 0 results in the destination image.

      ``color_palette`` if not ``None`` can be `image.PALETTE_RAINBOW`, `image.PALETTE_IRONBOW`, or
      a 256 pixel in total RGB565 image to use as a color lookup table on the grayscale value of
      whatever the source image is. This is applied after ``rgb_channel`` extraction if used.

      ``alpha_palette`` if not ``None`` can be a 256 pixel in total GRAYSCALE image to use as a alpha
      palette which modulates the ``alpha`` value of the source image being at a pixel pixel
      level allowing you to precisely control the alpha value of pixels based on their grayscale value.
      A pixel value of 255 in the alpha lookup table is opaque which anything less than 255 becomes
      more transparent until 0. This is applied after ``rgb_channel`` extraction if used.

      ``hint`` can be a logical OR of the flags:

         * `image.AREA`: Use area scaling when downscaling versus the default of nearest neighbor.
         * `image.BILINEAR`: Use bilinear scaling versus the default of nearest neighbor scaling.
         * `image.BICUBIC`: Use bicubic scaling versus the default of nearest neighbor scaling.
         * `image.CENTER`: Center the image being operated on the display. This is applied after scaling.
         * `image.HMIRROR`: Horizontally mirror the source image.
         * `image.VFLIP`: Vertically flip the source image.
         * `image.TRANSPOSE`: Transpose the source image (swap x/y).
         * `image.EXTRACT_RGB_CHANNEL_FIRST`: Do rgb_channel extraction before scaling.
         * `image.APPLY_COLOR_PALETTE_FIRST`: Apply color palette before scaling.
         * `image.SCALE_ASPECT_KEEP`: Scale the source image being operated to fit inside the destination.
         * `image.SCALE_ASPECT_EXPAND`: Scale the source image being operated to fill the destination (results in cropping)
         * `image.SCALE_ASPECT_IGNORE`: Scale the source image being operated to fill the destination (results in stretching).
         * `image.ROTATE_90`: Rotate the source image by 90 degrees (this is just VFLIP | TRANSPOSE).
         * `image.ROTATE_180`: Rotate the source image by 180 degrees (this is just HMIRROR | VFLIP).
         * `image.ROTATE_270`: Rotate the source image by 270 degrees (this is just HMIRROR | TRANSPOSE).
         * `image.BLACK_BACKGROUND`: Assume the destination image being operated on is black speeding up blending.

      ``dssim`` if true will compute the structural disimilarity index (DSSIM) instead of the SSIM. A
      value of 0 means the images are identical. A value of 1 means the images are completely different.

      Returns a `image.Similarity` object.

   .. method:: get_histogram(thresholds:Optional[List[Tuple[int,int]]]=None, invert=False, roi:Optional[Tuple[int,int,int,int]]=None, bins=256, l_bins=256, a_bins=256, b_bins=256, difference:Optional[Image]=None) -> histogram

      Computes the normalized histogram on all color channels for an ``roi`` and
      returns a `image.histogram` object. Please see the `image.histogram` object for more
      information. You can also invoke this method by using ``Image.get_hist()`` or
      ``Image.histogram()``. If you pass a list of ``thresholds`` then the histogram
      information will only be computed from pixels within the threshold list.

      ``thresholds`` must be a list of tuples
      ``[(lo, hi), (lo, hi), ..., (lo, hi)]`` defining the ranges of color you
      want to track. For
      grayscale images each tuple needs to contain two values - a min grayscale
      value and a max grayscale value. Only pixel regions that fall between these
      thresholds will be considered. For RGB565 images each tuple needs to have
      six values (l_lo, l_hi, a_lo, a_hi, b_lo, b_hi) - which are minimums and
      maximums for the LAB L, A, and B channels respectively. For easy usage this
      function will automatically fix swapped min and max values. Additionally,
      if a tuple is larger than six values the rest are ignored. Conversely, if the
      tuple is too short the rest of the thresholds are assumed to be at maximum
      range.

      .. note::

         To get the thresholds for the object you want to track just select (click
         and drag) on the object you want to track in the IDE frame buffer. The
         histogram will then update to just be in that area. Then just write down
         where the color distribution starts and falls off in each histogram channel.
         These will be your low and high values for ``thresholds``. It's best to
         manually determine the thresholds versus using the upper and lower
         quartile statistics because they are too tight.

         You may also determine color thresholds by going into
         ``Tools->Machine Vision->Threshold Editor`` in OpenMV IDE and selecting
         thresholds from the GUI slider window.

      ``invert`` inverts the thresholding operation such that instead of matching
      pixels inside of some known color bounds pixels are matched that are outside
      of the known color bounds.

      Unless you need to do something advanced with color statistics just use the
      `Image.get_statistics()` method instead of this method for looking at pixel
      areas in an image.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``bins`` and others are the number of bins to use for the histogram
      channels. For grayscale images use ``bins`` and for RGB565 images use
      the others for each channel. The bin counts must be greater than 2 for each
      channel. Additionally, it makes no sense to set the bin count larger than
      the number of unique pixel values for each channel. By default, the histogram
      will have the maximum number of bins per channel.

      ``difference`` may be set to an image object to cause this method to operate
      on the difference image between the current image and the ``difference`` image
      object. This saves having to use a separate buffer.

      Not supported on compressed images or bayer images.

   .. method:: get_statistics(thresholds:Optional[List[Tuple[int,int]]]=None, invert=False, roi:Optional[Tuple[int,int,int,int]]=None, bins=256, l_bins=256, a_bins=256, b_bins=256, difference:Optional[Image]=None) -> statistics

      Computes the mean, median, mode, standard deviation, min, max, lower
      quartile, and upper quartile for all color channels for an ``roi`` and
      returns a `image.statistics` object. Please see the `image.statistics`
      object for more information. You can also invoke this method by using
      ``Image.get_stats()`` or ``Image.statistics()``. If you pass a list of
      ``thresholds`` then the histogram information will only be computed from
      pixels within the threshold list.

      ``thresholds`` must be a list of tuples
      ``[(lo, hi), (lo, hi), ..., (lo, hi)]`` defining the ranges of color you
      want to track. For
      grayscale images each tuple needs to contain two values - a min grayscale
      value and a max grayscale value. Only pixel regions that fall between these
      thresholds will be considered. For RGB565 images each tuple needs to have
      six values (l_lo, l_hi, a_lo, a_hi, b_lo, b_hi) - which are minimums and
      maximums for the LAB L, A, and B channels respectively. For easy usage this
      function will automatically fix swapped min and max values. Additionally,
      if a tuple is larger than six values the rest are ignored. Conversely, if the
      tuple is too short the rest of the thresholds are assumed to be at maximum
      range.

      .. note::

         To get the thresholds for the object you want to track just select (click
         and drag) on the object you want to track in the IDE frame buffer. The
         histogram will then update to just be in that area. Then just write down
         where the color distribution starts and falls off in each histogram channel.
         These will be your low and high values for ``thresholds``. It's best to
         manually determine the thresholds versus using the upper and lower
         quartile statistics because they are too tight.

         You may also determine color thresholds by going into
         ``Tools->Machine Vision->Threshold Editor`` in OpenMV IDE and selecting
         thresholds from the GUI slider window.

      ``invert`` inverts the thresholding operation such that instead of matching
      pixels inside of some known color bounds pixels are matched that are outside
      of the known color bounds.

      You'll want to use this method any time you need to get information about
      the values of an area of pixels in an image. For example, after if you're
      trying to detect motion using frame differencing you'll want to use this
      method to determine a change in the color channels of the image to trigger
      your motion detection threshold.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``bins`` and others are the number of bins to use for the histogram
      channels. For grayscale images use ``bins`` and for RGB565 images use
      the others for each channel. The bin counts must be greater than 2 for each
      channel. Additionally, it makes no sense to set the bin count larger than
      the number of unique pixel values for each channel. By default, the histogram
      will have the maximum number of bins per channel.

      ``difference`` may be set to an image object to cause this method to operate
      on the difference image between the current image and the ``difference`` image
      object. This saves having to use a separate buffer.

      Not supported on compressed images or bayer images.

   .. method:: get_regression(thresholds:List[Tuple[int,int]], invert=False, roi:Optional[Tuple[int,int,int,int]]=None, x_stride=2, y_stride=1, area_threshold=10, pixels_threshold=10, robust=False) -> line

      Computes a linear regression on all the thresholded pixels in the image. The
      linear regression is computed using least-squares normally which is fast but
      cannot handle any outliers. If ``robust`` is True then the Theil–Sen linear
      regression is used instead which computes the median of all slopes between
      all thresholded pixels in the image. This is an N^2 operation which may drops
      your FPS down to below 5 even on an 80x60 image if too many pixels are set
      after thresholding. However, as long as the number of pixels set after
      thresholding remains low the linear regression will be valid even in the case
      of up to 30% of the thresholded pixels being outliers (e.g. it's robust).

      This method returns a `image.line` object. See this blog post on how to use the
      line object easily: https://openmv.io/blogs/news/linear-regression-line-following

      ``thresholds`` must be a list of tuples
      ``[(lo, hi), (lo, hi), ..., (lo, hi)]`` defining the ranges of color you
      want to track. For
      grayscale images each tuple needs to contain two values - a min grayscale
      value and a max grayscale value. Only pixel regions that fall between these
      thresholds will be considered. For RGB565 images each tuple needs to have
      six values (l_lo, l_hi, a_lo, a_hi, b_lo, b_hi) - which are minimums and
      maximums for the LAB L, A, and B channels respectively. For easy usage this
      function will automatically fix swapped min and max values. Additionally,
      if a tuple is larger than six values the rest are ignored. Conversely, if the
      tuple is too short the rest of the thresholds are assumed to be at maximum
      range.

      .. note::

         To get the thresholds for the object you want to track just select (click
         and drag) on the object you want to track in the IDE frame buffer. The
         histogram will then update to just be in that area. Then just write down
         where the color distribution starts and falls off in each histogram channel.
         These will be your low and high values for ``thresholds``. It's best to
         manually determine the thresholds versus using the upper and lower
         quartile statistics because they are too tight.

         You may also determine color thresholds by going into
         ``Tools->Machine Vision->Threshold Editor`` in OpenMV IDE and selecting
         thresholds from the GUI slider window.

      ``invert`` inverts the thresholding operation such that instead of matching
      pixels inside of some known color bounds pixels are matched that are outside
      of the known color bounds.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``x_stride`` is the number of x pixels to skip over when evaluating the image.

      ``y_stride`` is the number of y pixels to skip over when evaluating the image.

      If the regression's bounding box area is less than ``area_threshold`` then None is returned.

      If the regression's pixel count is less than ``pixels_threshold`` then None is returned.

      Not supported on compressed images or bayer images.

   Detection Methods
   ~~~~~~~~~~~~~~~~~

   .. method:: find_blobs(thresholds:List[Tuple[int,int]], invert=False, roi:Optional[Tuple[int,int,int,int]]=None, x_stride=2, y_stride=1, area_threshold=10, pixels_threshold=10, merge=False, margin=0, threshold_cb=None, merge_cb=None, x_hist_bins_max=0, y_hist_bins_max=0) -> List[blob]

      Finds all blobs (connected pixel regions that pass a threshold test) in the
      image and returns a list of `image.blob` objects which describe each blob.
      Please see the `image.blob` object more more information.

      ``thresholds`` must be a list of tuples
      ``[(lo, hi), (lo, hi), ..., (lo, hi)]`` defining the ranges of color you
      want to track. You may pass up to 32 threshold tuples in one call. For
      grayscale images each tuple needs to contain two values - a min grayscale
      value and a max grayscale value. Only pixel regions that fall between these
      thresholds will be considered. For RGB565 images each tuple needs to have
      six values (l_lo, l_hi, a_lo, a_hi, b_lo, b_hi) - which are minimums and
      maximums for the LAB L, A, and B channels respectively. For easy usage this
      function will automatically fix swapped min and max values. Additionally,
      if a tuple is larger than six values the rest are ignored. Conversely, if the
      tuple is too short the rest of the thresholds are assumed to be at maximum
      range.

      .. note::

         To get the thresholds for the object you want to track just select (click
         and drag) on the object you want to track in the IDE frame buffer. The
         histogram will then update to just be in that area. Then just write down
         where the color distribution starts and falls off in each histogram channel.
         These will be your low and high values for ``thresholds``. It's best to
         manually determine the thresholds versus using the upper and lower
         quartile statistics because they are too tight.

         You may also determine color thresholds by going into
         ``Tools->Machine Vision->Threshold Editor`` in OpenMV IDE and selecting
         thresholds from the GUI slider window.

      ``invert`` inverts the thresholding operation such that instead of matching
      pixels inside of some known color bounds pixels are matched that are outside
      of the known color bounds.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``x_stride`` is the number of x pixels to skip when searching for a blob.
      Once a blob is found the line fill algorithm will be pixel accurate.
      Increase ``x_stride`` to speed up finding blobs if blobs are known to be large.

      ``y_stride`` is the number of y pixels to skip when searching for a blob.
      Once a blob is found the line fill algorithm will be pixel accurate.
      Increase ``y_stride`` to speed up finding blobs if blobs are known to be large.

      If a blob's bounding box area is less than ``area_threshold`` it is filtered
      out.

      If a blob's pixel count is less than ``pixels_threshold`` it is filtered out.

      ``merge`` if True merges all not filtered out blobs whose bounding
      rectangles intersect each other. ``margin`` can be used to increase or
      decrease the size of the bounding rectangles for blobs during the
      intersection test. For example, with a margin of 1 blobs whose bounding
      rectangles are 1 pixel away from each other will be merged.

      Merging blobs allows you to implement color code tracking. Each blob object
      has a ``code`` value which is a bit vector made up of 1s for each color
      threshold. For example, if you pass `Image.find_blobs` two color
      thresholds then the first threshold has a code of 1 and the second 2 (a
      third threshold would be 4 and a fourth would be 8 and so on). Merged blobs
      logically OR all their codes together so that you know what colors produced
      them. This allows you to then track two colors if you get a blob object
      back with two colors then you know it might be a color code.

      You might also want to merge blobs if you are using tight color bounds which
      do not fully track all the pixels of an object you are trying to follow.

      Finally, if you want to merge blobs, but, don't want two color thresholds to
      be merged then just call `Image.find_blobs` twice with separate thresholds
      so that blobs aren't merged.

      ``threshold_cb`` may be set to the function to call on every blob after its
      been thresholded to filter it from the list of blobs to be merged. The call
      back function will receive one argument - the blob object to be filtered.
      The call back then must return True to keep the blob and False to filter it.

      ``merge_cb`` may be set to the function to call on every two blobs about to
      be merged to prevent or allow the merge. The call back function will receive
      two arguments - the two blob objects to be merged. The call back then must
      return True to merge the blobs or False to prevent merging the blobs.

      ``x_hist_bins_max`` if set to non-zero populates a histogram buffer in each
      blob object with an x_histogram projection of all columns in the object. This
      value then sets the number of bins for that projection.

      ``y_hist_bins_max`` if set to non-zero populates a histogram buffer in each
      blob object with an y_histogram projection of all rows in the object. This
      value then sets the number of bins for that projection.

      Not supported on compressed images or bayer images.

   .. method:: find_lines(roi:Optional[Tuple[int,int,int,int]]=None, x_stride=2, y_stride=1, threshold=1000, theta_margin=25, rho_margin=25) -> List[line]

      Finds all infinite lines in the image using the hough transform. Returns a list
      of `image.line` objects.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``x_stride`` is the number of x pixels to skip when doing the hough transform.
      Only increase this if lines you are searching for are large and bulky.

      ``y_stride`` is the number of y pixels to skip when doing the hough transform.
      Only increase this if lines you are searching for are large and bulky.

      ``threshold`` controls what lines are detected from the hough transform. Only
      lines with a magnitude greater than or equal to ``threshold`` are returned. The
      right value of ``threshold`` for your application is image dependent. Note that
      the magnitude of a line is the sum of all sobel filter magnitudes of pixels
      that make up that line.

      ``theta_margin`` controls the merging of detected lines. Lines which are
      ``theta_margin`` degrees apart and ``rho_margin`` rho apart are merged.

      ``rho_margin`` controls the merging of detected lines. Lines which are
      ``theta_margin`` degrees apart and ``rho_margin`` rho apart are merged.

      This method working by running the sobel filter over the image and taking
      the magnitude and gradient responses from the sobel filter to feed a hough
      transform. It does not require any preprocessing on the image first. However,
      my cleaning up the image using filtering you may get more stable results.

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_line_segments(roi:Optional[Tuple[int,int,int,int]]=None, merge_distance=0, max_theta_difference=15) -> List[line]

      Finds line segments in the image using the hough transform. Returns a list
      of `image.line` objects .

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``merge_distance`` specifies the maximum number of pixels two line segments
      can be separated by each other (at any point on one line) to be merged.

      ``max_theta_difference`` is the maximum theta difference in degrees two line
      segments that are ``merge_distance`` apart to be merged.

      This method uses the LSD library (also used by OpenCV) to find line segments
      in the image. It's somewhat slow but very accurate and lines don't jump around.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_circles(roi:Optional[Tuple[int,int,int,int]]=None, x_stride=2, y_stride=1, threshold=2000, x_margin=10, y_margin=10, r_margin=10, r_min=2, r_max:Optional[int]=None, r_step=2) -> List[circle]

      Finds circles in the image using the hough transform. Returns a list of
      `image.circle` objects.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``x_stride`` is the number of x pixels to skip when doing the hough transform.
      Only increase this if circles you are searching for are large and bulky.

      ``y_stride`` is the number of y pixels to skip when doing the hough transform.
      Only increase this if circles you are searching for are large and bulky.

      ``threshold`` controls what circles are detected from the hough transform. Only
      circles with a magnitude greater than or equal to ``threshold`` are returned. The
      right value of ``threshold`` for your application is image dependent. Note that
      the magnitude of a circle is the sum of all sobel filter magnitudes of pixels
      that make up that circle.

      ``x_margin`` controls the merging of detected circles. Circles which are
      ``x_margin``, ``y_margin``, and ``r_margin`` pixels apart are merged.

      ``y_margin`` controls the merging of detected circles. Circles which are
      ``x_margin``, ``y_margin``, and ``r_margin`` pixels apart are merged.

      ``r_margin`` controls the merging of detected circles. Circles which are
      ``x_margin``, ``y_margin``, and ``r_margin`` pixels apart are merged.

      ``r_min`` controls the minimum circle radius detected. Increase this to speed
      up the algorithm. Defaults to 2.

      ``r_max`` controls the maximum circle radius detected. Decrease this to speed
      up the algorithm. Defaults to min(roi.w/2, roi.h/2).

      ``r_step`` controls how to step the radius detection by. Defaults to 2.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_rects(roi:Optional[Tuple[int,int,int,int]]=None, threshold=10000) -> List[rect]

      Find rectangles in the image using the same quad detection algorithm used to
      find apriltags. Works best of rectangles that have good contrast against the
      background. The apriltag quad detection algorithm can handle any
      scale/rotation/shear on rectangles. Returns a list of `image.rect` objects.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      Rectangles which have an edge magnitude (which is computed by sliding the
      sobel operator across all pixels on the edges of the rectangle and summing
      their values) less than ``threshold`` are filtered out of the returned list.
      The correct value of ``threshold`` is depended on your application/scene.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_qrcodes(roi:Optional[Tuple[int,int,int,int]]=None) -> List[qrcode]

      Finds all qrcodes within the ``roi`` and returns a list of `image.qrcode`
      objects. Please see the `image.qrcode` object for more information.

      QR Codes need to be relatively flat in the image for this method to work.
      You can achieve a flatter image that is not effected by lens distortion by
      either using the `sensor.set_windowing()` function to zoom in the on the
      center of the lens, `Image.lens_corr()` to undo lens barrel distortion, or
      by just changing out the lens for something with a narrower fields of view.
      There are machine vision lenses available which do not cause barrel
      distortion but they are much more expensive to than the standard lenses
      supplied by OpenMV.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_apriltags(roi:Optional[Tuple[int,int,int,int]]=None, families=TAG36H11, fx=0.0, fy=0.0, cx:Optional[int]=None, cy:Optional[int]=None) -> List[apriltag]

      Finds all apriltags within the ``roi`` and returns a list of `image.apriltag`
      objects. Please see the `image.apriltag` object for more information.

      Unlike QR Codes, AprilTags can be detected at much farther distances, worse
      lighting, in warped images, etc. AprilTags are robust too all kinds of
      image distortion issues that QR Codes are not to. That said, AprilTags
      can only encode a numeric ID as their payload.

      AprilTags can also be used for localization purposes. Each `image.apriltag`
      object returns its translation and rotation from the camera. The units
      of the translation are determined by ``fx``, ``fy``, ``cx``, and ``cy``
      which are the focal lengths and center points of the image in the X and
      Y directions respectively.

      .. note::

         To create AprilTags use the tag generator tool built-in to OpenMV IDE.
         The tag generator can create printable 8.5"x11" AprilTags.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``families`` is bitmask of tag families to decode. It is the logical OR of:

        * `image.TAG16H5`
        * `image.TAG25H7`
        * `image.TAG25H9`
        * `image.TAG36H10`
        * `image.TAG36H11`
        * `image.ARTOOLKIT`

      By default it is just `image.TAG36H11` which is the best tag family to
      use. Note that `Image.find_apriltags()` slows down per enabled tag family.

      ``fx`` is the camera X focal length in pixels. For the standard OpenMV Cam
      this is (2.8 / 3.984) * 656. Which is the lens focal length in mm, divided
      by the camera sensor length in the X direction multiplied by the number of
      camera sensor pixels in the X direction (for the OV7725 camera).

      ``fx`` is the camera Y focal length in pixels. For the standard OpenMV Cam
      this is (2.8 / 2.952) * 488. Which is the lens focal length in mm, divided
      by the camera sensor length in the Y direction multiplied by the number of
      camera sensor pixels in the Y direction (for the OV7725 camera).

      ``cx`` is the image center which is just ``image.width()/2``. This is not
      ``roi.w()/2``.

      ``cy`` is the image center which is just ``image.height()/2``. This is not
      ``roi.h()/2``.

      Not supported on compressed images.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_datamatrices(roi:Optional[Tuple[int,int,int,int]]=None, effort=200) -> List[datamatrix]

      Finds all datamatrices within the ``roi`` and returns a list of `image.datamatrix`
      objects. Please see the `image.datamatrix` object for more information.

      Data Matrices need to be relatively flat in the image for this method to work.
      You can achieve a flatter image that is not effected by lens distortion by
      either using the `sensor.set_windowing()` function to zoom in the on the
      center of the lens, `Image.lens_corr()` to undo lens barrel distortion, or
      by just changing out the lens for something with a narrower fields of view.
      There are machine vision lenses available which do not cause barrel
      distortion but they are much more expensive to than the standard lenses
      supplied by OpenMV.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``effort`` controls how much time to spend trying to find data matrix matches.
      The default value of 200 should be good for all use-cases. However, you may
      increase the effort, at a cost of the frame rate, to increase detection. You
      may also lower the effort to increase the frame rate, but, at a cost of
      detections... note that when ``effort`` is set to below 160 or so you will
      not detect anything anymore. Also note that you can make ``effort`` as high
      as you like. But, any values above 240 or so do not result in much increase
      in the detection rate.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_barcodes(roi:Optional[Tuple[int,int,int,int]]=None) -> List[barcode]

      Finds all 1D barcodes within the ``roi`` and returns a list of `image.barcode`
      objects. Please see the `image.barcode` object for more information.

      For best results use a 640 by 40/80/160 window. The lower the vertical res
      the faster everything will run. Since bar codes are linear 1D images you
      just need a lot of resolution in one direction and just a little resolution
      in the other direction. Note that this function scans both horizontally and
      vertically so you can use a 40/80/160 by 480 window if you want. Finally,
      make sure to adjust your lens so that the bar code is positioned where the
      focal length produces the sharpest image. Blurry bar codes can't be decoded.

      This function supports all these 1D barcodes (basically all barcodes):

        * `image.EAN2`
        * `image.EAN5`
        * `image.EAN8`
        * `image.UPCE`
        * `image.ISBN10`
        * `image.UPCA`
        * `image.EAN13`
        * `image.ISBN13`
        * `image.I25`
        * `image.DATABAR` (RSS-14)
        * `image.DATABAR_EXP` (RSS-Expanded)
        * `image.CODABAR`
        * `image.CODE39`
        * `image.PDF417`
        * `image.CODE93`
        * `image.CODE128`

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_displacement(template:Image, roi:Optional[Tuple[int,int,int,int]]=None, template_roi:Optional[Tuple[int,int,int,int]]=None, logpolar=False) -> List[displacement]

      Find the translation offset of the this image from the template. This
      method can be used to do optical flow. This method returns a `image.displacement`
      object with the results of the displacement calculation using phase correlation.

      ``roi`` is the region-of-interest rectangle (x, y, w, h) to work in.
      If not specified, it is equal to the image rectangle.

      ``template_roi`` is the region-of-interest rectangle (x, y, w, h) to work in.
      If not specified, it is equal to the image rectangle.

      ``roi`` and ``template`` roi must have the same w/h but may have any x/y
      location in the image. You may slide smaller rois around a larger image to
      get an optical flow gradient image...

      `Image.find_displacement()` normally computes the x/y translation between two
      images. However, if you pass ``logpolar=True`` it will instead find rotation
      and scale changes between the two images. The same `image.displacement` object
      result encodes both possible responses.

      Not supported on compressed images or bayer images.

      .. note::

         Please use this method on power-of-2 image sizes (e.g. `sensor.B64X64`).

      Not supported on compressed images or bayer images.

      This method is not available on the OpenMV Cam M4.

   .. method:: find_template(template:Image, threshold:float, roi:Optional[Tuple[int,int,int,int]]=None, step=2, search=SEARCH_EX) -> Tuple[int,int,int,int]

      Tries to find the first location in the image where template matches using
      Normalized Cross Correlation. Returns a bounding box tuple (x, y, w, h) for
      the matching location otherwise None.

      ``template`` is a small image object that is matched against this image
      object. Note that both images must be grayscale.

      ``threshold`` is floating point number (0.0-1.0) where a higher threshold
      prevents false positives while lowering the detection rate while a lower
      threshold does the opposite.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``step`` is the number of pixels to skip past while looking for the
      template. Skipping pixels considerably speeds the algorithm up. This only
      affects the algorithm in SERACH_EX mode.

      ``search`` can be either ``image.SEARCH_DS`` or ``image.SEARCH_EX``.
      ``image.SEARCH_DS`` searches for the template using as faster algorithm
      than ``image.SEARCH_EX`` but may not find the template if it's near the
      edges of the image. ``image.SEARCH_EX`` does an exhaustive search for the
      image but can be much slower than ``image.SEARCH_DS``.

      Only works on grayscale images.

   .. method:: find_features(cascade, threshold=0.5, scale=1.5, roi:Optional[Tuple[int,int,int,int]]=None) -> List[Tuple[int,int,int,int]]

      This method searches the image for all areas that match the passed in Haar
      Cascade and returns a list of bounding box rectangles tuples (x, y, w, h)
      around those features. Returns an empty list if no features are found.

      ``cascade`` is a Haar Cascade object. See `image.HaarCascade()` for more
      details.

      ``threshold`` is a threshold (0.0-1.0) where a smaller value increase the
      detection rate while raising the false positive rate. Conversely, a higher
      value decreases the detection rate while lowering the false positive rate.

      ``scale`` is a float that must be greater than 1.0. A higher scale
      factor will run faster but will have much poorer image matches. A good
      value is between 1.35 and 1.5.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

   .. method:: find_eye(roi:Tuple[int,int,int,int]) -> Tuple[int,int]

      Searches for the pupil in a region-of-interest (x, y, w, h) tuple around an
      eye. Returns a tuple with the (x, y) location of the pupil in the image.
      Returns (0,0) if no pupils are found.

      To use this function first use `Image.find_features()` with the
      ``frontalface`` HaarCascade to find someone's face. Then use
      `Image.find_features()` with the ``eye`` HaarCascade to find the eyes on the
      face. Finally, call this method on the eye ROI returned by
      `Image.find_features()` to get the pupil coordinates.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      Only works on grayscale images.

   .. method:: find_lbp(roi:Tuple[int,int,int,int])

      Extracts LBP (local-binary-patterns) keypoints from the region-of-interest
      (x, y, w, h) tuple. You can then use then use the `image.match_descriptor()`
      function to compare two sets of keypoints to get the matching distance.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      Only works on grayscale images.

   .. method:: find_keypoints(roi:Optional[Tuple[int,int,int,int]]=None, threshold=20, normalized=False, scale_factor=1.5, max_keypoints=100, corner_detector=CORNER_AGAST)

      Extracts ORB keypoints from the region-of-interest (x, y, w, h) tuple. You
      can then use then use the `image.match_descriptor()` function to compare
      two sets of keypoints to get the matching areas. Returns None if no
      keypoints were found.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      ``threshold`` is a number (between 0 - 255) which controls the number of
      extracted corners. For the default AGAST corner detector this should be
      around 20. FOr the FAST corner detector this should be around 60-80. The
      lower the threshold the more extracted corners you get.

      ``normalized`` is a boolean value that if True turns off extracting
      keypoints at multiple resolutions. Set this to true if you don't care
      about dealing with scaling issues and want the algorithm to run faster.

      ``scale_factor`` is a float that must be greater than 1.0. A higher scale
      factor will run faster but will have much poorer image matches. A good
      value is between 1.35 and 1.5.

      ``max_keypoints`` is the maximum number of keypoints a keypoint object may
      hold. If keypoint objects are too big and causing out of RAM issues then
      decrease this value.

      ``corner_detector`` is the corner detector algorithm to use which extracts
      keypoints from the image. It can be either `image.CORNER_FAST` or
      `image.CORNER_AGAST`. The FAST corner detector is faster but much less accurate.

      Only works on grayscale images.

   .. method:: find_edges(edge_type, threshold=(100, 200))

      Turns the image to black and white leaving only the edges as white pixels.

         * image.EDGE_SIMPLE - Simple thresholded high pass filter algorithm.
         * image.EDGE_CANNY - Canny edge detection algorithm.

      ``threshold`` is a two valued tuple containing a low threshold and high
      threshold. You can control the quality of edges by adjusting these values.
      It defaults to (100, 200).

      Only works on grayscale images.

   .. method:: find_hog(roi:Optional[Tuple[int,int,int,int]]=None, size=8)

      Replaces the pixels in the ROI with HOG (histogram of orientated graidients)
      lines.

      ``roi`` is the region-of-interest rectangle tuple (x, y, w, h). If not
      specified, it is equal to the image rectangle. Only pixels within the
      ``roi`` are operated on.

      Only works on grayscale images.

      This method is not available on the OpenMV Cam M4.

   .. method:: stero_disparity(reversed:bool=False, max_disparity:int=64, threshold:int=64)

      Takes a double wide grayscale image that contains the output of two camera sensors
      side-by-side and replaces one of the images in the double wide image with the stero-disparity
      image where each pixel represents depth. E.g. if you have two 320x240 cameras then this method
      takes a 640x240 image.

      ``reversed`` By default the left image is compared to the right image and the right image
      is then replaced. Pass true to compare the right image to the left image and replace the left
      image.

      .. note::

         The algorithm only works comparing left->right or right->left. If your camrea setup does
         not match this then you will get useless results.

      ``max_disparity`` is the maximum distance to search for a matching pixel block using the
      sum-of-absolute differences algorithm. Larger values take exponentially longer to search with
      but result in higher quality images. Lower values make the algorithm run faster but may
      result in nonsense output.

      ``threshold`` if the sum-of-absolute differences between two blocks is less than or equal
      to this threshold they are considered to be matching.

      This method is only available on the Arduino Portenta.

      .. note::

         Even with our best SIMD effort this algorithm is not real-time on the Cortex-M7 processor.
         This is just a toy example algorithm showing off stero-disparity.

Constants
---------

.. data:: BINARY
   :type: int

   BINARY (bitmap) pixel format. Each pixel is 1-bit.

.. data:: GRAYSCALE
   :type: int

   GRAYSCALE pixel format. Each pixel is 8-bits, 1-byte.

.. data:: RGB565
   :type: int

   RGB565 pixel format. Each pixel is 16-bits, 2-bytes. 5-bits are used for red,
   6-bits are used for green, and 5-bits are used for blue.

.. data:: BAYER
   :type: int

   RAW BAYER image pixel format. If you try to make the frame size too big
   to fit in the frame buffer your OpenMV Cam will set the pixel format
   to BAYER so that you can capture images but no image processing methods
   will be operational.

.. data:: YUV422
   :type: int

   A pixel format that is very easy to jpeg compress. Each pixel is stored as a grayscale
   8-bit Y value followed by alternating 8-bit U/V color values that are shared between two
   Y values (8-bits Y1, 8-bits U, 8-bits Y2, 8-bits V, etc.). Only some image processing
   methods work with YUV422.

.. data:: JPEG
   :type: int

   A JPEG image.

.. data:: PNG
   :type: int

   A PNG image.

.. data:: PALETTE_RAINBOW
   :type: int

   Default OpenMV Cam color palette for thermal images using a smooth color wheel.

.. data:: PALETTE_IRONBOW
   :type: int

   Makes images look like the FLIR Lepton thermal images using a very non-linear color palette.

.. data:: AREA
   :type: int

   Use area scaling when downscaling an image (Nearest Neighbor is used for upscaling).

   You should use area scaling when downscaling for the highest visual quality.

.. data:: BILINEAR
   :type: int

   Use bilinear scaling when upscaling an image. This produces a good quality scaled image output
   and is fast.

   When downscaling an image this method will subsample the input image to produce the downscaled
   image. Use `image.AREA` for the highest quality downscaling if speed is not an issue.

.. data:: BICUBIC
   :type: int

   Use bicubic scaling when upscaling an image. This produces a high quality scaled image output,
   but is slow.

   When downscaling an image this method will subsample the input image to produce the downscaled
   image. Use `image.AREA` for the highest quality downscaling if speed is not an issue.

.. data:: VFLIP
   :type: int

   Vertically flip the image being drawn by `draw_image`.

.. data:: HMIRROR
   :type: int

   Horizontally mirror the image being drawn by `draw_image`.

.. data:: TRANSPOSE
   :type: int

   Transpose (swap x/y) the image being draw by `draw_image`.

.. data:: CENTER
   :type: int

   Center the image being drawn to the center of the image/canvas it's being drawn on. Any x/y
   offsets passed will move the image being drawn from the center by that amount.

.. data:: EXTRACT_RGB_CHANNEL_FIRST
   :type: int

   When extracting an RGB channel from an RGB image using `draw_image` extract the channel first
   before scaling versus afterwards to prevent any artifacts.

.. data:: APPLY_COLOR_PALETTE_FIRST
   :type: int

   When applying a color lookup table to an image using `draw_image` apply the color look table
   first before scaling versus afterwards to prevent any artifacts.

.. data:: SCALE_ASPECT_KEEP
   :type: int

   Scale the image being drawn to fit inside of the image/canvas being drawn on while maintaining
   the aspect ratio. Unless the image aspect ratios match the image being drawn will not completely
   cover the image/canvas being drawn on. Any x_scale/y_scale values passed will additionally scale
   the scaled image.

.. data:: SCALE_ASPECT_EXPAND
   :type: int

   Scale the image being drawn to fill image/canvas being drawn on while maintaining
   the aspect ratio. Unless the image aspect ratios match the image being drawn will be cropped.
   Any x_scale/y_scale values passed will additionally scale the scaled image.

.. data:: SCALE_ASPECT_IGNORE
   :type: int

   Scale the image being drawn to fill the image/canvas being drawn on. This does not maintain
   the aspect ratio of the image being drawn. Any x_scale/y_scale values passed will additionally
   scale the scaled image.

.. data:: BLACK_BACKGROUND
   :type: int

   Speeds up `draw_image` when drawing on a black destination image when using alpha effects that
   require reading both source and destination pixels. This skips reading the destination pixel.

.. data:: ROTATE_90
   :type: int

   Rotate the image by 90 degrees (this is just `image.VFLIP` ORed with `image.TRANSPOSE`).

.. data:: ROTATE_180
   :type: int

   Rotate the image by 180 degrees (this is just `image.HMIRROR` ORed with `image.VFLIP`).

.. data:: ROTATE_270
   :type: int

   Rotate the image by 270 degrees (this is just `image.HMIRROR` ORed with `image.TRANSPOSE`).

.. data:: JPEG_SUBSAMPLING_AUTO
   :type: int

   Automatically select the best JPEG subsampling based on the image quality parameter.

.. data:: JPEG_SUBSAMPLING_444
   :type: int

   Use 4:4:4 JPEG subsampling.

.. data:: JPEG_SUBSAMPLING_422
   :type: int

   Use 4:2:2 JPEG subsampling. Note, you should force the jpeg subsampling to be 4:2:2 if you are
   streaming video via MJPEG for the best compatibility with third-party video players.

.. data:: JPEG_SUBSAMPLING_420
   :type: int

   Use 4:2:0 JPEG subsampling.

.. data:: SEARCH_EX
   :type: int

   Exhaustive template matching search.

.. data:: SEARCH_DS
   :type: int

   Faster template matching search.

.. data:: EDGE_CANNY
   :type: int

   Use the canny edge detection algorithm for doing edge detection on an image.

.. data:: EDGE_SIMPLE
   :type: int

   Use a simple thresholded high pass filter algorithm for doing edge detection
   on an image.

.. data:: CORNER_FAST
   :type: int

   Faster and less accurate corner detection algorithm for ORB keypoints.

.. data:: CORNER_AGAST
   :type: int

   Slower and more accurate corner detection algorithm for ORB keypoints.

.. data:: TAG16H5
   :type: int

   TAG1H5 tag family bit mask enum. Used for AprilTags.

.. data:: TAG25H7
   :type: int

   TAG25H7 tag family bit mask enum. Used for AprilTags.

.. data:: TAG25H9
   :type: int

   TAG25H9 tag family bit mask enum. Used for AprilTags.

.. data:: TAG36H10
   :type: int

   TAG36H10 tag family bit mask enum. Used for AprilTags.

.. data:: TAG36H11
   :type: int

   TAG36H11 tag family bit mask enum. Used for AprilTags.

.. data:: ARTOOLKIT
   :type: int

   ARTOOLKIT tag family bit mask enum. Used for AprilTags.

.. data:: EAN2
   :type: int

   EAN2 barcode type enum.

.. data:: EAN5
   :type: int

   EAN5 barcode type enum.

.. data:: EAN8
   :type: int

   EAN8 barcode type enum.

.. data:: UPCE
   :type: int

   UPCE barcode type enum.

.. data:: ISBN10
   :type: int

   ISBN10 barcode type enum.

.. data:: UPCA
   :type: int

   UPCA barcode type enum.

.. data:: EAN13
   :type: int

   EAN13 barcode type enum.

.. data:: ISBN13
   :type: int

   ISBN13 barcode type enum.

.. data:: I25
   :type: int

   I25 barcode type enum.

.. data:: DATABAR
   :type: int

   DATABAR barcode type enum.

.. data:: DATABAR_EXP
   :type: int

   DATABAR_EXP barcode type enum.

.. data:: CODABAR
   :type: int

   CODABAR barcode type enum.

.. data:: CODE39
   :type: int

   CODE39 barcode type enum.

.. data:: PDF417
   :type: int

   PDF417 barcode type enum - Future (e.g. doesn't work right now).

.. data:: CODE93
   :type: int

   CODE93 barcode type enum.

.. data:: CODE128
   :type: int

   CODE128 barcode type enum.
