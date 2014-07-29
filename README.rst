mini-cli
========
The mini-cli is define for embedded device, it make the command line define as easy as we define a function. 
It also support define the command line which the callback in kernel space.

Components
--------
- Command auto complete
- Authentication management
- Argument validation
- Console command line 
- Multiple Session telnet server 
- Command hook in Linux kernel space
- Error code interpretation to format string
- Virtual terminal(Vt100) short cut support
- Command history management

Example command define
-------
- Define the command ::

      CLI_DEFINE(
          cli_handler_cwmp_debug,
          cli_cmd_cwmp_debug,
          "debug (stack|cm) (on|off)",
          "Cwmp debug setting \n"
          "Stack debug\n"
          "Configure management debug\n"
          "Enable the debug \n"
          "Disable the debug\n"
      )
      {
          cs_uint8 debug = 0;
          cs_uint8 cm = 0;
      
          if(strcmp(argv[0],"cm") == 0){
              cm = 1;
          }
      
          if(strcmp(argv[1],"on") == 0){
              debug = 1;
          }
          
          /* do stuff */
          
          return CLI_CMD_OK;
      }
      
      /* install the command */
      cli_install_pri_cmd(CLI_PRI_DEBUG,CLI_NODE_CWMP,&cli_cmd_cwmp_debug);

- Define the command which callback in Linux kernel space ::

        CLI_KERNEL_DEFINE(
                cli_handle_kernel,
                cli_kernel_cmd,
                "show kernel <1-20000> ",
                "show information\n"
                "kernel config \n"
                "block id"
                )
        #ifdef __KERNEL__
        {
            cli_print(cli,"block id = %s \n",argv[0]);
            /*call any kernel space function*/
            return CLI_CMD_E_PARAM;
        }
        #endif
      
        /* this line must be called in userspace and kernel space */
        CLI_KERNEL_CMD_INSTALL(CLI_NODE_ID_CONFIG,&cli_kernel_cmd);
      
Command define rules
--------
- Lowercase word is keyword ::

   show interface <1-3> 
     
- Uppercase word is input anything ::

   add user NAME
   
- Range define between < and > ::
 
   show interface <0-5>
   
- Mac address define A:B:C:D:E:F ::

   show fdb A:B:C:D:E:F
   
- IPv4 Address define as A.B.C.D ::

   ping A.B.C.D
   
- IPv6 address define as A::B ::

   ping A::B 
   
- Option parameter should at end and define between [ and ] ::

   add USER [PASSWD] 
   
- One parameter support more then one type, use | to split the type and surround with () ::

   debug (on|off) 
   config ip (A.B.C.D|A::B) 
   
 
   
