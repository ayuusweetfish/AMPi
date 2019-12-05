#ifndef _linux_assert_h
#define _linux_assert_h

#ifdef __cplusplus
extern "C" {
#endif

#include <ampienv.h>

#ifdef NDEBUG
	#define assert(expr)	((void) 0)
#else

	#define assert(expr)	(  __builtin_expect (!!(expr), 1)	\
				 ? ((void) 0)		\
				 : ampi_assertion_failed (#expr, __FILE__, __LINE__))
#endif

#define ASSERT_STATIC(expr)	extern int assert_static[(expr) ? 1 : -1]

#ifdef __cplusplus
}
#endif

#endif
