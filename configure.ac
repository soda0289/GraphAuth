AC_PREREQ(2.63)
AC_INIT([graph-auth],[0.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([passwd/main.c])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-xz foreign])

LT_INIT([])

AC_CONFIG_FILES([
	Makefile
	js/Makefile
	pam/Makefile
	pam_gobject/Makefile
	passwd/Makefile
])

AC_OUTPUT
