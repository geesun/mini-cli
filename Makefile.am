ACLOCAL_AMFLAGS=-I m4
AUTOMAKE_OPTIONS = foreign
EXTRA_DIST=demo/module/Makefile \
		   src/inc/cli_api.h \
		   src/inc/cli_auth.h \
		   src/inc/cli_cmd.h \
		   src/inc/cli_error.h \
		   src/inc/cli_history.h \
		   src/inc/cli_history_item.h \
		   src/inc/cli_node.h \
		   src/inc/cli_parser.h \
		   src/inc/cli_readline.h \
		   src/inc/cli_server.h \
		   src/inc/cli_shell.h \
		   src/inc/cli_tty.h \
		   src/inc/cli_utils.h \
		   src/inc/cli_vector.h \
		   src/kmod/inc/cli_cmn.h \
		   src/kmod/inc/cli_kmod_msg.h \
		   src/kmod/kernel/cli_kmod_kern.c \
		   src/kmod/kernel/Makefile \
		   src/kmod/kernel/cli_kmod_kern.h  \
		   demo/cli_demo.h \
		   README.rst

include_HEADERS = src/kmod/inc/cli_cmn.h \
				  src/inc/cli_error.h \
				  src/inc/cli_api.h 

VERSION_INFO = 1:0:0

CLI_FLAGS = -Isrc/inc  -Isrc/kmod/inc
if CLI_HAS_TELNETD
CLI_FLAGS += -DCLI_HAS_TELNETD
endif

if HAVE_KERNEL_CLI
include_HEADERS += src/kmod/inc/cli_cmn.h 
endif 

if CLI_OS_LINUX
CLI_FLAGS += -DCLI_OS_LINUX
endif

if CLI_OS_VXWORKS
CLI_FLAGS += -DCLI_OS_VXWORKS
endif

if CLI_OS_ECOS
CLI_FLAGS += -DCLI_OS_ECOS
endif

noinst_PROGRAMS = cli_demo
cli_demo_SOURCES = \
				   demo/cli_demo.c 
if HAVE_KERNEL_CLI
cli_demo_SOURCES += \
				   demo/cli_kmod_demo.c 
CLI_FLAGS += -DHAVE_KERNEL_CLI
endif
cli_demo_CFLAGS = $(CLI_FLAGS) -Idemo/
cli_demo_LDFLAGS = -lmini_cli 

lib_LTLIBRARIES = libmini_cli.la
libmini_cli_la_SOURCES =  \
	src/cli_auth.c \
	src/cli_builtin_cmd.c \
	src/cli_cmd.c \
	src/cli_console.c \
	src/cli_error.c \
	src/cli_history.c \
	src/cli_history_item.c \
	src/cli_main.c \
	src/cli_node.c \
	src/cli_parser.c \
	src/cli_readline.c \
	src/cli_server.c \
	src/cli_shell.c \
	src/cli_telnet.c \
	src/cli_tty.c \
	src/cli_utils.c \
	src/cli_vector.c 

if HAVE_KERNEL_CLI
libmini_cli_la_SOURCES +=  \
	src/kmod/user/cli_kmod_user.c 
endif

libmini_cli_la_CFLAGS = $(CLI_FLAGS)
libmini_cli_la_LDFLAGS = -version-info $(VERSION_INFO)

if HAVE_KERNEL_CLI
demo/module/minicli.ko:
	 test -f $(builddir)/demo/module/cli_kmod_demo.c  || \
		$(LN_S) $(abs_top_srcdir)/demo/cli_kmod_demo.c $(builddir)/demo/module/cli_kmod_demo.c ;
	 cp $(abs_top_builddir)/src/kmod/kernel/Module.symvers  $(abs_top_srcdir)/demo/module
	 $(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/demo/module EXTRA_CFLAGS="-I$(abs_top_srcdir)/src/kmod/inc -I$(abs_top_srcdir)/src/inc -DHAVE_KERNEL_CLI"

src/kmod/kernel/miniclidev.ko:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/src/kmod/kernel EXTRA_CFLAGS="-I$(abs_top_srcdir)/src/kmod/inc -I$(abs_top_srcdir)/src/inc -DHAVE_KERNEL_CLI"

all-local: src/kmod/kernel/miniclidev.ko demo/module/minicli.ko

clean-local:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/src/kmod/kernel clean
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/demo/module clean

endif
