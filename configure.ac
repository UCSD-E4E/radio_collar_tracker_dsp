#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rct_payload], [2.0], [nthui@eng.ucsd.edu])
AC_PROG_CXX
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for python.
AM_PATH_PYTHON(2.7)
AC_PATH_PROGS(BASH, bash sh)

# Checks for programs.
AC_PROG_SED
AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
if test -z "$PDFLATEX"; then
	AC_MSG_WARN([Unable to create user manuals!])
fi
AM_CONDITIONAL([HAVE_PDFLATEX], test -n "$PDFLATEX")
AC_CHECK_PROG(MAKEGLOSSARIES, makeglossaries, makeglossaries)


# Checks for libraries.
PKG_CHECK_MODULES([uhd], [uhd])

# Checks for Python libraries
AC_PYTHON_MODULE([pynmea2], [fatal])
AC_PYTHON_MODULE([serial], [fatal])
AC_PYTHON_MODULE([mraa], [fatal])

AC_PYTHON_MODULE([subprocess], [fatal])
AC_PYTHON_MODULE([os], [fatal])
AC_PYTHON_MODULE([threading], [fatal])
AC_PYTHON_MODULE([signal], [fatal])
AC_PYTHON_MODULE([enum], [fatal])
AC_PYTHON_MODULE([time], [fatal])
AC_PYTHON_MODULE([mmap], [fatal])
AC_PYTHON_MODULE([sys], [fatal])
AC_PYTHON_MODULE([calendar], [fatal])
AC_PYTHON_MODULE([math], [fatal])

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h syslog.h signal.h error.h uhd.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 scripts/Makefile
                 scripts/autostart/Makefile
                 scripts/gps_logger/Makefile
                 src/Makefile
                 src/sdr_record/Makefile])
AC_OUTPUT
