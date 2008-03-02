include $(top_srcdir)/Makefile.decl

test_programs = 	\
	test-01		\
	test-02		\
	test-03		\
	test-04		\
	test-05		\
	test-06		\
	test-07		\
	test-08

noinst_PROGRAMS = $(TEST_PROGS)	$(test_programs)

INCLUDES = -I$(top_srcdir) $(JSON_CFLAGS)
progs_ldadd = $(top_builddir)/json-glib/libjson-glib-1.0.la $(JSON_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/json-glib \
	$(JSON_DEBUG_CFLAGS)

AM_CFLAGS = -g

TESTS = $(test_programs)
TESTS_ENVIRONMENT = srcdir=$(srcdir)

test_01_LDADD = $(progs_ldadd)
test_01_SOURCES = test-01.c

test_02_LDADD = $(progs_ldadd)
test_02_SOURCES = test-02.c

test_03_LDADD = $(progs_ldadd)
test_03_SOURCES = test-03.c

test_04_LDADD = $(progs_ldadd)
test_04_SOURCES = test-04.c

test_05_LDADD = $(progs_ldadd)
test_05_SOURCES = test-05.c

test_06_LDADD = $(progs_ldadd)
test_06_SOURCES = test-06.c

test_07_LDADD = $(progs_ldadd)
test_07_SOURCES = test-07.c

test_08_LDADD = $(progs_ldadd)
test_08_SOURCES = test-08.c
