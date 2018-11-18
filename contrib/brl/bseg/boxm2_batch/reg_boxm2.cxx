#include <iostream>
#include "reg_boxm2.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <brdb/brdb_selection.h>

// processes
#include <boxm2/pro/boxm2_register.h>

#include <boxm2/cpp/pro/boxm2_cpp_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <vil_pro/vil_register.h>
#include <bvgl_pro/bvgl_register.h>
#include <sdet_pro/sdet_register.h>
#include <brip_pro/brip_register.h>
#include <bsgm/pro/bsgm_register.h>
#if defined(HAS_OPENCL) && HAS_OPENCL
#include <bocl/pro/bocl_register.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/pro/boxm2_ocl_register.h>
#include <boxm2_multi/pro/boxm2_multi_register.h>
#include <boxm2/vecf/ocl/pro/boxm2_vecf_ocl_register.h>
#if defined(HAS_GLEW) && HAS_GLEW
#include <boxm2/view/pro/boxm2_view_register.h>
#endif
#endif
#include <bbas_pro/bbas_register.h>
#include <brad_pro/brad_register.h>
#include <bsta/pro/bsta_register.h>
#include <bvpl/kernels/pro/bvpl_kernels_register.h>
#include <ihog_pro/ihog_register.h>
#if defined(HAS_HDFS) && HAS_HDFS
#include <bhdfs/pro/bhdfs_register.h>
#endif
#include <bxml/bsvg/pro/bsvg_register.h>
#include <bvrml/pro/bvrml_register.h>
#include <bbgm/pro/bbgm_register.h>
#include <icam_pro/icam_register.h>
#if defined(USE_VOLM) && USE_VOLM
#include <volm/pro/volm_register.h>
#include <boxm2/volm/pro/boxm2_volm_register.h>
#endif
PyObject *
    register_processes(PyObject *self, PyObject *args)
{
    boxm2_register::register_process();
    boxm2_cpp_register::register_process();
    vpgl_register::register_process();
    vil_register::register_process();
    bvgl_register::register_process();
    brip_register::register_process();
    bsgm_register::register_process();
    bvpl_kernels_register::register_process();
#if defined(HAS_OPENCL) && HAS_OPENCL
    bocl_register::register_process();
    boxm2_ocl_register::register_process();
    boxm2_multi_register::register_process();
    boxm2_vecf_ocl_register::register_process();
#if defined(HAS_GLEW) && HAS_GLEW
    boxm2_view_register::register_process();
#endif
#endif
    bbas_register::register_process();
    brad_register::register_process();
    bsta_register::register_process();
    ihog_register::register_process();
    sdet_register::register_process();
#if defined(HAS_HDFS) && HAS_HDFS
    bhdfs_register::register_process();
#endif
    bsvg_register::register_process();
    bvrml_register::register_process();
    bbgm_register::register_process();
    icam_register::register_process();
#if defined(USE_VOLM) && USE_VOLM
#if defined(HAS_OPENCL) && HAS_OPENCL
    boxm2_volm_register::register_process();
#endif
    volm_register::register_process();
#endif
    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *
    register_datatypes(PyObject *self, PyObject *args)
{
    register_basic_datatypes();

    boxm2_register::register_datatype();
    boxm2_cpp_register::register_datatype();
    vpgl_register::register_datatype();
    vil_register::register_datatype();
    bvgl_register::register_datatype();
    brip_register::register_datatype();
    bsgm_register::register_datatype();
    bvpl_kernels_register::register_datatype();
#if defined(HAS_OPENCL) && HAS_OPENCL
    bocl_register::register_datatype();
    boxm2_ocl_register::register_datatype();
    boxm2_multi_register::register_datatype();
#if defined(HAS_GLEW) && HAS_GLEW
    boxm2_view_register::register_datatype();
#endif
#endif
    bbas_register::register_datatype();
    sdet_register::register_datatype();
    brad_register::register_datatype();
    bsta_register::register_datatype();
    ihog_register::register_datatype();
#if defined(HAS_HDFS) && HAS_HDFS
    bhdfs_register::register_datatype();
#endif
    bsvg_register::register_datatype();
    bvrml_register::register_datatype();
    bbgm_register::register_datatype();
    icam_register::register_datatype();
    boxm2_register::register_datatype();
#if defined(USE_VOLM) && USE_VOLM
#if defined(HAS_OPENCL) && HAS_OPENCL
    boxm2_volm_register::register_datatype();
#endif
    volm_register::register_datatype();
#endif
    Py_INCREF(Py_None);
    return Py_None;
}

#if defined(HAS_OPENCL) && HAS_OPENCL
void release_ocl_contexts()
{
    if (bocl_manager_child::is_instantiated())
    {
      std::cout << "Py_AtExit::Releasing ocl contexts" << std::endl;
      bocl_manager_child::instance().clear_cl();
    }
}
#endif

PyMODINIT_FUNC
    initboxm2_batch(void)
{
    PyMethodDef reg_pro;
    reg_pro.ml_name = "register_processes";
    reg_pro.ml_meth = register_processes;
    reg_pro.ml_doc = "register_processes() create instances of each defined process";
    reg_pro.ml_flags = METH_VARARGS;

    PyMethodDef reg_data;
    reg_data.ml_name = "register_datatypes";
    reg_data.ml_meth = register_datatypes;
    reg_data.ml_doc = "register_datatypes() insert tables in the database for each type";
    reg_data.ml_flags = METH_VARARGS;

    boxm2_batch_methods[0]=reg_pro;
    boxm2_batch_methods[1]=reg_data;

    for (int i=0; i<METHOD_NUM; ++i) {
        boxm2_batch_methods[i+2]=batch_methods[i];
    }

#if defined(HAS_OPENCL) && HAS_OPENCL
    Py_AtExit(release_ocl_contexts);
#endif
    Py_InitModule("boxm2_batch", boxm2_batch_methods);
}
