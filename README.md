JSON-GLib
===============================================================================

JSON-GLib implements a full suite of JSON-related tools using GLib and GObject.

Use JSON-GLib it is possible to parse and generate valid JSON data
structures using a DOM-like API. JSON-GLib also integrates with GObject to
provide the ability to serialize and deserialize GObject instances to and from
JSON data types.

JSON is the JavaScript Object Notation; it can be used to represent objects and
object hierarchies while retaining human-readability.

GLib is a C library providing common and efficient data types for the C
developers.

GObject is a library providing a run-time Object Oriented type system for C
developers. GLib and GObject are extensively used by the GTK+ toolkit and by the
GNOME project.

For more information, see:

 * [JSON][json]
 * [GLib and GObject][glib]
 * [JSON-GLib][json-glib]

REQUIREMENTS
------------
In order to build JSON-GLib you will need:

 * pkg-config
 * gtk-doc ≥ 1.13
 * GLib, GIO ≥ 2.38

Optionally, JSON-GLib depends on:

 * GObject-Introspection ≥ 1.38
 * LCov ≥ 1.6

INSTALLATION
------------
To build JSON-GLib just run:

    $ ./configure
    $ make all
    # make install

BUGS
----
JSON-GLib tracks bugs in the GNOME Bugzilla.

If you find a bug in JSON-GLib, please file an issue using
[the appropriate form][bugzilla-enter-bug]. You can also check
[the list of open bugs][bugzilla-bug-page].

Required information:

 * the version of JSON-GLib
  * if it is a development version, the branch of the git repository
 * the JSON data that produced the bug (if any)
 * a small, self-contained test case, if none of the test units exhibit the
   buggy behaviour
 * in case of a segmentation fault, a full stack trace with debugging
   symbols obtained through gdb is greatly appreaciated

RELEASE NOTES
-------------
 * Prior to JSON-GLib 0.10, a JsonSerializable implementation could
   automatically fall back to the default serialization code by simply
   returning NULL from an overridden JsonSerializable::serialize-property
   virtual function. Since JSON-GLib 0.10 this is not possible any more. A
   JsonSerializable is always expected to serialize and deserialize all
   properties. JSON-GLib provides public API for the default implementation
   in case the serialization code wants to fall back to that.

HACKING
-------
JSON-GLib is developed mainly inside a GIT repository available at:

    https://git.gnome.org/browse/json-glib

You can clone the GIT repository with:

    git clone git://git.gnome.org/json-glib

If you want to contribute functionality or bug fixes to JSON-GLib you can either
notify me to pull from your Git repository, or you can attach patches for review
to a bug on [the bug tracking system][bugzilla] using

    git format-patch master -k -s

to generate the patches from each commit. Using [git-bz][git-bz] to automate
this process is strongly encouraged.

Please, try to conform to the coding style used by JSON-GLib, which is the same
used by projects like GLib, GTK+, and Clutter. Coding style conformance is a
requirement for upstream acceptance.

Make sure you always run the test suite when you are fixing bugs. New features
should come with a test unit. Patches that regress the test suite will be
rejected.

AUTHOR, COPYRIGHT AND LICENSING
-------------------------------
JSON-GLib has been written by Emmanuele Bassi

JSON-GLib is released under the terms of the GNU Lesser General Public License,
either version 2.1 or (at your option) any later version.

See the file COPYING for details.

Copyright (C) 2007, 2008  OpenedHand Ltd
Copyright (C) 2009, 2010, 2011, 2012  Intel Corp.
Copyright (C) 2013  Emmanuele Bassi

[json]: http://www.json.org "JSON"
[glib]: http://www.gtk.org "GTK+"
[json-glib]: https://wiki.gnome.org/Project/JsonGlib "JSON-GLib wiki"
[bugzilla]: https://bugzilla.gnome.org "GNOME Bugzilla"
[bugzilla-bug-page]: https://http://bugzilla.gnome.org/browse.cgi?product=json-glib "GNOME Bugzilla - Browse: json-glib"
[bugzilla-enter-bug]: https://bugzilla.gnome.org/enter_bug.cgi?product=json-glib "GNOME Bugzilla - Enter bug: json-glib"
[git-bz]: http://blog.fishsoup.net/2008/11/16/git-bz-bugzilla-subcommand-for-git/
