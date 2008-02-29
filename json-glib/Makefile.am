NULL =

INCLUDES = \
	-I$(top_srcdir) 		\
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\" 	\
	-DG_DISABLE_DEPRECATED 		\
	-DG_LOG_DOMAIN=\"Json\" 	\
	$(JSON_CFLAGS) 			\
	$(JSON_DEBUG_CFLAGS) 		\
	$(NULL)

LDADD = $(JSON_LT_LDFLAGS) -export-dynamic -rpath $(libdir)

BUILT_SOURCES = json-marshal.h json-marshal.c

STAMP_FILES = stamp-json-marshal.h

json-marshal.h: stamp-json-marshal.h
	@true
stamp-json-marshal.h: Makefile json-marshal.list
	$(GLIB_GENMARSHAL) \
		--prefix=_json_marshal \
		--header \
	$(srcdir)/json-marshal.list > xgen-jmh && \
	(cmp -s xgen-jmh json-marshal.h || cp -f xgen-jmh json-marshal.h) && \
	rm -f xgen-jmh && \
	echo timestamp > $(@F)

json-marshal.c: Makefile json-marshal.list
	(echo "#include \"json-marshal.h\"" ; \
	 $(GLIB_GENMARSHAL) \
		--prefix=_json_marshal \
		--body \
	 $(srcdir)/json-marshal.list ) > xgen-jmc && \
	cp -f xgen-jmc json-marshal.c && \
	rm -f xgen-jmc

source_h = \
	$(top_srcdir)/json-glib/json-generator.h \
	$(top_srcdir)/json-glib/json-gobject.h \
	$(top_srcdir)/json-glib/json-parser.h \
	$(top_srcdir)/json-glib/json-scanner.h \
	$(top_srcdir)/json-glib/json-types.h \
	$(top_srcdir)/json-glib/json-version.h \
	$(NULL)

source_h_private =

source_c = \
	json-array.c \
	json-generator.c \
	json-gobject.c \
	json-node.c \
	json-object.c \
	json-parser.c \
	json-scanner.c \
	$(NULL)

lib_LTLIBRARIES = libjson-glib-1.0.la

libjson_glib_1_0_la_LIBADD = $(JSON_LIBS)
libjson_glib_1_0_la_SOURCES = \
	$(source_c) \
	$(source_h) \
	$(source_h_priv) \
	$(BUILT_SOURCES) \
	$(NULL)
libjson_glib_1_0_la_LDFLAGS = $(LDADD)

jsonincludedir = $(includedir)/json-glib-1.0/json-glib
jsoninclude_DATA = \
	$(source_h) \
	$(top_srcdir)/json-glib/json-glib.h

CLEANFILES = $(STAMP_FILES) $(BUILT_SOURCES)

DISTCLEANFILES = json-version.h

EXTRA_DIST = json-version.h.in json-glib.h json-marshal.list

