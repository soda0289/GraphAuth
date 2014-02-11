CC=gcc
PAM_GRAPH = pam_graph.so

#Install in security directory
pamdir = $(PAMDIR) 

pam_LTLIBRARIES = pam_graph.la

pam_graph_la_SOURCES = pam_graph.c
pam_graph_la_LIBADD = -lpam -lcrypt
pam_graph_la_LDFLAGS = -export-dynamic -avoid-version -module -rpath '$(pamdir)'

CLEANFILES = pam_graph.so
