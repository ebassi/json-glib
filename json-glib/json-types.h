#ifndef __JSON_TYPES_H__
#define __JSON_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define JSON_NODE_TYPE(node)    (((JsonNode *) (node))->type)
#define JSON_TYPE_OBJECT        (json_object_get_type ())
#define JSON_TYPE_ARRAY         (json_array_get_type ())

typedef struct _JsonObject      JsonObject;
typedef struct _JsonArray       JsonArray;
typedef struct _JsonNode        JsonNode;

typedef enum {
  JSON_NODE_OBJECT,
  JSON_NODE_ARRAY,
  JSON_NODE_VALUE,
  JSON_NODE_NULL
} JsonNodeType;

struct _JsonNode
{
  /*< private >*/
  JsonNodeType type;

  union {
    JsonObject *object;
    JsonArray *array;
    GValue value;
  } data;

  JsonNode *parent;
};

JsonObject *          json_node_get_object    (JsonNode    *node);
JsonArray *           json_node_get_array     (JsonNode    *node);
void                  json_node_get_value     (JsonNode    *node,
                                               GValue      *value);
JsonNode *            json_node_get_parent    (JsonNode    *node);
G_CONST_RETURN gchar *json_node_type_name     (JsonNode    *node);

JsonObject *          json_object_ref         (JsonObject  *object);
void                  json_object_unref       (JsonObject  *object);
GList *               json_object_get_members (JsonObject  *object);
JsonNode *            json_object_get_member  (JsonObject  *object,
                                               const gchar *member_name);
gboolean              json_object_has_member  (JsonObject  *object,
                                               const gchar *member_name);
guint                 json_object_get_size    (JsonObject  *object);

JsonArray *           json_array_ref          (JsonArray   *array);
void                  json_array_unref        (JsonArray   *array);
GList *               json_array_get_elements (JsonArray   *array);
JsonNode *            json_array_get_element  (JsonArray   *array,
                                               guint        index_);
guint                 json_array_get_length   (JsonArray   *array);

G_END_DECLS

#endif /* __JSON_TYPES_H__ */
