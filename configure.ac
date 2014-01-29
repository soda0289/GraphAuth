AC_PREREQ(2.63)
AC_INIT([graph-auth],[0.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([passwd/main.c])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-xz foreign])

LT_INIT([])

#Check for PAM
AC_CHECK_HEADERS([security/pam_appl.h], [],
	[AC_MSG_ERROR([[PAM header files not found, install libpam-dev.]])])
  
AC_CHECK_HEADERS([security/pam_modules.h security/_pam_macros.h security/pam_modutil.h], [], [],
	[#include <sys/types.h>
	 #include <security/pam_appl.h>])

AC_CHECK_LIB([pam], [pam_start])

AC_SUBST(PAMDIR, "\$(prefix)/lib64/security")
AC_ARG_WITH(pam-dir,
  AC_HELP_STRING([--with-pam-dir=DIR],
                 [Where to install PAM module [[PREFIX/lib/security]]]),
            [case "${withval}" in
            /*) PAMDIR="${withval}";;
            ./*|../*) AC_MSG_ERROR(Bad value for --with-pam-dir);;
            *)  PAMDIR="\$(prefix)/lib64/${withval}";;
            esac])
AC_MSG_NOTICE([PAM installation path $PAMDIR])


AC_CONFIG_FILES([
	Makefile
	js/Makefile
	pam/Makefile
	pam_gobject/Makefile
	passwd/Makefile
])

AC_OUTPUT
