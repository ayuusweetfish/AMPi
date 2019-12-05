#ifndef _linux_slab_h
#define _linux_slab_h

#include <ampienv.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GFP_ATOMIC	0
#define GFP_KERNEL	1

static inline void *kmalloc (size_t size, int flags)
{
	return ampi_malloc (size);
}

static inline void *kzalloc (size_t size, int flags)
{
	void *p = ampi_malloc (size);
	if (p != 0)
	{
		memset (p, 0, size);
	}

	return p;
}

static inline void kfree (void *p)
{
	ampi_free (p);
}

#ifdef __cplusplus
}
#endif

#endif
