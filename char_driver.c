#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 756

/* Define device_buffer and other global data structures you will need here */

static char *device_buffer = NULL;
static int times_opened = 0;
static int times_closed = 0;

ssize_t pa3_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	if (length == 0)
		return 0;
	
	if (length > BUFFER_SIZE - *offset)
		return -1;
	
	//Make sure it's not reading from beyond bounds
	if (*offset < 0 || *offset >= BUFFER_SIZE)
		return -1;

	copy_to_user(buffer, device_buffer + *offset, length);

	*offset += length;

	printk(KERN_ALERT "pa3 driver: %d bytes have been read", length);

	return length;
}



ssize_t pa3_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	if (length == 0)
		return 0;
	
	if (length > BUFFER_SIZE - *offset)
		return -1;
	
	if (*offset < 0 || *offset >= BUFFER_SIZE)
		return -1;

	copy_from_user(device_buffer + *offset, buffer, length);

	*offset += length;

	printk(KERN_ALERT "pa3 driver: %d bytes have been written", length);

	return length;
}


int pa3_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	times_opened++;
	printk(KERN_ALERT "pa3 driver: The file has been opened %d times\n", times_opened);
	return 0;
}

int pa3_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	times_closed++;
	printk(KERN_ALERT "pa3 driver: The file has been close %d times\n", times_closed);
	return 0;
}

loff_t pa3_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t position = 0;

	//Check for whence
	if (whence == SEEK_SET)
	{
		position = offset;
	}
	else if (whence == SEEK_CUR)
	{
		position = pfile -> f_pos + offset;
	}
	else if (whence == SEEK_END)
	{
		position = BUFFER_SIZE + offset;
	}

	printk(KERN_ALERT "pa3 driver: Whence is %d, offset is %d, position is %d", whence, offset, position);

	//Check for offset that is out of bounds
	if (position < 0)
	{
		position = -position % BUFFER_SIZE;
		position = BUFFER_SIZE - position;

	}
	else if (position > BUFFER_SIZE - 1)
	{
		position %= BUFFER_SIZE;
	}

	printk(KERN_ALERT "pa3 driver: Updated position is %d", position);

	pfile -> f_pos = position;

	return position;
}

struct file_operations pa3_operations = {

	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.owner   = THIS_MODULE,
	.open	 = pa3_open,
	.release = pa3_close,
	.read    = pa3_read,
	.write   = pa3_write,
	.llseek  = pa3_seek
};

static int pa3_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	printk(KERN_ALERT "pa3 driver: Init function has been called");
	register_chrdev(240, "My_driver", &pa3_operations);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	return 0;
}

static void pa3_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_ALERT "pa3 driver: Exit function has been called");
	unregister_chrdev(240, "My_driver");
	kfree(device_buffer);
}

/* add module_init and module_exit to point to the corresponding init and exit function*/

module_init(pa3_init);
module_exit(pa3_exit);
