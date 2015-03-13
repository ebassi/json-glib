DOC_MODULE = json-glib
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml
DOC_SOURCE_DIR = $(top_srcdir)/json-glib $(top_builddir)/json-glib

SCANGOBJ_OPTIONS =
SCAN_OPTIONS = --deprecated-guards="JSON_DISABLE_DEPRECATED" --rebuild-types
MKDB_OPTIONS = --output-format=xml --name-space=json
MKTMPL_OPTIONS =
FIXXREF_OPTIONS = \
	--extra-dir=$(GLIB_PREFIX)/share/gtk-doc/html/glib \
	--extra-dir=$(GLIB_PREFIX)/share/gtk-doc/html/gobject \
	--extra-dir=$(GLIB_PREFIX)/share/gtk-doc/html/gio

HFILE_GLOB = $(top_srcdir)/json-glib/*.h $(top_builddir)/json-glib/*.h
CFILE_GLOB = $(top_srcdir)/json-glib/*.c

IGNORE_HFILES = \
	json-scanner.h 		\
	json-types-private.h 	\
	json-gobject-private.h	\
	json-private.h 		\
	json-marshal.h 		\
	json-glib.h 		\
	json-enum-types.h	\
	json-debug.h

EXTRA_HFILES =

HTML_IMAGES =

content_files = \
	json-glib-format.xml \
	json-glib-validate.xml \
	version.xml

expand_content_files =

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) $(JSON_CFLAGS)
GTKDOC_LIBS = $(top_builddir)/json-glib/libjson-glib-1.0.la $(JSON_LIBS)

include $(top_srcdir)/gtk-doc.make

EXTRA_DIST += version.xml.in

man_MANS =

if ENABLE_GTK_DOC
TESTS_ENVIRONMENT = \
	DOC_MODULE=$(DOC_MODULE) \
	DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
	SRCDIR=$(abs_srcdir) \
	BUILDDIR=$(abs_builddir)

TESTS = $(GTKDOC_CHECK)
endif

if ENABLE_MAN
man_MANS += \
	json-glib-format.1 \
	json-glib-validate.1

XSLTPROC_FLAGS = \
	--nonet \
	--stringparam man.output.quietly 1 \
	--stringparam funcsynopsis.style ansi \
	--stringparam man.th.extra1.suppress 1 \
	--stringparam man.authors.section.enabled 0 \
	--stringparam man.copyright.section.enabled 0

.xml.1:
	$(AM_V_GEN) $(XSLTPROC) $(XSLTPROC_FLAGS) http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<

endif

CLEANFILES ?=
CLEANFILES += $(man_MANS)
EXTRA_DIST += $(man_MANS)
