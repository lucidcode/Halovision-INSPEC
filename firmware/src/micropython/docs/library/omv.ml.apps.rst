.. currentmodule:: ml.apps

:mod:`ml.apps` --- ML Apps
==========================

.. module:: ml.apps
    :synopsis: ML Apps

The `ml.apps` module contains various ML application classes.

.. _apps.MicroSpeech:

class MicroSpeech -- Speech Recognition
---------------------------------------

The MicroSpeech object is used to recognize simple spoken words using the MicroSpeech model from
TensorFlow Lite for Microcontrollers. The model recognizes the words "yes" and "no" by default.

You can customize the model to recognize other words by training a new model. See the
`Micro Speech <https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech>`__
guide.

Constructors
~~~~~~~~~~~~

.. class:: MicroSpeech(preprocessor:str=None, micro_speech:str=None, labels:list[str, ...]=None) -> MicroSpeech

    Creates a MicroSpeech object. If no preprocessor is provided, the default preprocessor is used.
    If no micro_speech model is provided, the default model is used. If no labels are provided, the
    default labels are used from the default model.

    Methods
    ~~~~~~~

    .. method:: listen(timeout:int=0, callback=None, threshold:float=0.65, filter:list[str, ...]=["Yes", "No"]) -> tuple[str, float]

       Listens for a spoken word and returns the word and confidence level as a tuple if the
       confidence level is above the threshold and the word is in the filter list.

       ``timeout`` is the maximum time in milliseconds to listen for a word. If zero, the method
       will listen indefinitely until a word is recognized. If -1 is passed, the method will not
       block and will return immediately with the result tuple which may contain ``None`` if no
       word is recognized. If a positive value is passed, the method will listen for that amount
       of time in milliseconds and then return the result tuple.

       ``callback`` is a function that will be called with the word and confidence level instead
       of returning the result. When combined with a timeout of zero, this allows you to listen
       for words indefinitely and process them as they are recognized.

       ``threshold`` is the minimum confidence level required to return a result.

       ``filter`` is a list of words that the model should recognize. If the recognized word is
       not in the filter list, the result is ignored.
