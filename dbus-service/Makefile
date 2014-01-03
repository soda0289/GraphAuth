interface_xml:= interface.xml
pkg_packages := dbus-1 dbus-glib-1 gobject-introspection-1.0

PKG_CFLAGS := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

PAM_LDFLAGS := -lpam 

DBG_CFLAGS := -g -Wall

CFLAGS := $(PKG_CFLAGS) $(DBG_CFLAGS) 
LDFLAGS := $(PKG_LDFLAGS) $(PAM_LDFLAGS) $(LDFLAGS)

G_OBJ_PREFIX := graph_auth

.PHONY: clean

all:  ga_serv

ga_serv: service.o ga_obj.o
	$(CC) $^ $(LDFLAGS) -o $@

dbus_binding_server.h: interface.xml
	dbus-binding-tool --mode=glib-server --prefix=$(G_OBJ_PREFIX) $^ > $@

%.o: %.c dbus_binding_server.h 
	$(CC) -c $< $(CFLAGS) -o $@

clean: 
	rm *.o
	rm dbus_binding_server.h
	rm ga_serv
