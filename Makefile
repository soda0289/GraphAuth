.PHONY: clean pam_module dbus-service gobj

all: pam_module dbus-service gobj

pam_module: 
	$(MAKE) -C pam

dbus-service:
	$(MAKE) -C dbus-service

gobj: 
	$(MAKE) -C gobj


clean:
	$(MAKE) -C dbus-service clean
	$(MAKE) -C pam clean
	$(MAKE) -C gobj clean
