#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include <json-glib/json-types.h>

G_BEGIN_DECLS

#define JSON_TYPE_PARSER                (json_parser_get_type ())
#define JSON_PARSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), JSON_TYPE_PARSER, JsonParser))
#define JSON_IS_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), JSON_TYPE_PARSER))
#define JSON_PARSER_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), JSON_TYPE_PARSER, JsonParserClass))
#define JSON_IS_PARSER_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), JSON_TYPE_PARSER))
#define JSON_PARSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), JSON_TYPE_PARSER, JsonParserClass))

#define JSON_PARSER_ERROR               (json_parser_error_quark ())

typedef struct _JsonParser              JsonParser;
typedef struct _JsonParserPrivate       JsonParserPrivate;
typedef struct _JsonParserClass         JsonParserClass;

typedef enum {
  JSON_PARSER_ERROR_PARSE,
  
  JSON_PARSER_ERROR_UNKNOWN
} JsonParserError;

typedef enum {
  JSON_TOKEN_INVALID = G_TOKEN_LAST,
  JSON_TOKEN_TRUE,
  JSON_TOKEN_FALSE,
  JSON_TOKEN_NULL,
  JSON_TOKEN_LAST
} JsonTokenType;

struct _JsonParser
{
  /*< private >*/
  GObject parent_instance;

  JsonParserPrivate *priv;
};

struct _JsonParserClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public  >*/
  void (* error) (JsonParser   *parser,
                  const GError *error);

  /*< private >*/
  /* padding for future expansion */
  void (* _json_reserved1) (void);
  void (* _json_reserved2) (void);
  void (* _json_reserved3) (void);
  void (* _json_reserved4) (void);
  void (* _json_reserved5) (void);
  void (* _json_reserved6) (void);
  void (* _json_reserved7) (void);
  void (* _json_reserved8) (void);
};

GQuark      json_parser_error_quark    (void);
GType       json_parser_get_type       (void) G_GNUC_CONST;

JsonParser *json_parser_new            (void);
gboolean    json_parser_load_from_file (JsonParser   *parser,
                                        const gchar  *filename,
                                        GError      **error);
gboolean    json_parser_load_from_data (JsonParser   *parser,
                                        const gchar  *data,
                                        gsize         length,
                                        GError      **error);
JsonNode *  json_parser_get_root       (JsonParser   *parser);

G_END_DECLS

#endif /* __JSON_PARSER_H__ */
