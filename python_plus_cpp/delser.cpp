#include "../cpp/Delser.hpp"
#include <Python.h>
#include "structmember.h"
#include "assert.h"

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

namespace Delser_py {
    namespace Delser_Obj {
        typedef struct {
            PyObject_HEAD
            Delser *delser_inst;
            /* type specific fields go here */
        } Delser_Object;

        static PyObject *make_key(PyObject *self, PyObject *args) {
            int seed;

            if(!PyArg_ParseTuple(args, "i", &seed)) {
                return NULL;
            }
            
            Delser * delser_inst = ((Delser_Object *)self)->delser_inst;
            std::string key = delser_inst->make_key(seed);

            PyObject *pyKey = PyUnicode_FromString(key.c_str());
            Py_XINCREF(pyKey);

            return pyKey;
        }

        static PyObject *check_key(PyObject *self, PyObject *args) {
            char *skey;
            if(!PyArg_ParseTuple(args, "s", &skey))
                return NULL;
            std::string key = skey;

            Delser * delser_inst = ((Delser_Object *)self)->delser_inst;
            long good_long = delser_inst->check_key(key) ? 1 : 0;
    
            PyObject *good_bool = PyBool_FromLong(good_long);
            Py_XINCREF(good_bool);

            return good_bool;
        }

        static PyMethodDef methods[] = {
            {"make_key", make_key, METH_VARARGS, ""},
            {"check_key", check_key, METH_VARARGS, ""},
            {NULL, NULL, 0, NULL}   /* Sentinal */
        };

        static PyMemberDef members[] = {
            {NULL} /* sentinal */
        };

        static int init(PyObject *self, PyObject *args) {
            // setup needed variables
            int byte_to_check;
            PyObject *py_sequences;

            // read in the arguments
            if(!PyArg_ParseTuple(args, "i|O", &byte_to_check, &py_sequences))
                return -1;

            // convert the PyList of python tuples into a vector of cpp tuples
            std::vector<Delser::sequence> sequences;
            Py_ssize_t list_size = PyList_Size(py_sequences);
            for (Py_ssize_t i=0; i<list_size; i++) {
                int p1, p2, p3;

                // grab the tuple
                PyObject *tuple = PyList_GetItem(args, i);
                if (tuple == NULL) {
                    std::cout << "Bad tuple!" << std::endl;
                    return -1;
                }

                // parse the tuple
                if (!PyArg_ParseTuple(tuple, "iii;Bad sequence", &p1, &p2, &p3))
                    return -1;

                // and add the tuple to the vector 
                sequences.push_back(std::make_tuple(p1, p2, p3));
                std::cout << "Tuple: " << p1 << ", " << p2 << ", " << p3 << std::endl;
            }

            try {
                Delser *delser_inst = new Delser(byte_to_check, sequences);

                ((Delser_Object *)self)->delser_inst = delser_inst;
                return 0;

            } catch (bad_byte_to_check_error& e) {
                PyErr_SetString(PyExc_ValueError, "Invalid byte_to_check specified");
                return -1;
            }
        }

        static void dealloc(PyObject *self){
            delete ((Delser_Object *)self)->delser_inst;

            Py_TYPE(self)->tp_free((PyObject*)self);
        }

        static PyTypeObject type = {
            PyVarObject_HEAD_INIT(NULL, 0)
            "delser.Delser",           /* tp_name */
            sizeof(Delser_Object),     /* tp_basicsize */
            0,                         /* tp_itemsize */
            Delser_Obj::dealloc,       /* tp_dealloc */
            0,                         /* tp_print */
            0,                         /* tp_getattr */
            0,                         /* tp_setattr */
            0,                         /* tp_reserved */
            0,                         /* tp_repr */
            0,                         /* tp_as_number */
            0,                         /* tp_as_sequence */
            0,                         /* tp_as_mapping */
            0,                         /* tp_hash  */
            0,                         /* tp_call */
            0,                         /* tp_str */
            0,                         /* tp_getattro */
            0,                         /* tp_setattro */
            0,                         /* tp_as_buffer */
            Py_TPFLAGS_DEFAULT |
                Py_TPFLAGS_BASETYPE,   /* tp_flags */
            "Serial key generator",    /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            Delser_Obj::methods,       /* tp_methods */
            Delser_Obj::members,       /* tp_members */
            0,                         /* tp_getset */
            0,                         /* tp_base */
            0,                         /* tp_dict */
            0,                         /* tp_descr_get */
            0,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc)Delser_Obj::init,     /* tp_init */
            0,                         /* tp_alloc */
            PyType_GenericNew          /* tp_new */
        };    
    }

    static struct PyModuleDef Module = {
        PyModuleDef_HEAD_INIT,
        "delser",   /* name of module */
        "Serial key generator module",       /* module doc */
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    PyMODINIT_FUNC PyInit_delser(void) {
        if (PyType_Ready(&Delser_Obj::type) < 0) return NULL;
        
        PyObject *m = PyModule_Create(&Module);
        if (!m) return NULL;

        Py_INCREF(&Delser_Obj::type);
        PyModule_AddObject(m, "Delser", (PyObject *)&Delser_Obj::type);
        return m;
    }
}


