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

/*This file should compile both at user space and kernel space */

#ifdef HAVE_KERNEL_CLI
#ifdef __KERNEL__
#include <linux/module.h>
#endif
#include "cli_cmn.h"

extern void cli_printk(const char * fmt, ...);

CLI_KERNEL_DEFINE(
        cli_handle_kernel,
        kernel_cmd,
        "kernel info <1-2> ",
        "debug information\n"
        "inter face id \n")
#ifdef __KERNEL__
{
    unsigned int i = 0; 

    if(argc == 0){
        cli_print(cli,"no parameter \n");
    }
    for(; i < argc; i++){
        cli_print(cli,"\n\nparam[%d] = %s \n",i,argv[i]);
    }
    cli_print(cli,"cli done \r\r\n");
    return CLI_CMD_OK;
}
#endif


typedef struct {
    unsigned char  us_tc;
    unsigned char  us_mark_flag;
    unsigned char  us_cos;
    unsigned char  us_cos_te;
    unsigned char  us_color;

    unsigned char  ds_tc;
    unsigned char  ds_mark_flag;
    unsigned char    ds_cos;
    unsigned char    ds_cos_te;
    unsigned char    ds_color;
}aal_qos_map_rx_tc_cfg_t;


CLI_KERNEL_DEFINE(
        cli_handle_kernel1,
        kernel_cmd1,
        "kernel show <1-20000> ",
        "debug information\n"
        "inter face id \n")
#ifdef __KERNEL__
{
#if 1
    unsigned int i = 0; 
    aal_qos_map_rx_tc_cfg_t  tc_cfg; 

    if(argc == 0){
        cli_print(cli,"no parameter \n");
    }
    for(; i < argc; i++){
        cli_print(cli,"param[%d] = %s \n",i,argv[i]);
    }

    for(i = 0; i < 10; i++){
        cli_print(cli, "%-3u %-5u %-7u %-6u %-9u %-8u %-5u %-7u %-6u %-9u %-8u\r\n", 
                i, 
                tc_cfg.us_tc,
                tc_cfg.us_mark_flag, 
                tc_cfg.us_cos, 
                tc_cfg.us_cos_te,
                tc_cfg.us_color, 
                tc_cfg.ds_tc, 
                tc_cfg.ds_mark_flag, 
                tc_cfg.ds_cos, 
                tc_cfg.ds_cos_te, 
                tc_cfg.ds_color);

    }
#endif 
    cli_print(cli,"cli done \n");
    return CLI_CMD_OK;
}
#endif

#ifdef __KERNEL__
static int __init  cli_k_demo_init(void)
#else
int cli_k_demo_init(void)
#endif
{
    CLI_KERNEL_CMD_INSTALL(CLI_NODE_ID_CONFIG,&kernel_cmd);
    CLI_KERNEL_GLOBAL_CMD_INSTALL(&kernel_cmd1);
    return 0;
}

#ifdef __KERNEL__
static void __exit cli_k_demo_exit(void)
{
    
}
MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Qixiang Xu");    
MODULE_DESCRIPTION("Demo Command line");  
MODULE_VERSION("0.1"); 
module_init(cli_k_demo_init);
module_exit(cli_k_demo_exit);
#endif
#endif
