#ifndef __JSON_PRIVATE_H__
#define __JSON_PRIVATE_H__

#include <glib-object.h>
#include "json-types.h"

G_BEGIN_DECLS

JsonNode *            json_node_new          (JsonNodeType  type);
JsonNode *            json_node_copy         (JsonNode     *node);
void                  json_node_set_object   (JsonNode     *node,
                                              JsonObject   *object);
void                  json_node_set_array    (JsonNode     *node,
                                              JsonArray    *array);
void                  json_node_set_value    (JsonNode     *node,
                                              const GValue *value);
void                  json_node_free         (JsonNode     *node);

JsonObject *          json_object_new        (void);
void                  json_object_add_member (JsonObject   *object,
                                              const gchar  *member_name,
                                              JsonNode     *node);

JsonArray *           json_array_new         (void);
JsonArray *           json_array_sized_new   (guint         n_elements);
void                  json_array_add_element (JsonArray    *array,
                                              JsonNode     *node);

G_END_DECLS

#endif /* __JSON_PRIVATE_H__ */
