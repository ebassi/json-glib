/* json-test.vala - Test app for the JSON-GLib Vala bindings
 *
 * Copyright (C) 2007  OpenedHand Ltd.
 *
 * Released under the terms of the GNU General Public License,
 * version 2.1.
 *
 * Author: Emmanuele Bassi  <ebassi@openedhand.com>
 */

using GLib;
using Json;

public class Sample : GLib.Object {
        public void parse (string buffer) {
                var parser = new Json.Parser ();

                parser.parse_start += p => {
                        stdout.printf ("parsing started\n");
                };

                parser.array_end += (p, a) => {
                        stdout.printf ("array (lenght: %d) parsed\n",
                                       a.get_length ());
                };

                parser.object_end += (p, o) => {
                        stdout.printf ("object (size: %d) parsed\n",
                                       o.get_size ());

                        var node = new Json.Node (Json.NodeType.OBJECT);
                        node.set_object (o);

                        var gen = new Json.Generator ();
                        gen.set_root (node);

                        var len = 0;
                        var dump = gen.to_data (ref len);
                        stdout.printf ("** object dump (length: %d): %s\n",
                                       len, dump);
                };

                parser.parse_end += p => {
                        stdout.printf ("parsing complete\n");
                };

                try { parser.load_from_data (buffer); }
                catch (Json.ParserError e) {
                        stdout.printf ("%s\n", e.message);
                        return;
                }

                var root = parser.get_root ();
                stdout.printf ("root node type: %s\n", root.type_name ());

                switch (root.type ()) {
                  case Json.NodeType.OBJECT:
                    break;
                  case Json.NodeType.ARRAY:
                    var array = root.get_array ();
                    stdout.printf ("*  length: %d\n", array.get_length ());

                    var count = 0;
                    foreach (weak Json.Node element in array.get_elements ()) {
                      stdout.printf ("*  element[%d] type: %s\n",
                                     count++,
                                     element.type_name ());
                    }
                    break;
                  case Json.NodeType.VALUE:
                    break;
                  case Json.NodeType.NULL:
                    stdout.printf ("null node\n");
                    break;
                }
                
                return;
        }

        static int main (string[] args) {
                var sample = new Sample ();

                sample.parse ("[ 42, true, { \"foo\" : \"bar\" } ]");

                return 0;
        }
}
