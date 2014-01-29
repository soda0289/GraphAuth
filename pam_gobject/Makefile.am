interface_xml:= interface.xml
pkg_packages := gobject-introspection-1.0

G_OBJ_PREFIX := graph_auth

GRAPH_AUTH_TYPELIB := GraphAuth-0.1.typelib
GRAPH_AUTH_LIB := libgraphauth.so

lib_LTLIBRARIES = libgraphauth.la

libgraphauth_la_SOURCES = graph_auth.c
libgraphauth_la_LDFLAGS = $(shell pkg-config --libs $(pkg_packages))
libgraphauth_la_CFLAGS = $(shell pkg-config --cflags $(pkg_packages))
libgraphauth_la_LIBADD = -lpam

all-local: GraphAuth-0.1.typelib 

$(GRAPH_AUTH_TYPELIB): GraphAuth.gir
	g-ir-compiler -o $@ $< 


GraphAuth.gir: libgraphauth.la
	g-ir-scanner graph_auth.c graph_auth.h --library=graphauth $(PKG_CFLAGS) $(LDFLAGS) --include=GObject-2.0 --namespace=GraphAuth --nsversion=0.1 --output=$@

install-data-local:
	cp $(GRAPH_AUTH_TYPELIB) /usr/lib64/girepository-1.0/

CLEANFILES = GraphAuth.gir GraphAuth-0.1.typelib
