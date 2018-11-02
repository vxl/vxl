#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include "../bmdl_processes.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>

static void test_bmdl_trace_boundaries_process(int argc, char* argv[])
{
  REG_PROCESS_FUNC(bprb_func_process, bprb_batch_process_manager, bmdl_trace_boundaries_process, "bmdlTraceBoundariesProcess");
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  std::string image_base;
  if ( argc >= 2 ) {
    image_base = argv[1];
    image_base += "/";
  }

  //create vil_image_view_base_sptr
  std::string label_img_path = image_base + "label.tif";
  vil_image_view_base_sptr label = vil_load(label_img_path.c_str());

  std::string polygons_path = "polygons.bin";

  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(label);
  brdb_value_sptr v1 = new brdb_value_t<std::string>(polygons_path);

  bool good = bprb_batch_process_manager::instance()->init_process("bmdlTraceBoundariesProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  TEST("run trace boundaries process", good ,true);

  // check the polygons size
  // read polygons
  vsl_b_ifstream os(polygons_path);
  unsigned char ver; //version();
  vsl_b_read(os, ver);
  unsigned int size;
  vsl_b_read(os, size);
  vgl_polygon<double> polygon;
  std::vector<vgl_polygon<double> > boundaries;
  for (unsigned i = 0; i < size; i++) {
    vsl_b_read(os, polygon);
    boundaries.push_back(polygon);
  }

  good = (boundaries.size()>0);
  TEST("polygons are read successfully", good ,true);
}

TESTMAIN_ARGS(test_bmdl_trace_boundaries_process);
