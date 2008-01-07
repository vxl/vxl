// This is brl/bseg/vpro/vpro_edge_line_process.cxx
#include "vpro_edge_line_process.h"
//:
// \file
#include <vcl_iostream.h>
#include <sdet/sdet_fit_lines.h>
#include "vpro_edge_process.h"
#include "vpro_line_fit_process.h"

vpro_edge_line_process::vpro_edge_line_process(sdet_detector_params& dp,
                                               sdet_fit_lines_params& flp)
  : sdet_detector_params(dp), sdet_fit_lines_params(flp)
{
}

vpro_edge_line_process::~vpro_edge_line_process()
{
}
//------------------------------------------------------------------
//: setup a pipeline for edges and line segments
//
bool vpro_edge_line_process::execute()
{
  this->clear_output();
  //my_spat_objs_.clear();
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In vpro_edge_line_process::execute() - not exactly one input image\n";
    return false;
  }
  vpro_edge_process ep(*((sdet_detector_params*)this));
  ep.add_input_image(this->get_input_image(0));
  if (!ep.execute())
  {
    this->clear_input();
    return false;
  }
  vpro_line_fit_process lfp(*((sdet_fit_lines_params*)this));
  lfp.add_input_topology(ep.get_output_topology());
  if (!lfp.execute())
  {
    this->clear_input();
    return false;
  }
  output_spat_objs_ = lfp.get_output_spatial_objects();
  this->clear_input();
  return true;
}
