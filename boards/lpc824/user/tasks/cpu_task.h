#ifndef  __CPU_TASK_H__
#define  __CPU_TASK_H__


extern osThreadId   cpu_task_hdl;
void cpu_task(void const * argument);


#define  CPU_TASK_WTG_TIMEOUT_VALUE            1100
#define  CPU_TASK_INTERVAL_VALUE               1000

















#endif