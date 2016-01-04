# NMake Makefile to build Introspection Files for JSON-GLib

!include detectenv-msvc.mak

APIVERSION = 1.0

CHECK_PACKAGE =  gio-2.0

!include introspection-msvc.mak

!if "$(BUILD_INTROSPECTION)" == "TRUE"
all: setgirbuildnev Json-$(APIVERSION).gir Json-$(APIVERSION).typelib

setgirbuildenv:
	@set PYTHONPATH=$(PREFIX)\lib\gobject-introspection
	@set PATH=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(PREFIX)\bin;$(PATH)
	@set PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)
	@set LIB=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(LIB)

!include introspection.body.mak

install-introspection: setgirbuildenv Json-$(APIVERSION).gir Json-$(APIVERSION).typelib
	@-copy Json-$(APIVERSION).gir $(G_IR_INCLUDEDIR)
	@-copy /b Json-$(APIVERSION).typelib $(G_IR_TYPELIBDIR)

!else
all:
	@-echo $(ERROR_MSG)
!endif

clean:
	@-del /f/q Json-$(APIVERSION).typelib
	@-del /f/q Json-$(APIVERSION).gir
