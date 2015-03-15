# preamble

NULL =
AM_CPPFLAGS =
AM_CFLAGS =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
DISTCLEANFILES =
lib_LTLIBRARIES =
bin_PROGRAMS =

# build the tests last
SUBDIRS = . tests

# main library
source_h = \
	json-builder.h		\
	json-generator.h 	\
	json-gobject.h 		\
	json-gvariant.h		\
	json-parser.h 		\
	json-path.h		\
	json-reader.h		\
	json-types.h 		\
	json-version-macros.h	\
	$(NULL)

source_h_private = \
	json-debug.h		\
	json-gobject-private.h	\
	json-scanner.h 		\
	json-types-private.h	\
	$(NULL)

source_c = \
	json-array.c 		\
	json-builder.c	\
	json-debug.c		\
	json-gboxed.c		\
	json-generator.c 	\
	json-gobject.c 	\
	json-gvariant.c	\
	json-node.c 		\
	json-object.c 	\
	json-parser.c 	\
	json-path.c		\
	json-reader.c		\
	json-scanner.c 	\
	json-serializable.c	\
	json-value.c		\
	$(NULL)

DISTCLEANFILES += json-version.h

# glib-mkenums rules
glib_enum_h = json-enum-types.h
glib_enum_c = json-enum-types.c
glib_enum_headers = $(source_h)
include $(top_srcdir)/build/autotools/Makefile.am.enums

# glib-genmarshal rules
glib_marshal_list = json-marshal.list
glib_marshal_prefix = json_marshal
include $(top_srcdir)/build/autotools/Makefile.am.marshal

lib_LTLIBRARIES += libjson-glib-1.0.la

libjson_glib_1_0_la_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DJSON_LOCALEDIR=\""$(localedir)"\" \
	-DJSON_COMPILATION=1 \
	-DG_LOG_DOMAIN=\"Json\" \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(JSON_DEBUG_CFLAGS) \
	$(NULL)
libjson_glib_1_0_la_CFLAGS = $(JSON_CFLAGS) $(JSON_HIDDEN_VISIBILITY_CFLAG) $(MAINTAINER_CFLAGS) $(JSON_GCOV_CFLAGS)
libjson_glib_1_0_la_LIBADD = $(JSON_LIBS)
libjson_glib_1_0_la_SOURCES = $(source_c) $(source_h) $(source_h_private) $(BUILT_SOURCES)
libjson_glib_1_0_la_LDFLAGS = $(JSON_LT_LDFLAGS) $(JSON_GCOV_LDADD) -export-dynamic

jsonincludedir = $(includedir)/json-glib-1.0/json-glib
jsoninclude_DATA = \
	$(source_h) \
	json-enum-types.h \
	json-version.h \
	json-glib.h \
	$(NULL)

EXTRA_DIST += json-version.h.in json-glib.h json-version.h

pcfiles = json-glib-1.0.pc

json-glib-1.0.pc: json-glib.pc
	$(AM_V_GEN)cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig
CLEANFILES += $(pcfiles)
EXTRA_DIST += json-glib.pc.in

# tools
bin_PROGRAMS += json-glib-validate
json_glib_validate_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -DJSON_LOCALEDIR=\""$(localedir)"\"
json_glib_validate_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
json_glib_validate_SOURCES = json-glib-validate.c
json_glib_validate_LDADD = $(JSON_LIBS) libjson-glib-1.0.la

bin_PROGRAMS += json-glib-format
json_glib_format_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -DJSON_LOCALEDIR=\""$(localedir)"\"
json_glib_format_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
json_glib_format_SOURCES = json-glib-format.c
json_glib_format_LDADD = $(JSON_LIBS) libjson-glib-1.0.la

# intospection
-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS = Json-1.0.gir

introspection_source = \
	$(source_c) \
	$(source_h) \
	json-enum-types.h \
	json-enum-types.c \
	json-version.h

Json-1.0.gir: libjson-glib-1.0.la Makefile

Json_1_0_gir_NAMESPACE = Json
Json_1_0_gir_VERSION = 1.0
Json_1_0_gir_LIBS = libjson-glib-1.0.la
Json_1_0_gir_FILES = $(introspection_source)
Json_1_0_gir_CFLAGS = -DJSON_COMPILATION=1 -I$(top_srcdir) -I$(top_builddir) $(JSON_CFLAGS)
Json_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
Json_1_0_gir_SCANNERFLAGS = --warn-all --pkg-export json-glib-1.0 --c-include "json-glib/json-glib.h"

girdir = $(datadir)/gir-1.0
dist_gir_DATA = Json-1.0.gir

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = Json-1.0.typelib

CLEANFILES += $(dist_gir_DATA) $(typelibs_DATA)
endif # HAVE_INTROSPECTION

# visual studio
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
