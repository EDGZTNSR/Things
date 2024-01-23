#include <linux/module.h>       // Needed by all modules
#include <linux/kernel.h>       // Needed for KERN_INFO
#include <linux/init.h>         // Needed for the macros
#include <linux/fs.h>           // Needed for file operations
#include <linux/uaccess.h>      // Needed for copy_to_user function

#define MAJOR_NUM 100
#define TEMP_FILE "/tmp/kernel_output.txt"

// Buffer to store the data
static char kernel_buffer[256] = {0};
static int buffer_size = 0;

static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "mydriver: device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "mydriver: device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
    int error_count = 0;

    // Simple example to copy a static message to the buffer
    error_count = copy_to_user(buffer, kernel_buffer, buffer_size);

    if (error_count == 0) {
        printk(KERN_INFO "mydriver: Sent %d characters to the user\n", buffer_size);
        return (buffer_size = 0); // Clear the buffer
    } else {
        printk(KERN_INFO "mydriver: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static struct file_operations fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release,
};

static int __init mydriver_init(void) {
    register_chrdev(MAJOR_NUM, "mydriver", &fops);
    printk(KERN_INFO "mydriver: registered with major number %d\n", MAJOR_NUM);

    // Example of writing to a file (not recommended in real kernel module)
    struct file *f;
    loff_t pos = 0;
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    f = filp_open(TEMP_FILE, O_WRONLY|O_CREAT, 0644);
    if (IS_ERR(f)) {
        printk(KERN_ALERT "mydriver: Failed to open %s\n", TEMP_FILE);
        set_fs(old_fs);
        return PTR_ERR(f);
    }

    // Example data to write
    snprintf(kernel_buffer, sizeof(kernel_buffer), "Example kernel data\n");
    buffer_size = strlen(kernel_buffer);

    // Writing data to the file
    vfs_write(f, kernel_buffer, buffer_size, &pos);

    // Closing the file
    filp_close(f, NULL);
    set_fs(old_fs);

    return 0;
}

static void __exit mydriver_exit(void) {
    unregister_chrdev(MAJOR_NUM, "mydriver");
    printk(KERN_INFO "mydriver: unregistered\n");
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux driver for capturing and storing data");
MODULE_VERSION("0.1");
