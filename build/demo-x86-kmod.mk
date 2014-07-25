include $(TOOL_ROOT)/configs/x86-linux.mk
include $(TOOL_ROOT)/cmn.mk

SOURCES=	\
			$(CMN_SRCS) \
			src/kmod/user/cli_kmod_user.c

TARGET_STATIC_LIB=libcli.a
TARGET_STATIC_SRCS= \
					$(SOURCES)

TARGET_STATIC_CFLAGS=-Isrc/inc/ -DCLI_OS_LINUX -Isrc/kmod/inc

TARGET_SHARE_LIB=libcli.so
TARGET_SHARE_SRCS= \
				   $(SOURCES) 
TARGET_SHARE_CFLAGS= $(TARGET_STATIC_CFLAGS) -fPIC
TARGET_SHARE_LD_FLAGS= 

TARGET_PROG =cli_demo
TARGET_PROG_SRCS = \
				   $(SOURCES)  \
				   demo/cli_demo.c \
				   demo/cli_kmod_demo.c 
			
TARGET_PROG_CFLAGS = $(TARGET_STATIC_CFLAGS) -Idemo -DCLI_KMOD_DEMO -DCLI_HAS_TELNETD -DCLI_OS_LINUX -g
TARGET_PROG_LD_FLAGS =-lpthread -lrt

TARGET_KERNEL=libcli
TARGET_KERNEL_SRCS=src/kmod/kernel/cli_kmod_kern.c \
				   demo/cli_kmod_demo.c


TARGET_KERNEL_INC=src/inc  src/kmod/inc
TARGET_KERNEL_CFLAGS= -DCLI_KERNEL_SPACE -DCLI_OS_LINUX



RELEASE_SOURCES=$(CMN_REL_SRCS) \
				$(KMOD_RES_SRCS)

