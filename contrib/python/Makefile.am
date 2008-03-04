INCLUDES = $(PYTHON_INCLUDES)  $(PYBEAGLE_CFLAGS)  -I$(top_srcdir)

libjson_glib_1_0_ladir = $(pyexecdir)
libjson_glib_1_0_la_LTLIBRARIES = libjson-glib-1.0.la
libjson_glib_1_0_la_LDFLAGS = -module -avoid-version -export-symbols-regex initjsonglib 
libjson_glib_1_0_la_LIBADD = \
	$(PYJSON_LIBS) \
	$(JSON_LIBS) \
 	../../json-glib/libjson-glib-1.0.la 
			 
libjson_glib_1_0_la_SOURCES = json-glib-module.c 
nodist_libjson_glib_1_0_la_SOURCES = json-glib.c
	
CLEANFILES = json-glib.c
EXTRA_DIST = \
	json-glib.defs \
	json-glib.override \
	gen-defs.sh

json-glib.c: json-glib.defs 
.defs.c:
	(cd $(srcdir) \
	 && $(PYGTK_CODEGEN) \
	    --override $*.override \
	    --register $(PYGTK_DEFSDIR)/gtk-types.defs \
	    --register $(PYGTK_DEFSDIR)/gdk-types.defs \
	    --prefix py$* $*.defs) > gen-$*.c \
	&& cp gen-$*.c $*.c \
	&& rm -f gen-$*.c

