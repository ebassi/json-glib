INCLUDES = $(PYTHON_INCLUDES) $(PYJSON_CFLAGS) -I$(top_srcdir)

libjson_glib_ladir = $(pyexecdir)
libjson_glib_la_LTLIBRARIES = libjson-glib.la
libjson_glib_la_LDFLAGS = -module -avoid-version -export-symbols-regex initjsonglib 
libjson_glib_la_LIBADD = \
	$(PYJSON_LIBS) \
	$(JSON_LIBS) \
 	../../json-glib/libjson-glib-1.0.la 
			 
libjson_glib_la_SOURCES = jsonglib-module.c
nodist_libjson_glib_la_SOURCES = jsonglib.c
	
CLEANFILES = jsonglib.c
EXTRA_DIST = \
	jsonglib.defs \
	jsonglib.override \
	gen-defs.sh

jsonglib.c: jsonglib.defs
.defs.c:
	(cd $(srcdir) \
	 && $(PYGTK_CODEGEN) \
	    --py_ssize_t-clean \
	    --override $*.override \
	    --register $(PYGTK_DEFSDIR)/gtk-types.defs \
	    --register $(PYGTK_DEFSDIR)/gdk-types.defs \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

