#ifndef __JSON_PRIVATE_H__
#define __JSON_PRIVATE_H__

#include <glib-object.h>
#include <json-glib/json-types.h>

G_BEGIN_DECLS

JsonObject *json_object_new            (void);
void        json_object_add_member     (JsonObject   *object,
                                        const gchar  *member_name,
                                        const GValue *value);

JsonArray * json_array_new             (void);
JsonArray * json_array_sized_new       (guint         n_elements);
void        json_array_append_element  (JsonArray    *array,
                                        const GValue *value);
void        json_array_prepend_element (JsonArray    *array,
                                        const GValue *value);
void        json_array_insert_element  (JsonArray    *array,
                                        gint          index_,
                                        const GValue *value);

G_END_DECLS

#endif /* __JSON_PRIVATE_H__ */
