m4_define([json_major_version], [0])
m4_define([json_minor_version], [5])
m4_define([json_micro_version], [0])

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

m4_define([glib_req_version], [2.12])

AC_PREREQ(2.59)
AC_INIT([json-glib], [json_version], [], [json-glib])
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
JSON_LT_LDFLAGS="-versio-info $JSON_LT_VERSION"
AC_SUBST(JSON_LT_LDFLAGS)

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP

PYTHON_REQUIRED=2.3
PYGTK_REQUIRED=2.6

PKG_CHECK_MODULES(JSON, gobject-2.0 >= glib_req_version)

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

AC_ARG_ENABLE(debug,
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

dnl ----------------------------------------------
dnl Python 2.4

AC_ARG_ENABLE(python,
	AC_HELP_STRING([--disable-python], [Disable python support (default auto)]),
	enable_python=$enableval,
	enable_python=auto)

have_python=no
if test "x$enable_python" != "xno"; then
  have_python_version=no
  have_python_headers=no
  have_pygtk_deps=no
  have_pygtk_codegen=no

  AM_PATH_PYTHON()
  if test -z "$PYTHON"; then
    AC_MSG_WARN([Python not found])
  else
    AM_PYTHON_CHECK_VERSION($PYTHON, $PYTHON_REQUIRED,
                            have_python_version=yes,
                            have_python_version=no)
  fi

  if test "x$have_python_version" != "xyes"; then
    AC_MSG_WARN([Python version $PYTHON_REQUIRED not found])
  else
    AM_CHECK_PYTHON_HEADERS(have_python_headers=yes, have_python_headers=no)
  fi

  if test "x$have_python_headers" != "xyes"; then
    AC_MSG_WARN([could not find Python headers])
  else
    PKG_CHECK_MODULES(PYJSON,
                      pygtk-2.0 >= $PYGTK_REQUIRED
                      pygobject-2.0 >= $PYGTK_REQUIRED,
                      have_pygtk_deps=yes,
                      have_pygtk_deps=no)
  fi

  if test "x$have_pygtk_deps" != "xyes"; then
    AC_MSG_WARN([could not find pygtk])
  else
    AC_PATH_PROG(PYGTK_CODEGEN, pygtk-codegen-2.0, no)
  fi

  if test -z "$PYGTK_CODEGEN"; then
    AC_MSG_WARN([could not find pygtk-codegen in path])
  else
    AC_MSG_CHECKING(for pygtk defs)
    PYGTK_DEFSDIR=`$PKG_CONFIG --variable=defsdir pygtk-2.0`
    AC_MSG_RESULT($PYGTK_DEFSDIR)

    have_python=yes
  fi
fi

if test "x$enable_python" = "xyes"; then
  if test "x$have_python" = "xno"; then
    AC_MSG_ERROR([Python bindings are requested, but cannot be built])
  fi
fi

AM_CONDITIONAL(ENABLE_PYTHON, test "x$have_python" = "xyes")
AC_SUBST(PYJSON_LIBS)
AC_SUBST(PYJSON_CFLAGS)
AC_SUBST(PYGTK_DEFSDIR)

dnl ----------------------------------------------

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
        tests/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        contrib/Makefile
        contrib/python/Makefile
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
echo " Build python bindings: ${have_python}"
echo ""
