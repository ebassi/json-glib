# bump micro_version to the next even number for each point release
# bump micro_version to the next odd number after each release
m4_define([json_major_version], [0])
m4_define([json_minor_version], [10])
m4_define([json_micro_version], [4])

m4_define([json_version], [json_major_version.json_minor_version.json_micro_version])

m4_define([json_release_status],
          [m4_if(m4_eval(json_micro_version % 2), [1], [git],
                 [m4_if(m4_eval(json_minor_version % 2), [1], [snapshot],
                                                              [release])])])

# bump up by 1 for every micro release with no API changes, otherwise
# set to 0. after release, bump up by 1
m4_define([json_interface_age], [3])
m4_define([json_binary_age], [m4_eval(100 * json_minor_version + json_micro_version)])

m4_define([lt_current], [m4_eval(100 * json_minor_version + json_micro_version - json_interface_age)])
m4_define([lt_revision], [json_interface_age])
m4_define([lt_age], [m4_eval(json_binary_age - json_interface_age)])

m4_define([glib_req_version], [2.15])

AC_PREREQ([2.61])

AC_INIT([json-glib],
        [json_version],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=json-glib],
        [json-glib])

AC_CONFIG_SRCDIR([json-glib/json-glib.h])
AC_CONFIG_MACRO_DIR([build/autotools])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_CC_C_O
AM_PROG_LIBTOOL

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

JSON_MAJOR_VERSION=json_major_version
JSON_MINOR_VERSION=json_minor_version
JSON_MICRO_VERSION=json_micro_version
JSON_VERSION=json_version
JSON_RELEASE_STATUS=json_release_status
AC_SUBST(JSON_MAJOR_VERSION)
AC_SUBST(JSON_MICRO_VERSION)
AC_SUBST(JSON_MINOR_VERSION)
AC_SUBST(JSON_VERSION)
AC_SUBST(JSON_RELEASE_STATUS)

JSON_LT_CURRENT=lt_current
JSON_LT_REVISION=lt_revision
JSON_LT_AGE=lt_age
JSON_LT_VERSION="$JSON_LT_CURRENT:$JSON_LT_REVISION:$JSON_LT_AGE"
JSON_LT_LDFLAGS="-version-info $JSON_LT_VERSION"
AC_SUBST(JSON_LT_LDFLAGS)

AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
AC_SUBST(GLIB_PREFIX)

PKG_CHECK_MODULES(JSON, [gobject-2.0 >= glib_req_version])
AC_SUBST(JSON_CFLAGS)
AC_SUBST(JSON_LIBS)

AM_CONDITIONAL(ENABLE_GLIB_TEST, [test "x$enable_glibtest" = "xyes"])

dnl = Enable debug level ===================================================

m4_define([debug_default], [m4_if(m4_eval(json_minor_version % 2), [1], [yes], [minimum])])

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [turn on debugging @<:@default=debug_default@:>@])],
              [],
              [enable_debug=debug_default])

AS_CASE([$enable_debug],

        [yes],
        [
          test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
          JSON_DEBUG_CFLAGS="-DJSON_ENABLE_DEBUG"
        ],

        [minimum],
        [
          JSON_DEBUG_CFLAGS="-DJSON_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
        ],

        [no],
        [
          JSON_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
        ],

        [AC_MSG_ERROR([Unknown argument to --enable-debug])]
)

AC_SUBST(JSON_DEBUG_CFLAGS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(json_minor_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=maintainer_flags_default@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-Werror -Wall -Wshadow -Wcast-align
                            -Wno-uninitialized -Wformat-security
                            -Wno-strict-aliasing -Winit-self
                            -Wempty-body -Wdeclaration-after-statement
                            -Wno-strict-aliasing -Wno-strict-aliasing"])
      ]
)
AC_SUBST([MAINTAINER_CFLAGS])

GTK_DOC_CHECK([1.11])

GOBJECT_INTROSPECTION_CHECK([0.6.4])

m4_ifdef([AM_SILENT_RULES],
         [
           AM_SILENT_RULES([yes])
           use_shave=no
         ],
         [
           SHAVE_INIT([build/autotools], [enable])
           AC_CONFIG_FILES([
                build/autotools/shave-libtool
                build/autotools/shave
           ])
           use_shave=yes
         ])

AM_CONDITIONAL([USE_SHAVE], [test "x$use_shave" = "xyes"])

AC_CONFIG_FILES([
        Makefile
        build/Makefile
        build/autotools/Makefile
        json-glib/Makefile
        json-glib/json-version.h
        json-glib/tests/Makefile
        tests/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        json-glib.pc
])

AC_OUTPUT

dnl === Summary ===============================================================

echo ""
echo " Json-GLib - $VERSION (${JSON_RELEASE_STATUS})"
echo ""
echo " • Prefix: ${prefix}"
echo ""
echo " • Debug level: ${enable_debug}"
echo " • Compiler flags: ${MAINTAINER_CFLAGS}"
echo " • Use shave: ${use_shave}"
echo ""
echo " • API reference: ${enable_gtk_doc}"
echo ""
echo " • Enable test suite: ${enable_glibtest}"
echo " • Build introspection data: ${enable_introspection}"
echo ""
