/* json-glib-1.0.vapi: Vala bindings for JSON-GLib
 *
 * Copyright (C) 2007  OpenedHand Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 *
 * Author: Emmanuele Bassi  <ebassi@openedhand.com>
 */

[Import ()]
[CCode (cprefix = "Json", lower_case_cprefix = "json_", cheader_filename = "json-glib/json-glib.h")]
namespace Json {
        [CCode (ref_function = "json_object_ref", unref_function = "json_object_unref")]
        public class Object : GLib.Boxed {
                [CCode (cname = "json_object_new")]
                public Object ();
                public void add_member (string! name, Json.Node# node);
                public bool has_member (string! name);
                public weak Json.Node get_member (string! name);
                public Json.Node dup_member (string! name);
                public GLib.List<string> get_members ();
                public GLib.List<weak Json.Node> get_values ();
                public void remove_member (string! name);
                public uint get_size ();
        }

        [CCode (ref_function = "json_array_ref", unref_function = "json_array_unref")]
        public class Array : GLib.Boxed {
                [CCode (cname = "json_array_new")]
                public Array ();
                [CCode (cname = "json_array_sized_new")]
                public Array.sized (uint reserved_size);
                public void add_element (Json.Node# node);
                public weak Json.Node get_element (uint index_);
                public Json.Node dup_element (uint index_);
                public GLib.List<weak Json.Node> get_elements ();
                public void remove_element (uint index_);
                public uint get_length ();
        }

        [CCode (cprefix = "JSON_NODE_", cheader_filename = "json-glib/json-types.h")]
        public enum NodeType {
                OBJECT,
                ARRAY,
                VALUE,
                NULL
        }

        [CCode (copy_function = "json_node_copy", free_function = "json_node_free", type_id = "JSON_TYPE_NODE")]
        public class Node : GLib.Boxed {
                [CCode (cname = "JSON_NODE_TYPE")]
                public NodeType type ();
                public Node (NodeType type);
                public Node copy ();
                public GLib.Type get_value_type ();
                public void set_object (Json.Object v_object);
                public void take_object (Json.Object# v_object);
                public weak Json.Object get_object ();
                public Json.Object dup_object ();
                public void set_array (Json.Array v_array);
                public void take_array (Json.Array# v_array);
                public weak Json.Array get_array ();
                public Json.Array dup_array ();
                public void set_string (string value);
                public weak string get_string ();
                public string dup_string ();
                public void set_int (int value);
                public int get_int ();
                public void set_double (double value);
                public double get_double ();
                public void set_boolean (bool value);
                public bool get_boolean ();
                public weak Json.Node get_parent ();
                public weak string type_name ();
        }

        [CCode (cprefix = "JSON_PARSER_ERROR_", cheader_filename = "json-glib/json-parser.h")]
        public enum ParserError {
                PARSE,
                UNKNOWN
        }

        [CCode (cheader_filename = "json-glib/json-parser.h")]
        public class Parser : GLib.Object {
                public Parser ();
                public bool load_from_file (string filename) throws GLib.Error;
                public bool load_from_data (string buffer, ulong length = -1) throws ParserError;
                public weak Json.Node peek_root ();
                public Json.Node get_root ();
                public uint get_current_line ();
                public uint get_current_pos ();
                public bool has_assingment (out string variable_name);
                public signal void parse_start ();
                public signal void parse_end ();
                public signal void object_start ();
                public signal void object_member (Json.Object object, string name);
                public signal void object_end (Json.Object object);
                public signal void array_start ();
                public signal void array_element (Json.Array array, uint index_);
                public signal void array_end (Json.Array array);
                public signal void error (GLib.Error error);
        }

        [CCode (cheader_filename = "json-glib/json-generator.h")]
        public class Generator : GLib.Object {
                public Generator ();
                public string to_data (out ulong length = null);
                public bool to_file (string! filename) throws GLib.FileError;
                public void set_root (Json.Node node);
        }
}
