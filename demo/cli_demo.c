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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cli_demo.h"
CLI_DEFINE(
        cli_handle_test,
        test_cmd1,
        "show info (test1|test2) NAME <1-3> test [(opt1|pt02)]",
        "Show interface information\n"
        "inter face id \n"
        "test1 param \n"
        "test2 param \n"
        "NAME param \n"
        "<1-3> param \n"
        "test keyword \n"
        "opt1 param \n"
        "opt3 param \n"
        "")
{
    cli_uint32 i = 0; 

    if(argc == 0){
        cli_print(cli,"\r\nno parameter \r\r\n");
    }
    for(; i < argc; i++){
        cli_print(cli,"param[%d] = %s \r\r\n",i,argv[i]);
    }
    return CLI_CMD_E_NO_RESOURCE;
}

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd2, 
        "config info (test|test2) STR <3-4> [<5-6>]",
        "config interface information\n"
        "inter face id \n"
        "test param2 \n"
        "test2 param2 \n"
        "STR param3 \n"
        "<3-4> param3 \n"
        "<5-6> param3 \n"
        "test param3 \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd3,
        "debug info ",
        "debug information\n"
        "inter face id \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd4,
        "showcomplete  test <1-2> <3-4> [(<5-6>|<7-8>)]",
        "Showcomplete desc \n"
        "inter face id \n"
        "<1-2> - desc \n"
        "<3-4> - desc \n"
        "<5-6> - desc \n"
        "<7-8> - desc \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd15,
        "show test <3-4> [(<5-6>|<7-8>)]",
        "Show desc \n"
        "inter face id \n"
        "<3-4> - desc \n"
        "<5-6> - desc \n"
        "<7-8> - desc \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd5,
        "show infocomplete  igmp <1-2> <3-4> [<5-6>]",
        "Show interface \n"
        "infomation  complete\n"
        "igmp info \n"
        "<1-2> param2 \n"
        "<3-4> param3 \n"
        "<5-6> param4 \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd6,
        "con_no_param",
        "config with no parameter \n");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd7,
        "con_one_param <1-3>",
        "config with one parameter \n"
        "parameter one ");

CLI_DEFINE_ALIAS(
        cli_handle_test,
        test_cmd8,
        "config con_no_param",
        "config \n config with no parameter \n");

CLI_DEFINE(
        cli_handle_test9,
        test_cmd9,
        "config interactive <1-3>",
        "config \n interactive commmand \n"
        "parameter one \n")
{
    cli_int8   answer[10] = {0};

    cli_get_input(cli,
                "Give me a string with len 10, I will print for you:",answer,10);

    cli_print(cli,"test = %s \n",answer);
    return CLI_CMD_OK;
}

/*demo command */
CLI_DEFINE(
        cli_handle_test10,
        test_cmd10,
        "show port (eth0|<1-3>)",
        "show interface\n"
        "show port information\n"
        "eth0 inforamtion \n"
        "port id\n")
{
    cli_uint32 context_port = 0;
    cli_print(cli,"port id = %s \n",argv[0]);

    context_port = cli_context_get(cli,0);
    cli_print(cli,"context port id = %d \n", context_port);
    return CLI_CMD_OK;
}

CLI_DEFINE(
        cli_handle_test11,
        test_cmd11,
        "add <1-4094> <0-1> <0-7> [(dynamic|static)]",
        "Add fdb entry\n"
        "Vlan Id\n"
        "SC IND\n"
        "802.1p\n"
        "Dynamic \n"
        "Static ")
{
    cli_print(cli,"vlan id = %s \n",argv[0]);
    cli_print(cli,"sc ind = %s \n",argv[1]);
    cli_print(cli,"802.1p = %s \n",argv[2]);

    if(argc == 4){
        cli_print(cli,"static = %s \n",argv[3]);
    }
    return CLI_CMD_OK;
}


CLI_DEFINE(
        cli_handle_test12,
        test_cmd12,
        "fib set  <1-200> <0-2> (valid|invalid)",
        "fib configuration\n"
        "set \n"
        "fib id\n"
        "fib type: 0 - pon mapping, 1 - cvlan mapping, 2 - s-vlan mapping \n"
        "dscp valid\n"
        "dscp invalid")
{
    cli_print(cli,"fib id = %s \n",argv[0]);
    cli_print(cli,"fib type = %s \n",argv[1]);
    cli_print(cli,"dscp = %s \n",argv[2]);

    return CLI_CMD_OK;
}



CLI_DEFINE(
        cli_handle_test13,
        test_cmd13,
        "rule add  <0-2> A:B:C:D:E:F A:B:C:D:E:F  A.B.C.D A::B",
        "classification rule\n"
        "add rule\n"
        "mode: 0 - normal , 1 - DA range , 2 - SA range\n"
        "dst mac  \n"
        "src mac \n"
        "IPV4 Address \n"
        "IPv6 ADDRESS")
{
    cli_print(cli,"mode  = %s \n",argv[0]);
    cli_print(cli,"dst mac = %s \n",argv[1]);
    cli_print(cli,"src mac = %s \n",argv[2]);
    cli_print(cli,"IPv4 address = %s \n",argv[3]);
    cli_print(cli,"IPv6 address = %s \n",argv[4]);

    return CLI_CMD_OK;
}



CLI_DEFINE(cli_node_enter_sdk,cli_cmd_enter_sdk,"sdk",
        "Enter sdk node\n"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_SDK,"-sdk");

    return CLI_OK;
}


CLI_DEFINE(cli_node_enter_port,cli_cmd_enter_port,"port (ge|fe|pon) <1-4>",
        "Enter port node\n"
        "GE port\n"
        "FE port\n"
        "PON port\n"
        "Port Id"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_PORT,"-port-%s-%s",argv[0],argv[1]);
    
    cli_context_set(cli,0,atoi(argv[1]));

    return CLI_OK;
}



CLI_DEFINE(cli_node_enter_vlan,cli_cmd_enter_vlan,"vlan",
        "Enter vlan node\n"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_VLAN,"-vlan");

    return CLI_OK;
}


CLI_DEFINE(cli_node_enter_fdb,cli_cmd_enter_fdb,"fdb",
        "Enter fdb node\n"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_FDB,"-fdb");

    return CLI_OK;
}


CLI_DEFINE(cli_node_enter_qos,cli_cmd_enter_qos,"cls",
        "Enter cls node\n"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_QOS,"-cls");

    return CLI_OK;
}


CLI_DEFINE(cli_idle_time_hdl,cli_cmd_idle_time,"timeout <3-60>",
        "Set session idle timeout\n"
        "Idle timeout (minutes)\n"
        )
{
    cli_session_idle_time_set(cli,atoi(argv[0]) * 60  * 1000);
    return CLI_OK;
}



CLI_DEFINE(
        cli_handle_match_test,
        test_match_cmd,
        "config port",
        "config \n"
        "port\n")
{
    cli_print(cli,"config port\n");
    return CLI_OK;
}


CLI_DEFINE(
        cli_handle_match_test2,
        test_match_cmd2,
        "config portinfo ",
        "config \n"
        "port\n")
{
    cli_print(cli,"config portinfo\n");
    return CLI_OK;
}



CLI_DEFINE(
        cli_handle_match_test3,
        test_match_cmd3,
        "config port <1-0xffffffff>",
        "config \n"
        "port\n")
{
    cli_print(cli,"config port <1-3>\n");
    return CLI_OK;
}


CLI_DEFINE(
        cli_handle_match_test4,
        test_match_cmd4,
        "config portinfo <0x1-0xfff>",
        "config \n"
        "port\n")
{
    cli_print(cli,"config portinfo <0x1-0x3>\n");
    return CLI_OK;
}

CLI_DEFINE(
        cli_handle_one_char_handler,
        test_one_char_handler_cmd,
        "head p",
        "char test\n"
        "p\n")
{
    cli_print(cli,"char p \n");
    return CLI_OK;
}


CLI_DEFINE(
        cli_handle_one_char_handler_o,
        test_one_char_o_handler_cmd,
        "head o",
        "char test\n"
        "o\n")
{
    cli_print(cli,"char o \n");
    return CLI_OK;
}

#if 0
void * cli_test_timer_add(
        cli_uint32 timeout,
        cli_shell_t * shell,
        cli_timeout_handle_func_t * hdr
        )
{
    return util_timer_add(
            TIMER_TYPE_ONCE,
            timeout,
            (util_timer_handler_t*)hdr,
            shell);
}

void  cli_test_timer_del(
        void * timer
        )
{
    util_timer_del((util_timer_t*)timer);
}

#endif 

char demo_logo[] = "\
               \r\n\r\n\
               .88b  d88. d888888b d8b   db d888888b        db       .d88b.   d888b   .d88b. \r\n\
               88'YbdP`88   `88'   888o  88   `88'          88      .8P  Y8. 88' Y8b .8P  Y8.\r\n\
               88  88  88    88    88V8o 88    88           88      88    88 88      88    88\r\n\
               88  88  88    88    88 V8o88    88    C8888D 88      88    88 88  ooo 88    88\r\n\
               88  88  88   .88.   88  V888   .88.          88booo. `8b  d8' 88. ~8~ `8b  d8'\r\n\
               YP  YP  YP Y888888P VP   V8P Y888888P        Y88888P  `Y88P'   Y888P   `Y88P' \r\n\
    \r\n\r\n";

int main()
{
    cli_init(CLI_NODE_ID_MAX,0);
    cli_logo_set(demo_logo,strlen(demo_logo));
    cli_hostname_set("MINI-CLI",strlen("MINI-CLI"));
    
    cli_global_cmd_node_list(CLI_NODE_ID_CONFIG,
            CLI_NODE_ID_PORT,
            CLI_NODE_ID_SDK,
            CLI_NODE_ID_INVALID);

    cli_install_global_cmd(&test_one_char_handler_cmd);
    cli_global_cmd_node_range(CLI_NODE_ID_CONFIG,CLI_NODE_ID_TEST7);

    cli_install_global_cmd(&test_one_char_o_handler_cmd);


    cli_add_node(CLI_NODE_ID_SDK);
    cli_add_node(CLI_NODE_ID_PORT);
    cli_add_node(CLI_NODE_ID_FDB);
    cli_add_node(CLI_NODE_ID_QOS);
    cli_add_node(CLI_NODE_ID_VLAN);

    cli_install_cmd(CLI_NODE_ID_CONFIG,&cli_cmd_enter_sdk);
    cli_install_cmd(CLI_NODE_ID_SDK,&cli_cmd_enter_port);
    cli_install_cmd(CLI_NODE_ID_SDK,&cli_cmd_enter_vlan);
    cli_install_cmd(CLI_NODE_ID_SDK,&cli_cmd_enter_qos);
    cli_install_cmd(CLI_NODE_ID_SDK,&cli_cmd_enter_fdb);

    cli_install_cmd(CLI_NODE_ID_CONFIG,&cli_cmd_idle_time);

    /* install custom command */
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd1);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd2);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd3);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd4);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd5);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd6);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd7);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd8);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd9);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_cmd15);

    cli_install_cmd(CLI_NODE_ID_PORT,&test_cmd10);
    cli_install_cmd(CLI_NODE_ID_VLAN,&test_cmd12);
    cli_install_cmd(CLI_NODE_ID_QOS,&test_cmd13);
    cli_install_cmd(CLI_NODE_ID_FDB,&test_cmd11);

    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_match_cmd);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_match_cmd2);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_match_cmd3);
    cli_install_cmd(CLI_NODE_ID_CONFIG,&test_match_cmd4);

#if 0
    cli_session_timer_set(cli_test_timer_add,cli_test_timer_del);
    cli_session_idle_time_set(NULL,1000*3*60);
#endif
#ifdef CLI_KMOD_DEMO
    cli_k_demo_init();
#endif
#if 1
	cli_server_start(0);
#else
    cli_telnetd_start(2300);
    while(1){
        sleep(10);
    }
#endif
    return 0;
}
