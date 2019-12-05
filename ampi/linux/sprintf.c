#include <linux/kernel.h>

int sprintf (char *buf, const char *fmt, ...)
{
	va_list var;
	va_start (var, fmt);
	va_end (var);
	buf[0] = '\0';
	return 0;
}

int snprintf (char *buf, size_t size, const char *fmt, ...)
{
	va_list var;
	va_start (var, fmt);
	va_end (var);
	buf[0] = '\0';
	return 0;
}

int vsnprintf (char *buf, size_t size, const char *fmt, va_list var)
{
	buf[0] = '\0';
	return 0;
}
