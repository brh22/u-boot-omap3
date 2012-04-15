#
# Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation version 2.
#
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp
# This will be used by mkimage extension to select header for image
TI_DEVICE = ti81xx
# ROM code will load u-boot to this address
TI_LOAD_ADDR = 0x40300000
# TEXT_BASE for the 2 stages is different and comes from the Makefile
CROSS_COMPILE:= arm-none-linux-gnueabi-
LDSCRIPT:=board/asi1230/u-boot.lds

# Attempt at making build process less verbose

V = 0

USE_PRIVATE_LIBGCC := $(OBJTREE)/arch/$(ARCH)/lib

RELDIR = $(subst $(TOPDIR)/,,$(CURDIR))

ORIG_CC := $(CC)
CC_0 = @echo -e "gcc\t$(RELDIR)/$<" && $(ORIG_CC)
CC_1 := $(CC)
CC = $(CC_$(V))

CPP	= $(CC) -E
CPP_1 := $(CC_1) -E

MAKE_0 := @$(MAKE) -s
MAKE_1 := $(MAKE)
MAKE = $(MAKE_$(V))

ORIG_OBJCOPY := $(OBJCOPY)
OBJCOPY_0 = @echo -e "objcopy\t$(RELDIR)/$<" && $(ORIG_OBJCOPY)
OBJCOPY_1 := $(OBJCOPY)
OBJCOPY = $(OBJCOPY_$(V))

ORIG_LD := $(LD)
LD_0 = @echo -e "ld\t$(RELDIR)/$<" && $(ORIG_LD)
LD_1 := $(LD)
LD = $(LD_$(V))

#AS_0 := $(XECHO) -t "AS $<..."; $(AS)
#AS_1 := $(AS)
#AS = $(AS_$(V))

#AR_0 := $(XECHO) -t "AR $<..."; $(AR)
#AR_1 := $(AR)
#AR = $(AR_$(V))

#NM_0 := $(XECHO) -t "NM $<..."; $(NM)
#NM_1 := $(NM)
#NM = $(NM_$(V))

#LDR_0 := $(XECHO) -t "LDR $<..."; $(LDR)
#LDR_1 := $(LDR)
#LDR = $(LDR_$(V))

#STRIP_0 := $(XECHO) -t "STRIP $<..."; $(STRIP)
#STRIP_1 := $(STRIP)
#STRIP = $(STRIP_$(V))

#OBJDUMP_0 := $(XECHO) -t "OBJDUMP $<..."; $(OBJDUMP)
#OBJDUMP_1 := $(OBJDUMP)
#OBJDUMP = $(OBJDUMP_$(V))

#RANLIB_0 := $(XECHO) -t "RANLIB $<..."; $(RANLIB)
#RANLIB_1 := $(RANLIB)
#RANLIB = $(RANLIB_$(V))



