AARCH	 ?= 32
RASPPI	 ?= 1
PREFIX	 ?= arm-none-eabi-
PREFIX64 ?= aarch64-elf-

# set this to "softfp" if you want to link specific libraries
FLOAT_ABI ?= hard

CC	= $(PREFIX)gcc
AS	= $(CC)
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar

ifeq ($(strip $(AARCH)),32)
ifeq ($(strip $(RASPPI)),1)
ARCH	?= -DAARCH=32 -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=$(FLOAT_ABI)
else ifeq ($(strip $(RASPPI)),2)
ARCH	?= -DAARCH=32 -mcpu=cortex-a7 -marm -mfpu=neon-vfpv4 -mfloat-abi=$(FLOAT_ABI)
else ifeq ($(strip $(RASPPI)),3)
ARCH	?= -DAARCH=32 -mcpu=cortex-a53 -marm -mfpu=neon-fp-armv8 -mfloat-abi=$(FLOAT_ABI)
else ifeq ($(strip $(RASPPI)),4)
ARCH	?= -DAARCH=32 -mcpu=cortex-a72 -marm -mfpu=neon-fp-armv8 -mfloat-abi=$(FLOAT_ABI)
else
$(error RASPPI must be set to 1, 2, 3 or 4)
endif
LOADADDR = 0x8000
else ifeq ($(strip $(AARCH)),64)
ifeq ($(strip $(RASPPI)),3)
ARCH	?= -DAARCH=64 -mcpu=cortex-a53 -mlittle-endian -mcmodel=small
else ifeq ($(strip $(RASPPI)),4)
ARCH	?= -DAARCH=64 -mcpu=cortex-a72 -mlittle-endian -mcmodel=small
else
$(error RASPPI must be set to 3 or 4)
endif
PREFIX	= $(PREFIX64)
LOADADDR = 0x80000
else
$(error AARCH must be set to 32 or 64)
endif

MAKE_VERSION_MAJOR := $(firstword $(subst ., ,$(MAKE_VERSION)))
ifneq ($(filter 0 1 2 3,$(MAKE_VERSION_MAJOR)),)
$(error Requires GNU make 4.0 or newer)
endif

OPTIMIZE ?= -O2

INCLUDE	+= -I $(AMPIHOME)
DEFINE	+= -D__circle__ -DRASPPI=$(RASPPI)

AFLAGS	+= $(ARCH) $(DEFINE) $(INCLUDE) $(OPTIMIZE)
CFLAGS	+= $(ARCH) -Wall -fsigned-char -ffreestanding $(DEFINE) $(INCLUDE) $(OPTIMIZE) -g
LDFLAGS	+= --section-start=.init=$(LOADADDR)

%.o: %.S
	@echo "  AS    $@"
	@$(AS) $(AFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -std=gnu99 -c -o $@ $<
