AC_INIT([mini-cli], [1.0], [geesun@gmail.com],[https://github.com/geesun/mini-cli])

AC_PREREQ([2.68])

AC_CONFIG_SRCDIR([src/cli_main.c])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])


dnl Example of default-enabled feature
AC_ARG_ENABLE([telnet],
    AS_HELP_STRING([--disable-telnet], [Disable telnet server]))

AS_IF([test "x$enable_telnet" != "xno"], [
  AC_DEFINE([CLI_HAS_TELNETD], [1], [Enable the telnet server ])
])

dnl Example of default-disabled feature
AC_ARG_ENABLE([kernel],
              AS_HELP_STRING([--enable-kernel], [Enable kernel space cli]))

AS_IF([test "x$enable_kernel" = "xyes"], [
                AC_DEFINE([HAVE_KERNEL_CLI], [1], [Enable kernel space cli])
                ])


AC_ARG_WITH(
  [os],
  [AS_HELP_STRING([--with-os=os type], [build with the given target os, TYPE=linux|vxworks|ecos @<:@default=linux@:>@])],
  [
    case "${withval}" in
      linux|vxworks|ecos) ;;
      *) AC_MSG_ERROR([bad value ${withval} for --with-os]) ;;
    esac
  ],
  [with_os="linux"]
)

case "${with_os}" in
  linux)
    AC_DEFINE([CLI_OS_LINUX], [1], [Build for Linux systems])
    ;;
  vxworks)
    AC_DEFINE([CLI_OS_VXWORKS], [1], [Build for vxWorks systems])
    ;;
  ecos)
    AC_DEFINE([CLI_OS_ECOS], [1], [Build for Ecos systems])
    ;;
esac

# Enable "automake" to simplify creating makefiles:
AM_INIT_AUTOMAKE([1.11 silent-rules subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])

# Checks for programs, e.g., AC_PROG_CC
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR
AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([pthread.h stdarg.h string.h fcntl.h unistd.h sys/types.h sys/socket.h arpa/inet.h ])
# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_SEARCH_LIBS([pthread_create], [pthread])
# Do final output.
AC_OUTPUT
