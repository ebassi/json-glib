include $(top_srcdir)/Makefile.decl

NULL =

noinst_PROGRAMS = $(TEST_PROGS)

INCLUDES = -I$(top_srcdir) $(JSON_CFLAGS)
progs_ldadd = $(top_builddir)/json-glib/libjson-glib-1.0.la $(JSON_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/json-glib \
	$(JSON_DEBUG_CFLAGS)

AM_CFLAGS = -g

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
