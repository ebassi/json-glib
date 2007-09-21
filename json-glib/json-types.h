#ifndef __JSON_TYPES_H__
#define __JSON_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define JSON_DATA_TYPE(data)    (((JsonData *) (data))->type)
#define JSON_TYPE_OBJECT        (json_object_get_type ())
#define JSON_TYPE_ARRAY         (json_array_get_type ())

typedef struct _JsonObject      JsonObject;
typedef struct _JsonArray       JsonArray;
typedef struct _JsonData        JsonData;

typedef enum {
  JSON_DATA_OBJECT,
  JSON_DATA_ARRAY
} JsonDataType;

struct _JsonData
{
  /*< private >*/
  JsonDataType type;

  union {
    JsonObject *object;
    JsonArray *array;
  } data;
};

JsonObject * json_data_get_object    (JsonData    *data);
JsonArray *  json_data_get_array     (JsonData    *data);

JsonObject * json_object_ref         (JsonObject  *object);
void         json_object_unref       (JsonObject  *object);
GList *      json_object_get_members (JsonObject  *object);
GValue *     json_object_get_member  (JsonObject  *object,
                                      const gchar *member_name);
gboolean     json_object_has_member  (JsonObject  *object,
                                      const gchar *member_name);
guint        json_object_get_size    (JsonObject  *object);

JsonArray *  json_array_ref          (JsonArray   *array);
void         json_array_unref        (JsonArray   *array);
GList *      json_array_get_elements (JsonArray   *array);
GValue *     json_array_get_element  (JsonArray   *array,
                                      guint        index_);
guint        json_array_get_length   (JsonArray   *array);

G_END_DECLS

#endif /* __JSON_TYPES_H__ */
