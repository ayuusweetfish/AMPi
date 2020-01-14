#include <linux/semaphore.h>
#include <linux/bug.h>
#include <linux/coroutine.h>

void down (struct semaphore *sem)
{
#ifdef ARM_ALLOW_MULTI_CORE
	BUG_ON (CMultiCoreSupport::ThisCore () != 0);
#endif

	while (sem->count == 0)
	{
		ampi_co_yield();
	}

	sem->count--;
}

void up (struct semaphore *sem)
{
#ifdef ARM_ALLOW_MULTI_CORE
	BUG_ON (CMultiCoreSupport::ThisCore () != 0);
#endif

	sem->count++;
}

int down_trylock (struct semaphore *sem)
{
#ifdef ARM_ALLOW_MULTI_CORE
	BUG_ON (CMultiCoreSupport::ThisCore () != 0);
#endif

	if (sem->count == 0)
	{
		return 1;
	}

	sem->count--;

	return 0;
}
