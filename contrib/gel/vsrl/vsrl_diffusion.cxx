// This is gel/vsrl/vsrl_diffusion.cxx
#include "vsrl_diffusion.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for std::sprintf()
#include <vxl_config.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>
#include <vsrl/vsrl_parameters.h>

vsrl_diffusion::vsrl_diffusion(vsrl_dense_matcher *matcher)
: width_(matcher->get_width())
, height_(matcher->get_height())
, disparity_matrix_(new vnl_matrix<double>(width_,height_))
, matcher_(matcher)
{
}

vsrl_diffusion::~vsrl_diffusion()
{
  delete disparity_matrix_;
}

double vsrl_diffusion::get_disparity(int x, int y)
{
  // we want to get the disparity for the point x, y

  if (x>=0 && x<get_width() && y>=0 && y < get_height()) {

    return (*disparity_matrix_)(x,y);
  }
  return 0;
}


void vsrl_diffusion::write_image(char *file_name,int it_num, vnl_matrix<double> *mat)
{
  // we want to write the file_name concatenated with the it_number

  char new_name[512];

  if (it_num < 10) {
    vcl_sprintf(&(new_name[0]),"%s00%d.ppm",file_name,it_num);
    write_image(&(new_name[0]),mat);
    return;
  }

  if (it_num < 100) {
    vcl_sprintf(&(new_name[0]),"%s0%d.ppm",file_name,it_num);
    write_image(&(new_name[0]),mat);
    return;
  }

  vcl_sprintf(&(new_name[0]),"%s%d.ppm",file_name,it_num);
  write_image(&(new_name[0]),mat);
  return;
}

void vsrl_diffusion::write_image(char *file_name,vnl_matrix<double> *mat)
{
  // write an image of the disparity_matrix_

   // we want to write a disparity image

  // make a buffer which has the size of image1

  vcl_cout << "Writing file " << file_name << vcl_endl;

  vil1_memory_image_of<vxl_byte> buffer(get_width(),get_height());

  int x,y;
  int disparity;
  int value;

  for (x=0;x<buffer.width();x++){
    for (y=0;y<buffer.height();y++){
      buffer(x,y)=0;
    }
  }

  // go through each point, get the disparity and save it into the buffer

  int corr_range = vsrl_parameters::instance()->correlation_range;

  for (y=0;y<buffer.height();y++){
     for (x=0;x<buffer.width();x++){
       disparity = (int)((*mat)(x,y));
       value = disparity + corr_range+1;
       if (value < 0) {
         value = 0;
       }
       if (value>2*corr_range+1) {
         value=0;
       }
       // buffer(x,y)=value*10;
       buffer(x,y)=value;
     }
  }

  // save the file
  // vil1_save(buffer, filename, image1_.file_format());
  vil1_save(buffer, file_name);
}

void vsrl_diffusion::write_disparity_image(char *filename)
{
  write_image(filename,disparity_matrix_);
}
