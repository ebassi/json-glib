include $(top_srcdir)/build/autotools/Makefile.am.gtest
include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

noinst_PROGRAMS = $(TEST_PROGS)

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/json-glib
AM_CPPFLAGS = -DJSON_DISABLE_DEPRECATED $(JSON_DEBUG_CFLAGS)
AM_CFLAGS = $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)

progs_ldadd = $(top_builddir)/json-glib/libjson-glib-1.0.la $(JSON_LIBS)

TESTS_ENVIRONMENT = srcdir=$(srcdir)

TEST_PROGS                    += test-serialize-simple
test_serialize_simple_SOURCES  = test-serialize-simple.c
test_serialize_simple_LDADD    = $(progs_ldadd)

TEST_PROGS                     += test-serialize-complex
test_serialize_complex_SOURCES  = test-serialize-complex.c
test_serialize_complex_LDADD    = $(progs_ldadd)

TEST_PROGS                   += test-serialize-boxed
test_serialize_boxed_SOURCES  = test-serialize-boxed.c
test_serialize_boxed_LDADD    = $(progs_ldadd)

TEST_PROGS                  += test-serialize-full
test_serialize_full_SOURCES  = test-serialize-full.c
test_serialize_full_LDADD    = $(progs_ldadd)
