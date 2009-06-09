include $(top_srcdir)/Makefile.decl

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

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
	$(srcdir)/json-generator.h 	\
	$(srcdir)/json-gobject.h 	\
	$(srcdir)/json-parser.h 	\
	$(srcdir)/json-types.h 		\
	$(NULL)

source_h_private = \
	$(srcdir)/json-scanner.h 	\
	$(srcdir)/json-types-private.h	\
	$(NULL)

source_c = \
	json-array.c 		\
	json-generator.c 	\
	json-gobject.c 		\
	json-node.c 		\
	json-object.c 		\
	json-parser.c 		\
	json-scanner.c 		\
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
	json-scanner.h 		\
	json-glib.h 		\
	json-marshal.list 	\
	json-enum-types.h.in 	\
	json-enum-types.c.in	\
	$(NULL)

TESTS_ENVIRONMENT = srcdir="$(srcdir)" json_all_c_sources="$(source_c)"
