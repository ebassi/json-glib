include $(top_srcdir)/Makefile.decl

INCLUDES = \
	-g				\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/json-glib	\
	$(JSON_DEBUG_CFLAGS)		\
	$(JSON_CFLAGS)

noinst_PROGRAMS = $(TEST_PROGS)
progs_ldadd = $(top_builddir)/json-glib/libjson-glib-1.0.la

TEST_PROGS         += array-test
array_test_SOURCES  = array-test.c
array_test_LDADD    = $(progs_ldadd)

TEST_PROGS         += node-test
node_test_SOURCES   = node-test.c
node_test_LDADD     = $(progs_ldadd)
