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
   
 
   
