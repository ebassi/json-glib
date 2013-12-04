# preamble
ACLOCAL_AMFLAGS = -I build/autotools
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-man --enable-maintainer-flags --enable-introspection

NULL =
AM_CPPFLAGS =
AM_CFLAGS =
BUILT_SOURCES =
BUILT_EXTRA_DIST =
CLEANFILES =
EXTRA_DIST =
DISTCLEANFILES =
MAINTAINERCLEANFILES =
lib_LTLIBRARIES =
bin_PROGRAMS =
TESTS =
TESTS_ENVIRONMENT =

# Build tools
EXTRA_DIST += \
	as-compiler-flag.m4	\
	as-linguas.m4		\
	glib-tap.mk		\
	glibtests.m4		\
	introspection.m4	\
	jh-catalog.m4		\
	Makefile.am.enums	\
	Makefile.am.marshal	\
	Makefile.am.gitignore	\
	tap-driver.sh

# ChangeLog generation from the commit log during dist
dist-hook:
	@if test -d "$(srcdir)/.git"; then \
	  echo Generating ChangeLog ; \
	  ( cd "$(srcdir)" \
	    && $(top_srcdir)/build/missing --run git log --stat ) > ChangeLog.tmp \
	  && mv -f ChangeLog.tmp $(top_distdir)/ChangeLog \
	  || ( rm -f ChangeLog.tmp; \
	       echo Failed to generate ChangeLog >&2 ); \
	else \
	  echo A git checkout is required to generate a ChangeLog >&2; \
	fi

# coverage report
if ENABLE_GCOV
# use recursive makes in order to ignore errors during check/perf
lcov:
	-$(MAKE) $(AM_MAKEFLAGS) check
	$(MAKE) $(AM_MAKEFLAGS) genlcov

# we have to massage the lcov.info file slightly to hide the effect of libtool
# placing the objects files in the .libs/ directory separate from the *.c
genlcov:
	$(LTP) --directory $(top_builddir) --capture --output-file json-lcov.info --test-name JSON_TEST --no-checksum
	$(SED) -e 's#.libs/##' < json-lcov.info > json-lcov.info.tmp
	LANG=C $(LTP_GENHTML) --prefix $(top_builddir) --output-directory json-lcov --title "JSON-GLib Code Coverage" --show-details json-lcov.info.tmp
	rm -f json-lcov.info.tmp

lcov-clean:
	-$(LTP) --directory $(top_builddir) -z
	-$(RM) -rf json-lcov.info json-lcov
else
lcov genlcov lcov-clean:
	@echo You need to configure JSON-GLib with support for gcov enabled.
	@echo e.g., ./configure --enable-gcov
endif

CLEANFILES += *.gcda *.gcno

.PHONY: lcov genlcov lcov-clean

# gettext and gtk-doc do not cope well with non-recursive layouts
# we need to ensure that this directory is built before the rest,
# so leave the '.' at the beginning
SUBDIRS = . po doc

# main library
source_h = \
	json-glib/json-builder.h	\
	json-glib/json-generator.h 	\
	json-glib/json-gobject.h 	\
	json-glib/json-gvariant.h	\
	json-glib/json-parser.h 	\
	json-glib/json-path.h		\
	json-glib/json-reader.h		\
	json-glib/json-types.h 		\
	$(NULL)

source_h_private = \
	json-glib/json-debug.h			\
	json-glib/json-gobject-private.h	\
	json-glib/json-scanner.h 		\
	json-glib/json-types-private.h		\
	$(NULL)

source_c = \
	json-glib/json-array.c 		\
	json-glib/json-builder.c	\
	json-glib/json-debug.c		\
	json-glib/json-gboxed.c		\
	json-glib/json-generator.c 	\
	json-glib/json-gobject.c 	\
	json-glib/json-gvariant.c	\
	json-glib/json-node.c 		\
	json-glib/json-object.c 	\
	json-glib/json-parser.c 	\
	json-glib/json-path.c		\
	json-glib/json-reader.c		\
	json-glib/json-scanner.c 	\
	json-glib/json-serializable.c	\
	json-glib/json-value.c		\
	$(NULL)

DISTCLEANFILES += json-glib/json-version.h

# glib-mkenums rules
glib_enum_h = json-glib/json-enum-types.h
glib_enum_c = json-glib/json-enum-types.c
glib_enum_headers = $(source_h)
include $(top_srcdir)/build/autotools/Makefile.am.enums

# glib-genmarshal rules
glib_marshal_list = json-glib/json-marshal.list
glib_marshal_prefix = _json_marshal
include $(top_srcdir)/build/autotools/Makefile.am.marshal

lib_LTLIBRARIES += libjson-glib-1.0.la

libjson_glib_1_0_la_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 		\
	-DJSON_LOCALEDIR=\""$(localedir)"\" 	\
	-DJSON_COMPILATION=1			\
	-DG_LOG_DOMAIN=\"Json\" 		\
	-I$(top_srcdir) 			\
	-I$(top_builddir) 			\
	$(JSON_DEBUG_CFLAGS) 			\
	$(NULL)
libjson_glib_1_0_la_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS) $(JSON_GCOV_CFLAGS)
libjson_glib_1_0_la_LIBADD = $(JSON_LIBS)
libjson_glib_1_0_la_SOURCES = $(source_c) $(source_h) $(source_h_private) $(BUILT_SOURCES)
libjson_glib_1_0_la_LDFLAGS = $(JSON_LT_LDFLAGS) $(JSON_GCOV_LDADD) -export-symbols $(top_srcdir)/json-glib/json-glib.symbols

EXTRA_DIST += json-glib.symbols

jsonincludedir = $(includedir)/json-glib-1.0/json-glib
jsoninclude_DATA = \
	$(source_h) 					\
	$(top_builddir)/json-glib/json-enum-types.h 	\
	$(top_builddir)/json-glib/json-version.h	\
	$(top_srcdir)/json-glib/json-glib.h		\
	$(NULL)

pcfiles = json-glib-1.0.pc

json-glib-1.0.pc: $(top_builddir)/json-glib/json-glib.pc
	$(AM_V_GEN)cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig
CLEANFILES += $(pcfiles)
EXTRA_DIST += json-glib/json-glib.pc.in

# tools
bin_PROGRAMS += json-glib-validate
json_glib_validate_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" 		\
	-DJSON_LOCALEDIR=\""$(localedir)"\" 	\
	-DG_LOG_DOMAIN=\"Json-Validate\"	\
	-I$(top_srcdir) 			\
	-I$(top_builddir) 			\
	$(JSON_DEBUG_CFLAGS) 			\
	$(NULL)
json_glib_validate_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
json_glib_validate_SOURCES = json-glib/json-glib-validate.c
json_glib_validate_LDADD = $(JSON_LIBS) libjson-glib-1.0.la
json_glib_validate_DEPENDENCIES = libjson-glib-1.0.la

bin_PROGRAMS += json-glib-format
json_glib_format_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" 		\
	-DJSON_LOCALEDIR=\""$(localedir)"\" 	\
	-DG_LOG_DOMAIN=\"Json-Format\"		\
	-I$(top_srcdir) 			\
	-I$(top_builddir) 			\
	$(JSON_DEBUG_CFLAGS) 			\
	$(NULL)
json_glib_format_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
json_glib_format_SOURCES = json-glib/json-glib-format.c
json_glib_format_LDADD = $(JSON_LIBS) libjson-glib-1.0.la
json_glib_format_DEPENDENCIES = libjson-glib-1.0.la

# intospection
-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS = Json-1.0.gir

Json-1.0.gir: libjson-glib-1.0.la Makefile

Json_1_0_gir_NAMESPACE = Json
Json_1_0_gir_VERSION = 1.0
Json_1_0_gir_LIBS = libjson-glib-1.0.la
Json_1_0_gir_FILES = $(source_h) $(source_c)
Json_1_0_gir_CFLAGS = -DJSON_COMPILATION=1 -I$(top_srcdir) -I$(top_builddir) $(JSON_CFLAGS)
Json_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
Json_1_0_gir_SCANNERFLAGS = --warn-all --pkg-export json-glib-1.0 --c-include "json-glib/json-glib.h"

girdir = $(datadir)/gir-1.0
dist_gir_DATA = Json-1.0.gir

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = Json-1.0.typelib

CLEANFILES += $(dist_gir_DATA) $(typelibs_DATA)
endif # HAVE_INTROSPECTION

# tests
include $(top_srcdir)/build/autotools/glib-tap.mk

TESTS_ENVIRONMENT += \
	G_TEST_SRCDIR="$(abs_srcdir)/json-glib/tests" 	\
	G_TEST_BUILDDIR="$(abs_builddir)" 		\
	$(NULL)

if OS_LINUX
# ABI checking can only happen on Linux
TESTS += json-glib/abicheck.sh
endif

EXTRA_DIST += json-glib/abicheck.sh

dist_test_data = \
	json-glib/tests/stream-load.json	\
	$(NULL)

test_programs = \
	array			\
	boxed			\
	builder			\
	generator		\
	gvariant		\
	invalid			\
	node			\
	object			\
	parser			\
	path			\
	reader			\
	serialize-simple	\
	serialize-complex	\
	serialize-full		\
	$(NULL)

common_cppflags = -I$(top_srcdir) -I$(top_builddir)
common_cflags = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS) $(JSON_GCOV_CFLAGS)
common_ldadd = $(JSON_LIBS) libjson-glib-1.0.la

array_CFLAGS = $(common_cflags)
array_CPPFLAGS = $(common_cppflags)
array_LDADD = $(common_ldadd)
array_SOURCES = json-glib/tests/array.c

boxed_CFLAGS = $(common_cflags)
boxed_CPPFLAGS = $(common_cppflags)
boxed_LDADD = $(common_ldadd)
boxed_SOURCES = json-glib/tests/boxed.c

builder_CFLAGS = $(common_cflags)
builder_CPPFLAGS = $(common_cppflags)
builder_LDADD = $(common_ldadd)
builder_SOURCES = json-glib/tests/builder.c

generator_CFLAGS = $(common_cflags)
generator_CPPFLAGS = $(common_cppflags)
generator_LDADD = $(common_ldadd)
generator_SOURCES = json-glib/tests/generator.c

gvariant_CFLAGS = $(common_cflags)
gvariant_CPPFLAGS = $(common_cppflags)
gvariant_LDADD = $(common_ldadd)
gvariant_SOURCES = json-glib/tests/gvariant.c

invalid_CFLAGS = $(common_cflags)
invalid_CPPFLAGS = $(common_cppflags)
invalid_LDADD = $(common_ldadd)
invalid_SOURCES = json-glib/tests/invalid.c

node_CFLAGS = $(common_cflags)
node_CPPFLAGS = $(common_cppflags)
node_LDADD = $(common_ldadd)
node_SOURCES = json-glib/tests/node.c

object_CFLAGS = $(common_cflags)
object_CPPFLAGS = $(common_cppflags)
object_LDADD = $(common_ldadd)
object_SOURCES = json-glib/tests/object.c

parser_CFLAGS = $(common_cflags)
parser_CPPFLAGS = $(common_cppflags)
parser_LDADD = $(common_ldadd)
parser_SOURCES = json-glib/tests/parser.c

path_CFLAGS = $(common_cflags)
path_CPPFLAGS = $(common_cppflags)
path_LDADD = $(common_ldadd)
path_SOURCES = json-glib/tests/path.c

reader_CFLAGS = $(common_cflags)
reader_CPPFLAGS = $(common_cppflags)
reader_LDADD = $(common_ldadd)
reader_SOURCES = json-glib/tests/reader.c

serialize_simple_CFLAGS = $(common_cflags)
serialize_simple_CPPFLAGS = $(common_cppflags)
serialize_simple_LDADD = $(common_ldadd)
serialize_simple_SOURCES = json-glib/tests/serialize-simple.c

serialize_complex_CFLAGS = $(common_cflags)
serialize_complex_CPPFLAGS = $(common_cppflags)
serialize_complex_LDADD = $(common_ldadd)
serialize_complex_SOURCES = json-glib/tests/serialize-complex.c

serialize_full_CFLAGS = $(common_cflags)
serialize_full_CPPFLAGS = $(common_cppflags)
serialize_full_LDADD = $(common_ldadd)
serialize_full_SOURCES = json-glib/tests/serialize-full.c
