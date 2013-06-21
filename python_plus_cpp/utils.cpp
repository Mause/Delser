const char * objects_dir(PyObject *obj){
    PyObject* r = PyObject_Dir(obj);
    if (r == NULL) return NULL;

    PyObject* o = PyObject_Repr(r);
    if (o == NULL) return NULL;

    PyObject *pyStr = PyUnicode_AsEncodedString(o, "utf-8", "");
    if (pyStr == NULL) return NULL;

    const char *str = PyBytes_AS_STRING(pyStr);
    if (str == NULL) return NULL;
    return str;
}

const char * objects_type(PyObject *obj){
    PyObject* r = PyObject_Type(obj);
    if (r == NULL) return NULL;

    PyObject* o = PyObject_Repr(r);
    if (o == NULL) return NULL;

    PyObject *pyStr = PyUnicode_AsEncodedString(o, "utf-8", "");
    if (pyStr == NULL) return NULL;

    const char *str = PyBytes_AS_STRING(pyStr);
    if (str == NULL) return NULL;
    return str;
}

