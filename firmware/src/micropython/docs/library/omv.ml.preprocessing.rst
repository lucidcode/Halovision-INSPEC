.. currentmodule:: ml.preprocessing

:mod:`ml.preprocessing` --- ML Preprocessing
============================================

.. module:: ml.preprocessing
    :synopsis: ML Preprocessing

The `ml.preprocessing` module contains classes for preprocessing images for use with machine learning models.

.. _preprocessing.Normalization:

class Normalization -- Image Normalization
------------------------------------------

The `Normalization` object is used to convert image objects to numpy ``ndarray`` objects for use with the `Model` object.
It's automatically created by the `Model` object when an image object is passed to `predict()`. However,
you can also manually create a `Normalization` object to control the conversion process, select an ROI, etc.

For example::

    model = ml.Model("model.tflite")
    norm = ml.Normalization(scale=(0.0, 1.0), mean=(0.485, 0.456, 0.406), stdev=(0.229, 0.224, 0.225))
    outputs = model.predict([norm(image)])

Constructors
~~~~~~~~~~~~

.. class:: Normalization(scale:tuple[float, float]=(0.0, 1.0), mean:tuple[float, float, float]=(0.0, 0.0, 0.0), stdev:tuple[float, float, float]=(1.0, 1.0, 1.0), roi:tuple[int,int,int,int]=None) -> Normalization

   Creates a `Normalization` object which is used to convert image objects to numpy arrays for use with the
   `predict()`. The object can also be used to select a region of interest (ROI) in the image to
   convert to a numpy array.

   The Normalization object automatically converts any image type passed (including compressed images)
   into either a single channel (grayscale) or three channel (RGB888) image which is passed to the tensor
   input of the model. Images are centered, scaled up/down (using bilinear/area scaling), and cropped as
   necessary to match the input tensor size of the model.

   For ``uint8`` input tensors the image is directly passed ignoring scale and mean/stdev. For ``int8``
   input tensors the image is shifted to be within the ``int8`` range from the ``uint8`` range and
   then directly passed ignoring scale and mean/stdev. Tensors that accept either of these formats
   can be processed more quickly than tensors that require floating point inputs.

   For floating point input tensors it's not possible to guess the correct range that the model
   expects. While each input tensor encodes a scale and zero point value that can be used to
   convert the input to the correct range, these values do not tell you what the range
   of the input data should be in floating point. E.g. should image RGB values be within the range
   of (0.0, 1.0), (-1.0, 1.0), (0.0, 255.0), and etc. before applying a scale and zero point? The
   answer is that it depends on the model and how it was trained. So, the normalization object instead
   allows you to directly specify the range of the input data, the mean, and the standard deviation. The
   Grayscale or RGB88 image is then converted into a floating point tensor for the model to process
   based on these values.

   uint16 and int16 input tensors are not supported.
