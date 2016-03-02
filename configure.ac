AC_INIT([mini-cli], [1.0], [geesun@gmail.com])

AC_PREREQ([2.68])

AC_CONFIG_SRCDIR([src/cli_main.c])

AC_CONFIG_HEADERS([config.h])

AC_SUBST([with_kernel], [`uname -r`])
AC_SUBST([with_kernel_mod], [/lib/modules/$with_kernel/extra])
AC_SUBST([KERNEL_SRC], [/lib/modules/$with_kernel/build])
AC_SUBST([KERNEL_MOD], [$with_kernel_mod])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])


AC_ARG_ENABLE([telnet],
    AS_HELP_STRING([--disable-telnet], [Disable telnet server]))

AM_CONDITIONAL([CLI_HAS_TELNETD], [true])
AS_IF([test "x$enable_telnet" = "xno"], [
  AM_CONDITIONAL([CLI_HAS_TELNETD], [false])
])

AC_ARG_ENABLE([kernel],
              AS_HELP_STRING([--enable-kernel], [Enable kernel space cli]))

AM_CONDITIONAL([HAVE_KERNEL_CLI], [false])
AS_IF([test "x$enable_kernel" = "xyes"], [
   AM_CONDITIONAL([HAVE_KERNEL_CLI], [true])
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

AM_CONDITIONAL([CLI_OS_LINUX], [false]) 
AM_CONDITIONAL([CLI_OS_VXWORKS], [false]) 
AM_CONDITIONAL([CLI_OS_ECOS], [false]) 

case "${with_os}" in
  linux)
    AM_CONDITIONAL([CLI_OS_LINUX], [true]) 
    ;;
  vxworks)
    AM_CONDITIONAL([CLI_OS_VXWORKS], [true]) 
    ;;
  ecos)
    AM_CONDITIONAL([CLI_OS_ECOS], [true]) 
    ;;
esac

# Enable "automake" to simplify creating makefiles:
AM_INIT_AUTOMAKE([1.11 silent-rules subdir-objects foreign -Wall -Werror])
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
