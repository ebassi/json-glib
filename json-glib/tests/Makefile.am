include $(top_srcdir)/build/autotools/glib-tap.mk

AM_CFLAGS = -g $(JSON_CFLAGS) $(MAINTAINER_CFLAGS)
LDADD = $(top_builddir)/json-glib//libjson-glib-1.0.la $(JSON_LIBS)
AM_CPPFLAGS = \
	$(JSON_DEBUG_CFLAGS) \
	-DG_LOG_DOMAIN=\"Json\" \
	-I$(top_srcdir)	\
	-I$(top_builddir) \
	-I$(top_srcdir)/json-glib \
	-I$(top_builddir)/json-glib \
	$(NULL)

dist_test_data = \
	stream-load.json	\
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

include $(top_srcdir)/build/autotools/Makefile.am.gitignore
