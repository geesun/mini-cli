/* Mini-cli is Copyright (c) 2000-2013 Geesun Xu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/cdev.h>
#include <asm/uaccess.h>          // Required for the copy to user function
#include "cli_cmn.h"
#include "cli_kmod_msg.h"
#include "cli_kmod_kern.h"

#define  CLASS_NAME  "mincli"       

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Qixiang Xu");    
MODULE_DESCRIPTION("A Linux char driver for the Command line");  
MODULE_VERSION("0.1");           


static struct file_operations cli_ioctl_fops;
static struct class *  cli_ioctl_class  = NULL; 
static struct device * cli_ioctl_device = NULL; 
static struct cdev * cli_ioctl_cdev = NULL;
static cli_int32 cli_ioctl_major = 0;

static cli_int8 cfg_value[sizeof(cli_cmd_msg_t)];

static cli_kern_cmd_map_t  g_cli_cmd_mapping[CLI_CMD_MAX_NUM];
static cli_uint32 g_cli_cmd_num = 0;
static cli_int8   g_cli_format[CLI_CMD_BUFF_LEN * 2];

void cli_printk(const cli_int8 * fmt, ...)
{
    cli_int8 buffer[512];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    printk(KERN_ALERT"%s",buffer);
}

cli_int32 cli_print(
        cli_int8  * buf,
        const cli_int8 *fmt,...
        )
{

    va_list args;
    cli_int32 len = CLI_CMD_BUFF_LEN - 1 - strlen(buf);
    cli_int32 ret = 0;
    const cli_int8 * p = fmt;
    cli_int8 * next = NULL;
    cli_int8 * format = NULL;
    cli_int8   new_line = 0;

    if(len <=0 || strlen(fmt) >= CLI_CMD_BUFF_LEN){
        return 0;
    }
    
    do{
        next = strchr(p,'\n');
        new_line = 0;
        if(next != NULL){
            if(next > p && *(next - 1) == '\r'){
                new_line = 1;
            }

            if(format == NULL){
                format = g_cli_format;
                memset(format,0x00,CLI_CMD_BUFF_LEN*2);
                if(next != p){
                    strncpy(format,p,next - p);
                }
            }else{
                if(next != p){
                    strncat(format,p,next - p);
                }
            }
            p = next + 1;
            if(new_line == 1){
                strcat(format,"\n");
            }else{
                strcat(format,"\r\n");
            }
        }else{
            if(format == NULL){
                format = g_cli_format;
                memset(format,0x00,CLI_CMD_BUFF_LEN*2);
                strcpy(format,p);
            }else{
                strcat(format,p);
            }
            break;
        }
    }while(p != NULL);

    va_start(args,fmt);
    ret = vsnprintf(buf + strlen(buf),len,format,args);
    va_end(args);

    return len;

}


void  cli_k_cmd_init(void)
{
    cli_uint32 i = 0; 

    for(i = 0 ; i < CLI_CMD_MAX_NUM; i++){
        memset(g_cli_cmd_mapping[i].cmd_name,0x00,CLI_CMD_NAME_MAX_LEN);
        g_cli_cmd_mapping[i].handler = NULL;
    }

    g_cli_cmd_num = 0;

}

void cli_k_cmd_reg(cli_int8 * cmd, cli_k_func_t* func)
{
    if(g_cli_cmd_num >= CLI_CMD_MAX_NUM){
        cli_printk("cmd reach max num \n");
        return;
    }

    g_cli_cmd_mapping[g_cli_cmd_num].handler = func; 
    strncpy(g_cli_cmd_mapping[g_cli_cmd_num].cmd_name,cmd,CLI_CMD_NAME_MAX_LEN -1);
    g_cli_cmd_num ++;
}


cli_int32 cli_k_run_cmd(cli_int8 * cmd,void * cli,cli_uint32 argc, cli_int8 ** argv)
{
    cli_uint32 i = 0; 

    for(i = 0 ; i < CLI_CMD_MAX_NUM; i++){
        if(0 == strcmp(g_cli_cmd_mapping[i].cmd_name,cmd)){
            if(g_cli_cmd_mapping[i].handler != NULL){
                return g_cli_cmd_mapping[i].handler(cli,argc,argv);
            }
        }
    }
    return CLI_CMD_E_NO_KERNEL_HDL;
}

static long cli_cmd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    cli_cmd_msg_t *pMsg = NULL;
    cli_int32 ret =  -1;
    cli_int8*argv[CLI_CMD_MAX_ARG_NUM]  = {NULL};
    cli_int32 i  = 0 ;
    
    memset(cfg_value,0x00,sizeof(cfg_value));
    if (cmd != CLI_IOCTL_CMD)
        return ret;
   
    pMsg = (cli_cmd_msg_t*)cfg_value;
    if (copy_from_user(cfg_value, (cli_int8 *)arg, sizeof(cli_cmd_msg_t)))
        return -1;

    for(i = 0; i < pMsg->argc; i++){
        argv[i] = pMsg->argv[i];
    }

    pMsg->ret = cli_k_run_cmd(pMsg->cmd_name,pMsg->ret_buf,pMsg->argc,argv);
    ret = copy_to_user((cli_int8 *)arg , pMsg , sizeof(cli_cmd_msg_t));
    return 0;
}

static struct file_operations cli_ioctl_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = cli_cmd_ioctl,
};

static int cli_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init cli_kioctl_init(void)
{
    int err = 0;
    cli_printk("kernel cli initializing.\n");

    cli_ioctl_major = register_chrdev(0, CLI_IOCTL_NAME, &cli_ioctl_fops);
    if (cli_ioctl_major<0){
        cli_printk("kernel cli init failure\n");
        return cli_ioctl_major;
   }
 
   cli_ioctl_cdev = cdev_alloc();
   cdev_init(cli_ioctl_cdev, &cli_ioctl_fops);
   cli_ioctl_cdev->owner = THIS_MODULE;
   err = cdev_add(cli_ioctl_cdev, MKDEV(cli_ioctl_major, 0), 1);
    
   if (err != 0)
        cli_printk("cli device register failed!\n");
   // Register the device class
   cli_ioctl_class = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(cli_ioctl_class)){                // Check for error and clean up if there is
      unregister_chrdev(cli_ioctl_major, CLI_IOCTL_NAME);
      cli_printk("Failed to register device class\n");
      return PTR_ERR(cli_ioctl_class);          // Correct way to return an error on a pointer
   }

   cli_ioctl_class->dev_uevent = cli_dev_uevent;
   // Register the device driver
   cli_ioctl_device = device_create(cli_ioctl_class, NULL, MKDEV(cli_ioctl_major, 0), NULL, CLI_IOCTL_NAME);
   if (IS_ERR(cli_ioctl_device)){               // Clean up if there is an error
      class_destroy(cli_ioctl_class);           // Repeated code but the alternative is goto statements
      unregister_chrdev(cli_ioctl_major, CLI_IOCTL_NAME);
      cli_printk("Failed to create the device\n");
      return PTR_ERR(cli_ioctl_device);
   }

    cli_k_cmd_init();

    cli_printk("kernel cli init done.\n");

    return 0;
}

static void __exit cli_kioctl_exit(void)
{
    cdev_del(cli_ioctl_cdev);
    device_destroy(cli_ioctl_class, MKDEV(cli_ioctl_major, 0));    
    class_unregister(cli_ioctl_class);                          
    class_destroy(cli_ioctl_class);                           
    unregister_chrdev(cli_ioctl_major, CLI_IOCTL_NAME);        
    cli_printk("%s unregister successfully\n", CLI_IOCTL_NAME);
}

module_init(cli_kioctl_init);
module_exit(cli_kioctl_exit);

EXPORT_SYMBOL_GPL(cli_k_cmd_reg);
EXPORT_SYMBOL_GPL(cli_print);
