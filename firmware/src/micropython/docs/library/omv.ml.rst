:mod:`ml` --- Machine Learning
==============================

.. module:: ml
   :synopsis: Machine Learning

The `ml` module contains functionality for processing machine learning models on the OpenMV Cam.

The heart of the `ml` module is the `Model()` object which is used to load and execute
TensorFlow Lite models. The `Model()` object accepts a list of up to 4D input tensors for
each model input tensor and returns a list of up to 4D output tensors for each model output
tensor. Each input/output tensor works using a numpy ``ndarray``.

For TensorFlow Lite models, the `Model()` object handles all ops enabled
`here <https://github.com/openmv/openmv/blob/master/src/lib/tflm/tflm_backend.cc>`_. The `Model()`
object will automatically leverage CMSIS-NN, Helium, and an Ethos NPU if available to speed up
inference. Availability of these accelerators is dependent on the OpenMV Cam model.

For image processing support the `ml` module automatically converts passed image objects to numpy
``ndarray`` objects by wrapping them with the `Normalization()` object which handles this conversion. The
`Normalization()` object can also be manually created to control the conversion process, select an
ROI, and etc.

For more information on ``ndarray`` objects see the
`ulab documentation <https://micropython-ulab.readthedocs.io/en/latest/>`_. All OpenMV Cams support
ndarray objects up to rank 4 (meaning 4D tensors).

.. note::

   Complex number support and the `scipy special module <https://micropython-ulab.readthedocs.io/en/latest/scipy-special.html>`_
   are currently disabled on all OpenMV Cams at the moment to save flash space.

Sub Modules
-----------

.. toctree::
    :maxdepth: 2

    omv.ml.apps.rst
    omv.ml.preprocessing.rst
    omv.ml.utils.rst

class model -- Model Container
------------------------------

A model object is used to load and execute TensorFlow Lite models. The model object accepts a list
of up to 4D input tensors per model corresponding to the number of tensor inputs of the model
and returns a list of up to 4D output tensors corresponding to the number of tensor outputs of the
model. Each input/output tensor is an numpy ``ndarray``.

Constructors
~~~~~~~~~~~~

.. class:: Model(path:str, load_to_fb:bool=False) -> Model

   Loads a model from ``path`` into memory and prepares it for being executed. ``path`` can either
   be a file on disk or the name of a built-in model which will be loaded from internal flash. Models that are
   built-in to the internal flash firmware image do not take up RAM to store the model weights when used.

   If the model you are trying to load is very large and doesn't fit in the MicroPython heap you
   can set ``load_to_fb`` to True to load the model into the frame buffer stack instead. This allows
   you to get around the heap size limitations. However, models loaded this way need to be deallocated
   in-order with anything else that uses the frame buffer stack versus the MicroPython heap. Typically,
   the frame buffer stack is much larger than the MicroPython heap so you can load much larger models
   using this option, but, you need to be careful if you deallocate.

   Once a model is loaded you can execute it multiple times with different inputs using `predict()`.
   The model will remember its internal state between calls to `predict()`.

   When deleted the model will automatically free up any memory it used from the heap or frame buffer stack.

   Methods
   ~~~~~~~

   .. method:: predict(inputs:list, callback=None) -> list

      Executes the model with the given inputs. The inputs should be a list of numpy ``ndarray`` objects corresponding
      to the number of input tensors the model supports. The method returns a list of numpy ``ndarray`` objects
      corresponding to the number of output tensors the model has.

      The model input tensors can be up to 4D tensors of uint8, int8, uint16, int16, or float32 values. The passed
      numpy ``ndarray`` for an input tensor is then converted to floating point and scaled/offset based on
      the input tensor's scale and zero point values before being passed to the model. For example, an ``ndarray``
      of uint8 values will be converted to float32s between 0.0-255.0, divided by the input tensor's scale, and
      then have the input tensor's zero point added to it. The same process is done for int8, uint16, and int16 values
      whereas float32 values are passed directly to the model ignoring the scale and zero point values.

      The model's output tensors can be up to 4D tensors of uint8, int8, uint16, int16, or float32 values. For uint8,
      int8, uint16, and int16 tensors the returned numpy ndarray is created by subtracting the output tensor's zero
      point value before multiplying by the output tensor's scale value. For float32 tensors, values are
      passed directly to the output without any scaling or offset being applied.

      Note that `predict()` requires the shape of the input ``ndarray`` objects to match the shape of the model
      input tensors exactly. You can use the ``reshape()`` method of an ndarray with the `input_shape`
      attribute of the model to reshape the input data to the correct shape if necessary.

      If a ``callback`` is passed then it will receive the `Model`, ``inputs``, and ``outputs`` as arguments
      which allows for custom post-processing of the model outputs. The callback may then return
      whatever it likes which will be returned by `predict()`. The ``callback`` method allows for building
      up a library of post-processing functions that can be used on demand for different models.

      For custom pre-processing, `predict()` also accepts "callable" objects as inputs. Any object
      implementing the ``__call__`` method can be passed to `predict()` as an input. `predict()` will
      then call the object with a writeable bytearray representing the input tensor, the input tensor's shape tuple,
      and the input tensors data type value (as an int). The object should then set the input tensor data in the
      bytearray to what the model expects. This is how `Normalization()` converts image objects to input tensors.

   Attributes
   ~~~~~~~~~~

   .. attribute:: len
      :type: int

      The size of the loaded model in bytes.

   .. attribute:: ram
      :type: int
         
      The amount of RAM used by the model for it's tensor arena.

   .. attribute:: input_shape
      :type: list[tuple[int, ...]]

      A list of tuples containing the shape of each input tensor.

   .. attribute:: input_dtype
      :type: list[str]

      A list of strings containing the data type of each input tensor.
      'B', 'b', 'H', 'h', and 'f' respectively for uint8, int8, uint16, int16, and float32.

   .. attribute:: input_scale
      :type: list[float]
         
      A list of floats containing the scale of each input tensor.

   .. attribute:: input_zero_point
      :type: list[int]

      A list of integers containing the zero point of each input tensor.

   .. attribute:: output_shape
      :type: list[tuple[int, ...]]

      A list of tuples containing the shape of each output tensor.

   .. attribute:: output_dtype
      :type: list[str]
         
      A list of strings containing the data type of each output tensor.
      'b', 'B' and 'f' respectively for uint8, int8 and float32.

   .. attribute:: output_scale
      :type: list[float]
         
      A list of floats containing the scale of each output tensor.

   .. attribute:: output_zero_point
      :type: list[int]
         
      A list of integers containing the zero point of each output tensor.

   .. attribute:: labels
      :type: list[str]
         
      A list of strings containing the labels for the model (if it was built-in to the firmware with labels,
      otherwise, ``None``).
