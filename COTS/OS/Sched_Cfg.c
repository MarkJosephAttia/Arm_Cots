/**
 * @file Sched_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This file contains the configurations implementation for the Scheduler
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Std_Types.h"
#include "Sched_Cfg.h"
#include "Sched.h"

extern const task_t CLcd_task;
extern const task_t Switch_task;

const sysTaskInfo_t Sched_sysTaskInfo[SCHED_NUMBER_OF_TASKS] = 
{
    {&Switch_task, 0},
    {&CLcd_task, 1}
};
