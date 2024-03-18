Releasing your OpenMV Cam Scripts for Production
================================================

Once you've got your OpenMV Cam Scripts working like you want and you are ready to ship your
product powered by OpenMV you need to freeze your scripts into the firmware. To do this you will
need to compile our firmware. Luckily, thanks to GitHub actions you can do this in the cloud
without having to install our build system.

For power users, we recommend setting up our firmware development environment so you can build the
firmware from the source. See how to do so `here <https://github.com/openmv/openmv/blob/master/src/README.md>`_.

For everyone else, you can use GitHub actions. To do so, follow the below steps:

#. Get a GitHub account if you don't already have one.
#. Fork the OpenMV firmware `repo <https://github.com/openmv/openmv>`_.
#. Enable GitHub actions on your fork.

After doing this, you can then edit files online in GitHub and your fork will automatically build a
new firmware image for your OpenMV Cam.

How to freeze your scripts
--------------------------

The process of freezing scripts into the firmware ensures that your code cannot be edited by your
end-users. Best of all, it means that your code will run without needing any files on the internal
flash and/or SD card. Additionally, any files on the internal flash or SD card will be overridden
by what scripts you freeze that have the same name.

Anyway, to freeze scripts you simply need to do the following:

#. Add your custom scripts to the `libraries folder <https://github.com/openmv/openmv/tree/master/scripts/libraries>`_.
#. Rename your ``main.py`` script to ``boot.py`` so that it runs first before anything else.
#. Determine which `board folder <https://github.com/openmv/openmv/tree/master/src/omv/boards>`_
   corresponds to your OpenMV Cam.

      * OPENMV2 -> OpenMV Cam M4.
      * OPENMV3 -> OpenMV Cam M7.
      * OPENMV4 -> OpenMV Cam H7.
      * OPENMV4P -> OpenMV Cam H7 Plus.
      * Everything else should be straight forwards.

#. Edit the ``manifest.py`` file for your board type to include your scripts. If you just have
   one script then this should at minimum include you adding ``freeze ("$(OMV_LIB_DIR)/", "boot.py")``
   to the ``manifest.py`` file.
#. Compile the code and generate the firmware image for your OpenMV Cam.

Once you flash this new firmware image to your OpenMV Cam it will automatically start running
your script before doing anything else. No files on the internal flash or attached SD card will
be looked at before running your code. That said, your script can still determine if an SD card
is attached or not and read/write files. So, if you still want to be able to execute generic
scripts from the flash and/or sd card you can do so.

How to protect your code
------------------------

Once you freeze your scripts into the firmware your code can only be extracted by tools that can
dump your OpenMV Cam's program flash (or if you code in a back door vulnerability). All modern MCUs
feature ways for developers to protect their application from being read using debugging tools.
Right now, OpenMV does not offer a generic way for you to do this. We leave this up to you. However,
rest assured. If you really need to prevent anyone from seeing your scripts it's absolutely possible.


