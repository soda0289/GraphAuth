NULL = 

js_resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/js-resources.gresource.xml)
js-resources.h: js-resources.gresource.xml $(js_resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --sourcedir=$(builddir) --generate --c-name graphauth_js_resources $<
js-resources.c: js-resources.gresource.xml $(js_resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --sourcedir=$(builddir) --generate --c-name graphauth_js_resources $<

js_built_sources = js-resources.c js-resources.h

BUILT_SOURCES = $(js_built_sources)

all-local: $(js_built_sources)


EXTRA_DIST = \
	js-resources.gresource.xml \
	$(NULL)

CLEANFILES = \
	$(js_built_sources) \
	$(NULL)
