#include "array_stats.h"
#include <asm/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>


asmlinkage long sys_array_stats( struct array_stats* stats,
				                 long data[],
				                 long size )
{
	struct array_stats kernal_stats;
	long counter = 0;	
	long temp = 0;

	if(size <= 0)
	{
		printk("Size <= 0.\n");
		return -EINVAL;
	}
	
	if(!stats)
	{
		printk("Problem occurs when access stats.\n");
		return -EFAULT;
	}

	// if(copy_from_user(&temp, &data[0], sizeof(long)))
	// {
	// 	printk("Problem occurs when copy from data[0].\n");
	// 	return -EFAULT;
	// }
	

	// tmp_stats.sum = tmp_stats.min = tmp_stats.max = tmp;
	
	while(counter<size)
	{
		if(copy_from_user(&temp, &data[counter], sizeof(long)))
		{
			printk("Problem occurs when copy from data[%li].\n",counter);
			return -EFAULT;
		}

		if(counter == 0)
		{
			kernal_stats.min = temp;
			kernal_stats.max = temp;
			kernal_stats.sum = temp;
		}
		else
		{
			if(temp > (kernal_stats.max))
			{
				kernal_stats.max = temp;
			}
			else if(temp < (kernal_stats.min))
			{
				kernal_stats.min = temp;
			}

			(kernal_stats.sum) += temp;
		}
		counter++;
	}

	printk("Min:%li Max:%li Sum:%li\n", kernal_stats.min,
										kernal_stats.max,
										kernal_stats.sum);

	if( copy_to_user(&(stats->min), &(kernal_stats.min), sizeof(long)) ||
		copy_to_user(&(stats->max), &(kernal_stats.max), sizeof(long)) ||
		copy_to_user(&(stats->sum), &(kernal_stats.sum), sizeof(long))   )
	{
		printk("Problem occurs when copy from kernal_stats to user pointer stats.\n");
		return -EFAULT;
	}
	
	/* Return 0 when successful. */
	return 0;		 
}
