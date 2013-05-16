include $(top_srcdir)/build/autotools/Makefile.am.gtest
include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

DISTCLEANFILES =

insttestdir=$(pkglibexecdir)/installed-tests

AM_CPPFLAGS = \
	$(JSON_DEBUG_CFLAGS) \
	-DTESTS_DATA_DIR=\""$(top_srcdir)/json-glib/tests"\" \
	-I$(top_srcdir)	\
	-I$(top_srcdir)/json-glib \
	$(NULL)

AM_CFLAGS = -g $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)

LDADD = \
	../libjson-glib-1.0.la \
	$(JSON_LIBS) \
	$(NULL)

all_test_programs = \
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

test_files = \
	stream-load.json 	\
	$(NULL)

if BUILD_MODULAR_TESTS
TEST_PROGS += $(all_test_programs)
noinst_PROGRAMS = $(TEST_PROGS)
endif

if BUILDOPT_INSTALL_TESTS
insttest_PROGRAMS = $(all_test_programs)

testmetadir = $(datadir)/installed-tests/$(PACKAGE)
testmeta_DATA = $(all_test_programs:=.test)

testdatadir=$(insttestdir)
testdata_DATA = $(test_files)

testdata_SCRIPTS = $(test_script_files)
endif

EXTRA_DIST += $(test_files)

%.test: % Makefile
	$(AM_V_GEN) (echo '[Test]' > $@.tmp; \
	echo 'Type=session' >> $@.tmp; \
	echo 'Exec=env JSON_TEST_DATA=$(pkglibexecdir)/installed-tests $(pkglibexecdir)/installed-tests/$<' >> $@.tmp; \
	mv $@.tmp $@)

-include $(top_srcdir)/build/autotools/Makefile.am.gitignore
