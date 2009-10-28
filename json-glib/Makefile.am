include $(top_srcdir)/Makefile.decl

if USE_SHAVE
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)
else
QUIET_GEN = $(AM_V_GEN)
endif # USE_SHAVE

if ENABLE_GLIB_TEST
SUBDIRS = . tests
endif

DIST_SUBDIRS = tests

NULL =

INCLUDES = \
	-I$(top_srcdir) 		\
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\" 	\
	-DJSON_COMPILATION=1		\
	-DG_DISABLE_DEPRECATED 		\
	-DG_LOG_DOMAIN=\"Json\" 	\
	$(NULL)

LDADD = \
	$(JSON_LT_LDFLAGS) \
	-export-dynamic \
	-export-symbols-regex "^json.*" \
	-rpath $(libdir)

AM_CPPFLAGS = $(JSON_CFLAGS) $(JSON_DEBUG_CFLAGS) $(MAINTAINER_CFLAGS)

BUILT_SOURCES = json-marshal.h json-marshal.c json-enum-types.h json-enum-types.c

STAMP_FILES = stamp-json-marshal.h stamp-json-enum-types.h

source_h = \
	$(top_srcdir)/json-glib/json-generator.h 	\
	$(top_srcdir)/json-glib/json-gobject.h 		\
	$(top_srcdir)/json-glib/json-parser.h 		\
	$(top_srcdir)/json-glib/json-types.h 		\
	$(NULL)

source_h_private = \
	$(top_srcdir)/json-glib/json-scanner.h 		\
	$(top_srcdir)/json-glib/json-types-private.h	\
	$(NULL)

source_c = \
	$(srcdir)/json-array.c 		\
	$(srcdir)/json-generator.c 	\
	$(srcdir)/json-gobject.c 	\
	$(srcdir)/json-node.c 		\
	$(srcdir)/json-object.c 	\
	$(srcdir)/json-parser.c 	\
	$(srcdir)/json-scanner.c 	\
	$(NULL)

json-marshal.h: stamp-json-marshal.h
	@true
stamp-json-marshal.h: Makefile json-marshal.list
	$(QUIET_GEN)$(GLIB_GENMARSHAL) \
		--prefix=_json_marshal \
		--header \
	$(srcdir)/json-marshal.list > xgen-jmh \
	&& (cmp -s xgen-jmh json-marshal.h || cp -f xgen-jmh json-marshal.h) \
	&& rm -f xgen-jmh \
	&& echo timestamp > $(@F)

json-marshal.c: Makefile json-marshal.list
	$(QUIET_GEN)(echo "#include \"json-marshal.h\"" ; \
	  $(GLIB_GENMARSHAL) \
		--prefix=_json_marshal \
		--body \
	  $(srcdir)/json-marshal.list ) > xgen-jmc \
	&& cp -f xgen-jmc json-marshal.c \
	&& rm -f xgen-jmc

json-enum-types.h: stamp-json-enum-types.h
	@true
stamp-json-enum-types.h: $(source_h) Makefile
	$(QUIET_GEN)$(GLIB_MKENUMS) \
		--template $(srcdir)/json-enum-types.h.in \
	$(source_h) > xgen-ceth \
	&& (cmp -s xgen-ceth json-enum-types.h || cp xgen-ceth json-enum-types.h) \
	&& rm -f xgen-ceth \
	&& echo timestamp > $(@F)

json-enum-types.c: json-enum-types.h
	$(QUIET_GEN)$(GLIB_MKENUMS) \
		--template $(srcdir)/json-enum-types.c.in \
	$(source_h) > xgen-cetc \
	&& cp xgen-cetc json-enum-types.c \
	&& rm -f xgen-cetc

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

CLEANFILES = $(STAMP_FILES) $(BUILT_SOURCES)

DISTCLEANFILES = json-version.h

EXTRA_DIST += \
	json-version.h.in 	\
	json-types-private.h	\
	json-scanner.h 		\
	json-glib.h 		\
	json-marshal.list 	\
	json-enum-types.h.in 	\
	json-enum-types.c.in	\
	$(NULL)

TESTS_ENVIRONMENT = srcdir="$(srcdir)" json_all_c_sources="$(source_c)"

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

Json-1.0.gir: $(INTROSPECTION_SCANNER) libjson-glib-1.0.la
	$(QUIET_GEN)$(INTROSPECTION_SCANNER) \
		-v --namespace Json --nsversion=1.0 \
		$(INCLUDES) \
		--c-include='json-glib/json-gobject.h' \
		--include=GObject-2.0 \
		--library=libjson-glib-1.0.la \
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
	$(QUIET_GEN)\
	$(DEBUG) $(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=. \
		$(INTROSPECTION_COMPILER_OPTS) $< \
		-o $(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif # HAVE_INTROSPECTION
