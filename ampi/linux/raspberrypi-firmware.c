#include <linux/raspberrypi-firmware.h>
#include <linux/errno.h>
#include <linux/bug.h>
#include <ampienv.h>

int rpi_firmware_property (struct rpi_firmware *fw, u32 tag, void *data, size_t len)
{
	BUG_ON (tag != 0x48010 || len != 4);

	if (len > 2048)
	{
		return -ENOMEM;
	}

	u32 result = EnableVCHIQ (*(u32 *)data);
	*(u32 *)data = result;

	return 0;
}
