using GLib;
using Json;

public class Sample : GLib.Object {
        public bool toggle { get; set; }
        public string name { get; set; }

        public Sample () {
                name = "Hello, world!";
                toggle = true;
        }

        public string to_json () {
                var obj = new Json.Object ();

                var node = new Json.Node (Json.NodeType.VALUE);
                node.set_string (name);
                obj.add_member ("name", node.copy ());

                node = new Json.Node (Json.NodeType.VALUE);
                node.set_boolean (toggle);
                obj.add_member ("toggle", node.copy ());
                
                var root = new Json.Node (Json.NodeType.OBJECT);
                root.set_object (obj);

                var generator = new Json.Generator ();
                generator.pretty = true;
                generator.root = root;

                return generator.to_data ();
        }

        static int main (string[] args) {
                var sample = new Sample ();
                
                stdout.printf ("[manual]    var sample = %s;\n",
                               sample.to_json ());

                var buf = Json.serialize_gobject (sample);
                stdout.printf ("[automatic] var sample = %s;\n",
                               buf);

                return 0;
        }
}
