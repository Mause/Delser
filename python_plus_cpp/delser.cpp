#include "../cpp/Delser.hpp"
#include <Python.h>
#include "structmember.h"
#include "utils.cpp"

namespace Delser_py {
    namespace Delser_Obj {
        typedef struct {
            PyObject_HEAD
            Delser *delser_inst;
        } Delser_Object;

        PyObject * KeyGenException         = PyErr_NewException("delser.KeyGenException",       NULL, NULL);
        PyObject * KeyInvalidException     = PyErr_NewException("delser.KeyInvalidException",   KeyGenException, NULL);
        PyObject * KeyBlacklistedException = PyErr_NewException("delser.KeyBlacklistException", KeyGenException, NULL);
        PyObject * KeyPhonyException       = PyErr_NewException("delser.KeyPhonyException",     KeyGenException, NULL);
        PyObject * KeyBadChecksumException = PyErr_NewException("delser.KeyBadChecksumException",       KeyGenException, NULL);

        PyObject * DelserInternalException = PyErr_NewException("delser.DelserInternalException", NULL, NULL);
        PyObject * BadByteToCheckError     = PyErr_NewException("delser.BadByteToCheckError",   DelserInternalException, NULL);

        static PyObject *make_key(PyObject *self, PyObject *args) {
            // grab the integer argument
            int seed;
            if(!PyArg_ParseTuple(args, "i", &seed))
                return NULL;

            // grab a pointer to the Delser instance, and make a key
            Delser * delser_inst = ((Delser_Object *)self)->delser_inst;
            std::string key = delser_inst->make_key(seed);

            // create a corresponding PyObject
            PyObject *pyKey = PyUnicode_FromString(key.c_str());
            Py_XINCREF(pyKey);

            // and return the new unicode object
            return pyKey;
        }

        static PyObject *check_key(PyObject *self, PyObject *args) {
            char *skey;
            if(!PyArg_ParseTuple(args, "s", &skey))
                return NULL;
            std::string key = skey;

            Delser * delser_inst = ((Delser_Object *)self)->delser_inst;

            try {
                PyObject *good_bool = PyBool_FromLong(delser_inst->check_key(key) ? 1 : 0);
                Py_XINCREF(good_bool);

                return good_bool;

            } catch (exceptions::key_invalid& e) {
                PyErr_SetString(KeyInvalidException, CSTR("Wrong number of sections in key: " << e.what()));
                return NULL;
            } catch (exceptions::key_blacklisted& e) {
                PyErr_SetString(KeyBlacklistedException, CSTR("Blacklisted key: \"" << e.what() << "\""));
                return NULL;
            } catch (exceptions::key_phony& e) {
                PyErr_SetString(KeyPhonyException, CSTR("Phony key: \"" << e.what() << "\""));
                return NULL;
            } catch (exceptions::key_bad_checksum& e) {
                PyErr_SetString(KeyBadChecksumException, CSTR("Bad checksum: " << e.what()));
                return NULL;
            }
        }

        static PyMethodDef methods[] = {
            {"make_key", make_key, METH_VARARGS, ""},
            {"check_key", check_key, METH_VARARGS, ""},
            {NULL, NULL, 0, NULL}   /* Sentinal */
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
                // new Delser instance!
                Delser *delser_inst = new Delser(byte_to_check, sequences);

                // assign it to the appropriate place on our PyObject-like struct
                ((Delser_Object *)self)->delser_inst = delser_inst;
                return 0;

            } catch (exceptions::bad_byte_to_check_error& e) {
                // inform the user if they failed to pass a valid byte_to_check
                PyErr_SetString(PyExc_ValueError, CSTR("Invalid byte_to_check specified: " << e.what()));
                return -1;
            }
        }

        static void dealloc(PyObject *self){
            // deallocate the Delser instance
            delete ((Delser_Object *)self)->delser_inst;

            // deallocate ourselves
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
            0,                         /* tp_members */
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

    // define the module struct :D
    static struct PyModuleDef Module = {
        PyModuleDef_HEAD_INIT,
        "delser",                            /* name of module */
        "Serial key generator module",       /* module doc */
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    // initialise the module
    PyMODINIT_FUNC PyInit_delser(void) {
        // ensure the type is good to go :D
        if (PyType_Ready(&Delser_Obj::type) < 0) return NULL;

        // create the new module object
        PyObject *m = PyModule_Create(&Module);
        if (m == NULL) return NULL;

        // increase the reference number of the type, register the delser.Delser object/type
        Py_INCREF(&Delser_Obj::type);
        PyModule_AddObject(m, "Delser", (PyObject *)&Delser_Obj::type);
        return m;
    }
}


