#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib-object.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#define TEST_TYPE_ENUM                  (test_enum_get_type ())
#define TEST_TYPE_FLAGS                 (test_flags_get_type ())
#define TEST_TYPE_BOXED                 (test_boxed_get_type ())
#define TEST_TYPE_OBJECT                (test_object_get_type ())
#define TEST_OBJECT(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_OBJECT, TestObject))
#define TEST_IS_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_OBJECT))
#define TEST_OBJECT_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_OBJECT, TestObjectClass))
#define TEST_IS_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_OBJECT))
#define TEST_OBJECT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_OBJECT, TestObjectClass))

typedef enum {
  TEST_ENUM_FOO,
  TEST_ENUM_BAR,
  TEST_ENUM_BAZ
} TestEnum;

typedef enum {
  TEST_FLAGS_NONE = 0,
  TEST_FLAGS_FOO  = 1 << 0,
  TEST_FLAGS_BAR  = 1 << 1,
  TEST_FLAGS_BAZ  = 1 << 2
} TestFlags;

typedef struct _TestBoxed               TestBoxed;
typedef struct _TestObject              TestObject;
typedef struct _TestObjectClass         TestObjectClass;

struct _TestBoxed
{
  gint foo;
  gboolean bar;
};

struct _TestObject
{
  GObject parent_instance;

  gint m_int;
  gboolean m_bool;
  gchar *m_str;
  TestBoxed m_boxed;
  TestEnum m_enum;
  gchar **m_strv;
  TestFlags m_flags;

  TestObject *m_obj;
};

struct _TestObjectClass
{
  GObjectClass parent_class;
};

GType test_object_get_type (void);

/*** implementation ***/

static TestBoxed *
test_boxed_copy (const TestBoxed *src)
{
  TestBoxed *copy = g_slice_new (TestBoxed);

  *copy = *src;

  return copy;
}

static void
test_boxed_free (TestBoxed *boxed)
{
  if (G_LIKELY (boxed))
    {
      g_slice_free (TestBoxed, boxed);
    }
}

GType
test_boxed_get_type (void)
{
  static GType b_type = 0;

  if (G_UNLIKELY (b_type == 0))
    b_type = g_boxed_type_register_static ("TestBoxed",
                                           (GBoxedCopyFunc) test_boxed_copy,
                                           (GBoxedFreeFunc) test_boxed_free);

  return b_type;
}

GType
test_enum_get_type (void)
{
  static GType e_type = 0;

  if (G_UNLIKELY (e_type == 0))
    {
      static const GEnumValue values[] = {
        { TEST_ENUM_FOO, "TEST_ENUM_FOO", "foo" },
        { TEST_ENUM_BAR, "TEST_ENUM_BAR", "bar" },
        { TEST_ENUM_BAZ, "TEST_ENUM_BAZ", "baz" },
        { 0, NULL, NULL }
      };

      e_type = g_enum_register_static ("TestEnum", values);
    }

  return e_type;
}

GType
test_flags_get_type (void)
{
  static GType e_type = 0;

  if (G_UNLIKELY (e_type == 0))
    {
      static const GFlagsValue values[] = {
        { TEST_FLAGS_NONE, "TEST_FLAGS_NONE", "none" },
        { TEST_FLAGS_FOO, "TEST_FLAGS_FOO", "foo" },
        { TEST_FLAGS_BAR, "TEST_FLAGS_BAR", "bar" },
        { TEST_FLAGS_BAZ, "TEST_FLAGS_BAZ", "baz" },
        { 0, NULL, NULL }
      };

      e_type = g_flags_register_static ("TestFlags", values);
    }

  return e_type;
}

enum
{
  PROP_0,

  PROP_FOO,
  PROP_BAR,
  PROP_BAZ,
  PROP_BLAH,
  PROP_MEH,
  PROP_MAH,
  PROP_FLAGS,
  PROP_TEST
};

static void json_serializable_iface_init (gpointer g_iface);

G_DEFINE_TYPE_WITH_CODE (TestObject, test_object, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (JSON_TYPE_SERIALIZABLE,
                                                json_serializable_iface_init));

static JsonNode *
test_object_serialize_property (JsonSerializable *serializable,
                                const gchar      *name,
                                const GValue     *value,
                                GParamSpec       *pspec)
{
  JsonNode *retval;

  if (strcmp (name, "blah") == 0)
    {
      TestBoxed *boxed;
      JsonObject *obj;

      retval = json_node_new (JSON_NODE_OBJECT);
      obj = json_object_new ();
      
      boxed = g_value_get_boxed (value);

      json_object_set_int_member (obj, "foo", boxed->foo);
      json_object_set_boolean_member (obj, "bar", boxed->bar);

      json_node_take_object (retval, obj);

      test_boxed_free (boxed);
    }
  else
    {
      GValue copy = { 0, };

      retval = json_node_new (JSON_NODE_VALUE);

      g_value_init (&copy, G_PARAM_SPEC_VALUE_TYPE (pspec));
      g_value_copy (value, &copy);
      json_node_set_value (retval, &copy);
      g_value_unset (&copy);
    }

  return retval;
}

static void
json_serializable_iface_init (gpointer g_iface)
{
  JsonSerializableIface *iface = g_iface;

  iface->serialize_property = test_object_serialize_property;
}

static void
test_object_finalize (GObject *gobject)
{
  g_free (TEST_OBJECT (gobject)->m_str);
  g_strfreev (TEST_OBJECT (gobject)->m_strv);

  if (TEST_OBJECT (gobject)->m_obj != NULL)
    g_object_unref (TEST_OBJECT (gobject)->m_obj);

  G_OBJECT_CLASS (test_object_parent_class)->finalize (gobject);
}

static void
test_object_set_property (GObject      *gobject,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  switch (prop_id)
    {
    case PROP_FOO:
      TEST_OBJECT (gobject)->m_int = g_value_get_int (value);
      break;

    case PROP_BAR:
      TEST_OBJECT (gobject)->m_bool = g_value_get_boolean (value);
      break;

    case PROP_BAZ:
      g_free (TEST_OBJECT (gobject)->m_str);
      TEST_OBJECT (gobject)->m_str = g_value_dup_string (value);
      break;

    case PROP_MEH:
      TEST_OBJECT (gobject)->m_enum = g_value_get_enum (value);
      break;

    case PROP_MAH:
      g_strfreev (TEST_OBJECT (gobject)->m_strv);
      TEST_OBJECT (gobject)->m_strv = g_strdupv (g_value_get_boxed (value));
      break;

    case PROP_FLAGS:
      TEST_OBJECT (gobject)->m_flags = g_value_get_flags (value);
      break;

    case PROP_TEST:
      g_clear_object (&(TEST_OBJECT (gobject)->m_obj));
      TEST_OBJECT (gobject)->m_obj = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
test_object_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  switch (prop_id)
    {
    case PROP_FOO:
      g_value_set_int (value, TEST_OBJECT (gobject)->m_int);
      break;

    case PROP_BAR:
      g_value_set_boolean (value, TEST_OBJECT (gobject)->m_bool);
      break;

    case PROP_BAZ:
      g_value_set_string (value, TEST_OBJECT (gobject)->m_str);
      break;

    case PROP_BLAH:
      g_value_set_boxed (value, &(TEST_OBJECT (gobject)->m_boxed));
      break;

    case PROP_MEH:
      g_value_set_enum (value, TEST_OBJECT (gobject)->m_enum);
      break;

    case PROP_MAH:
      g_value_set_boxed (value, TEST_OBJECT (gobject)->m_strv);
      break;

    case PROP_FLAGS:
      g_value_set_flags (value, TEST_OBJECT (gobject)->m_flags);
      break;

    case PROP_TEST:
      g_value_set_object (value, TEST_OBJECT (gobject)->m_obj);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
test_object_class_init (TestObjectClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = test_object_set_property;
  gobject_class->get_property = test_object_get_property;
  gobject_class->finalize = test_object_finalize;

  g_object_class_install_property (gobject_class,
                                   PROP_FOO,
                                   g_param_spec_int ("foo", "Foo", "Foo",
                                                     0, G_MAXINT, 42,
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_BAR,
                                   g_param_spec_boolean ("bar", "Bar", "Bar",
                                                         FALSE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (gobject_class,
                                   PROP_BAZ,
                                   g_param_spec_string ("baz", "Baz", "Baz",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_BLAH,
                                   g_param_spec_boxed ("blah", "Blah", "Blah",
                                                       TEST_TYPE_BOXED,
                                                       G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_MEH,
                                   g_param_spec_enum ("meh", "Meh", "Meh",
                                                      TEST_TYPE_ENUM,
                                                      TEST_ENUM_BAR,
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_CONSTRUCT));
  g_object_class_install_property (gobject_class,
                                   PROP_MAH,
                                   g_param_spec_boxed ("mah", "Mah", "Mah",
                                                       G_TYPE_STRV,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_FLAGS,
                                   g_param_spec_flags ("flags", "Flags", "Flags",
                                                       TEST_TYPE_FLAGS,
                                                       TEST_FLAGS_NONE,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_CONSTRUCT));
  g_object_class_install_property (gobject_class,
                                   PROP_TEST,
                                   g_param_spec_object ("test", "Test", "Test",
                                                        TEST_TYPE_OBJECT,
                                                        G_PARAM_READWRITE));
}

static void
test_object_init (TestObject *object)
{
  object->m_int = 0;
  object->m_bool = FALSE;
  object->m_str = NULL;

  object->m_boxed.foo = object->m_int;
  object->m_boxed.bar = object->m_bool;

  object->m_enum = TEST_ENUM_BAR;

  object->m_strv = NULL;

  object->m_flags = TEST_FLAGS_NONE;

  object->m_obj = NULL;
}

static const gchar *var_test =
"{\n"
"  \"foo\"  : 42,\n"
"  \"bar\"  : true,\n"
"  \"baz\"  : \"hello\",\n"
"  \"meh\"  : \"baz\",\n"
"  \"mah\"  : [ \"hello\", \", \", \"world\", \"!\" ],\n"
"  \"test\" : {\n"
"    \"bar\"   : true,\n"
"    \"baz\"   : \"world\",\n"
"    \"meh\"   : 0,\n"
"    \"flags\" : \"foo|bar\""
"  }\n"
"}";

static void
test_deserialize (void)
{
  TestObject *test;
  GObject *object;
  GError *error;
  gchar *str;

  error = NULL;
  object = json_gobject_from_data (TEST_TYPE_OBJECT, var_test, -1, &error);
  if (error)
    g_error ("*** Unable to parse buffer: %s\n", error->message);

  if (g_test_verbose ())
    g_print ("*** TestObject ***\n"
             " foo: %s\n"
             " bar: %s\n"
             " baz: %s\n"
             " meh: %s\n",
             TEST_OBJECT (object)->m_int == 42             ? "<true>" : "<false>",
             TEST_OBJECT (object)->m_bool == TRUE          ? "<true>" : "<false>",
             TEST_OBJECT (object)->m_str != NULL           ? "<true>" : "<false>",
             TEST_OBJECT (object)->m_enum == TEST_ENUM_BAZ ? "<true>" : "<false>");

  g_assert_cmpint (TEST_OBJECT (object)->m_int, ==, 42);
  g_assert (TEST_OBJECT (object)->m_bool);
  g_assert_cmpstr (TEST_OBJECT (object)->m_str, ==, "hello");
  g_assert_cmpint (TEST_OBJECT (object)->m_enum, ==, TEST_ENUM_BAZ);

  g_assert (TEST_OBJECT (object)->m_strv != NULL);
  g_assert_cmpint (g_strv_length (TEST_OBJECT (object)->m_strv), ==, 4);

  str = g_strjoinv (NULL, TEST_OBJECT (object)->m_strv);
  g_assert_cmpstr (str, ==, "hello, world!");
  g_free (str);

  g_assert (TEST_IS_OBJECT (TEST_OBJECT (object)->m_obj));
  test = TEST_OBJECT (TEST_OBJECT (object)->m_obj);
  g_assert (test->m_bool);
  g_assert_cmpstr (test->m_str, ==, "world");
  g_assert_cmpint (test->m_enum, ==, TEST_ENUM_FOO);
  g_assert_cmpint (test->m_flags, ==, TEST_FLAGS_FOO | TEST_FLAGS_BAR);

  g_object_unref (object);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/deserialize/json-to-gobject", test_deserialize);

  return g_test_run ();
}
