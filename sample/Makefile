#
# Makefile
#

AMPIHOME = ../ampi

OBJS	= main.o env.o coroutine.o stub.o sysinit.o circle/util.o

LIBS	= $(AMPIHOME)/libampi.a

TARGET  = kernel

PREFIX	?= arm-none-eabi-
CC	= $(PREFIX)gcc
FLOAT_ABI ?= hard
ARCH	?= -DAARCH=32 -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=$(FLOAT_ABI)
LIBM	!= $(CC) $(ARCH) -print-file-name=libm.a
EXTRALIBS += $(LIBM)

ifeq ($(strip $(OGG)),1)
LIBGCC	!= $(CC) $(ARCH) -print-file-name=libgcc.a
EXTRALIBS += $(LIBGCC)
LIBC	!= $(CC) $(ARCH) -print-file-name=libc.a
EXTRALIBS += $(LIBC)
DEFINE	+= -DOGG
OBJS	+= synth_ogg.o
MUSIC	= music.h
else
OBJS	+= synth.o
MUSIC	= 
endif

$(TARGET).img: $(MUSIC) $(OBJS) $(LIBS) link.ld
	@echo "  LD    $(TARGET).elf"
	@$(LD) -o $(TARGET).elf $(LDFLAGS) \
		-T link.ld $(CRTBEGIN) $(OBJS) \
		--start-group $(LIBS) $(EXTRALIBS) --end-group $(CRTEND)
	@echo "  COPY  $(TARGET).img"
	@$(PREFIX)objcopy $(TARGET).elf -O binary $(TARGET).img
	@echo -n "  WC    $(TARGET).img => "
	@wc -c < $(TARGET).img

music.h: music.ogg
	@xxd -i music.ogg > music.h

clean:
	rm -f *.o $(TARGET).elf $(TARGET).img music.h

include $(AMPIHOME)/Rules.mk
