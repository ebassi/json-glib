SUBDIRS = json-glib doc tests

pcfiles = json-glib-1.0.pc

%-1.0.pc: %.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

EXTRA_DIST = json-glib.pc.in

CLEANFILES = $(pcfiles)

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk --enable-maintainer-flags
