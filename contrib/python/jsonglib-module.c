/* -*- Mode: C; c-basic-offset: 4 -*- */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>

/* include any extra headers needed here */
#include <json-glib/json-glib.h>

void pyjsonglib_register_classes(PyObject *d);
void pyjsonglib_add_constants(PyObject *module, const gchar *strip_prefix);
extern PyMethodDef pyjsonglib_functions[];

DL_EXPORT(void)
initjsonglib(void)
{
    PyObject *m, *d;

    /* perform any initialisation required by the library here */
	init_pygobject();
	
    m = Py_InitModule("json_glib", pyjsonglib_functions);
    d = PyModule_GetDict(m);
    
    /* add anything else to the module dictionary (such as constants) */
    pyjsonglib_register_classes(d);
    pyjsonglib_add_constants(m, "JSON_");
    
    if (PyErr_Occurred())
        Py_FatalError("could not initialise module json_glib");
}
