include $(top_srcdir)/Makefile.decl

ACLOCAL_AMFLAGS = -I build/autotools

SUBDIRS = json-glib doc

if ENABLE_GLIB_TEST
SUBDIRS += tests
endif

DIST_SUBDIRS = json-glib doc tests

pcfiles = json-glib-1.0.pc

json-glib-1.0.pc: json-glib.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

EXTRA_DIST += json-glib.pc.in

CLEANFILES = $(pcfiles) test-report.xml

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-maintainer-flags

dist-hook:
	git-log --stat > ChangeLog.in && \
	cp -f ChangeLog.in $(distdir)/ChangeLog && \
	rm -f ChangeLog.in
