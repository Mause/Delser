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
            PyObject *delser_inst;
            /* type specific fields go here */
        } Delser_Object;

        static PyObject *make_key(PyObject *self, PyObject *args) {
            int seed;

            if(!PyArg_ParseTuple(args, "i", &seed)) {
                return NULL;
            }
            
            /*std::cout << "self: " << objects_type(self) << std::endl;
            std::cout << "args: " << objects_type(args) << std::endl;
            std::cout << "seed: " << seed << std::endl;*/

            PyObject *delser_inst = PyObject_GetAttrString(self, "delser_inst");
            if (delser_inst == NULL) {
                return NULL;
            }
            if (!PyCapsule_CheckExact(delser_inst)) {
                PyErr_BadInternalCall();
                return NULL;
            }
            if (!PyCapsule_IsValid(delser_inst, "delser_inst")) {
                PyErr_BadInternalCall();
            }

            PyCapsule_GetPointer(delser_inst, "delser_inst");
            if (delser_inst == NULL) return NULL;

            std::cout << "Calling now" << std::endl;
            std::string key = ((Delser *)delser_inst)->make_key(seed);
            std::cout << "Called" << std::endl;

            PyObject *pyKey = PyUnicode_FromString(key.c_str());
            Py_XINCREF(pyKey);

            return pyKey;
        }

        static PyObject *check_key(PyObject *self, PyObject *args) {
           /* std::string key;
            if(!PyArg_ParseTuple(args, "s", &key))
                return NULL;*/

            return Py_NotImplemented;
        }

        static PyMethodDef methods[] = {
            {"make_key", make_key, METH_VARARGS, ""},
            {"check_key", check_key, METH_VARARGS, ""},
            {NULL, NULL, 0, NULL}   /* Sentinal */
        };

        static PyMemberDef members[] = {
            {
                "delser_inst",
                T_OBJECT,
                offsetof(Delser_Object, delser_inst),
                0,
                "Pointer to underlyling Delser instance"
            },
            {NULL} /* sentinal */
        };

        static PyObject *Delser_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
        {
            Delser_Object *self;

            self = (Delser_Object *)type->tp_alloc(type, 0);

            return (PyObject *)self;
        } 
/*
        PyCapsule_Destructor Delser_dealloc(PyObject *capsule) {
            PyObject *delser_inst = PyObject_GetAttrString(capsule, "delser_inst");
            if (delser_inst != NULL) {
                PyCapsule_GetPointer(delser_inst, "delser_inst");
                // TODO: fix this!
                delete (Delser *)delser_inst; 
            }
            return;
        }
*/
        static int init(PyObject *self) {
            // TODO: fill in destructor
            PyObject *delser_inst = PyCapsule_New(new Delser(), "delser_inst", NULL);
            if (PyObject_SetAttrString(self, "delser_inst", delser_inst) == -1) return -1;
            assert ( PyCapsule_IsValid(delser_inst, "delser_inst") );

            return 0;
        }

        static void dealloc(PyObject *self){
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
            Delser_Obj::Delser_new,                         /* tp_new */
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


