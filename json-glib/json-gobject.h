/* json-gobject.h - JSON GObject integration
 * 
 * This file is part of JSON-GLib
 * Copyright (C) 2007  OpenedHand Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Author:
 *   Emmanuele Bassi  <ebassi@openedhand.com>
 */

#ifndef __JSON_GOBJECT_H__
#define __JSON_GOBJECT_H__

#include <json-glib/json-types.h>
#include <glib-object.h>

G_BEGIN_DECLS

gchar *json_serialize_gobject (GObject *gobject,
                               gsize   *length);

G_END_DECLS

#endif /* __JSON_GOBJECT_H__ */
