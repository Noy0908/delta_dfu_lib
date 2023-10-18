## Introduction  

This docs demonstrate an easy way to understand the delta dfu theory . Delta dfu is an update that requires downloading only the parts of the application firmware that have changed, instead of downloading the whole firmware. Such updates can save significant amounts of time and bandwidth; On the other hand , it can improve the usability of flash since it is not necessary to  reserve half of the flash as the upgrade backup area, it only need a small flash size to save patch image.



### 1. Theory

___

Delta upgrade is know as differential compression. Differential Compression is the process of creating and applying delta patch.  The ideal patch is minimal in space complexity and has high usability, so the patch file must be easy to compress; When application applying, we need to decompress the patch first, then combine the source image to generate a new image. This implies that finding an algorithm for differential compression includes diff algorithm, compress/decompress algorithm.

#### 1.1. Algorithms

___

A diff algorithm outputs the set of differences between two inputs which is called *patch* or *delta*, it opens the door to using compression algorithms to generate a diff and delta file which can greatly reduced size of the delta file.  Data compression is a process used to encode information using fewer bits than the origina representation. It comes in the forms of lossy and lossless compression. Lossless data compression means that the compressed data will contain the same information as the uncompressed version, which enables the compression to be reversed. For delta updates, this is obviously needed, as the point of the process is to recreate the target. 

____

#### 1.2. BSDiff

The [BSDiff](http://www.daemonology.net/bsdiff/?ref=hackernoon.com) algorithm belongs to the block move family and is focused on achieving minimal delta/patch size. It is also specifically optimized for executable files. A major advantage of the  [BSDiff](http://www.daemonology.net/bsdiff/?ref=hackernoon.com) algorithm is that it offers a solution to the pointer problem, which is a commonly occurring problem when creating binary patches. The pointer problem arises from the nature of the executable file. Small, one-line, source code adjustments to the code or data changes the relative positions of blocks, which makes all pointers jumping over the modified regions outdated, if not dealt with, it will creates unnecessarily large patches. BSDiff exploits two facts to solve the pointer problem. Firstly, that the changes outside the modified region generally will be rather sparse, and mostly concern the least significant one or two byte. Secondly, that data and code usually moves around in blocks, which leads to a large number of nearby pointers having to be adjusted by the same amount. This information can be used to deduce that the byte-wise differences between old binary and the new are highly compressible. 

___

#### 1.3. heat-shrink

[heat-shrink](https://github.com/atomicobject/heatshrink) is an excellent compression/decompression algorithm with the following advantages:

- **Low memory usage (as low as 50 bytes)** It is useful for some cases with less than 50 bytes, and useful for many general cases with < 300 bytes.
- **Incremental, bounded CPU use** You can chew on input data in arbitrarily tiny bites. This is a useful property in hard real-time environments.
- **Can use either static or dynamic memory allocation** The library doesn't impose any constraints on memory management.
- **ISC license** You can use it freely, even for commercial purposes.

Our project port [heat-shrink](https://github.com/atomicobject/heatshrink)  algorithm to compress/decompress the patch. it is based on [LZSS](http://en.wikipedia.org/wiki/Lempel-Ziv-Storer-Szymanski) .LZSS is a dictionary coding technique, which means it replaces a string with a reference to a dictionary location of the same string. The idea is to record the locations of previously unseen sub-strings. Sub-strings which, once seen in the text again, are replaced by a pointer to the first occurrence of this sub-string  

___

#### 1.4. Patching

In the same manner that the creation of a patch is related to data compression, the application of a patch is related to decompression. The patch created by the [detools](https://github.com/eerimoq/detools) differencing algorithm is a sequential patch, and thus has a repeating layout . Sequential patching uses two memory regions, one containing the source and one containing the target, but in our implement, target is a patch file instead of a whole firmware. In general each sequence consists of the parts: *diff*, *extra*, and *adjustment*, which together make up the instructions for a small section of the target image. Sequential patching is hence a loop which repeats the same three steps until the patch is fully applied.  

___



### 2. Solutions

> In our project  we selected [detools](https://github.com/eerimoq/detools) patching in combination with heatshrink compression/decompression which is highly portable to Zephyr, just need a little modify  when port it. 
>
> we create patch with a python script , it calls [detools](https://github.com/eerimoq/detools) command to generate a patch between source image and target image, this step is completed on your PC. The patch created by the [detools](https://github.com/eerimoq/detools) differencing algorithm is a sequential patch. Consequently, the patching algorithm consists of the looping of several stages. these are: reading a chunk (512 byte), getting the size of the diff, applying the diff, getting the size of the extra, applying the extra, and adjusting the position of the buffer reading from the source image.  However, it does also require that the source image can not be moved during run-time, but we only have one memory regions to save the application firmware, so we must find out the source image sections which will be used in the later procedure and save them in a backup area first , then we start the real apply process. 

___



#### 2.1. boot

The delta dfu can support a lot of bootloders, it's easy to port to other platforms as it is written in C language. In order to keep coinsistent with NCS platform, I port it to the mcuboot .  As you know that mcuboot can not support two different slot size, but now differnt slot size can be allocated with delta dfu. I suggest to allocate a  large flash space to the primary slot which used to save the application image, and allocate a smaller flash to secondary slot to save the patch image and the backup image during applying .

___

#### 2.2. Images location

At first glance it might seems as only one option exists when it comes to the location of the source image - the primary partition. As the secondary image usually contains a version of the firmware - the version of the firmware that was replaced during the last firmware upgrade. But  limited by the flash size,  I decided to place the patch image to the secondary slot instead of a whole firmware. This will have a risk that if the delta upgrade failed, then device can not get a valid image, it will be get stuck forever except reflash it, So we have to make sure that everything is safe.

>|                 Boot(mcuboot)                  |
>| :--------------------------------------------: |
>|         **primary slot(source image)**         |
>| **secondary slot(patch image + backup space)** |
>|              **settings storage**              |
>
>

#### 2.3. Security

Firmware delta updates implement security features to ensure that only firmware updates from authorized images. Delta update files are encrypted and signed. The boot takes care of verifying the signature and decrypting the content as necessary, and it also supports  several fail-safe mechanisms to ensure that the MCU continues to operate normally in case of errors.

- If the MCU detects issues with the new firmware before commencing the update process at boot time, the MCU automatically aborts the update process and runs the previous firmware when it boots again.
- If the device suddenly loses power during the transfer of the new firmware, the application can resume the operation when power is restored and the modem boots again.
- If the device suddenly loses power during the update process at boot time, the MCU resumes the operation automatically when it boots again.

___

  



### 3. Delta dfu procedure

> This is a user guider to tell you how to test the delta dfu , it is really ease for you to merge it to your SDK. 
>
> A delta firmware upgrade consists of the following steps:
>
> - install essential tools  
> - pull the new boot code to replace your old code 
> - prepare test application
> - generate patch file and transfer it to the secondary slot
> - Restart MCU to perform the update
> - Checking the result of the delta update

___

#### 3.1 Demonstrator setup

The "demonstrator" refers to the setup surrounding the implementation that is used to demonstrate its functionality.  A delta firmware upgrade consists of the following steps:

##### (1). install essential tools

you should install detools on your PC: enter  <font color = "green">"**pip install detools**"</font>  command in the python environment.

you should install cryptography,intelhex,click,cbor: enter <font color = "green">"**pip install -r requirements.txt**"</font> command in the python environment.

##### (2). pull the new boot code to replace your old code 

you should pull the new boot code from below url and replace the boot folder in your SDK directory (==v2.x.x/bootloader/ mcubboot/boot==),  remember to change the folder name to boot or copy the folder contents to boot folder. Currently we only provide library.

​		https://github.com/Noy0908/delta_dfu_lib.git

​		branch **delta_dfu_lib_9160** is the library for nRF9160

​		branch **delta_dfu_lib_52840** is the library for nRF9160

##### (3). prepare test application

- you can test it on our demoes. We provide two demoes, one is for nRF9160, the other is for nRF52840, below is the url and branches.

  https://github.com/Noy0908/delta_dfu.git

  **9160-delta-ota** is the demo for nRF9160

  **52840-delta-ota** is the demo for nRF52840

- Of course you can test it on any application samples,  but need to prepare two folders in your project root directory to save images and patch file. one is ==binaries/signed_images==  which used to save source image and target image; the other is ==binaries/patches== which used to save patch image. 

![binaries](C:\tech_docs\delta_dfu\folders.png)

- copy the scripts folder(==boot/zephyr/scripts==) from delta_mcuboot folder to the root directory of your application , and double-click the ==scripts/patch.exe== file when generating the patch image. 

- allocate your flash partition. you can create a ==pm_static.yml== file in your project root directory to redefine the flash partition. 

  <font color = "red">**remember that you must define the primary slot and secondary slot, and these two slots support differnet size**</font>.

- enable delta dfu.  you can create a ==child_image/mcuboot.conf== file to set these macros:

  >**`CONFIG_BOOT_MAX_IMG_SECTORS=160`**
  >
  >**`CONFIG_BOOT_UPGRADE_APP_DELTA=y`**

- enable mcuboot in your project config files(==prj.conf==). 

  > **`CONFIG_BOOTLOADER_MCUBOOT=y`**
  >
  > **`CONFIG_IMG_MANAGER=y `**
  >
  > **`CONFIG_MCUBOOT_IMG_MANAGER=y `**

##### (4). generate patch file and transfer it to the secondary slot

- Edit ==scripts/signature.py== file. replace the imgtool.py path and root-ec-p256.pem path in the file with your own path, and modify the primary slot size to your own define.

  ![signature](C:\tech_docs\delta_dfu\signature.png)

- Generate the patch file. The patch file is generated by comparing the difference between the source image and the target image. The source image is a binary file which converted from the "app_signed.hex" file that compiled from the source project, we can directly use the J-Flash tool to convert "app_signed.hex" to a binary file by saving it to a binary file named "source_xxx.bin"; or convert it with command line : **arm-none-eabi-objcopy --input-target=ihex --output-target=binary --gap-fill=0xff app_signed.hex source_xxx.bin**. Modify the source project and compile again, then get the target file: app_update.bin, rename it to target_xxx.bin, such as target_2.0.0.bin. Copy the source_ xxx.bin and target_xxx. bin to binaries/signed_images folder, then execute scripts/patch.exe, and the differential file will be automatically generated in the directory binaries/patches. Please use the differential file signed_ patch.bin as patch file.

- Transmit the patch image to secondary slot. Now we supports multiple OTA methods, such as **4G/WiFi/Bluetooth/NFC**, etc. It can also be delivered through wired methods, such as **UART, USB or SPI bus**.

##### (5). Restart MCU to perform the update

After the patch image is saved to flash, you should call the function `(boot_request_upgrade(BOOT_UPGRADE_PERMANENT)` and then restart MCU. After the MCU restarts, it will check whether the differential image in the flash area is valid, verify the signature and hash data. Differential upgrading will only be performed when all these information are matched. While applying patch.bin, the old image (source. bin) and patch image combines to generate a new image and save it in the flash primary slot area, then MCU jump to the application entry, the whole process supports power down protection.

![applying](C:\tech_docs\delta_dfu\applying.png)

##### (6). Checking the result of the update

you can check the output log to see if the upgrade is successful and the MCU run the target image.

![upgrade successful](C:\tech_docs\delta_dfu\upgrated.png)

___

### 4. Tests

The aims of this document is to demonstrate how to perform delta updates on resource constrained embedded systems and that these updates cause a meaningful reduction of data transmitted during a firmware upgrade. To test that these aims have been reached we need to verify that any application created can perform a delta update and that the patch used in this process is significantly smaller than the target image.  Below is the test results.

|                    chip                    | old image size | new image size | patch size |
| :----------------------------------------: | :------------: | :------------: | :--------: |
|  <font color = "red"> **nRF9160** </font>  |     214KB      |     270KB      |    95KB    |
|  <font color = "red"> **nRF9160** </font>  |     268KB      |     270KB      |    21KB    |
|  <font color = "red"> **nRF9160** </font>  |     141KB      |     129KB      |    11KB    |
|  **<font color = "red"> nRF9160 </font>**  |     111KB      |     149KB      |    57KB    |
|  <font color = "red"> **nRF9160** </font>  |     137KB      |     149KB      |    29KB    |
|  <font color = "red"> **nRF9160** </font>  |     147KB      |     149KB      |    23KB    |
|  <font color = "red"> **nRF9160** </font>  |     145KB      |     149KB      |    13KB    |
|                                            |                |                |            |
| <font color = "blue"> **nRF52840** </font> |     239KB      |     293KB      |   109KB    |
| <font color = "blue"> **nRF52840** </font> |     288KB      |     293KB      |    24KB    |
| <font color = "blue"> **nRF52840** </font> |     265KB      |     293KB      |    70KB    |


