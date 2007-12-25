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
        public void dump_node (Json.Node node)
        {
          switch (node.type ())
            {
            case Json.NodeType.OBJECT:
              var obj = node.get_object ();
              stdout.printf ("*  size: %d\n", obj.get_size ());

              foreach (weak string member in obj.get_members ())
                {
                  var val = obj.dup_member (member);

                  stdout.printf ("*  member[\"%s\"] type: %s, value:\n",
                                 member,
                                 val.type_name ());

                  dump_node (val);
                }
              break;
            
            case Json.NodeType.ARRAY:
              var array = node.get_array ();
              stdout.printf ("*  length: %d\n", array.get_length ());

              var count = 0;
              foreach (weak Json.Node element in array.get_elements ())
                {
                  stdout.printf ("*  element[%d] type: %s, value:\n",
                                 count++,
                                 element.type_name ());

                  dump_node (element);
                }
              break;

            case Json.NodeType.VALUE:
              switch (node.get_value_type ())
                {
                case typeof (int):
                  stdout.printf ("*** %d\n", node.get_int ());
                  break;
                case typeof (double):
                  stdout.printf ("*** %f\n", node.get_double ());
                  break;
                case typeof (string):
                  stdout.printf ("*** %s\n", node.get_string ());
                  break;
                }
              break;

            case Json.NodeType.NULL:
              stdout.printf ("*** null\n");
              break;
            }
        }

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
                        gen.root = node;

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
                
                dump_node (root);

                return;
        }

        static int main (string[] args) {
                var sample = new Sample ();

                sample.parse ("[ 42, true, { \"foo\" : \"bar\" } ]");

                return 0;
        }
}
