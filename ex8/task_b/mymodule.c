#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> // included for KERN_INFO

// included for all kernel modules
// included for __init and __exit macros

static int __init hello_init(void){

	printk(KERN_INFO "Hello world task b!\n");

	return 0;
	// Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void){
}

module_init(hello_init);
module_exit(hello_cleanup);
