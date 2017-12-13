#include "common.hpp"

#define N 2

PyObject * GLMMat2Array_tp_new(PyTypeObject * type, PyObject * args, PyObject * kwargs){
    GLMMat2Array * self = (GLMMat2Array *)type->tp_alloc(type, 0);

    if(self){
        self->val = 0;
    }

    return (PyObject *)self;
}

void GLMMat2Array_tp_dealloc(GLMMat2Array * self){
    Py_TYPE(self)->tp_free((PyObject *)self);
}

int GLMMat2Array_tp_init(GLMMat2Array * self, PyObject * args, PyObject * kwargs){
    PyObject * iterable;

    int arg_ok = PyArg_ParseTuple(args, "O", &iterable);

    if(!arg_ok){
        return -1;
    }
    
    if(Py_TYPE(iterable) == &PyTuple_Type && PyTuple_GET_SIZE(iterable) % (N * N) == 0){
        self->size = PyTuple_GET_SIZE(iterable) / (N * N);
        self->val = new glm::mat2[self->size + 1];

        int size = self->size;

        for(int i = 0; i < size; ++i){
            self->val[i][0][0] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(iterable, i * N * N + 0));
            self->val[i][0][1] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(iterable, i * N * N + 1));
            self->val[i][1][0] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(iterable, i * N * N + 2));
            self->val[i][1][1] = (float)PyFloat_AsDouble(PyTuple_GET_ITEM(iterable, i * N * N + 3));
        }

        if(PyErr_Occurred()){
            return -1;
        }
        return 0;
    }

    PyObject * iterator = PyObject_GetIter(iterable);
    int size = PyObject_Size(iterable);

    if(!iterator || size < 0 || size % (N * N)){
        PyErr_Format(PyExc_Exception, "not iterable or wrong size");
        return 0;
    }

    self->size = size / (N * N);
    self->val = new glm::mat2[self->size + 1];

    for(int k = 0; k < self->size; ++k){
        for(int i = 0; i < N; ++i){
            for(int j = 0; j < N; j++){
                PyObject * item = PyIter_Next(iterator);
                self->val[k][i][j] = (float)PyFloat_AsDouble(item);
                Py_DECREF(item);
            }
        }
    }

    Py_DECREF(iterator);

    if(PyErr_Occurred()){
        return -1;
    }

    return 0;

}

PyObject * GLMMat2Array_tp_repr(GLMMat2Array * self){
    PyObject * temp = GLMMat2Array_Tuple(self);
    PyObject * res = PyObject_Repr(temp);
    Py_DECREF(temp);
    return (PyObject *)res;
}

PyObject * GLMMat2Array_nb_add(PyObject * lhs, PyObject * rhs){
    if(Py_TYPE(lhs) == &GLMMat2Array_Type && Py_TYPE(rhs) == &GLMMat2Array_Type){
        int lhs_size = ((GLMMat2Array *)lhs)->size;
        int rhs_size = ((GLMMat2Array *)rhs)->size;

        if(lhs_size != rhs_size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
        res->size = lhs_size;
        res->val = new glm::mat2[res->size + 1];

        glm::mat2 * lhs_val = ((GLMMat2Array *)lhs)->val;
        glm::mat2 * rhs_val = ((GLMMat2Array *)rhs)->val;

        for(int i = 0; i < res->size; ++i){
            res->val[i] = lhs_val[i] + rhs_val[i];
        }
        
        return (PyObject *)res;
    }
    
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_subtract(PyObject * lhs, PyObject * rhs){
     if(Py_TYPE(lhs) == &GLMMat2Array_Type && Py_TYPE(rhs) == &GLMMat2Array_Type){
        int lhs_size = ((GLMMat2Array *)lhs)->size;
        int rhs_size = ((GLMMat2Array *)rhs)->size;

        if(lhs_size != rhs_size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
        res->size = lhs_size;
        res->val = new glm::mat2[res->size + 1];

        glm::mat2 * lhs_val = ((GLMMat2Array *)lhs)->val;
        glm::mat2 * rhs_val = ((GLMMat2Array *)rhs)->val;

        for(int i = 0; i < res->size; ++i){
            res->val[i] = lhs_val[i] - rhs_val[i];
        }
        
        return (PyObject *)res;
    }
    
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_multiply(PyObject * lhs, PyObject * rhs){
    if(Py_TYPE(lhs) == &GLMMat2Array_Type && Py_TYPE(rhs) == &GLMMat2Array_Type){
        int lhs_size = ((GLMMat2Array *)lhs)->size;
        int rhs_size = ((GLMMat2Array *)rhs)->size;

        if(lhs_size != rhs_size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
        res->size = lhs_size;
        res->val = new glm::mat2[res->size + 1];

        glm::mat2 * lhs_val = ((GLMMat2Array *)lhs)->val;
        glm::mat2 * rhs_val = ((GLMMat2Array *)rhs)->val;

        for(int i = 0; i < res->size; ++i){
            res->val[i] = lhs_val[i] * rhs_val[i];
        }
        
        return (PyObject *)res;
    }

    if(Py_TYPE(lhs) == &GLMMat2Array_Type){
        if(Py_TYPE(rhs) == &GLMVec2Array_Type){
            int lhs_size = ((GLMMat2Array *)lhs)->size;
            int rhs_size = ((GLMVec2Array *)rhs)->size;

            if(lhs_size != rhs_size){
                PyErr_Format(PyExc_Exception, "different sizes");
                return 0;
            }

            GLMVec2Array * res = (GLMVec2Array *)GLMVec2Array_tp_new(&GLMVec2Array_Type, 0, 0);
            res->size = lhs_size;
            res->val = new glm::vec2[res->size + 1];

            glm::mat2 * lhs_val = ((GLMMat2Array *)lhs)->val;
            glm::vec2 * rhs_val = ((GLMVec2Array *)rhs)->val;

            for(int i = 0; i < res->size; ++i){
                res->val[i] = lhs_val[i] * rhs_val[i];
            }

            return (PyObject *)res;
   
        }
        
        float scalar = (float)PyFloat_AsDouble(rhs);
        if(!PyErr_Occurred()){
            GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
            int lhs_size = ((GLMMat2Array*)lhs)->size;
            res->size = lhs_size;
            res->val = new glm::mat2[lhs_size + 1];
            
            for(int i = 0; i < lhs_size; ++i){
                res->val[i] = ((GLMMat2Array *)lhs)->val[i] * scalar;
            }

            return (PyObject *)res;

        }
    }

    if(Py_TYPE(rhs) == &GLMMat2Array_Type){
        float lhs_float = (float)PyFloat_AsDouble(lhs);
        if(!PyErr_Occurred()){            
            GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
            int rhs_size = ((GLMMat2Array *)rhs)->size;
            res->size = rhs_size;
            res->val = new glm::mat2[rhs_size + 1];
            
            for(int i = 0; i < rhs_size; ++i){
                res->val[i] = lhs_float * ((GLMMat2Array *)rhs)->val[i];
            }

            return (PyObject *)res;
        }
    }

    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;
}


PyObject * GLMMat2Array_nb_true_divide(PyObject * lhs, PyObject * rhs){
    if(Py_TYPE(lhs) == &GLMMat2Array_Type){
        float rhs_float = (float)PyFloat_AsDouble(rhs);
        if(!PyErr_Occurred()){
            
            GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
            int lhs_size = ((GLMMat2Array *)lhs)->size;
            res->size = lhs_size;
            res->val = new glm::mat2[lhs_size + 1];

            for(int i = 0; i < lhs_size; ++i){
                res->val[i] = ((GLMMat2Array *)lhs)->val[i] / rhs_float;
            }

            return (PyObject *)res;
        }
    }
    
    if(Py_TYPE(rhs) == &GLMMat2Array_Type){
        float lhs_float = (float)PyFloat_AsDouble(lhs);
        if(!PyErr_Occurred()){
            
            GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
            int rhs_size = ((GLMMat2Array *)rhs)->size;
            res->size = rhs_size;
            res->val = new glm::mat2[rhs_size + 1];

            for(int i = 0; i < rhs_size; ++i){
                res->val[i] = ((GLMMat2Array *)rhs)->val[i] / lhs_float;
            }

            return (PyObject *)res;
        }
    }
    
    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;    
}


PyObject * GLMMat2Array_nb_inplace_add(PyObject * lhs, PyObject * rhs){
    if(Py_TYPE(rhs) == &GLMMat2Array_Type){
        if(((GLMMat2Array *)lhs)->size != ((GLMMat2Array *)rhs)->size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        for(int i = 0; i < ((GLMMat2Array *)lhs)->size; ++i){
            ((GLMMat2Array *)lhs)->val[i] += ((GLMMat2Array *)rhs)->val[i];
        }

        return lhs;
    }

    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_inplace_subtract(PyObject * lhs, PyObject * rhs){
    if (Py_TYPE(rhs) == &GLMMat2Array_Type) {
        if(((GLMMat2Array *)lhs)->size != ((GLMMat2Array *)rhs)->size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        int lhs_size = ((GLMMat2Array *)lhs)->size;
        
        for (int i = 0; i < lhs_size; ++i) {
            ((GLMMat2Array *)lhs)->val[i] -= ((GLMMat2Array *)rhs)->val[i];
        }

        return lhs;
    }

    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_inplace_multiply(PyObject * lhs, PyObject * rhs){
    if (Py_TYPE(rhs) == &GLMMat2Array_Type){
        if(((GLMMat2Array *)lhs)->size != ((GLMMat2Array *)rhs)->size){
            PyErr_Format(PyExc_Exception, "different sizes");
            return 0;
        }

        int lhs_size = ((GLMMat2Array *)lhs)->size;
        
        for (int i = 0; i < lhs_size; ++i) {
            ((GLMMat2Array *)lhs)->val[i] *= ((GLMMat2Array *)rhs)->val[i];
        }

        return lhs;
    }

    float rhs_float = (float)PyFloat_AsDouble(rhs);
    if(!PyErr_Occurred()){
        int lhs_size = ((GLMMat2Array *)lhs)->size;
        for(int i = 0; i < lhs_size; ++i){
            ((GLMMat2Array *)lhs)->val[i] *= rhs_float;
        }
        return lhs;
    }

    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_inplace_true_divide(PyObject * lhs, PyObject * rhs){
    float rhs_float = (float)PyFloat_AsDouble(rhs);
    if(!PyErr_Occurred()){
        int lhs_size = ((GLMMat2Array *)lhs)->size;
        for(int i = 0; i < lhs_size; ++i){
            ((GLMMat2Array *)lhs)->val[i] /= rhs_float;
        }

        return lhs;
    }

    PyErr_Clear();
    Py_RETURN_NOTIMPLEMENTED;
}

PyObject * GLMMat2Array_nb_negative(PyObject * self){
    GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
    res->size = ((GLMMat2Array *)self)->size;
    res->val = new glm::mat2[res->size + 1];
    int self_size = res->size;

    for(int i = 0; i < self_size; ++i){
        res->val[i] = -((GLMMat2Array *)self)->val[i];
    }

    return (PyObject *)res;
}

PyObject * GLMMat2Array_nb_positive(PyObject * self){
    GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
    res->size = ((GLMMat2Array *)self)->size;
    res->val = new glm::mat2[res->size + 1];
    int self_size = res->size;

    for(int i = 0; i < self_size; ++i){
        res->val[i] = +((GLMMat2Array *)self)->val[i];
    }

    return (PyObject *)res;
}

PyNumberMethods GLMMat2Array_tp_as_number = {
	(binaryfunc)GLMMat2Array_nb_add, // binaryfunc nb_add;
	(binaryfunc)GLMMat2Array_nb_subtract, // binaryfunc nb_subtract;
	(binaryfunc)GLMMat2Array_nb_multiply, // binaryfunc nb_multiply;
	0, // binaryfunc nb_remainder;
	0, // binaryfunc nb_divmod;
	0, // ternaryfunc nb_power;
	(unaryfunc)GLMMat2Array_nb_negative, // unaryfunc nb_negative;
	(unaryfunc)GLMMat2Array_nb_positive, // unaryfunc nb_positive;
	0, // unaryfunc nb_absolute;
	0, // inquiry nb_bool;
	0, // unaryfunc nb_invert;
	0, // binaryfunc nb_lshift;
	0, // binaryfunc nb_rshift;
	0, // binaryfunc nb_and;
	0, // binaryfunc nb_xor;
	0, // binaryfunc nb_or;
	0, // unaryfunc nb_int;
	0, // void *nb_reserved;
	0, // unaryfunc nb_float;
	(binaryfunc)GLMMat2Array_nb_inplace_add, // binaryfunc nb_inplace_add;
	(binaryfunc)GLMMat2Array_nb_inplace_subtract, // binaryfunc nb_inplace_subtract;
	(binaryfunc)GLMMat2Array_nb_inplace_multiply, // binaryfunc nb_inplace_multiply;
	0, // binaryfunc nb_inplace_remainder;
	0, // ternaryfunc nb_inplace_power;
	0, // binaryfunc nb_inplace_lshift;
	0, // binaryfunc nb_inplace_rshift;
	0, // binaryfunc nb_inplace_and;
	0, // binaryfunc nb_inplace_xor;
	0, // binaryfunc nb_inplace_or;
	0, // binaryfunc nb_floor_divide;
	(binaryfunc)GLMMat2Array_nb_true_divide, // binaryfunc nb_true_divide;
	0, // binaryfunc nb_inplace_floor_divide;
	(binaryfunc)GLMMat2Array_nb_inplace_true_divide, // binaryfunc nb_inplace_true_divide;
	0, // unaryfunc nb_index;
	// binaryfunc nb_matrix_multiply;
	// binaryfunc nb_inplace_matrix_multiply;
};

Py_ssize_t GLMMat2Array_sq_length(GLMMat2Array * self){
    return self->size * N * N;
}

PyObject * GLMMat2Array_sq_item(GLMMat2Array * self, Py_ssize_t key){
    if(key < self->size * N * N){
        int i = key / (N * N);
        int k = key % (N * N);
        return PyFloat_FromDouble(self->val[i][k / N][k % N]);
    }
    return 0;
}

PyObject * GLMMat2Array_sq_ass_item(GLMMat2Array * self, Py_ssize_t key, PyObject * value){
    if(key < self->size * N * N){
        int i = key / (N * N);
        int k = key % (N * N);
        float val = PyFloat_AsDouble(value);
        if(PyErr_Occurred()){
            return (PyObject *) -1;
        }
        self->val[i][k / N][k % N] = val;
        return 0;
    }
    return (PyObject *) -1;
}

PySequenceMethods GLMMat2Array_tp_as_sequence = {
	(lenfunc)GLMMat2Array_sq_length, // lenfunc sq_length;
	0, // binaryfunc sq_concat;
	0, // ssizeargfunc sq_repeat;
	(ssizeargfunc)GLMMat2Array_sq_item, // ssizeargfunc sq_item;
	0, // void *was_sq_slice;
	(ssizeobjargproc)GLMMat2Array_sq_ass_item, // ssizeobjargproc sq_ass_item;
	0, // void *was_sq_ass_slice;
	0, // objobjproc sq_contains;
	0, // binaryfunc sq_inplace_concat;
	0, // ssizeargfunc sq_inplace_repeat;
};

int GLMMat2Array_bf_getbuffer(GLMMat2Array * self, Py_buffer * view, int flags) {
	view->buf = (void *)self->val;
	view->len = sizeof(self->val[0]) * self->size;
	view->itemsize = 1;

	view->format = 0;
	view->ndim = 0;
	view->shape = 0;
	view->strides = 0;
	view->suboffsets = 0;

	Py_INCREF(self);
	view->obj = (PyObject *)self;
	return 0;
}

PyBufferProcs GLMMat2Array_tp_as_buffer = {
    (getbufferproc)GLMMat2Array_bf_getbuffer, // getbufferproc bf_getbuffer;
	0, // releasebufferproc bf_releasebuffer;
};

PyObject * GLMMat2Array_tp_meth_row(GLMMat2 * self, PyObject * args){
    int id = PyLong_AsLong(PyTuple_GetItem(args, 0));
    GLMVec2Array * res = (GLMVec2Array *)GLMVec2Array_tp_new(&GLMVec2Array_Type, 0, 0);
    res->size = ((GLMMat2Array *)self)->size;
    res->val = new glm::vec2[res->size + 1];
    int size = res->size;

    for(int i = 0; i < size; ++i){
        res->val[i] = glm::row(((GLMMat2Array *)self)->val[i], id);
    }

    return (PyObject *)res;
}

PyObject * GLMMat2Array_tp_meth_col(GLMMat2 * self, PyObject * args){
    int id = PyLong_AsLong(PyTuple_GetItem(args, 0));
    GLMVec2Array * res = (GLMVec2Array *)GLMVec2Array_tp_new(&GLMVec2Array_Type, 0, 0);
    res->size = ((GLMMat2Array *)self)->size;
    res->val = new glm::vec2[res->size + 1];
    int size = res->size;

    for(int i = 0; i < size; ++i){
        res->val[i] = glm::column(((GLMMat2Array *)self)->val[i], id);
    }

    return (PyObject *)res;
}

PyMethodDef GLMMat2Array_tp_methods[] = {
	{"row", (PyCFunction)GLMMat2Array_tp_meth_row, METH_VARARGS, 0},
	{"col", (PyCFunction)GLMMat2Array_tp_meth_col, METH_VARARGS, 0},
	{0},
};

PyObject * GLMMat2Array_tp_get_trans(GLMMat2Array * self, void * closure){
    GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
    int size = self->size;
    res->size = size;
    res->val = new glm::mat2[size + 1];

    for(int i = 0; i < size; ++i){
        res->val[i] = glm::transpose(self->val[i]);
    }

    return (PyObject *)res;
}

PyObject * GLMMat2Array_tp_get_det(GLMMat2Array * self, void * closure){
    int size = self->size;
    GLMFloatArray * res = (GLMFloatArray *)GLMFloatArray_tp_new(&GLMFloatArray_Type, 0, 0);
    res->size = size;
    res->val = new float[size + 1];

    for(int i = 0; i < size; ++i){
        res->val[i] = glm::determinant(self->val[i]);
    }
    return (PyObject *)res; 
}

PyObject * GLMMat2Array_tp_get_inv(GLMMat2Array * self, void * closure){
    GLMMat2Array * res = (GLMMat2Array *)GLMMat2Array_tp_new(&GLMMat2Array_Type, 0, 0);
    int size = self->size;
    res->size = size;
    res->val = new glm::mat2[size + 1];

    for(int i = 0; i < size; ++i){
        res->val[i] = glm::inverse(self->val[i]);
    }
    return (PyObject *)res;
}

PyObject * GLMMat2Array_tp_get_tup(GLMMat2Array * self, void * closure){
    return (PyObject *) GLMMat2Array_Tuple(self);
}

PyGetSetDef GLMMat2Array_tp_getseters[] = {
	{(char *)"trans", (getter)GLMMat2Array_tp_get_trans, 0, 0, 0},
	{(char *)"det", (getter)GLMMat2Array_tp_get_det, 0, 0, 0},
	{(char *)"inv", (getter)GLMMat2Array_tp_get_inv, 0, 0, 0},
	{(char *)"tup", (getter)GLMMat2Array_tp_get_tup, 0, 0, 0},
	{0},
};

PyTypeObject GLMMat2Array_Type = {
	PyVarObject_HEAD_INIT(0, 0)
	"glm.Mat2", // tp_name
	sizeof(GLMMat2), // tp_basicsize
	0, // tp_itemsize
	(destructor)GLMMat2Array_tp_dealloc, // tp_dealloc
	0, // tp_print
	0, // tp_getattr
	0, // tp_setattr
	0, // tp_reserved
	(reprfunc)GLMMat2Array_tp_repr, // tp_repr
	&GLMMat2Array_tp_as_number, // tp_as_number
	&GLMMat2Array_tp_as_sequence, // tp_as_sequence
	0, // tp_as_mapping
	0, // tp_hash
	0, // tp_call
	0, // tp_str
	0, // tp_getattro
	0, // tp_setattro
	&GLMMat2Array_tp_as_buffer, // tp_as_buffer
	Py_TPFLAGS_DEFAULT, // tp_flags
	0, // tp_doc
	0, // tp_traverse
	0, // tp_clear
	0, // tp_richcompare
	0, // tp_weaklistoffset
	0, // tp_iter
	0, // tp_iternext
	GLMMat2Array_tp_methods, // tp_methods
	0, // tp_members
	GLMMat2Array_tp_getseters, // tp_getset
	0, // tp_base
	0, // tp_dict
	0, // tp_descr_get
	0, // tp_descr_set
	0, // tp_dictoffset
	(initproc)GLMMat2Array_tp_init, // tp_init
	0, // tp_alloc
	GLMMat2Array_tp_new, // tp_new
};

PyObject * GLMMat2Array_Tuple(GLMMat2Array * self){
    int size = self->size;
    PyObject * tuple = PyTuple_New(size * N * N);

    for(int i = 0; i < size; ++i){
        PyTuple_SET_ITEM(tuple, i * N * N + 0, PyFloat_FromDouble(self->val[i][0][0]));
        PyTuple_SET_ITEM(tuple, i * N * N + 1, PyFloat_FromDouble(self->val[i][0][1]));
        PyTuple_SET_ITEM(tuple, i * N * N + 2, PyFloat_FromDouble(self->val[i][1][0]));
        PyTuple_SET_ITEM(tuple, i * N * N + 3, PyFloat_FromDouble(self->val[i][1][1]));
    }

    return tuple;
}