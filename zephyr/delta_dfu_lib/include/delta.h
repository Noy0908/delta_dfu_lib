/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DELTA_H_
#define __DELTA_H_

#include <zephyr/zephyr.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/dfu/mcuboot.h>
#include <sys/reboot.h>
#include "detools.h"


// #define DELTA_ENABLE_LOG			1

/* IMAGE OFFSETS AND SIZES */
#define PRIMARY_OFFSET 				FLASH_AREA_OFFSET(image_0)
#define PRIMARY_SIZE 				FLASH_AREA_SIZE(image_0)
#define SECONDARY_OFFSET 			FLASH_AREA_OFFSET(image_1)
#define SECONDARY_SIZE 				FLASH_AREA_SIZE(image_1)

/* PAGE SIZE */
#define PAGE_SIZE 					0x1000

#define SAVE_FLAG					0xA5A5A5A5

#define  BACKUP_STATUS_ADDRESS		(SECONDARY_OFFSET + SECONDARY_SIZE - 5*PAGE_SIZE)


/* PATCH HEADER SIZE */
#define HEADER_SIZE 				0x28			//"NEWP" + patch_size + source_version

#define DELTA_OP_NONE				0x00
#define DELTA_OP_START				0x0E
#define DELTA_OP_TRAVERSE			0x0A
#define DELTA_OP_APPLY				0x02


#define DATA_LEN 2
#define ADDR_LEN 4
#define DATA_HEADER (DATA_LEN+ADDR_LEN)

#define ERASE_PAGE_SIZE (PAGE_SIZE*2)

#define IMAGE_ARRAY_SIZE PAGE_SIZE
#define MAX_WRITE_UNIT 128  //relating to to/from array of process_data in detools.c

/* Error codes. */
#define DELTA_OK                                          0
#define DELTA_SLOT1_OUT_OF_MEMORY                        28
#define DELTA_READING_PATCH_ERROR						 29
#define DELTA_READING_SOURCE_ERROR						 30
#define DELTA_WRITING_ERROR			                     31
#define DELTA_SEEKING_ERROR								 32
#define DELTA_CASTING_ERROR                              33
#define DELTA_INVALID_BUF_SIZE							 34
#define DELTA_CLEARING_ERROR							 35
#define DELTA_NO_FLASH_FOUND							 36
#define DELTA_PATCH_HEADER_ERROR                         37
#define DELTA_SOURCE_HASH_ERROR                          38

/* FLASH MEMORY AND POINTERS TO CURRENT LOCATION OF BUFFERS AND END OF IMAGE AREAS.
 * - "Patch" refers to the area containing the patch image.
 * - "From" refers to the area containing the source image.
 * - "To" refers to the area where the target image is to be placed.
 */
struct flash_mem {
	/**Below are the apply patch status information*/
	uint32_t save_flag;
	size_t patch_offset;
    size_t to_offset;
    int from_offset;
    size_t chunk_size;
	size_t last_chunk_size;
	size_t chunk_offset;
	struct {
        int state;
        int value;
        int offset;
        bool is_signed;
    } size;
	union {
        struct detools_apply_patch_patch_reader_heatshrink_t heatshrink;
    } compression;
	enum detools_apply_patch_state_t state;
	uint8_t rest_buf[MAX_WRITE_UNIT];
	/**apply memory status */
	off_t patch_current;
	off_t patch_end;
	off_t from_current;
	off_t from_end;
	off_t to_current;
	off_t to_end;
	off_t erased_addr;		//the erase address of primary slot
	off_t backup_addr;		//save the old image which will be used after erased
	size_t write_size;
};


struct bak_flash_mem {
	uint8_t buffer[ERASE_PAGE_SIZE];
	// uint32_t save_flag;
	struct flash_mem flash;
};

/* FUNCTION DECLARATIONS */

/**
 * Functiong try to check if the apply process can excute correctly
 *
 * @param[in] flash the devices flash memory
 * @param[in] apply_patch a pointer to assign the write/read flash function.
 *
 * @return if not patch, a negative value (0>) representing an error, 
 * or a positive value (0<) representing the patch size.
 */
int traverse_delta_file(struct flash_mem *flash, struct detools_apply_patch_t *apply_patch);

/**
 * Functiong to init flash parameter
 *
 * @param[in] flash the devices flash memory
 * @param[in] patch_size the size of patch file.
 * @param[in] apply_patch a pointer to assign the write/read flash function.
 *
 * @return i a negative value (0>) representing an error, 
 * 			or a positive value (0<=) representing the patch size.
 */
int delta_apply_init(struct flash_mem *flash,uint32_t patch_size,struct detools_apply_patch_t *apply_patch);

/**
 * Functiong to excute the apply process, once it starts, the image file can not be restored
 *
 * @param[in] flash the devices flash memory
 * @param[in] apply_patch a pointer to assign the write/read flash function.
 *
 * @return i a negative value (0>) representing an error, 
 * 			or a positive value (0<=) representing the patch size.
 */
int delta_check_and_apply(struct flash_mem *flash, struct detools_apply_patch_t *apply_patch);

/**
 * Functiong for reading the metadata from the patch
 *
 * @param[in] hash_buf the source image hash value to verify if the patch file is valid.
 * @param[in] size a pointer to save the patch size value.
 * @param[in] op a pointer to indicate current apply operation.
 *
 * @return if not patch, a negative value (0>) representing an error, 
 * or a positive value (0<) representing the patch size.
 */
int delta_read_patch_header(uint8_t *hash_buf, uint32_t *size, uint8_t *op);

/**
 * Functiong for saving the apply status before you erase the image data
 * enable the dynamic save backup data
 *
 * @return  the flash address of backup data
 */
int apply_write_status(struct flash_mem *flash,off_t addr);

/**
 * Functiong for getting the address of backup data, this function is called only when you 
 * enable the dynamic save backup data
 *
 * @return  the flash address of backup data
 */
off_t get_status_address(void);


int apply_backup_write_status(struct flash_mem *flash_mem);


/**
 * Functiong for reading the apply status when device reboot
 *
 * @param[in] flash the devices flash memory
 *
 * @return if not patch, a negative value (0>) representing an error, 
 * 			or 0 representing the patch size.
 */
int apply_read_status(struct flash_mem *flash);


int increase_patch_offset(void *arg_p,off_t size);


int write_last_buffer(void *arg_p);


/**
 * Get the error string for given error code.
 *
 * @param[in] Error code.
 *
 * @return Error string.
 */
const char *delta_error_as_string(int error);

#endif