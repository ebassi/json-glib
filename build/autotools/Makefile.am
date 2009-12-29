EXTRA_DIST = \
	shave-libtool.in 	\
	shave.in 		\
	shave.m4 		\
	as-compiler-flag.m4	\
	introspection.m4	\
	Makefile.am.silent	\
	Makefile.am.enums	\
	Makefile.am.marshal	\
	Makefile.am.gtest	\
	Makefile.am.gcov

DISTCLEANFILES = shave-libtool shave

# needed to avoid including Makefile.am.gtest
test-report:
	@true

test:
	@true

check-local: test

.PHONY: test test-report check-local
