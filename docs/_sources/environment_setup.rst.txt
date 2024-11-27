.. _environment_setup:

Requirements
############

This page outlines the requirements that you need to meet before you
start working with the delta dfu boot.
Once completed, you will be able to run and test the differential
upgrade with Nordic nRF52 series, nRF91 series and nRF54L series.


.. _Hardware-requirements:

Hardware requirements
*********************

To meet the hardware requirements, ensure you have one Nordic's boards
from the list of three supported models:

 * `nRF54L15 DK`_

 * `nRF9160 DK`_

 * `nRF52840 DK`_

To start working with the devices, refer to the following guidelines:

 * `Getting started with nRF91 Series`_

 * `Getting started with other DKs`_


.. _Software-requirements:

Software requirements
**********************

To meet the software requirements, install `nRF Connect SDK`_ and `nRF Command Line Tools`_.
Currently we support three NCS SDK versions:

   * nRF Connect SDK v2.1.0

   * nRF Connect SDK v2.5.0

   * nRF Connect SDK v2.6.0

   * nRF Connect SDK v2.8.0


.. _nRF-Connect-SDK:

nRF Connect SDK
===============

Perform the following steps to install `nRF Connect SDK`_ and setup
boards:

#. Set up your development environment by the installation method below:

   * Follow `Installing automatically`_ guildelines to perform an automatic installation through the Toolchain Manager.

   .. note::
      For additional information on setting up the device as well as Nordic’s development environment and tools, see the `nRF Connect SDK Getting started guide`_.


#. go to the nrf directory of your SDK, and fetch the origin update.

   #. go to the nrf directory of your SDK, and fetch the origin update.

   .. code-block:: console

      $ git fetch origin

#. checkout to the main branch.

   .. code-block:: console

      $ git checkout main

#. pull the latest update to main branch.

   .. code-block:: console

      $ west update


.. _nRF-Command-Line-Tools:

nRF Command Line Tools
======================

Download the nRF Command Line from the `nRF Command Line Tools`_ page.



.. _nRF54L15 DK: https://www.nordicsemi.com/Products/Development-hardware/nRF54L15-DK
.. _nRF9160 DK: https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk  
.. _nRF52840 DK: https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dk
.. _nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html
.. _nRF Command Line Tools: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs
.. _Getting started with nRF91 Series: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/gsg_guides/nrf9160_gs.html
.. _Getting started with other DKs: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/gsg_guides/gsg_other.html
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.3.0/nrf/getting_started.html
.. _Installing automatically: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html
