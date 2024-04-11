.. _index:

Welcome to delta dfu – differential update based on MCUboot
############################################################

Delta dfu is an update that requires downloading only the parts of the application firmware that have changed, instead of downloading the whole firmware. Such updates can save significant amounts of time and bandwidth; On the other hand , it can improve the usability of flash since it is not necessary to  reserve half of the flash as the upgrade backup area, it only need a small flash size to save patch image.

Delta upgrade is know as differential compression. Differential Compression is the process of creating and applying delta patch.  The ideal patch is minimal in space complexity and has high usability, so the patch file must be easy to compress; When application applying, we need to decompress the patch first, then combine the source image to generate a new image. This implies that finding an algorithm for differential compression includes diff algorithm, compress/decompress algorithm.

This documentation gives a brief instructions of delta dfu and provides instructions on how to correctly setup it in  `nRF Connect SDK`_.

.. toctree::
   :maxdepth: 1
   :glob:
   :caption: Subpages:

   delta_dfu_introduction.rst
   solutions.rst
   environment_setup.rst
   testing.rst
   performance.rst
   additional_resources.rst

.. _nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/index.html

