# bump micro_version to the next even number for each point release
# bump micro_version to the next odd number after each release
m4_define([json_major_version], [0])
m4_define([json_minor_version], [15])
m4_define([json_micro_version], [2])

m4_define([json_version], [json_major_version.json_minor_version.json_micro_version])

m4_define([json_release_status],
          [m4_if(m4_eval(json_micro_version % 2), [1], [git],
                 [m4_if(m4_eval(json_minor_version % 2), [1], [snapshot],
                                                              [release])])])

# bump up by 1 for every micro release with no API changes, otherwise
# set to 0. after release, bump up by 1
m4_define([json_interface_age], [0])
m4_define([json_binary_age], [m4_eval(100 * json_minor_version + json_micro_version)])

m4_define([lt_current], [m4_eval(100 * json_minor_version + json_micro_version - json_interface_age)])
m4_define([lt_revision], [json_interface_age])
m4_define([lt_age], [m4_eval(json_binary_age - json_interface_age)])

m4_define([glib_req_version], [2.31.0])

AC_PREREQ([2.63])

AC_INIT([json-glib],
        [json_version],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=json-glib],
        [json-glib],
        [http://live.gnome.org/JsonGlib])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([json-glib/json-glib.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build/autotools])

AM_INIT_AUTOMAKE([1.11 no-define foreign -Wno-portability dist-xz no-dist-gzip tar-ustar])

AM_SILENT_RULES([yes])
AM_PATH_GLIB_2_0
AM_PROG_CC_C_O

LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

dnl === Versioning ============================================================

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

dnl === Platform checks =======================================================

platform_linux=no
platform_win32=no

AC_CANONICAL_HOST
AC_MSG_CHECKING([if building for some Win32 platform])
AS_CASE([$host],

        [*-*-mingw*|*-*-cygwin*],
        [
          JSON_LT_LDFLAGS="$JSON_LT_LDFLAGS -no-undefined"
          platform_win32=yes
        ],

        [*-*-linux*],
        [
          platform_linux=yes
        ],

        []
)
AC_MSG_RESULT([$platform_win32])

AM_CONDITIONAL(OS_LINUX, [test "$platform_linux" = "yes"])
AM_CONDITIONAL(OS_WIN32, [test "$platform_win32" = "yes"])

AC_SUBST(JSON_LT_LDFLAGS)

dnl === Dependencies ==========================================================

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
AC_SUBST(GLIB_PREFIX)

PKG_CHECK_MODULES(JSON, [gobject-2.0 >= glib_req_version gio-2.0])

AM_CONDITIONAL(ENABLE_GLIB_TEST, [test "x$enable_glibtest" = "xyes"])

dnl === Enable debug level ====================================================

m4_define([debug_default], [m4_if(m4_eval(json_minor_version % 2), [1], [yes], [minimum])])

AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [turn on debugging @<:@default=]debug_default[@:>@])],
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

dnl === Enable strict compiler flags ==========================================

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(json_minor_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AS_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=]maintainer_flags_default[@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-Wall -Wshadow -Wcast-align
                            -Wno-uninitialized -Wformat -Wformat-security
                            -Wno-strict-aliasing -Winit-self
                            -Wempty-body -Wdeclaration-after-statement
                            -Wno-strict-aliasing"])
      ]
)
MAINTAINER_CFLAGS="${MAINTAINER_CFLAGS#*  }"
AC_SUBST([MAINTAINER_CFLAGS])

dnl === Test coverage =========================================================

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov], [Enable gcov])],
              [use_gcov=$enableval],
              [use_gcov=no])

AS_IF([test "x$use_gcov" = "xyes"],
      [
        dnl we need gcc:
        AS_IF([test "$GCC" != "yes"], [AC_MSG_ERROR([GCC is required for --enable-gcov])])

        dnl Check if ccache is being used
        AC_CHECK_PROG(SHTOOL, shtool, shtool)
        AS_CASE([`$SHTOOL path $CC`],
                [*ccache*], [gcc_ccache=yes],
                [gcc_ccache=no])

        if test "$gcc_ccache" = "yes" && (test -z "$CCACHE_DISABLE" || test "$CCACHE_DISABLE" != "1"); then
          AC_MSG_ERROR([ccache must be disabled when --enable-gcov option is used. You can disable ccache by setting environment variable CCACHE_DISABLE=1.])
        fi

        ltp_version_list="1.6 1.7 1.8 1.9"
        AC_CHECK_PROG(LTP, lcov, lcov)
        AC_CHECK_PROG(LTP_GENHTML, genhtml, genhtml)

        if test "$LTP"; then
          AC_CACHE_CHECK([for ltp version], json_cv_ltp_version,
                         [
                           json_cv_ltp_version=invalid
                           ltp_version=`$LTP -v 2>/dev/null | $SED -e 's/^.* //'`
                           for ltp_check_version in $ltp_version_list; do
                             if test "$ltp_version" = "$ltp_check_version"; then
                               json_cv_ltp_version="$ltp_check_version (ok)"
                             fi
                           done
                         ])
        else
          ltp_msg="To enable code coverage reporting you must have one of the following LTP versions installed: $ltp_version_list"
          AC_MSG_ERROR([$ltp_msg])
        fi

        case $json_cv_ltp_version in
          ""|invalid[)]
          ltp_msg="You must have one of the following versions of LTP: $ltp_version_list (found: $ltp_version)."
          AC_MSG_ERROR([$ltp_msg])
          LTP="exit 0;"
          ;;
        esac

        if test -z "$LTP_GENHTML"; then
          AC_MSG_ERROR([Could not find genhtml from the LTP package])
        fi

        AC_DEFINE(HAVE_GCOV, 1, [Whether you have gcov])

        dnl Remove all optimization flags from CFLAGS
        changequote({,})
        CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
        JSON_CFLAGS=`echo "$JSON_CFLAGS" | $SED -e 's/-O[0-9]*//g'`
        changequote([,])

        dnl Define the special gcc flags
        JSON_GCOV_CFLAGS="-O0 -fprofile-arcs -ftest-coverage"
        JSON_GCOV_LDADD="-lgcov"

        AC_SUBST(JSON_GCOV_CFLAGS)
        AC_SUBST(JSON_GCOV_LDADD)

        JSON_CFLAGS="$JSON_CFLAGS $CLUTTER_GCOV_CFLAGS"
        JSON_LIBS="$JSON_LIBS $CLUTTER_GCOV_LDADD"
      ])

AM_CONDITIONAL(ENABLE_GCOV, test "x$use_gcov" = "xyes")

dnl === Documentation =========================================================

GTK_DOC_CHECK([1.13], [--flavour no-tmpl])

dnl === Introspection =========================================================

GOBJECT_INTROSPECTION_CHECK([0.9.5])

dnl === Localization ==========================================================

AM_GNU_GETTEXT_VERSION([0.18])
AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE="json-glib-1.0"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
                   ["$GETTEXT_PACKAGE"],
                   [The prefix for our gettext translation domains.])
AS_ALL_LINGUAS

dnl === Output ================================================================

AC_SUBST(JSON_CFLAGS)
AC_SUBST(JSON_LIBS)

AC_CONFIG_FILES([
        Makefile

        build/Makefile
        build/autotools/Makefile
        build/win32/Makefile
        build/win32/config.h.win32
        build/win32/vs9/Makefile
        build/win32/vs10/Makefile

        json-glib/Makefile
        json-glib/json-version.h
        json-glib/tests/Makefile

        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml

        po/Makefile.in

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
echo " • Compiler flags: ${CFLAGS} ${MAINTAINER_CFLAGS}"
echo ""
echo " • API reference: ${enable_gtk_doc}"
echo ""
echo " • Enable test suite: ${enable_glibtest}"
echo " • Build introspection data: ${enable_introspection}"
echo " • Enable test coverage: ${use_gcov}"
echo ""
