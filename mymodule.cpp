#include <Python.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

enum OperandType {
    SCALAR,
    VECTOR,
    QUATERNION,
    MATRIX,
};

struct Operand {
    OperandType type;
    union {
        double s;
        glm::dvec3 v;
        glm::dquat q;
        glm::dmat3 m;
    };
};

int converter(PyObject * obj, Operand * operand) {
    if (PyFloat_Check(obj)) {
        operand->type = SCALAR;
        operand->s = PyFloat_AsDouble(obj);
        return 1;
    }
    if (PyLong_Check(obj)) {
        operand->type = SCALAR;
        operand->s = PyLong_AsDouble(obj);
        return 1;
    }
    if (!PyTuple_CheckExact(obj)) {
        return 0;
    }
    if (PyTuple_Size(obj) == 3) {
        operand->type = VECTOR;
        operand->v.x = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 0));
        operand->v.y = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 1));
        operand->v.z = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 2));
        return 1;
    }
    if (PyTuple_Size(obj) == 4) {
        operand->type = QUATERNION;
        operand->q.x = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 0));
        operand->q.y = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 1));
        operand->q.z = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 2));
        operand->q.w = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 3));
        return 1;
    }
    if (PyTuple_Size(obj) == 9) {
        operand->type = MATRIX;
        operand->m[0].x = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 0));
        operand->m[0].y = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 1));
        operand->m[0].z = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 2));
        operand->m[1].x = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 3));
        operand->m[1].y = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 4));
        operand->m[1].z = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 5));
        operand->m[2].x = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 6));
        operand->m[2].y = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 7));
        operand->m[2].z = PyFloat_AsDouble(PyTuple_GET_ITEM(obj, 8));
        return 1;
    }
}

PyObject * tup(const glm::dvec3 & v) {
    PyObject * res = PyTuple_New(3);
    PyTuple_SET_ITEM(res, 0, PyFloat_FromDouble(v.x));
    PyTuple_SET_ITEM(res, 1, PyFloat_FromDouble(v.y));
    PyTuple_SET_ITEM(res, 2, PyFloat_FromDouble(v.z));
    return res;
}

PyObject * tup(const glm::dquat & q) {
    PyObject * res = PyTuple_New(4);
    PyTuple_SET_ITEM(res, 0, PyFloat_FromDouble(q.x));
    PyTuple_SET_ITEM(res, 1, PyFloat_FromDouble(q.y));
    PyTuple_SET_ITEM(res, 2, PyFloat_FromDouble(q.z));
    PyTuple_SET_ITEM(res, 3, PyFloat_FromDouble(q.w));
    return res;
}

PyObject * tup(const glm::dmat3 & m) {
    PyObject * res = PyTuple_New(9);
    PyTuple_SET_ITEM(res, 0, PyFloat_FromDouble(m[0].x));
    PyTuple_SET_ITEM(res, 1, PyFloat_FromDouble(m[0].y));
    PyTuple_SET_ITEM(res, 2, PyFloat_FromDouble(m[0].z));
    PyTuple_SET_ITEM(res, 3, PyFloat_FromDouble(m[1].x));
    PyTuple_SET_ITEM(res, 4, PyFloat_FromDouble(m[1].y));
    PyTuple_SET_ITEM(res, 5, PyFloat_FromDouble(m[1].z));
    PyTuple_SET_ITEM(res, 6, PyFloat_FromDouble(m[2].x));
    PyTuple_SET_ITEM(res, 7, PyFloat_FromDouble(m[2].y));
    PyTuple_SET_ITEM(res, 8, PyFloat_FromDouble(m[2].z));
    return res;
}

PyObject * bytes(double s) {
    float encoded[] = {(float)s};
    return PyBytes_FromStringAndSize((char *)encoded, sizeof(encoded));
}

PyObject * bytes(const glm::dvec3 & v) {
    float encoded[] = {(float)v.x, (float)v.y, (float)v.z};
    return PyBytes_FromStringAndSize((char *)encoded, sizeof(encoded));
}

PyObject * bytes(const glm::dquat & q) {
    float encoded[] = {(float)q.x, (float)q.y, (float)q.z, (float)q.w};
    return PyBytes_FromStringAndSize((char *)encoded, sizeof(encoded));
}

PyObject * bytes(const glm::dmat3 & m) {
    float encoded[] = {
        (float)m[0].x, (float)m[0].y, (float)m[0].z,
        (float)m[1].x, (float)m[1].y, (float)m[1].z,
        (float)m[2].x, (float)m[2].y, (float)m[2].z,
    };
    return PyBytes_FromStringAndSize((char *)encoded, sizeof(encoded));
}

PyObject * mymodule_meth_add(PyObject * self, PyObject * args) {
    Operand a, b;
    if (!PyArg_ParseTuple(args, "O&O&", converter, &a, converter, &b)) {
        return NULL;
    }
    if (a.type == VECTOR && b.type == VECTOR) {
        return tup(a.v + b.v);
    }
    if (a.type == VECTOR && b.type == SCALAR) {
        return tup(a.v + b.s);
    }
    return NULL;
}

PyObject * mymodule_meth_sub(PyObject * self, PyObject * args) {
    Operand a, b;
    if (!PyArg_ParseTuple(args, "O&O&", converter, &a, converter, &b)) {
        return NULL;
    }
    if (a.type == VECTOR && b.type == VECTOR) {
        return tup(a.v - b.v);
    }
    if (a.type == VECTOR && b.type == SCALAR) {
        return tup(a.v - b.s);
    }
    return NULL;
}

PyObject * mymodule_meth_mul(PyObject * self, PyObject * args) {
    Operand a, b;
    if (!PyArg_ParseTuple(args, "O&O&", converter, &a, converter, &b)) {
        return NULL;
    }
    if (a.type == VECTOR && b.type == SCALAR) {
        return tup(a.v * b.s);
    }
    if (a.type == VECTOR && b.type == VECTOR) {
        return tup(a.v * b.v);
    }
    if (a.type == QUATERNION && b.type == VECTOR) {
        return tup(a.q * b.v);
    }
    if (a.type == QUATERNION && b.type == QUATERNION) {
        return tup(a.q * b.q);
    }
    if (a.type == QUATERNION && b.type == MATRIX) {
        return tup(glm::dmat3(a.q) * b.m);
    }
    if (a.type == MATRIX && b.type == VECTOR) {
        return tup(a.m * b.v);
    }
    if (a.type == MATRIX && b.type == QUATERNION) {
        return tup(glm::dquat(a.m) * b.q);
    }
    if (a.type == MATRIX && b.type == MATRIX) {
        return tup(a.m * b.m);
    }
    return NULL;
}

PyObject * mymodule_meth_normalize(PyObject * self, PyObject * arg) {
    Operand a;
    if (!converter(arg, &a)) {
        return NULL;
    }
    if (a.type == VECTOR) {
        return tup(glm::normalize(a.v));
    }
    if (a.type == QUATERNION) {
        return tup(glm::normalize(a.q));
    }
    if (a.type == MATRIX) {
        return tup(glm::dmat3(glm::normalize(glm::dquat(a.m))));
    }
    return NULL;
}

PyObject * mymodule_meth_inverse(PyObject * self, PyObject * arg) {
    Operand a;
    if (!converter(arg, &a)) {
        return NULL;
    }
    if (a.type == QUATERNION) {
        return tup(glm::inverse(a.q));
    }
    if (a.type == MATRIX) {
        return tup(glm::inverse(a.m));
    }
    return NULL;
}

PyObject * mymodule_meth_cast(PyObject * self, PyObject * arg) {
    Operand a;
    if (!converter(arg, &a)) {
        return NULL;
    }
    if (a.type == QUATERNION) {
        return tup(glm::dmat3(a.q));
    }
    if (a.type == MATRIX) {
        return tup(glm::dquat(a.m));
    }
    return NULL;
}

PyObject * mymodule_meth_swizzle(PyObject * self, PyObject * args) {
    Operand a;
    const char * swizzle;
    if (!PyArg_ParseTuple(args, "O&s", converter, &a, &swizzle)) {
        return NULL;
    }
    if (a.type == VECTOR) {
        return tup({
            a.v[swizzle[0] - 'x'],
            a.v[swizzle[1] - 'x'],
            a.v[swizzle[2] - 'x'],
        });
    }
    if (a.type == QUATERNION) {
        return tup({
            a.q[swizzle[0] - 'w'],
            a.q[swizzle[1] - 'w'],
            a.q[swizzle[2] - 'w'],
            a.q[swizzle[3] - 'w'],
        });
    }
    return NULL;
}

PyObject * mymodule_meth_pack(PyObject * self, PyObject * arg) {
    Operand a;
    if (!converter(arg, &a)) {
        return NULL;
    }
    if (a.type == SCALAR) {
        return bytes(a.s);
    }
    if (a.type == VECTOR) {
        return bytes(a.v);
    }
    if (a.type == QUATERNION) {
        return bytes(a.q);
    }
    if (a.type == MATRIX) {
        return bytes(a.m);
    }
    return NULL;
}

PyMethodDef module_methods[] = {
    {"add", (PyCFunction)mymodule_meth_add, METH_VARARGS, NULL},
    {"sub", (PyCFunction)mymodule_meth_sub, METH_VARARGS, NULL},
    {"mul", (PyCFunction)mymodule_meth_mul, METH_VARARGS, NULL},
    {"normalize", (PyCFunction)mymodule_meth_normalize, METH_O, NULL},
    {"inverse", (PyCFunction)mymodule_meth_inverse, METH_O, NULL},
    {"cast", (PyCFunction)mymodule_meth_cast, METH_O, NULL},
    {"swizzle", (PyCFunction)mymodule_meth_swizzle, METH_VARARGS, NULL},
    {"pack", (PyCFunction)mymodule_meth_pack, METH_O, NULL},
    {},
};

PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "mymodule", NULL, -1, module_methods};

extern "C" PyObject * PyInit_mymodule() {
    PyObject * module = PyModule_Create(&module_def);
    return module;
}
