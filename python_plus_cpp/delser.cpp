#include "../cpp/Delser.hpp"
#include <Python.h>

// check_key make_key etc


namespace Delser_py {
    static PyObject * make_key(PyObject *self, PyObject *args) {
        int seed;

        if(!PyArg_ParseTuple(args, "i", &seed)){
            return NULL;
        }

        std::cout << "Hello: " << seed << std::endl;
        return PyUnicode_FromString("Heloo");
    }


    typedef struct {
        PyObject_HEAD
        Delser delser_inst;
        /* type specific fields go here */
    } Delser_Object;
 
    static PyMethodDef Delser_methods[] = {
        {"make_key", (PyCFunction)make_key, METH_VARARGS, ""},

        {NULL, NULL, 0, NULL}   /* Sentinal */
    };

    static int Delser_init(Delser_Object *self) {
        std::cout << "Hello" << std::endl;
        self->delser_inst = Delser();

        return 0;
    }

    static PyTypeObject delser_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "delser.Delser",            /* tp_name */
        sizeof(Delser_Object),      /* tp_basicsize */
        0,                         /* tp_itemsize */
        0,                         /* tp_dealloc */
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
        "Noddy objects",           /* tp_doc */
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        Delser_methods,            /* tp_methods */
        0,                         /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)Delser_init,      /* tp_init */
        0,                         /* tp_alloc */
        0,                         /* tp_new */
    };    


    static struct PyModuleDef Module = {
        PyModuleDef_HEAD_INIT,
        "delser",   /* name of module */
        "",       /* module doc */
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    PyMODINIT_FUNC PyInit_delser(void){
        PyObject *m;
        delser_Type.tp_new = PyType_GenericNew;
        if (PyType_Ready(&delser_Type) < 0)
            return NULL;
        
        m = PyModule_Create(&Module);
        if (m == NULL)
            return NULL;

        Py_INCREF(&delser_Type);
        PyModule_AddObject(m, "Delser", (PyObject *)&delser_Type);
        return m;
    }
}


