#include "vpro_roi_process.h"
//:
// \file
#include <vcl_iostream.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>
#include <vsol/vsol_box_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_roi.h>

vpro_roi_process::vpro_roi_process(vcl_string const & video_file,
                                   const unsigned x0,
                                   const unsigned y0,
                                   const unsigned xsize,
                                   const unsigned ysize,
                                   vcl_string const& type) :
  x0_(x0), y0_(y0), xsize_(xsize), ysize_(ysize), video_file_(video_file),
  type_(type)
{
}

vpro_roi_process::~vpro_roi_process()
{
}

bool vpro_roi_process::execute()
{
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In vpro_roi_process::execute() -"
             << " not exactly one input image\n";
  return false;
  }
  vil_image_resource_sptr img = vpro_vil_video_process::get_input_image(0);
  this->clear_input();
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(*img);
  brip_roi_sptr roi = new brip_roi(img->ni(), img->nj());
  roi->add_region(x0_, y0_, xsize_, ysize_);
  vsol_box_2d_sptr box = roi->region(0);
  vil_image_view<float> out;
  brip_vil_float_ops::chip(fimage, box, out);
  vil_image_view<unsigned char> outb;
  outb = brip_vil_float_ops::convert_to_byte(out);
  vil_image_resource_sptr outr = vil_new_image_resource_of_view(outb);
  out_frames_.push_back(outr);
  return true;
}

bool vpro_roi_process::finish()
{
  if (!out_frames_.size())
    return false;
  vidl1_clip_sptr clip = new vidl1_clip(out_frames_);
  vidl1_movie_sptr mov= new vidl1_movie();
  mov->add_clip(clip);
  if (!vidl1_io::save(mov.ptr(), video_file_.c_str(), type_)) {
    vcl_cerr << "In vpro_roi_process::finish() - failed to save video\n";
    return false;
  }
  out_frames_.clear();
  return true;
 }

