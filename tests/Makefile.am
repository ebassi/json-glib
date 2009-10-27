include $(top_srcdir)/build/autotools/Makefile.am.gtest
include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

noinst_PROGRAMS = $(TEST_PROGS)

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/json-glib
AM_CFLAGS = -g $(JSON_CFLAGS)
AM_CPPFLAGS = $(JSON_DEBUG_CFLAGS)

progs_ldadd = $(top_builddir)/json-glib/libjson-glib-1.0.la $(JSON_LIBS)

TESTS_ENVIRONMENT = srcdir=$(srcdir)

TEST_PROGS          += test-parser
test_parser_SOURCES  = test-parser.c
test_parser_LDADD    = $(progs_ldadd)

TEST_PROGS             += test-generator
test_generator_SOURCES  = test-generator.c
test_generator_LDADD    = $(progs_ldadd)

TEST_PROGS                    += test-serialize-simple
test_serialize_simple_SOURCES  = test-serialize-simple.c
test_serialize_simple_LDADD    = $(progs_ldadd)

TEST_PROGS                     += test-serialize-complex
test_serialize_complex_SOURCES  = test-serialize-complex.c
test_serialize_complex_LDADD    = $(progs_ldadd)

TEST_PROGS                  += test-serialize-full
test_serialize_full_SOURCES  = test-serialize-full.c
test_serialize_full_LDADD    = $(progs_ldadd)
