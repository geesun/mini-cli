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


#ifndef __CLI_DEMO_H__
#define __CLI_DEMO_H__
#include "cli_cmn.h"

typedef enum{
    CLI_NODE_ID_SDK   =    CLI_NODE_ID_USER_START,
    CLI_NODE_ID_FDB   ,   
    CLI_NODE_ID_QOS   ,  
    CLI_NODE_ID_VLAN  , 
    CLI_NODE_ID_PORT  , 
    CLI_NODE_ID_TEST  , 
    CLI_NODE_ID_TEST1 , 
    CLI_NODE_ID_TEST2 , 
    CLI_NODE_ID_TEST3 , 
    CLI_NODE_ID_TEST4 , 
    CLI_NODE_ID_TEST5 ,  
    CLI_NODE_ID_TEST6 ,  
    CLI_NODE_ID_TEST7 ,  
    CLI_NODE_ID_MAX   ,  
}cli_demo_node_id_t;
#endif
