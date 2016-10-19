NULL =

GENERATED_ITEMS =	\
	json-glib.vcxproj		\
	json-glib.vcxproj.filters	\
	json-glib-install.props

json-glib-install.props: $(top_srcdir)/build/win32/vs10/json-glib-install.propsin json-glib.vs10.headers
	-$(RM) $(top_builddir)/build/win32/vs11/json-glib-install.props
	-$(RM) $(top_builddir)/build/win32/vs12/json-glib-install.props
	-$(RM) $(top_builddir)/build/win32/vs14/json-glib-install.props
	$(CPP) -P - <$(top_srcdir)/build/win32/vs10/json-glib-install.propsin >$@
	rm json-glib.vs10.headers

EXTRA_DIST =	\
	json-glib.sln			\
	json-glib-version-paths.props	\
	json-glib-build-defines.props	\
	json-glib-gen-srcs.props	\
	json-glib-install.propsin	\
	json-glib.vcxprojin		\
	json-glib.vcxproj.filtersin	\
	json-glib-format.vcxproj		\
	json-glib-format.vcxproj.filters	\
	json-glib-validate.vcxproj		\
	json-glib-validate.vcxproj.filters	\
	array.vcxproj		\
	array.vcxproj.filters	\
	boxed.vcxproj	\
	boxed.vcxproj.filters	\
	builder.vcxproj		\
	builder.vcxproj.filters	\
	generator.vcxproj		\
	generator.vcxproj.filters	\
	gvariant.vcxproj		\
	gvariant.vcxproj.filters	\
	json-glib-install.vcxproj	\
	node.vcxproj		\
	node.vcxproj.filters	\
	object.vcxproj		\
	object.vcxproj.filters	\
	parser.vcxproj		\
	parser.vcxproj.filters	\
	path.vcxproj		\
	path.vcxproj.filters	\
	reader.vcxproj		\
	reader.vcxproj.filters	\
	serialize-complex.vcxproj	\
	serialize-complex.vcxproj.filters	\
	serialize-full.vcxproj	\
	serialize-full.vcxproj.filters	\
	serialize-simple.vcxproj	\
	serialize-simple.vcxproj.filters	\
	README.txt	\
	invalid.vcxproj	\
	invalid.vcxproj.filters	\
	$(GENERATED_ITEMS)		\
	$(NULL)

DISTCLEANFILES = $(GENERATED_ITEMS)
