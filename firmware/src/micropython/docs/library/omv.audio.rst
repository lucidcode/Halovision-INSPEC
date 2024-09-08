:mod:`audio` --- Audio Module
=============================

.. module:: audio
   :synopsis: Get audio samples.

The ``audio`` module is used to record audio samples from a microphone on the Arduino Portenta or the Arduino Nicla.

Functions
---------

.. function:: init(channels:int=2, frequency:int=16000, gain_db:float=24, highpass:float=0.9883, samples:int=-1) -> None

   Initializes the audio module. Must be called first before using the audio module.

   ``channels`` specifies the number of audio channels. May be 1 or 2. Audio samples are
   interleaved for two audio channels. Using more than one channel is only possible on boards
   with more than one mic.

   ``frequency`` is the sample frequency to run at. Running at a higher sample frequency results
   in a higher noise floor which means less effective bits per sample.

   ``gain_db`` is the microphone gain to apply.

   ``highpass`` is the high pass filter cut-off given the target sample frequency. This parameter
   is applicable for the Arduino Portenta H7 only.

   ``samples`` is the number of samples to accumulate per callback. This is typically calculated
   based on the decimation factor and number of channels. If set to -1, the number of samples
   will be calculated automatically based on the decimation factor and number of channels.

.. function:: start_streaming(callback) -> None

   Calls the ``callback`` that takes one argument ``pcmbuf`` automatically forever when enough
   PCM samples have accumulated based on the `audio` module settings. You can cast the ``pcmbuf``
   into an ``ndarray`` for processing the audio samples in numpy and then pass the ``ndarray``
   to a `ml.Model` object for inference.

   ``pcmbuf`` is a signed 16-bit array of audio samples who's sized based on the decimation factor
   and number of channels, or the number of samples specified in the `audio.init()` function.

   In single channel mode audio samples will be 16-bits each filling up the 16-bit array.

   In dual channel mode audio samples will be 16-bits each in pairs filling up the 16-bit array.

.. function:: stop_streaming() -> None

   Stops audio streaming and the callback from being called.
