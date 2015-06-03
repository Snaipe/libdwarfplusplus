AC_PREREQ([2.60])

AC_INIT([libdwarf++], [1.0.0], [], [libdwarf++], [franklinmathieu@gmail.com])
AC_CONFIG_SRCDIR([src/dwarf.cc])

LT_PREREQ([2.2.4])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip foreign subdir-objects parallel-tests color-tests])
LT_INIT([shared static])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX

AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

AC_PROG_MAKE_SET
AC_SUBST([LIBTOOL_DEPS])

AC_ARG_ENABLE([gcov],
  [AS_HELP_STRING([--enable-gcov],
    [Compile the project with converage enabled])],
    [COVERAGE_CFLAGS="-O0 -fprofile-arcs -ftest-coverage"
     COVERAGE_LDFLAGS="-lgcov"
     AC_SUBST([COVERAGE_CFLAGS])
     AC_SUBST([COVERAGE_LDFLAGS])
    ],
    [])

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
