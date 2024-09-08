.. currentmodule:: ml.utils

:mod:`ml.utils` --- ML Utils
============================

.. module:: ml.utils
    :synopsis: ML Utils

The `ml.utils` module contains utility classes and functions for machine learning.

.. _utils.NMS:

class NMS - Soft-Non-Maximum Suppression
----------------------------------------

The `NMS` object is used to collect a list of bounding boxes and their associated scores and then filter
out overlapping bounding boxes with lower scores. Additionally, it remaps bounding boxes detected
in a sub-window back to the original image coordinates.

Constructors
~~~~~~~~~~~~

.. class:: NMS(window_w:int, window_h:int, roi:tuple[int,int,int,int]) -> NMS

   Creates a `NMS` object with the given window size and region of interest (ROI). The window is
   width/height of the input tensor of image model. The ROI is the region of interest that returned by the
   `Normalization()` object which corresponds to the region of the image that the model was run on.
   This allows the `NMS` object to remap bounding boxes detected in a sub-window back to the original
   image coordinates.

   Methods
   ~~~~~~~

   .. method:: add_bounding_boxes(xmin:float, ymin:float, xmax:float, ymax:float, score:float, label_index:int) -> None

      Adds a bounding box to the `NMS` object with the given coordinates, score, and label index.

      ``xmin``, ``ymin``, ``xmax``, and ``ymax`` are the bounding box coordinates in the range of 0.0 to 1.0
      where (0.0, 0.0) is the top-left corner of the image and (1.0, 1.0) is the bottom-right corner of the image.

      ``score`` is the confidence score of the bounding box (0.0-1.0).

      ``label_index`` is the index of the label associated with the bounding box.

   .. method:: get_bounding_boxes(threshold:float=0.1, sigma:float=0.1) -> list[tuple[int,int,int,int,float,int]]
         
      Returns a list of bounding boxes that have been filtered by the `NMS` object and remapped
      to the original image coordinates. Bounding box tuples are
      ``(x, y, w, h, score, label_index)``. After calling this method you should create a new
      `NMS` object if you want to process a new set of bounding boxes.

      Bounding boxes must have a higher score then ``threshold`` to be kept.

      ``sigma`` controls the gaussian used to apply a score penalty to overlapping bounding boxes
      using the Soft-Non-Maximum-Suppression algorithm. A higher ``sigma`` will result in a more
      aggressive suppression of overlapping bounding boxes.
