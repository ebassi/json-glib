include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

EXTRA_DIST =

SUBDIRS = . tests

AM_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\" 	\
	-DJSON_COMPILATION=1		\
	-DG_LOG_DOMAIN=\"Json\" 	\
	-I$(top_srcdir) 		\
	-I$(top_builddir) 		\
	$(JSON_DEBUG_CFLAGS) 		\
	$(NULL)

AM_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS) $(JSON_GCOV_CFLAGS)

BUILT_SOURCES =

CLEANFILES =
DISTCLEANFILES = json-version.h

source_h = \
	$(top_srcdir)/json-glib/json-builder.h		\
	$(top_srcdir)/json-glib/json-generator.h 	\
	$(top_srcdir)/json-glib/json-gobject.h 		\
	$(top_srcdir)/json-glib/json-gvariant.h		\
	$(top_srcdir)/json-glib/json-parser.h 		\
	$(top_srcdir)/json-glib/json-path.h		\
	$(top_srcdir)/json-glib/json-reader.h		\
	$(top_srcdir)/json-glib/json-types.h 		\
	$(NULL)

source_h_private = \
	$(top_srcdir)/json-glib/json-debug.h		\
	$(top_srcdir)/json-glib/json-gobject-private.h	\
	$(top_srcdir)/json-glib/json-scanner.h 		\
	$(top_srcdir)/json-glib/json-types-private.h	\
	$(NULL)

source_c = \
	$(srcdir)/json-array.c 		\
	$(srcdir)/json-builder.c	\
	$(srcdir)/json-debug.c		\
	$(srcdir)/json-gboxed.c		\
	$(srcdir)/json-generator.c 	\
	$(srcdir)/json-gobject.c 	\
	$(srcdir)/json-gvariant.c	\
	$(srcdir)/json-node.c 		\
	$(srcdir)/json-object.c 	\
	$(srcdir)/json-parser.c 	\
	$(srcdir)/json-path.c		\
	$(srcdir)/json-reader.c		\
	$(srcdir)/json-scanner.c 	\
	$(srcdir)/json-serializable.c	\
	$(srcdir)/json-value.c		\
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
libjson_glib_1_0_la_SOURCES = $(source_c) $(source_h) $(source_h_private) $(BUILT_SOURCES)
libjson_glib_1_0_la_LDFLAGS = $(JSON_LT_LDFLAGS) $(JSON_GCOV_LDADD) -no-undefined -export-symbols $(top_srcdir)/json-glib/json-glib.symbols -rpath $(libdir)

EXTRA_DIST += json-glib.symbols

jsonincludedir = $(includedir)/json-glib-1.0/json-glib
jsoninclude_DATA = \
	$(source_h) 					\
	$(top_builddir)/json-glib/json-enum-types.h 	\
	$(top_builddir)/json-glib/json-version.h	\
	$(top_srcdir)/json-glib/json-glib.h		\
	$(NULL)

EXTRA_DIST += json-version.h.in json-glib.h json-version.h

TESTS_ENVIRONMENT = srcdir="$(srcdir)" json_all_c_sources="$(source_c)"

if OS_LINUX
TESTS = abicheck.sh
endif

EXTRA_DIST += abicheck.sh

# intospection
-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS = Json-1.0.gir

Json-1.0.gir: libjson-glib-1.0.la Makefile

Json_1_0_gir_NAMESPACE = Json
Json_1_0_gir_VERSION = 1.0
Json_1_0_gir_LIBS = libjson-glib-1.0.la
Json_1_0_gir_FILES = $(source_h) $(source_c)
Json_1_0_gir_CFLAGS = $(AM_CPPFLAGS) $(JSON_CFLAGS)
Json_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
Json_1_0_gir_SCANNERFLAGS = --warn-all --pkg-export json-glib-1.0 --c-include "json-glib/json-glib.h"

girdir = $(datadir)/gir-1.0
dist_gir_DATA = Json-1.0.gir

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = Json-1.0.typelib

CLEANFILES += $(dist_gir_DATA) $(typelibs_DATA)
endif # HAVE_INTROSPECTION

dist-hook: ../build/win32/vs9/json-glib.vcproj ../build/win32/vs10/json-glib.vcxproj ../build/win32/vs10/json-glib.vcxproj.filters

../build/win32/vs9/json-glib.vcproj: $(top_srcdir)/build/win32/vs9/json-glib.vcprojin
	for F in $(libjson_glib_1_0_la_SOURCES); do \
		case $$F in \
		*.c)	echo '   <File RelativePath="..\..\..\json-glib\'$$F'" />' \
			;; \
		esac; \
	done >jsonglib.sourcefiles
	$(CPP) -P - <$(top_srcdir)/build/win32/vs9/json-glib.vcprojin >$@
	rm jsonglib.sourcefiles
	
../build/win32/vs10/json-glib.vcxproj: $(top_srcdir)/build/win32/vs10/json-glib.vcxprojin
	for F in $(libjson_glib_1_0_la_SOURCES); do \
		case $$F in \
		*.c)	echo '    <ClCompile Include="..\..\..\json-glib\'$$F'" />' \
			;; \
		esac; \
	done >jsonglib.vs10.sourcefiles
	$(CPP) -P - <$(top_srcdir)/build/win32/vs10/json-glib.vcxprojin >$@
	rm jsonglib.vs10.sourcefiles
	
../build/win32/vs10/json-glib.vcxproj.filters: $(top_srcdir)/build/win32/vs10/json-glib.vcxproj.filtersin
	for F in $(libjson_glib_1_0_la_SOURCES); do \
		case $$F in \
		*.c)	echo '    <ClCompile Include="..\..\..\json-glib\'$$F'"><Filter>Sources</Filter></ClCompile>' \
			;; \
		esac; \
	done >jsonglib.vs10.sourcefiles.filters
	$(CPP) -P - <$(top_srcdir)/build/win32/vs10/json-glib.vcxproj.filtersin >$@
	rm jsonglib.vs10.sourcefiles.filters

# Let the VS9/VS10 Project files be cleared out before they are re-expanded...
DISTCLEANFILES += ../build/win32/vs9/json-glib.vcproj	\
		  ../build/win32/vs10/json-glib.vcxproj	\
		  ../build/win32/vs10/json-glib.vcxproj.filters
