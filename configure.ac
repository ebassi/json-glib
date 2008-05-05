m4_define([json_major_version], [0])
m4_define([json_minor_version], [6])
m4_define([json_micro_version], [1])

m4_define([json_version],
          [json_major_version.json_minor_version.json_micro_version])

m4_define([json_interface_age], [0])
m4_define([json_binary_age],
          [m4_eval(100 * json_minor_version + json_micro_version)])

m4_define([lt_current],
          [m4_eval(100 * json_minor_version + json_micro_version - json_interface_age)])
m4_define([lt_revision], [json_interface_age])
m4_define([lt_age],
          [m4_eval(json_binary_age - json_interface_age)])

m4_define([glib_req_version], [2.15])

AC_PREREQ([2.59])

AC_INIT([json-glib],
        [json_version],
        [http://bugzilla.openedhand.com/enter_bug.cgi],
        [json-glib])

AC_CONFIG_SRCDIR([json-glib/json-glib.h])

AM_INIT_AUTOMAKE([1.9])
AM_CONFIG_HEADER([config.h])

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_LIBTOOL

JSON_MAJOR_VERSION=json_major_version
JSON_MINOR_VERSION=json_minor_version
JSON_MICRO_VERSION=json_micro_version
JSON_VERSION=json_version
AC_SUBST(JSON_MAJOR_VERSION)
AC_SUBST(JSON_MICRO_VERSION)
AC_SUBST(JSON_MINOR_VERSION)
AC_SUBST(JSON_VERSION)

JSON_LT_CURRENT=lt_current
JSON_LT_REVISION=lt_revision
JSON_LT_AGE=lt_age
JSON_LT_VERSION="$JSON_LT_CURRENT:$JSON_LT_REVISION:$JSON_LT_AGE"
JSON_LT_LDFLAGS="-version-info $JSON_LT_VERSION"
AC_SUBST(JSON_LT_LDFLAGS)

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
AC_SUBST(GLIB_PREFIX)

PKG_CHECK_MODULES(JSON, gobject-2.0 >= glib_req_version)
AC_SUBST(JSON_CFLAGS)
AC_SUBST(JSON_LIBS)

has_vala=no
PKG_CHECK_MODULES(JSON_VALA, vala-1.0, has_vala=yes, has_vala=no)

if test "x$has_vala" = "xyes"; then
  VAPI_DIR=`pkg-config --variable=vapidir vala-1.0`
else
  VAPI_DIR=
fi

AM_CONDITIONAL(HAVE_VALA, [test "x$has_vala" = "xyes"])
AC_SUBST(VAPI_DIR)


dnl = Enable debug level ===================================================

m4_define([debug_default],
          m4_if(m4_eval(json_minor_version % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [turn on debugging @<:@default=debug_default@:>@]),
,
              enable_debug=debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  JSON_DEBUG_CFLAGS="-DJSON_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    JSON_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else # minimum
    JSON_DEBUG_CFLAGS="-DJSON_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(JSON_DEBUG_CFLAGS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
#m4_define([maintainer_flags_default],
#          m4_if(m4_eval(json_minor_version % 2), [1], [yes], [no]))
m4_define([maintainer_flags_default], [no])
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -g -Wall -Wshadow -Wcast-align -Wno-uninitialized -Werror"
else
  CPPFLAGS="$CPPFLAGS -g -Wall"
fi

GTK_DOC_CHECK([1.8])

AC_CONFIG_FILES([
        Makefile
        json-glib/Makefile
        json-glib/json-version.h
        json-glib/tests/Makefile
        tests/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        contrib/Makefile
        json-glib.pc
])

AC_OUTPUT

echo ""
echo " Json-GLib $VERSION"
echo ""
echo "                Prefix: ${prefix}"
echo "           Debug level: ${enable_debug}"
echo "        Compiler flags: ${CPPFLAGS}"
echo "         API reference: ${enable_gtk_doc}"
echo " Install vala bindings: ${has_vala} (${VAPI_DIR})"
echo ""
