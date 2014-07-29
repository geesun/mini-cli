mini-cli
========


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

Command defines rule
--------
- Keyword must be all in lowercase 

    set max-results 50
    set console-width 120
    set columns user:14 date comments rating likes dislikes category:9 views
    set order views
    
  Example::
     show interface <1-3> 
