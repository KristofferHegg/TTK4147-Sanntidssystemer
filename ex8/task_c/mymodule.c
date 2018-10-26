#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/proc_fs.h>

// included for all kernel modules
// included for __init and __exit macros

struct proc_dir_entry *myproc_pointer;

int procfile_read(char *buffer, char **buffer_location, off_t offset,
	int buffer_length, int *eof, void *data){
	if (offset > 0){
		return 0;
	} else {
		return sprintf(buffer, "Hello world task c (sprintf)!\n");
	}
}

static int __init hello_init(void){

	myproc_pointer = create_proc_entry("myproc", 0644, NULL);

	myproc_pointer->read_proc = procfile_read;

	printk(KERN_INFO "Hello world task c (Kernel)!\n");

	return 0;
	// Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void){
	remove_proc_entry("myproc", NULL);
}

module_init(hello_init);
module_exit(hello_cleanup);
