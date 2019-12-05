#
# Makefile
#

AMPIHOME = ../ampi

OBJS	= main.o synth_ogg.o env.o coroutine.o stub.o sysinit.o circle/util.o

LIBS	= $(AMPIHOME)/libampi.a

TARGET  = kernel

PREFIX	?= arm-none-eabi-
CC	= $(PREFIX)gcc
FLOAT_ABI ?= hard
ARCH	?= -DAARCH=32 -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=$(FLOAT_ABI)
LIBM	!= $(CC) $(ARCH) -print-file-name=libm.a
EXTRALIBS += $(LIBM)
LIBGCC	!= $(CC) $(ARCH) -print-file-name=libgcc.a
EXTRALIBS += $(LIBGCC)
LIBC	!= $(CC) $(ARCH) -print-file-name=libc.a
EXTRALIBS += $(LIBC)

$(TARGET).img: $(OBJS) $(LIBS) link.ld
	@echo "  LD    $(TARGET).elf"
	@$(LD) -o $(TARGET).elf $(LDFLAGS) \
		-T link.ld $(CRTBEGIN) $(OBJS) \
		--start-group $(LIBS) $(EXTRALIBS) --end-group $(CRTEND)
	@echo "  COPY  $(TARGET).img"
	@$(PREFIX)objcopy $(TARGET).elf -O binary $(TARGET).img
	@echo -n "  WC    $(TARGET).img => "
	@wc -c < $(TARGET).img

clean:
	rm -f *.o $(TARGET).elf $(TARGET).img

include $(AMPIHOME)/Rules.mk
