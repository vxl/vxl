// This is brl/vvid/vvid_grid_finder_process.cxx
#include "vvid_grid_finder_process.h"
//:
// \file
#include <vcl_iostream.h>
#include <vil/vil_image.h>
#include <vsol/vsol_line_2d.h>
#include <sdet/sdet_fit_lines.h>
#include <sdet/sdet_grid_finder.h>
#include <vvid/vvid_edge_process.h>
#include <vvid/vvid_line_fit_process.h>

vvid_grid_finder_process::vvid_grid_finder_process(sdet_detector_params& dp,
                                                   sdet_fit_lines_params& flp,
                                                   sdet_grid_finder_params& gfp)
  : sdet_detector_params(dp), sdet_fit_lines_params(flp), 
    sdet_grid_finder_params(gfp)
{
}

vvid_grid_finder_process::~vvid_grid_finder_process()
{
}

//------------------------------------------------------------------
//: setup a pipeline for edges and line segments
//
bool vvid_grid_finder_process::execute()
{
  this->clear_output();
  //my_spat_objs_.clear();
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vvid_grid_finder_process::execute() - not exactly one"
               << " input image\n";
      return false;
    }
  vil_image img = this->get_input_image(0);
  vvid_edge_process ep(*((sdet_detector_params*)this));
  ep.add_input_image(img);
  if (!ep.execute())
  { 
    this->clear_input();
    return false;
  }
  vvid_line_fit_process lfp(*((sdet_fit_lines_params*)this));
  lfp.add_input_topology(ep.get_output_topology());
  if (!lfp.execute())
  {
    this->clear_input();
    return false;
  }
  //convert spatial objects to lines, should be in bsol ops
  vcl_vector<vsol_line_2d_sptr> lines;
  vcl_vector<vsol_spatial_object_2d_sptr> sos = lfp.get_output_spatial_objects();
  for (vcl_vector<vsol_spatial_object_2d_sptr>::iterator sit = sos.begin();
       sit!=sos.end(); sit++)
    {
      vsol_line_2d_sptr l = (*sit)->cast_to_curve()->cast_to_line_2d();
      if (!l)
        continue;
      lines.push_back(l);
    }
  sdet_grid_finder gf(*((sdet_grid_finder_params*)this));
  if (!gf.set_lines(img.width(), img.height(), lines))
    {
      this->clear_input();
      return false;
    }
  //Get the backprojected grid
  vcl_vector<vsol_line_2d_sptr> mapped_lines;
  gf.compute_homography();
  if (!gf.get_backprojected_grid(mapped_lines))
    {
      this->clear_input();
      return false;
    }
  for (vcl_vector<vsol_line_2d_sptr>::iterator lit = mapped_lines.begin();
       lit != mapped_lines.end(); lit++)
    {
      output_spat_objs_.push_back((*lit)->cast_to_spatial_object_2d());
    }
  this->clear_input();
  return true;
}
