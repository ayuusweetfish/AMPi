#ifndef _linux_types_h
#define _linux_types_h

#include <stddef.h>
#include <stdint.h>

typedef unsigned long	loff_t;
typedef int		gfp_t;
typedef uintptr_t	uintptr;
typedef uintptr		dma_addr_t;

typedef unsigned char	u8;
typedef signed short	s16;
typedef unsigned int	u32;

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef bool		boolean;
#ifndef TRUE
#define TRUE	true
#endif
#ifndef FALSE
#define FALSE	false
#endif

#ifndef NULL
#define NULL	0
#endif

struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};

#endif
