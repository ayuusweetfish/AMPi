AMPIHOME = .

OBJS = \
	ampi.o \
	linux/linuxemu.o \
	linux/bug.o linux/completion.o linux/delay.o linux/device.o \
	linux/dma-mapping.o linux/interrupt.o linux/kthread.o linux/mutex.o \
	linux/platform_device.o linux/printk.o linux/raspberrypi-firmware.o \
	linux/rwlock.o linux/semaphore.o linux/spinlock.o linux/sprintf.o \
	linux/synchronize.o linux/timer.o \
	vc4/sound/vchiqsoundbasedevice.o \
	vc4/vchiq/vchiqdevice.o vc4/vchiq/vchiq_arm.o \
	vc4/vchiq/vchiq_2835_arm.o vc4/vchiq/vchiq_core.o \
	vc4/vchiq/vchiq_kern_lib.o vc4/vchiq/vchiq_connected.o \
	vc4/vchiq/vchiq_shim.o vc4/vchiq/vchiq_util.o

libampi.a: $(OBJS)
	@echo "  AR    $@"
	@rm -f $@
	@$(AR) cr $@ $(OBJS)

clean:
	rm -f $(OBJS) libampi.a

include Rules.mk
