// This is brl/bseg/vpro/vpro_grid_finder_process.cxx
#include <vpro/vpro_grid_finder_process.h>
//:
// \file
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vil1/vil1_image.h>
#include <sdet/sdet_fit_lines.h>
#include <sdet/sdet_grid_finder.h>
#include <vpro/vpro_edge_process.h>
#include <vpro/vpro_line_fit_process.h>
#include <vsol/vsol_line_2d.h>

vpro_grid_finder_process::vpro_grid_finder_process(sdet_detector_params& dp,
                                                   sdet_fit_lines_params& flp,
                                                   sdet_grid_finder_params& gfp)
  : sdet_detector_params(dp), sdet_fit_lines_params(flp),
    sdet_grid_finder_params(gfp)
{
  output_filename_ = "grid_points.txt";
}

vpro_grid_finder_process::~vpro_grid_finder_process()
{
}

//: set the output filename
void vpro_grid_finder_process::set_output_file(vcl_string filename)
{
  output_filename_ = filename;
}

//------------------------------------------------------------------
//: setup a pipeline for edges and line segments
//
bool vpro_grid_finder_process::execute()
{
  this->clear_output();
  //my_spat_objs_.clear();
  int nimages = -1;
  if ((nimages = this->get_N_input_images())!=1)
  {
    vcl_cout << "In vpro_grid_finder_process::execute() - not exactly one input image ("<<nimages<<")\n";
    frame_scores_.push_back(0.0);
    return false;
  }
  vil1_image img = this->get_input_image(0);
  vpro_edge_process ep(*((sdet_detector_params*)this));
  ep.add_input_image(img);
  if (!ep.execute())
  {
    this->clear_input();
    frame_scores_.push_back(0.0);
    return false;
  }
  vpro_line_fit_process lfp(*((sdet_fit_lines_params*)this));
  lfp.add_input_topology(ep.get_output_topology());
  if (!lfp.execute())
  {
    this->clear_input();
    frame_scores_.push_back(0.0);
    return false;
  }
  //convert spatial objects to lines, should be in bsol ops
  vcl_vector<vsol_line_2d_sptr> lines;
  vcl_vector<vsol_spatial_object_2d_sptr> sos = lfp.get_output_spatial_objects();
  for (vcl_vector<vsol_spatial_object_2d_sptr>::iterator sit = sos.begin();
       sit!=sos.end(); sit++)
  {
    vsol_line_2d_sptr l = (*sit)->cast_to_curve()->cast_to_line();
    if (!l)
      continue;
    lines.push_back(l);
  }
  sdet_grid_finder gf(*((sdet_grid_finder_params*)this));
  gf.unset_verbose();

  if (!gf.set_lines(float(img.width()), float(img.height()), lines))
  {
    this->clear_input();
    frame_scores_.push_back(0.0);
    return false;
  }
  //Get the backprojected grid
  vcl_vector<vsol_line_2d_sptr> mapped_lines;
  if (!gf.compute_homography())
  {
    vcl_cout << "compute_homography() failed\n";
    this->clear_input();
    frame_scores_.push_back(0.0);
    return false;
  }
  if (!gf.get_backprojected_grid(mapped_lines))
  {
    this->clear_input();
    frame_scores_.push_back(0.0);
    return false;
  }
  for (vcl_vector<vsol_line_2d_sptr>::iterator lit = mapped_lines.begin();
       lit != mapped_lines.end(); lit++)
    output_spat_objs_.push_back((*lit)->cast_to_spatial_object());
  // double check backprojected grid with image intensities
  if (!gf.check_grid_match(img))
  {
    vcl_cout << "check_grid_match() failed - disregarding homography\n";
    this->clear_input();
    frame_scores_.push_back(0.0);
    return true;
  }
  // TEMP - Write grid points to file
  if (output_filename_ != "")
  {
    static int view_count = 0;
    static vcl_ofstream outstream(output_filename_.c_str());

    if (view_count == 0)
      gf.init_output_file(outstream);

    gf.write_image_points(outstream);
    ++view_count;

    vcl_cout << "total of "<<view_count<<" views written.\n";
  }
  this->clear_input();
  frame_scores_.push_back(100.0);
  return true;
}
