#include <linux/kthread.h>
#include <linux/bug.h>
#include <ampienv.h>

struct task_struct *current = 0;

#define MAX_THREADS     16
static struct task_struct tasks[MAX_THREADS];

struct task_struct *kthread_create (int (*threadfn)(void *data),
				    void *data,
				    const char namefmt[], ...)
{
	int pid = SchedulerCreateThread (threadfn, data);

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

static void task_switch_handler (int pid)
{
	BUG_ON (pid < 0 || pid >= MAX_THREADS);
	current = &tasks[pid];
}

int linuxemu_init_kthread (void)
{
	SchedulerRegisterSwitchHandler (task_switch_handler);

	tasks[0].pid = 0;
	current = &tasks[0];

	return 0;
}
