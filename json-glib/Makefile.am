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

BUILT_SOURCES =

STAMP_FILES =

source_h = \
	json-parser.h \
	json-types.h \
	json-version.h \
	$(NULL)

source_h_private = json-private.h

source_c = \
	json-array.c \
	json-object.c \
	json-parser.c \
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

jsoninclude_DATA = $(source_h) json-glib.h
jsonincludedir = $(includedir)/json-glib-1.0/json-glib

CLEANFILES = $(STAMP_FILES) $(BUILT_SOURCES)

DISTCLEANFILES = json-version.h

EXTRA_DIST = json-version.h.in

