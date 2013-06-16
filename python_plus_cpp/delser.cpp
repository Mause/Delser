#include "../cpp/Delser.hpp"
#include <Python.h>

namespace Delser_py {
    namespace Delser_Obj {
        static PyObject * make_key(PyObject *self, PyObject *args) {
            int seed;

            if(!PyArg_ParseTuple(args, "i", &seed)){
                return NULL;
            }

            std::cout << "Hello: seed: " << seed << std::endl;
            return PyUnicode_FromString("Heloo");
        }


        typedef struct {
            PyObject_HEAD
            Delser delser_inst;
            /* type specific fields go here */
        } Delser_Object;
     
        static PyMethodDef methods[] = {
            {"make_key", make_key, METH_VARARGS, ""},

            {NULL, NULL, 0, NULL}   /* Sentinal */
        };

        static int init(Delser_Object *self) {
            std::cout << "Creating new Delser object" << std::endl;

//            try {
                self->delser_inst = *new Delser();
  /*          } catch (const std::bad_alloc&) {
                return -1;
            }*/

            std::cout << "Object created" << std::endl;

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
            0,                         /* tp_members */
            0,                         /* tp_getset */
            0,                         /* tp_base */
            0,                         /* tp_dict */
            0,                         /* tp_descr_get */
            0,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            (initproc)Delser_Obj::init,     /* tp_init */
            0,                         /* tp_alloc */
            0,                         /* tp_new */
        };    
    }

    static struct PyModuleDef Module = {
        PyModuleDef_HEAD_INIT,
        "delser",   /* name of module */
        "",       /* module doc */
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    PyMODINIT_FUNC PyInit_delser(void){
        PyObject *m;
        Delser_Obj::type.tp_new = PyType_GenericNew;
        if (PyType_Ready(&Delser_Obj::type) < 0)
            return NULL;
        
        m = PyModule_Create(&Module);
        if (m == NULL)
            return NULL;

        Py_INCREF(&Delser_Obj::type);
        PyModule_AddObject(m, "Delser", (PyObject *)&Delser_Obj::type);
        return m;
    }
}


