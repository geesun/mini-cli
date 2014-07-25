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


#include "cli_api.h"
#include "cli_shell.h"
#include "cli_auth.h"

/*lint -e64*/
static cli_user_t g_cli_users[CLI_MAX_USER_NUM];
static cli_boolean  g_cli_enable_auth = TRUE;
cli_boolean cli_user_exist(
        cli_int8 * name)
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(name != NULL,FALSE);

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
                ){
            return TRUE;
        }
    }

    return FALSE;
}

cli_cmd_status_t cli_user_add(
        cli_int8  perm,
        cli_int8 * name,
        cli_int8 * passwd)
{
    cli_uint32 i = 0;
    cli_uint32 len = 0;

    CLI_ASSERT_RET(
            name != NULL 
            && passwd != NULL,
            CLI_CMD_E_PARAM);
    
    if(cli_user_exist(name)){
        return CLI_CMD_E_USER_EXIST;
    }

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(!g_cli_users[i].valid){
            break;
        }
    }

    if( i == CLI_MAX_USER_NUM){
        return CLI_CMD_E_USER_TOO_MANY;
    }

    g_cli_users[i].valid = TRUE;
    g_cli_users[i].perm = perm;

    len = strlen(name) > CLI_MAX_USER_NAME_LEN? CLI_MAX_USER_NAME_LEN:strlen(name); 
    strncpy(g_cli_users[i].user,name,len);
    len = strlen(passwd) > CLI_MAX_USER_PASSWD_LEN? CLI_MAX_USER_PASSWD_LEN:strlen(passwd); 
    strncpy(g_cli_users[i].passwd,passwd,len);

    return CLI_CMD_OK;
}

cli_cmd_status_t cli_user_del(
        cli_int8 * name
        )
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(
            name != NULL ,
            CLI_CMD_E_USER_NOT_EXIST);

    if(!cli_user_exist(name)){
        return CLI_CMD_E_USER_NOT_EXIST;
    }

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
          ){
             memset(&g_cli_users[i],0x00,sizeof(g_cli_users[i]));
             g_cli_users[i].valid = FALSE;
        }
    }

    return CLI_CMD_OK ;
}

cli_cmd_status_t cli_user_passwd_modify(
        cli_int8 * name,
        cli_int8 * old_passwd,
        cli_int8 * new_passwd)
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(
            name != NULL 
            && old_passwd != NULL 
            && new_passwd != NULL ,
            CLI_CMD_E_USER_NOT_EXIST);

    if(!cli_user_exist(name)){
        return CLI_CMD_E_USER_NOT_EXIST;
    }
    
    if(!cli_user_valid(name,old_passwd)){
        return CLI_CMD_E_USER_WRONG_PASSWD;
    }

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
          ){
             memcpy(g_cli_users[i].passwd,new_passwd,CLI_MAX_USER_PASSWD_LEN);
             break;
        }
    }

    return CLI_CMD_OK ;
}

cli_cmd_status_t cli_user_passwd_reset(
        cli_int8 * name,
        cli_int8 * new_passwd)
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(
            name != NULL 
            && new_passwd != NULL ,
            CLI_CMD_E_USER_NOT_EXIST);

    if(!cli_user_exist(name)){
        return CLI_CMD_E_USER_NOT_EXIST;
    }

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
          ){
             memcpy(g_cli_users[i].passwd,new_passwd,CLI_MAX_USER_PASSWD_LEN);
             break;
        }
    }

    return CLI_CMD_OK ;
}


cli_boolean cli_user_valid(
        cli_int8 * name,
        cli_int8 * passwd)
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(
            name != NULL 
            && passwd != NULL,
            FALSE);
    
    if(!cli_user_exist(name)){
        return FALSE;
    }

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
                &&strcmp(g_cli_users[i].passwd,passwd) == 0
          ){
            return TRUE;
        }
    }

    return FALSE;
}

cli_int8 cli_user_get_perm(
        cli_int8 * name)
{
    cli_uint32 i = 0;

    CLI_ASSERT_RET(
            name != NULL ,
            CLI_PRI_INVALID);

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        if(g_cli_users[i].valid 
                &&strcmp(g_cli_users[i].user,name) == 0
          ){
            return g_cli_users[i].perm;
        }
    }

    return CLI_PRI_INVALID;
}


cli_boolean cli_user_login(
        cli_shell_t * shell,
        cli_int32 retry_time)
{
    cli_int8 user[CLI_MAX_USER_NAME_LEN + 1 ] = {0};
    cli_int8 password[CLI_MAX_USER_PASSWD_LEN + 1] = {0};
    cli_uint32 len = 0;
    cli_int32 i = 0;
    cli_boolean valid = FALSE;

    CLI_ASSERT_RET(shell != NULL,FALSE);

    if(!g_cli_enable_auth){
        cli_shell_set_user_pri(shell,CLI_PRI_DEBUG);
        return TRUE;
    }

    do{
        memset(user,0x00,sizeof(user));
        memset(password,0x00,sizeof(password));
        len = cli_get_input(shell,"User:",user,CLI_MAX_USER_NAME_LEN);
        if(len == 0){
            continue;
        }
        
        cli_echo_enable(shell,FALSE);
        cli_get_input(shell,"Password:",password,CLI_MAX_USER_PASSWD_LEN);
        cli_echo_enable(shell,TRUE);
        
        valid = cli_user_valid(user,password);

        if(!valid){
            cli_print(shell,"Login failed \n");
            i++;
        }else{

            cli_shell_set_user_pri(shell,cli_user_get_perm(user));
            
            return TRUE;
        }
    }while(!valid && (i < retry_time || retry_time == -1));

    return FALSE;
}

CLI_DEFINE(
        cli_user_add_handler,
        cli_cmd_user_add,
        "user add <1-3> NAME  PASSWORD",
        "User management\n"
        "Add User \n"
        "User Privilege, 1 - Normal user, 2 - Power user, 3 - Admin user \n"
        "User Name\n"
        "User password \n")
{
    cli_int32 pri = 0; 
    
    pri = atoi(argv[0]);
    return cli_user_add(pri, argv[1],argv[2]);
}

CLI_DEFINE(
        cli_user_del_handler,
        cli_cmd_user_del,
        "user del NAME",
        "User management\n"
        "Delete User \n"
        "User Name\n"
        )
{
    return cli_user_del(argv[0]);
}


CLI_DEFINE(
        cli_user_passwd_modify_handler,
        cli_cmd_user_modify,
        "user passwd NAME",
        "User management\n"
        "Change user password\n"
        "User Name\n"
        )
{
    cli_int8 old_passwd[CLI_MAX_USER_PASSWD_LEN + 1] = {0};
    cli_int8 new_passwd[CLI_MAX_USER_PASSWD_LEN + 1] = {0};
    cli_int8 confirm_passwd[CLI_MAX_USER_PASSWD_LEN + 1] = {0};


    cli_echo_enable(cli,FALSE);
    cli_get_input(cli,"Old password:",old_passwd,CLI_MAX_USER_PASSWD_LEN);
    cli_get_input(cli,"New password:",new_passwd,CLI_MAX_USER_PASSWD_LEN);
    cli_get_input(cli,"Confirm password:",confirm_passwd,CLI_MAX_USER_PASSWD_LEN);
    cli_echo_enable(cli,TRUE);

    if(strncmp(new_passwd,confirm_passwd,CLI_MAX_USER_PASSWD_LEN) != 0){
        return CLI_CMD_E_USER_NEW_PASSWD_NOT_MATCH;
    }
    return cli_user_passwd_modify(argv[0],old_passwd,new_passwd);
}

void cli_auth_init(cli_boolean auth)
{
    cli_int32 i = 0;

    memset(g_cli_users,0x00,sizeof(g_cli_users));

    for(i = 0; i < CLI_MAX_USER_NUM; i++){
        g_cli_users[i].valid = FALSE;
    }
    
    g_cli_enable_auth = auth;

    cli_user_add(CLI_PRI_NORMAL,"guest","guest");
    cli_user_add(CLI_PRI_POWER,"user","user");
    cli_user_add(CLI_PRI_ADMIN,"admin","admin");

    cli_install_pri_cmd(CLI_PRI_DEBUG,CLI_NODE_ID_CONFIG,&cli_cmd_user_add);
    cli_install_pri_cmd(CLI_PRI_DEBUG,CLI_NODE_ID_CONFIG,&cli_cmd_user_del);
    cli_install_pri_cmd(CLI_PRI_DEBUG,CLI_NODE_ID_CONFIG,&cli_cmd_user_modify);
}

/*lint +e64*/


