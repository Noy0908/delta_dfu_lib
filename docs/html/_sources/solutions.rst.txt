.. _Solutions:

Solutions
############

In our project we selected
`detools <https://github.com/eerimoq/detools>`__ patching in combination
with heatshrink compression/decompression which is highly portable to
Zephyr, just need a little modify when port it.
we create patch with a python script , it calls
`detools <https://github.com/eerimoq/detools>`__ command to generate a
patch between source image and target image, this step is completed on
your PC. The patch created by the
`detools <https://github.com/eerimoq/detools>`__ differencing algorithm
is a sequential patch. Consequently, the patching algorithm consists of
the looping of several stages. these are: reading a chunk (512 byte),
getting the size of the diff, applying the diff, getting the size of the
extra, applying the extra, and adjusting the position of the buffer
reading from the source image. However, it does also require that the
source image can not be moved during run-time, but we only have one
memory regions to save the application firmware, so we must find out the
source image sections which will be used in the later procedure and save
them in a backup area first , then we start the real apply process.

--------------

.. -boot:

boot
=========

The delta dfu can support a lot of bootloders, it's easy to port to
other platforms as it is written in C language. In order to keep
coinsistent with NCS platform, I port it to the mcuboot . As you know
that mcuboot can not support two different slot size, but now differnt
slot size can be allocated with delta dfu. I suggest to allocate a large
flash space to the primary slot which used to save the application
image, and allocate a smaller flash to secondary slot to save the patch
image and the backup image during applying .

--------------

.. -images-location:

Images location
====================

At first glance it might seems as only one option exists when it comes
to the location of the source image - the primary partition. As the
secondary image usually contains a version of the firmware - the version
of the firmware that was replaced during the last firmware upgrade. But
limited by the flash size, I decided to place the patch image to the
secondary slot instead of a whole firmware. This will have a risk that
if the delta upgrade failed, then device can not get a valid image, it
will be get stuck forever except reflash it, So we have to make sure
that everything is safe.

   +------------------------------------------------+
   | Boot(mcuboot)                                  |
   +================================================+
   | **primary slot(source image)**                 |
   +------------------------------------------------+
   | **secondary slot(patch image + backup space)** |
   +------------------------------------------------+
   | **settings storage**                           |
   +------------------------------------------------+

.. -security:

Security
=============

Firmware delta updates implement security features to ensure that only
firmware updates from authorized images. Delta update files are
encrypted and signed. The boot takes care of verifying the signature and
decrypting the content as necessary, and it also supports several
fail-safe mechanisms to ensure that the MCU continues to operate
normally in case of errors.

-  If the MCU detects issues with the new firmware before commencing the
   update process at boot time, the MCU automatically aborts the update
   process and runs the previous firmware when it boots again.

-  If the device suddenly loses power during the transfer of the new
   firmware, the application can resume the operation when power is
   restored and the modem boots again.

-  If the device suddenly loses power during the update process at boot
   time, the MCU resumes the operation automatically when it boots
   again.
