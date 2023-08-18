Introduction
============

.. image:: https://img.shields.io/discord/327254708534116352.svg
    :target: https://discord.gg/nBQh6qu
    :alt: Discord

.. image:: https://travis-ci.org/adafruit/samd-peripherals.svg?branch=master
    :target: https://travis-ci.org/adafruit/samd-peripherals
    :alt: Build Status

A thin unifying API atop the peripherals in the Broadcom microprocessors used in the Raspberry Pi boards.
It supports the BCM2711 currently.

Branches
=========

This repo has two primary branches:

* `main` - Maintained source files.
* `main-build` - auto-updated branch with generated files. Useful for submoduling the generated files into your own repo.

Using
======
First, add the broadcom-peripherals repo as a submodule within your own repo. For example:

.. code-block::

    git submodule add -b built https://github.com/adafruit/broadcom-peripherals.git peripherals

This will place the files from the repo in a peripherals directory. When your repo is checked out
or updated from before people will need to:

.. code-block::

    git submodule update --init --recursive

The header files in `broadcom` define the common API between the two series. Logic with most code shared
lives in a corresponding .c file. Includes are relative to the
top of the repo so make sure to add the location of the library to your includes path with something
like:

.. code-block::

    -Iperipherals/

In your Makefile create a variable which stores the series such as `CHIP_FAMILY` and alter the
source files depending on it. For example:


.. code-block::

    SRC_C = \
        peripherals/samd/clocks.c \
        peripherals/samd/dma.c \
        peripherals/samd/events.c \
        peripherals/samd/external_interrupts.c \
        peripherals/samd/sercom.c \
        peripherals/samd/timers.c \
        peripherals/samd/$(CHIP_FAMILY)/adc.c \
        peripherals/$(CHIP_FAMILY)/cache.c

Building
=========

To build the generated svd files you'll need some Python dependencies. To install them do:

.. code-block::

    pip install -r requirements.txt

Contributing
============

Contributions are welcome! Please read our `Code of Conduct
<https://github.com/adafruit/samd-peripherals/blob/master/CODE_OF_CONDUCT.md>`_
before contributing to help this project stay welcoming.
