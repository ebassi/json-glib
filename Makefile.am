ACLOCAL_AMFLAGS = -I build/autotools

EXTRA_DIST = \
	meson.build \
	meson_options.txt \
	doc/reference/meson.build \
	doc/reference/xml/meson.build \
	doc/reference/xml/gtkdocentities.ent.in \
	json-glib/meson.build \
	json-glib/tests/meson.build \
	po/meson.build \
	$()

SUBDIRS = json-glib doc build po

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-man --enable-maintainer-flags --enable-introspection

dist-hook:
	@if test -d "$(srcdir)/.git"; then \
	  echo Generating ChangeLog ; \
	  ( cd "$(srcdir)" \
	    && $(top_srcdir)/build/missing --run git log --stat ) > ChangeLog.tmp \
	  && mv -f ChangeLog.tmp $(top_distdir)/ChangeLog \
	  || ( rm -f ChangeLog.tmp; \
	       echo Failed to generate ChangeLog >&2 ); \
	else \
	  echo A git checkout is required to generate a ChangeLog >&2; \
	fi

if ENABLE_GCOV
# use recursive makes in order to ignore errors during check/perf
lcov:
	-$(MAKE) $(AM_MAKEFLAGS) check
	$(MAKE) $(AM_MAKEFLAGS) genlcov

# we have to massage the lcov.info file slightly to hide the effect of libtool
# placing the objects files in the .libs/ directory separate from the *.c
genlcov:
	$(LTP) --directory $(top_builddir) --capture --output-file json-lcov.info --test-name JSON_TEST --no-checksum
	$(SED) -e 's#.libs/##' < json-lcov.info > json-lcov.info.tmp
	LANG=C $(LTP_GENHTML) --prefix $(top_builddir) --output-directory json-lcov --title "JSON-GLib Code Coverage" --show-details json-lcov.info.tmp
	rm -f json-lcov.info.tmp

lcov-clean:
	-$(LTP) --directory $(top_builddir) -z
	-$(RM) -rf json-lcov.info json-lcov
else
lcov genlcov lcov-clean:
	@echo You need to configure JSON-GLib with support for gcov enabled.
	@echo e.g., ./configure --enable-gcov
endif

.PHONY: lcov genlcov lcov-clean
