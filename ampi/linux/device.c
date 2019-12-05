#include <linux/device.h>
#include <stdarg.h>

void __iomem *devm_ioremap_resource (struct device *dev, struct resource *res)
{
	if (   res == 0
	    || !(res->flags & IORESOURCE_MEM))
	{
		return 0;
	}

	return (void __iomem *) res->start;
}

void dev_err (const struct device *dev, const char *fmt, ...)
{
	va_list var;
	va_start (var, fmt);

	// XXX: snprintf has not been implemented here
	//CLogger::Get ()->WriteV (FromDevice, LogError, fmt, var);

	va_end (var);
}

void dev_warn (const struct device *dev, const char *fmt, ...)
{
	va_list var;
	va_start (var, fmt);

	//CLogger::Get ()->WriteV (FromDevice, LogWarning, fmt, var);

	va_end (var);
}
