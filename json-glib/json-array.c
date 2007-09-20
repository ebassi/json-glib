#include "config.h"

#include "json-types.h"
#include "json-private.h"

struct _JsonArray
{
  GValueArray *elements;

  volatile guint ref_count;
};

JsonArray *
json_array_new (void)
{
  JsonArray *array;

  array = g_slice_new (JsonArray);

  array->ref_count = 1;
  array->elements = g_value_array_new (0);

  return array;
}

JsonArray *
json_array_sized_new (guint n_elements)
{
  JsonArray *array;

  array = g_slice_new (JsonArray);
  
  array->ref_count = 1;
  array->elements = g_value_array_new (n_elements);

  return array;
}

/**
 * json_array_ref:
 * @array: a #JsonArray
 *
 * Increase by one the reference count of a #JsonArray.
 *
 * Return value: the passed #JsonArray, with the reference count
 *   increased by one.
 */
JsonArray *
json_array_ref (JsonArray *array)
{
  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (array->ref_count > 0, NULL);

  g_atomic_int_exchange_and_add (&array->ref_count, 1);

  return array;
}

/**
 * json_array_unref:
 * @array: a #JsonArray
 *
 * Decreases by one the reference count of a #JsonArray. If the
 * reference count reaches zero, the array is destroyed and all
 * its allocated resources are freed.
 */
void
json_array_unref (JsonArray *array)
{
  gint old_ref;

  g_return_if_fail (array != NULL);
  g_return_if_fail (array->ref_count > 0);

  old_ref = g_atomic_int_get (&array->ref_count);
  if (old_ref > 1)
    g_atomic_int_compare_and_exchange (&array->ref_count, old_ref, old_ref - 1);
  else
    {
      g_value_array_free (array->elements);
      array->elements = NULL;

      g_slice_free (JsonArray, array);
    }
}

/**
 * json_array_get_elements:
 * @array: a #JsonArray
 *
 * Gets the elements of a #JsonArray in list form.
 *
 * Return value: a #GList containing the elements of the array. The
 *   contents of the list are owned by the array and should never be
 *   modified or freed. Use g_list_free() on the returned list when
 *   done using it
 */
GList *
json_array_get_elements (JsonArray *array)
{
  GList *retval;
  guint i;

  g_return_val_if_fail (array != NULL, NULL);

  retval = NULL;
  for (i = 0; i < array->elements->n_values; i++)
    retval = g_list_prepend (retval,
                             g_value_array_get_nth (array->elements, i));

  return g_list_reverse (retval);
}

/**
 * json_array_get_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 * 
 * Retrieves the element at @index_ inside a #JsonArray.
 *
 * Return value: a pointer to the value at the requested position
 */
GValue *
json_array_get_element (JsonArray *array,
                        guint      index_)
{
  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->n_values, NULL);

  return g_value_array_get_nth (array->elements, index_);
}

/**
 * json_array_get_length:
 * @array: a #JsonArray
 *
 * Retrieves the length of a #JsonArray
 *
 * Return value: the length of the array
 */
guint
json_array_get_length (JsonArray *array)
{
  g_return_val_if_fail (array != NULL, 0);

  return array->elements->n_values;
}

void
json_array_append_element (JsonArray    *array,
                           const GValue *value)
{
  g_return_if_fail (array != NULL);

  g_value_array_append (array->elements, value);
}

void
json_array_prepend_element (JsonArray    *array,
                            const GValue *value)
{
  g_return_if_fail (array != NULL);

  g_value_array_prepend (array->elements, value);
}

void
json_array_insert_element (JsonArray    *array,
                           gint          index_,
                           const GValue *value)
{
  g_return_if_fail (array != NULL);
  g_return_if_fail (index_ <= array->elements->n_values);

  g_value_array_insert (array->elements, index_, value);
}
