#include <linux/bug.h>

void __warn (const char *file, const int line)
{
	LogWrite("WARN", LOG_ERROR, "%s:%d: WARN_ON failed", file, line);
}
