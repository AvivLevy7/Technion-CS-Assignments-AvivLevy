#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/pid.h>
#include <linux/capability.h>

#define SEC_SWORD    0x01  // 0000 0001
#define SEC_MIDNIGHT 0x02  // 0000 0010
#define SEC_CLAMP    0x04  // 0000 0100
#define SEC_DUTY     0x08  // 0000 1000
#define SEC_ISOLATE  0x10  // 0001 0000


asmlinkage long sys_hello(void) { 
 printk("Hello, World!\n"); 
 return 0; 
}

asmlinkage long sys_set_sec(int sword, int midnight, int clamp, int duty, int isolate) {

 if (sword < 0 || midnight < 0 || clamp < 0 || duty < 0 || isolate < 0)
  return -EINVAL;

 sword    = (sword > 0) ? 1 : 0;
 midnight = (midnight > 0) ? 1 : 0;
 clamp    = (clamp > 0) ? 1 : 0;
 duty     = (duty > 0) ? 1 : 0;
 isolate  = (isolate > 0) ? 1 : 0;

 if (!capable(CAP_SYS_ADMIN))
  return -EPERM;

  unsigned char new_clearance = 0;
  if (sword)
   new_clearance |= SEC_SWORD;
  if (midnight)
   new_clearance |= SEC_MIDNIGHT;
  if (clamp)
   new_clearance |= SEC_CLAMP;
  if (duty)
   new_clearance |= SEC_DUTY;
  if (isolate)
   new_clearance |= SEC_ISOLATE;

  current->clearance = new_clearance;

 return 0;
}

asmlinkage long sys_get_sec(char clr){
 unsigned char mask;

 switch (clr) {
  case 's': mask = SEC_SWORD; break;
  case 'm': mask = SEC_MIDNIGHT; break;
  case 'c': mask = SEC_CLAMP; break;
  case 'd': mask = SEC_DUTY; break;
  case 'i': mask = SEC_ISOLATE; break;
  default:  return -EINVAL;
 }

 return (current->clearance & mask) ? 1 : 0;
}

asmlinkage long sys_check_sec(pid_t pid, char clr){
  unsigned char mask;
  struct pid* pid_struct;
  struct task_struct* task;

  switch (clr) {
   case 's': mask = SEC_SWORD; break;
   case 'm': mask = SEC_MIDNIGHT; break;
   case 'c': mask = SEC_CLAMP; break;
   case 'd': mask = SEC_DUTY; break;
   case 'i': mask = SEC_ISOLATE; break;
   default:  return -EINVAL;
   }

    pid_struct = find_get_pid(pid);
    if (!pid_struct)
         return -ESRCH;

    task = pid_task(pid_struct, PIDTYPE_PID);
    put_pid(pid_struct);
    if (!task)
         return -ESRCH;

    if (!(current->clearance & mask))
         return -EPERM;

    return (task->clearance & mask) ? 1 : 0;
}

asmlinkage long sys_flip_sec_branch(int height, char clr) {
    unsigned char mask;
    int i;
    int added_count = 0;
    struct task_struct* parent;

    if (height <= 0)
         return -EINVAL;

    switch (clr) {
    case 's': mask = SEC_SWORD; break;
    case 'm': mask = SEC_MIDNIGHT; break;
    case 'c': mask = SEC_CLAMP; break;
    case 'd': mask = SEC_DUTY; break;
    case 'i': mask = SEC_ISOLATE; break;
    default:  return -EINVAL;
    }

    if (!(current->clearance & mask))
         return -EPERM;

    parent = current->real_parent;
    for (i = 0; i < height; i++) {
      if (!parent || parent == parent->real_parent) break;
      else if (parent->clearance & mask) parent->clearance &= ~mask;
      else {
        parent->clearance |= mask;
             added_count++;
      }
      parent = parent->real_parent;
    }

    return added_count;
}
