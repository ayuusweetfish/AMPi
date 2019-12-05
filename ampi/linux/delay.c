#include <linux/delay.h>
#include <ampienv.h>

void udelay (unsigned long usecs)
{
	usDelay(usecs);
}

void msleep (unsigned msecs)
{
	MsDelay(msecs);
}
