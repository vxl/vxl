// This is gel/vsrl/vsrl_diffusion.cxx
#include "vsrl_diffusion.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for std::sprintf()
#include <vil/vil_byte.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vsrl/vsrl_parameters.h>

vsrl_diffusion::vsrl_diffusion(vsrl_dense_matcher *matcher)
{
  _matcher = matcher;
  _width = matcher->get_width();
  _height = matcher->get_height();
  _disparity_matrix = new vnl_matrix<double>(_width,_height);
}

vsrl_diffusion::~vsrl_diffusion()
{
  delete _disparity_matrix;
}

int vsrl_diffusion::get_width()
{
  return _width;
}

int vsrl_diffusion::get_height()
{
  return _height;
}

double vsrl_diffusion::get_disparity(int x, int y)
{
  // we want to get the disparity for the point x, y

  if (x>=0 && x<_width && y>=0 && y < _height) {

    return (*_disparity_matrix)(x,y);
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
  // write an image of the _disparity_matrix

   // we want to write a disparity image

  // make a buffer which has the size of image1

  vcl_cout << "Writing file " << file_name << vcl_endl;

  vil_memory_image_of<vil_byte> buffer(_width,_height);

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
  // vil_save(buffer, filename, _image1.file_format());
  vil_save(buffer, file_name);
}

void vsrl_diffusion::write_disparity_image(char *filename)
{
  write_image(filename,_disparity_matrix);
}
