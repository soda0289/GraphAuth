NULL = 

bin_PROGRAMS = ga_passwd

ga_passwd_SOURCES = main.c \
					$(top_builddir)/js/js-resources.c \
					$(top_builddir)/js/js-resources.h \
					$(NULL)

ga_passwd_CFLAGS = $(shell pkg-config --cflags gjs-1.0 gio-2.0)
ga_passwd_LDFLAGS = $(shell pkg-config --libs gjs-1.0 gio-2.0)
