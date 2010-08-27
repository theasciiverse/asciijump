# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)
#VERSION=`cat VERSION` #anyone fancy to change it ??
AC_INIT(asciijump, "1.0.2beta", g.moskal@opengruop.org)
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB(slang, SLsmg_cls, [], [AC_ERROR("S-Lang library not found! (try install slang-devel)")])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([slang.h slang/slang.h stdlib.h string.h unistd.h])
AC_DEFINE(Version, $(cat V))
#AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
#AC_CHECK_HEADER([slang.h], [AC_DEFINE_UNQUOTED(HAVE_SLANG_H, 1)],
#		[AC_CHECK_HEADER([slang/slang.h])])                                           .

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
m4_ifdef([AC_FUNC_REALLOC], [AC_FUNC_REALLOC])
AC_CHECK_FUNCS([getcwd strdup])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
