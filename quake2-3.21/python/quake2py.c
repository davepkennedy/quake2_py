//
//  quake2py.c
//  Quake II
//
//  Created by Dave on 18/08/2013.
//
//

#include <Python/Python.h>
#include <stdio.h>

const char* INIT_SCRIPT =   "import quake2\n" \
                            "import sys\n" \
                            "sys.stdout.close()\n" \
                            /*"sys.stderr.close()\n" \*/
                            "sys.stdout = quake2.ConsoleIO()\n" \
                            "print '== Python =='\n" \
                            "print sys.version\n" \
                            "print sys.copyright";

//extern PyTypeObject PyTextIOBase_Type;
extern PyTypeObject PyConsoleIO_Type;

PyDoc_STRVAR(module_doc, "Python hooks for Quake2");

static PyMethodDef module_methods[] = {
    {NULL, NULL}
};

PyMODINIT_FUNC
Python_Init(void)
{
    Py_Initialize();
    PyObject *m = Py_InitModule4("quake2", module_methods,
                                 module_doc, NULL, PYTHON_API_VERSION);
    if (m == NULL)
        return;
    
#define ADD_TYPE(type, name) \
    if (PyType_Ready(type) < 0) {\
        Py_DECREF(m); \
        return; \
    } \
    Py_INCREF(type); \
    if (PyModule_AddObject(m, name, (PyObject *)type) < 0) {  \
        Py_DECREF(type); \
        Py_DECREF(m); \
        return; \
    }
//    PyConsoleIO_Type.tp_base = &PyTextIOBase_Type;
    ADD_TYPE(&PyConsoleIO_Type, "ConsoleIO");
    
    PyRun_SimpleString(INIT_SCRIPT);
    
}