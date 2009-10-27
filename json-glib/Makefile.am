include $(top_srcdir)/build/autotools/Makefile.am.gtest
include $(top_srcdir)/build/autotools/Makefile.am.silent

if ENABLE_GLIB_TEST
# build this directory *before* the tests/
SUBDIRS = . tests
endif

DIST_SUBDIRS = tests

NULL =

INCLUDES = -I$(top_srcdir)

LDADD = \
	$(JSON_LT_LDFLAGS) \
	-export-dynamic \
	-export-symbols-regex "^json.*" \
	-rpath $(libdir)

AM_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\" 	\
	-DJSON_COMPILATION=1		\
	-DG_DISABLE_DEPRECATED 		\
	-DG_LOG_DOMAIN=\"Json\" 	\
	$(JSON_DEBUG_CFLAGS) 		\
	$(NULL)

AM_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
BUILT_SOURCES =

CLEANFILES =
DISTCLEANFILES = json-version.h

source_h = \
	$(top_srcdir)/json-glib/json-generator.h 	\
	$(top_srcdir)/json-glib/json-gobject.h 		\
	$(top_srcdir)/json-glib/json-parser.h 		\
	$(top_srcdir)/json-glib/json-types.h 		\
	$(NULL)

source_h_private = \
	$(top_srcdir)/json-glib/json-scanner.h 		\
	$(top_srcdir)/json-glib/json-gobject-private.h	\
	$(top_srcdir)/json-glib/json-types-private.h	\
	$(NULL)

source_c = \
	$(srcdir)/json-array.c 		\
	$(srcdir)/json-gboxed.c		\
	$(srcdir)/json-generator.c 	\
	$(srcdir)/json-gobject.c 	\
	$(srcdir)/json-node.c 		\
	$(srcdir)/json-object.c 	\
	$(srcdir)/json-parser.c 	\
	$(srcdir)/json-scanner.c 	\
	$(srcdir)/json-serializable.c	\
	$(NULL)

# glib-mkenums rules
glib_enum_h = json-enum-types.h
glib_enum_c = json-enum-types.c
glib_enum_headers = $(source_h)
include $(top_srcdir)/build/autotools/Makefile.am.enums

# glib-genmarshal rules
glib_marshal_list = json-marshal.list
glib_marshal_prefix = _json_marshal
include $(top_srcdir)/build/autotools/Makefile.am.marshal

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
	$(source_h) 					\
	$(top_builddir)/json-glib/json-enum-types.h 	\
	$(top_builddir)/json-glib/json-version.h	\
	$(top_srcdir)/json-glib/json-glib.h		\
	$(NULL)

EXTRA_DIST += \
	json-version.h.in 	\
	json-types-private.h	\
	json-gobject-private.h	\
	json-scanner.h 		\
	json-glib.h 		\
	$(NULL)

TESTS_ENVIRONMENT = srcdir="$(srcdir)" json_all_c_sources="$(source_c)"

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

Json-1.0.gir: $(INTROSPECTION_SCANNER) libjson-glib-1.0.la
	$(QUIET_GEN)$(INTROSPECTION_SCANNER) \
		-v --namespace Json --nsversion=1.0 \
		$(INCLUDES) \
		$(AM_CPPFLAGS) \
		--c-include='json-glib/json-object.h' \
		--include=GObject-2.0 \
		--library=json-glib-1.0 \
		--libtool="$(top_builddir)/libtool" \
		--pkg gobject-2.0 \
		--output $@ \
		$(source_h) \
		$(source_c)

BUILT_GIRSOURCES += Json-1.0.gir

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(QUIET_GEN)$(DEBUG) $(INTROSPECTION_COMPILER) --includedir=$(srcdir) --includedir=. $(INTROSPECTION_COMPILER_OPTS) $< -o $(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif # HAVE_INTROSPECTION
