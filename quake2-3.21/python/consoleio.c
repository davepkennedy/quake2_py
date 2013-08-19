//
//  consoleio.c
//  Quake II
//
//  Created by Dave on 17/08/2013.
//
//

#include <Python/Python.h>
#include <stddef.h>
#include "qcommon.h"

typedef struct {
    PyObject_HEAD
    PyObject *weakreflist;
    PyObject *dict;
} consoleio;

static void
console_dealloc(consoleio *self)
{
    _PyObject_GC_UNTRACK(self);
    if (self->weakreflist != NULL)
        PyObject_ClearWeakRefs((PyObject *) self);
    Py_CLEAR(self->dict);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
consoleio_read(consoleio *self, PyObject *args)
{
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}

static PyObject *
consoleio_readall(consoleio *self, PyObject *args)
{
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}

static PyObject *
consoleio_readinto(consoleio *self, PyObject *args)
{
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}

static PyObject *
consoleio_write(consoleio *self, PyObject *args)
{
    Py_buffer pbuf;
    Py_ssize_t len;
    char* data;
    
    if (!PyArg_ParseTuple(args, "s*", &pbuf))
        return NULL;
    
    len = pbuf.len;
    data = calloc(sizeof(char), len+1);
    memcpy(data, pbuf.buf, len);
    Con_Print(data);
    free(data);
    
    PyBuffer_Release(&pbuf);
    
    return PyLong_FromSsize_t(len);
}

static PyObject *
consoleio_close(consoleio *self)
{
    /*
    if (!self->closefd) {
        self->fd = -1;
        Py_RETURN_NONE;
    }
    errno = internal_close(self);
    if (errno < 0)
        return NULL;
    
    return PyObject_CallMethod((PyObject*)&PyRawIOBase_Type,
                               "close", "O", self);
     */
    Py_RETURN_NONE;
}

static PyObject *
consoleio_seek(consoleio *self, PyObject *args)
{
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}

static PyObject *
consoleio_tell(consoleio *self, PyObject *args)
{
    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
}

static PyObject *
consoleio_seekable(consoleio *self)
{
    Py_RETURN_FALSE;
}

static PyObject *
consoleio_readable(consoleio *self)
{
    Py_RETURN_FALSE;
}

static PyObject *
consoleio_writable(consoleio *self)
{
    Py_RETURN_TRUE;
}

static PyObject *
consoleio_fileno(consoleio *self)
{
    return PyInt_FromLong(0);
}


static PyObject *
consoleio_repr(consoleio *self)
{
    /*
    PyObject *nameobj, *res;
    
    if (self->fd < 0)
        return PyString_FromFormat("<_io.FileIO [closed]>");
    
    nameobj = PyObject_GetAttrString((PyObject *) self, "name");
    if (nameobj == NULL) {
        if (PyErr_ExceptionMatches(PyExc_AttributeError))
            PyErr_Clear();
        else
            return NULL;
        res = PyString_FromFormat("<_io.FileIO fd=%d mode='%s'>",
                                  self->fd, mode_string(self));
    }
    else {
        PyObject *repr = PyObject_Repr(nameobj);
        Py_DECREF(nameobj);
        if (repr == NULL)
            return NULL;
        res = PyString_FromFormat("<_io.FileIO name=%s mode='%s'>",
                                  PyString_AS_STRING(repr),
                                  mode_string(self));
        Py_DECREF(repr);
    }
    return res;
     */
    return PyString_FromString("quake2.console");
}

static PyObject *
consoleio_isatty(consoleio *self)
{
    Py_RETURN_FALSE;
}

static int
consoleio_traverse(consoleio *self, visitproc visit, void *arg)
{
    Py_VISIT(self->dict);
    return 0;
}

static int
consoleio_clear(consoleio *self)
{
    Py_CLEAR(self->dict);
    return 0;
}

static PyObject *
consoleio_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    consoleio *self;
    
    assert(type != NULL && type->tp_alloc != NULL);
    
    self = (consoleio *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->weakreflist = NULL;
    }
    
    return (PyObject *) self;
}


PyDoc_STRVAR(consoleio_doc,
             "file(name: str[, mode: str]) -> file IO object\n"
             "\n"
             "Open a file.  The mode can be 'r', 'w' or 'a' for reading (default),\n"
             "writing or appending.  The file will be created if it doesn't exist\n"
             "when opened for writing or appending; it will be truncated when\n"
             "opened for writing.  Add a '+' to the mode to allow simultaneous\n"
             "reading and writing.");

PyDoc_STRVAR(read_doc,
             "read(size: int) -> bytes.  read at most size bytes, returned as bytes.\n"
             "\n"
             "Only makes one system call, so less data may be returned than requested\n"
             "In non-blocking mode, returns None if no data is available.\n"
             "On end-of-file, returns ''.");

PyDoc_STRVAR(readall_doc,
             "readall() -> bytes.  read all data from the file, returned as bytes.\n"
             "\n"
             "In non-blocking mode, returns as much as is immediately available,\n"
             "or None if no data is available.  On end-of-file, returns ''.");

PyDoc_STRVAR(write_doc,
             "write(b: bytes) -> int.  Write bytes b to file, return number written.\n"
             "\n"
             "Only makes one system call, so not all of the data may be written.\n"
             "The number of bytes actually written is returned.");

PyDoc_STRVAR(fileno_doc,
             "fileno() -> int. \"file descriptor\".\n"
             "\n"
             "This is needed for lower-level file interfaces, such the fcntl module.");

PyDoc_STRVAR(seek_doc,
             "seek(offset: int[, whence: int]) -> None.  Move to new file position.\n"
             "\n"
             "Argument offset is a byte count.  Optional argument whence defaults to\n"
             "0 (offset from start of file, offset should be >= 0); other values are 1\n"
             "(move relative to current position, positive or negative), and 2 (move\n"
             "relative to end of file, usually negative, although many platforms allow\n"
             "seeking beyond the end of a file)."
             "\n"
             "Note that not all file objects are seekable.");

PyDoc_STRVAR(tell_doc,
             "tell() -> int.  Current file position");

PyDoc_STRVAR(readinto_doc,
             "readinto() -> Same as RawIOBase.readinto().");

PyDoc_STRVAR(close_doc,
             "close() -> None.  Close the file.\n"
             "\n"
             "A closed file cannot be used for further I/O operations.  close() may be\n"
             "called more than once without error.  Changes the fileno to -1.");

PyDoc_STRVAR(isatty_doc,
             "isatty() -> bool.  True if the file is connected to a tty device.");

PyDoc_STRVAR(seekable_doc,
             "seekable() -> bool.  True if file supports random-access.");

PyDoc_STRVAR(readable_doc,
             "readable() -> bool.  True if file was opened in a read mode.");

PyDoc_STRVAR(writable_doc,
             "writable() -> bool.  True if file was opened in a write mode.");

static PyMethodDef consoleio_methods[] = {
    {"read",     (PyCFunction)consoleio_read,     METH_VARARGS,   read_doc},
    {"readall",  (PyCFunction)consoleio_readall,  METH_NOARGS,    readall_doc},
    {"readinto", (PyCFunction)consoleio_readinto, METH_VARARGS,   readinto_doc},
    {"write",    (PyCFunction)consoleio_write,    METH_VARARGS,   write_doc},
    {"seek",     (PyCFunction)consoleio_seek,     METH_VARARGS,   seek_doc},
    {"tell",     (PyCFunction)consoleio_tell,     METH_VARARGS,   tell_doc},
    {"close",    (PyCFunction)consoleio_close,    METH_NOARGS,    close_doc},
    {"seekable", (PyCFunction)consoleio_seekable, METH_NOARGS,    seekable_doc},
    {"readable", (PyCFunction)consoleio_readable, METH_NOARGS,    readable_doc},
    {"writable", (PyCFunction)consoleio_writable, METH_NOARGS,    writable_doc},
    {"fileno",   (PyCFunction)consoleio_fileno,   METH_NOARGS,    fileno_doc},
    {"isatty",   (PyCFunction)consoleio_isatty,   METH_NOARGS,    isatty_doc},
    {NULL,           NULL}             /* sentinel */
};

PyTypeObject PyConsoleIO_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "quake2.console",
    sizeof(consoleio),
    0,
    (destructor)console_dealloc,                /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    (reprfunc)consoleio_repr,                   /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    PyObject_GenericGetAttr,                    /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE
    | Py_TPFLAGS_HAVE_GC,                       /* tp_flags */
    consoleio_doc,                              /* tp_doc */
    (traverseproc)consoleio_traverse,           /* tp_traverse */
    (inquiry)consoleio_clear,                   /* tp_clear */
    0,                                          /* tp_richcompare */
    offsetof(consoleio, weakreflist),           /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,                                          /* tp_iternext */
    consoleio_methods,                          /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    offsetof(consoleio, dict),                  /* tp_dictoffset */
    0,                                          /* tp_init */
    PyType_GenericAlloc,                        /* tp_alloc */
    consoleio_new,                              /* tp_new */
    PyObject_GC_Del,                            /* tp_free */
};