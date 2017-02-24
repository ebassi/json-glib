SUBDIRS = vs9 vs10 vs11 vs12 vs14 vs15

if HAVE_INTROSPECTION
GENERATED_ITEMS = \
	introspection.body.mak	\
	Json_1_0_gir_list

introspection.body.mak: Json-1.0.gir.msvc.introspect
	-$(RM) introspection.body.mak
	for F in `ls *.msvc.introspect`; do \
		case $$F in \
			*)	cat $(top_builddir)/build/win32/$$F >>$@ \
			;; \
		esac; \
	done
	$(RM) Json-1.0.gir.msvc.introspect
else
GENERATED_ITEMS =
endif

EXTRA_DIST = \
	config.h.win32.in			\
	config.h.win32			\
	detectenv-msvc.mak			\
	introspection-msvc.mak		\
	json-glib-introspection-msvc.mak	\
	$(GENERATED_ITEMS)

DISTCLEANFILES = $(GENERATED_ITEMS)
