#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rct_payload], [1.0], [nthui@eng.ucsd.edu])
AM_PROG_CC_C_O
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/sdr_record/maxmin.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for python.
AM_PATH_PYTHON(2.7)
AC_PATH_PROGS(BASH, bash sh)

# Checks for programs.
AC_PROG_SED

# Checks for libraries.
PKG_CHECK_MODULES([rtlsdr], [librtlsdr])

# Checks for Python libraries
AC_PYTHON_MODULE([pynmea2], [fatal])
# AC_PYTHON_MODULE([pymavlink], [fatal])
AC_PYTHON_MODULE([serial], [fatal])

# Checks for header files.
AC_CHECK_HEADERS([float.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime sqrt])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 scripts/Makefile
                 scripts/autostart/Makefile
                 scripts/gps_logger/Makefile
                 src/Makefile
                 src/sdr_record/Makefile])
AC_OUTPUT
