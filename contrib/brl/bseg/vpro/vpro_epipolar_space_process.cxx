#include "vpro_epipolar_space_process.h"
//:
// \file
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

vpro_epipolar_space_process::vpro_epipolar_space_process()
{
}

vpro_epipolar_space_process::~vpro_epipolar_space_process()
{
}

bool vpro_epipolar_space_process::execute()
{
  if (this->get_N_input_images() != 2)
  {
    vcl_cout << "In vpro_epipolar_space_process::execute() - not 2 input images\n";
    return false;
  }
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> im0(vpro_video_process::get_input_image(0));
  vil1_memory_image_of<unsigned char> im1(vpro_video_process::get_input_image(1));
  vil1_memory_image_of<vil1_rgb<unsigned char> > im;
  unsigned width = im0.width(), height = im0.height();
  im.resize(width, width);
  vil1_rgb<unsigned char> white(255,255,255);
  int h2 = height/2, d = 10;
  for (unsigned int x1 = 0; x1<width; x1++)
    for (unsigned int x2 = 0; x2<width; x2++)
    {
      float p0=0.0, p1=0.0, n=0.0;
      for (int y = h2-d; y<h2+d; y++, n++)
      {
        p0 += im0(x1,y);
        p1 += im1(x2,y);
      }
      p0/=n; p1/=n;
      vil1_rgb<unsigned char> cp((unsigned char)p0, (unsigned char)p1, 0);
      //draw white lines of constant disparity
      if (x1==x2||x1==x2+150)
        im(x1,x2)=white;
      else
        im(x1,x2)=cp;
    }
  output_image_ = im;
  return true;
}

