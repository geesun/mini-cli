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

VERSION_INFO = 1:0:0

FLAGS=-Isrc/inc -Idemo/ -Isrc/kmod/inc 

bin_PROGRAMS = mini_cli
mini_cli_SOURCES = \
				   demo/cli_demo.c \
				   demo/cli_kmod_demo.c 
mini_cli_CPPFLAGS = $(FLAGS)
mini_cli_LDFLAGS = -lmini_cli 

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
	src/cli_vector.c \
	src/kmod/user/cli_kmod_user.c 

libmini_cli_la_CPPFLAGS = $(FLAGS)
libmini_cli_la_LDFLAGS = -version-info $(VERSION_INFO)

demo/module/minicli.ko:
	 test -f $(builddir)/demo/module/cli_kmod_demo.c  || \
		$(LN_S) $(abs_top_srcdir)/demo/cli_kmod_demo.c $(builddir)/demo/module/cli_kmod_demo.c ;
	 $(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/demo/module EXTRA_CFLAGS="-I$(abs_top_srcdir)/src/kmod/inc -I$(abs_top_srcdir)/src/inc -DHAVE_KERNEL_CLI"

src/kmod/kernel/miniclidev.ko:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/src/kmod/kernel EXTRA_CFLAGS="-I$(abs_top_srcdir)/src/kmod/inc -I$(abs_top_srcdir)/src/inc -DHAVE_KERNEL_CLI"

all-local: demo/module/minicli.ko src/kmod/kernel/miniclidev.ko

clean-local:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/src/kmod/kernel clean
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/demo/module clean
