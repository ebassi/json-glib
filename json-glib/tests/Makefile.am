include $(top_srcdir)/build/autotools/Makefile.am.gtest
include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

INCLUDES = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/json-glib	\
	$(NULL)

AM_CPPFLAGS = $(JSON_DEBUG_CFLAGS) -DTESTS_DATA_DIR=\""$(top_srcdir)/json-glib/tests"\"
AM_CFLAGS = -g $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)

EXTRA_DIST += stream-load.json

noinst_PROGRAMS = $(TEST_PROGS)
progs_ldadd = $(JSON_LIBS) \
	$(top_builddir)/json-glib/libjson-glib-1.0.la

TEST_PROGS             += array-test
array_test_SOURCES      = array-test.c
array_test_LDADD        = $(progs_ldadd)

TEST_PROGS             += object-test
object_test_SOURCES     = object-test.c
object_test_LDADD       = $(progs_ldadd)

TEST_PROGS             += node-test
node_test_SOURCES       = node-test.c
node_test_LDADD         = $(progs_ldadd)

TEST_PROGS             += parser-test
parser_test_SOURCES     = parser-test.c
parser_test_LDADD       = $(progs_ldadd)

TEST_PROGS             += generator-test
generator_test_SOURCES  = generator-test.c
generator_test_LDADD    = $(progs_ldadd)

TEST_PROGS             += builder-test
builder_test_SOURCES    = builder-test.c
builder_test_LDADD      = $(progs_ldadd)

TEST_PROGS             += reader-test
reader_test_SOURCES     = reader-test.c
reader_test_LDADD       = $(progs_ldadd)
