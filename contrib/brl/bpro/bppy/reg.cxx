#include "batch.h"
#include <bprb/bprb_macros.h>
#include <vil/vil_image_view_base.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <vidl2/vidl2_ostream_sptr.h>
#include <core/vil_pro/vil_load_image_view_process.h>
#include <core/vil_pro/vil_save_image_view_process.h>
#include <core/vidl2_pro/vidl2_open_istream_process.h>
#include <core/vidl2_pro/vidl2_open_ostream_process.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/pro/bbgm_load_image_of_process.h>
#include <bbgm/pro/bbgm_save_image_of_process.h>
#include <bbgm/pro/bbgm_update_dist_image_process.h>
#include <bbgm/pro/bbgm_update_dist_image_stream_process.h>
#include <bbgm/pro/bbgm_display_dist_image_process.h>
#include <bbgm/pro/bbgm_measure_process.h>
PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(bprb_null_process, bprb_batch_process_manager);
  REG_PROCESS(vil_load_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_open_istream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_open_ostream_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_load_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_save_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_stream_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_display_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_measure_process, bprb_batch_process_manager);
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vidl2_istream_sptr);
  REGISTER_DATATYPE(vidl2_ostream_sptr);
  REGISTER_DATATYPE(bbgm_image_sptr);
  Py_INCREF(Py_None);
  return Py_None;
}

