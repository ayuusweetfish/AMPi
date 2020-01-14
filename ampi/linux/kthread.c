#include <linux/kthread.h>
#include <linux/bug.h>
#include <linux/coroutine.h>

struct task_struct *current = 0;

static struct task_struct tasks[MAX_CO];

struct task_struct *kthread_create (int (*threadfn)(void *data),
				    void *data,
				    const char namefmt[], ...)
{
	// Such function pointer casts are safe for most platforms including ARM
	int pid = ampi_co_create ((void (*)(void *))threadfn, data);

	struct task_struct *task = &tasks[pid];
	task->pid = pid;

	return task;
}

void set_user_nice (struct task_struct *task, long nice)
{
}

// TODO: task is waken by default
int wake_up_process (struct task_struct *task)
{
	return 0;
}

void flush_signals (struct task_struct *task)
{
}

static void task_switch_handler (int8_t pid)
{
	BUG_ON (pid < 0 || pid >= MAX_CO);
	current = &tasks[pid];
}

int linuxemu_init_kthread (void)
{
	ampi_co_callback (task_switch_handler);

	tasks[0].pid = 0;
	current = &tasks[0];

	return 0;
}
