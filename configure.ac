#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([vektor-ds], [0.1], [ryan.lucchese@gmail.com])
AM_INIT_AUTOMAKE([vektor-ds],[0.1])
AC_CONFIG_SRCDIR([vektor-ds.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([inet_ntoa memset select socket])

AC_OUTPUT(Makefile)
