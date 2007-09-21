#include "config.h"

#include <glib.h>

#include "json-types.h"
#include "json-private.h"

JsonData *
json_data_new (JsonDataType type)
{
  JsonData *data;

  data = g_slice_new (JsonData);
  data->type = type;

  return data;
}

void
json_data_set_object (JsonData   *data,
                      JsonObject *object)
{
  g_return_if_fail (data != NULL);
  g_return_if_fail (JSON_DATA_TYPE (data) == JSON_DATA_OBJECT);
  g_return_if_fail (object != NULL);

  data->data.object = object;
}

/**
 * json_data_get_object:
 * @data: a #JsonData
 *
 * Retrieves the #JsonObject stored inside a #JsonData
 *
 * Return value: the #JsonObject
 */
JsonObject *
json_data_get_object (JsonData *data)
{
  g_return_val_if_fail (data != NULL, NULL);
  g_return_val_if_fail (JSON_DATA_TYPE (data) == JSON_DATA_OBJECT, NULL);

  return data->data.object;
}

void
json_data_set_array (JsonData  *data,
                     JsonArray *array)
{
  g_return_if_fail (data != NULL);
  g_return_if_fail (JSON_DATA_TYPE (data) == JSON_DATA_ARRAY);
  g_return_if_fail (array != NULL);

  data->data.array = array;
}

/**
 * json_data_get_array:
 * @data: a #JsonData
 *
 * Retrieves the #JsonArray stored inside a #JsonData
 *
 * Return value: the #JsonArray
 */
JsonArray *
json_data_get_array (JsonData *data)
{
  g_return_val_if_fail (data != NULL, NULL);
  g_return_val_if_fail (JSON_DATA_TYPE (data) == JSON_DATA_ARRAY, NULL);

  return data->data.array;
}

void
json_data_free (JsonData *data)
{
  if (data)
    {
      switch (data->type)
        {
        case JSON_DATA_OBJECT:
          json_object_unref (data->data.object);
          break;

        case JSON_DATA_ARRAY:
          json_array_unref (data->data.array);
          break;
        }

      g_slice_free (JsonData, data);
    }
}
