CC=gcc
PAM_GRAPH = pam_graph.so

#Install in security directory
libdir += security

pkglib_LTLIBRARIES = pam_graph.la

pam_graph_la_SOURCES = pam_graph.c
pam_graph_la_LIBADD = -lpam
pam_graph_la_LDFLAGS = -module -shared

CLEANFILES = pam_graph.so
