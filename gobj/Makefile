CC := gcc
LIBTOOL := libtool

interface_xml:= interface.xml
pkg_packages := gobject-introspection-1.0

PKG_CFLAGS := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

PAM_LDFLAGS := -lpam 

DBG_CFLAGS := -g -Wall

CFLAGS := $(PKG_CFLAGS) $(DBG_CFLAGS) 
LDFLAGS := $(PKG_LDFLAGS) $(PAM_LDFLAGS) $(LDFLAGS)

G_OBJ_PREFIX := graph_auth

GRAPH_AUTH_TYPELIB := GraphAuth-0.1.typelib
GRAPH_AUTH_LIB := libgraphauth.so

.PHONY: clean install

all: libgraphauth.la GraphAuth-0.1.typelib 

libgraphauth.la: graph_auth.lo
	$(LIBTOOL) link $(CC) $(LDFLAGS) -rpath /usr/local/lib $< -o $@

%.lo: %.c 
	$(LIBTOOL) compile $(CC) $(CFLAGS) -c $< -o $@

$(GRAPH_AUTH_TYPELIB): GraphAuth.gir
	g-ir-compiler -o $@ $< 


GraphAuth.gir: libgraphauth.la
	g-ir-scanner graph_auth.c graph_auth.h --library=graphauth $(PKG_CFLAGS) $(LDFLAGS) --include=GObject-2.0 --namespace=GraphAuth --nsversion=0.1 --output=$@

install:
	cp $(GRAPH_AUTH_TYPELIB) /usr/lib64/girepository-1.0/
	cp .libs/$(GRAPH_AUTH_LIB)* /usr/lib64/

clean: 
	-rm *.lo
	-rm libgraphauth.la 
	-rm *.gir
	-rm *.typelib
	-rm -rf .libs/
