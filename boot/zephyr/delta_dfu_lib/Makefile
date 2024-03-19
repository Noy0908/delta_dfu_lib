#
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

PREFIX ?= .
OBJ_DIR ?= $(PREFIX)/obj
LIB_DIR ?= $(PREFIX)/lib

ifeq ($(findstring Windows, $(OS)),Windows)
	DEL := rmdir /S /Q
	MKDIR := mkdir
else
	DEL := rm -rf
	MKDIR := mkdir -p
endif

objects = delta.o detools.o heatshrink.o
OBJ = $(OBJ_DIR)/delta.o $(OBJ_DIR)/detools.o $(OBJ_DIR)/heatshrink.o

delta_lib: $(objects)
	$(AR) -rcs $(LIB_DIR)/delta_lib.a $(OBJ)

delta.o: src/delta.c include/delta.h include/detools.h
	$(CC) -c $(CFLAGS) -MD -Iinclude src/delta.c -o $(OBJ_DIR)/delta.o

detools.o: src/detools.c include/detools.h include/delta.h include/heatshrink_decoder.h
	$(CC) -c $(CFLAGS) -MD -Iinclude src/detools.c -o $(OBJ_DIR)/detools.o

heatshrink.o: src/heatshrink_decoder.c include/heatshrink_decoder.h include/heatshrink_common.h include/heatshrink_config.h
	$(CC) -c $(CFLAGS) -MD -Iinclude src/heatshrink_decoder.c -o $(OBJ_DIR)/heatshrink.o

clean:
	$(DEL) "$(OBJ_DIR)" "$(LIB_DIR)"
