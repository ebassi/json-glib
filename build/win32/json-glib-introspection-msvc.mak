# NMake Makefile to build Introspection Files for JSON-GLib

!include detectenv_msvc.mak

APIVERSION = 1.0

CHECK_PACKAGE =  gio-2.0

!include introspection-msvc.mak

!if "$(BUILD_INTROSPECTION)" == "TRUE"
all: setgirbuildnev Json-$(APIVERSION).gir Json-$(APIVERSION).typelib

json_list:
	@-echo Generating Filelist to Introspect for JSON-GLib...
	$(PYTHON2) gen-file-list-jsonglib.py

vs$(VSVER)\$(CFG)\$(PLAT)\bin\Json-$(APIVERSION).lib:
	@-echo Copying Json-1.0.lib from json-glib-1.0.lib
	@-copy /b vs$(VSVER)\$(CFG)\$(PLAT)\bin\json-glib-$(APIVERSION).lib vs$(VSVER)\$(CFG)\$(PLAT)\bin\Json-$(APIVERSION).lib

setgirbuildnev:
	@set CC=$(CC)
	@set PYTHONPATH=$(BASEDIR)\lib\gobject-introspection
	@set PATH=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(BASEDIR)\bin;$(PATH);$(MINGWDIR)\bin
	@set PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)
	@set LIB=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(LIB)

Json-$(APIVERSION).gir: json_list vs$(VSVER)\$(CFG)\$(PLAT)\bin\Json-$(APIVERSION).lib
	@-echo Generating Json-$(APIVERSION).gir...
	$(PYTHON2) $(G_IR_SCANNER) --verbose -I..\..	\
	-I$(BASEDIR)\include\glib-2.0 -I$(BASEDIR)\lib\glib-2.0\include	\
	--namespace=Json --nsversion=$(APIVERSION)	\
	--include=GObject-2.0 --include=Gio-2.0	\
	--no-libtool --library=json-glib-1.0	\
	--reparse-validate --add-include-path=$(BASEDIR)\share\gir-1.0 --add-include-path=.	\
	--warn-all --pkg-export json-glib-$(APIVERSION) --c-include "json-glib/json-glib.h"	\
	-DJSON_COMPILATION=1 -DG_LOG_DOMAIN=\"Json\"	\
	--filelist=json_list -o $@

Json-$(APIVERSION).typelib: Json-$(APIVERSION).gir
	@-echo Compiling Json-$(APIVERSION).typelib...
	$(G_IR_COMPILER) --includedir=. --debug --verbose Json-$(APIVERSION).gir -o Json-$(APIVERSION).typelib

install-introspection: setgirbuildnev Json-$(APIVERSION).gir Json-$(APIVERSION).typelib
	@-copy Json-$(APIVERSION).gir $(G_IR_INCLUDEDIR)
	@-copy /b Json-$(APIVERSION).typelib $(G_IR_TYPELIBDIR)

!else
all:
	@-echo $(ERROR_MSG)
!endif

clean:
	@-del /f/q Json-$(APIVERSION).typelib
	@-del /f/q Json-$(APIVERSION).gir
	@-del /f/q vs$(VSVER)\$(CFG)\$(PLAT)\bin\Json-$(APIVERSION).lib
	@-del /f/q json_list
	@-del /f/q *.pyc
