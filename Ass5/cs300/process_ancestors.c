#include <asm/errno.h>
#include <linux/cred.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include "process_ancestors.h"


asmlinkage long sys_process_ancestors( struct process_info info_array[],
									   long size,
									   long *num_filled )
{
	struct task_struct* cur_task = current;
	long kernal_num_filled = 0;
	long kernal_num_children = 0;
	long kernal_num_sibling = 0;
	struct list_head* cur_node = NULL;

	if(size <= 0)
	{
		printk("Size <= 0.\n");
		return -EINVAL;
	}

	if(!num_filled)
	{
		printk("Problem occurs when access num_filled.\n");
		return -EFAULT;
	}

	while(cur_task->parent != cur_task)
	{
		kernal_num_children = 0;
		kernal_num_sibling = 0;

		if(kernal_num_filled > 0)
		{
			cur_task = cur_task->parent;
		}

		list_for_each(cur_node, &(cur_task->children))
		{
			kernal_num_children++;
		}
		
		
		list_for_each(cur_node, &(cur_task->sibling))
		{
			kernal_num_sibling++;
		}

		printk("pid: %li, name: %s, state: %li, uid: %li, nvcsw: %li, nivscw: %li, num_children: %li, num_siblings: %li\n",
									 (long)(cur_task->pid),
									 cur_task->comm,
									 cur_task->state,
									 (long)((cur_task->cred)->uid.val),
									 cur_task->nvcsw,
									 cur_task->nivcsw,
									 kernal_num_children,
									 kernal_num_sibling  );
		
		if(kernal_num_filled < size)
		{
			struct process_info temp_process;
			temp_process.pid = (long)(cur_task->pid);
			strcpy(temp_process.name, cur_task->comm);
			temp_process.state = cur_task->state;
			temp_process.uid = (long)((cur_task->cred)->uid.val);
			temp_process.nvcsw = cur_task->nvcsw;
			temp_process.nivcsw = cur_task->nivcsw;
			temp_process.num_children = kernal_num_children;
			temp_process.num_siblings = kernal_num_sibling;

			if(copy_to_user(&info_array[kernal_num_filled], &temp_process, sizeof(struct process_info)))
			{
				printk("Problem occurs when copy from temp_process to user pointer info_array[kernal_num_filled].\n");
				return -EFAULT;
			}

			kernal_num_filled++;
		}
	}

	if(copy_to_user(num_filled, &kernal_num_filled, sizeof(long)))
	{
		printk("Problem occurs when copy from kernal_num_filled to user pointer num_filled.\n");
		return -EFAULT;
	}
	
	return 0;
}
