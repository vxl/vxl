#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil1/vil1_rgb.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_lucas_kanade_process.h>

vpro_lucas_kanade_process::vpro_lucas_kanade_process(bool down_sample,
                                                     int window_size,
                                                     double thresh)
{
  downsample_ = down_sample;
  window_size_ = window_size;
  thresh_ = thresh;
  state_ = NO_IMAGE;
}

vpro_lucas_kanade_process::~vpro_lucas_kanade_process()
{
}

void vpro_lucas_kanade_process::
compute_lucas_kanade(vil1_memory_image_of<float>& image)
{
  int w = image.width(), h = image.height();
  vil1_memory_image_of<float> vx, vy;
  vx.resize(w,h);
  vy.resize(w,h);

  vil1_memory_image_of<float> prev(queue_[0]);
  brip_vil1_float_ops::Lucas_KanadeMotion(image, prev, window_size_, thresh_,
                                     vx, vy);
  vil1_memory_image_of< vil1_rgb<unsigned char> > output;
  output.resize(w,h);
  vil1_rgb<unsigned char> z(0,0,0);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      {
        double  fx = vx(x,y), fy = vy(x,y);
        if (!fx&&!fy)
          {
            output(x,y)= z;
            continue;
          }
        double ang = vcl_atan2(fy, fx);
        double red = 63*vcl_cos(ang), grn = 63*vcl_sin(ang);
        unsigned char  r = (unsigned char)(red+127);
        unsigned char  g = (unsigned char)(grn+127);
        vil1_rgb<unsigned char> v(r,g, 127);
        output(x,y) = v;
      }
  output_image_ = output;
}

void vpro_lucas_kanade_process::update_queue(vil1_image image)
{
  queue_[0]=queue_[1];
  queue_[1]=image;
}

bool vpro_lucas_kanade_process::execute()
{
  if (!this->get_N_input_images()==1)
    {
      vcl_cout << "In vpro_lucas_kanade_process::execute() -"
               << " not at exactly one input image \n";
      return false;
    }
  vil1_image img = vpro_video_process::get_input_image(0);
  vil1_memory_image_of<float> fimg = brip_vil1_float_ops::convert_to_float(img);
  vil1_memory_image_of<float> temp2;
  if (downsample_)
    temp2 = brip_vil1_float_ops::half_resolution(fimg);
  else
    temp2 = fimg;
  vil1_memory_image_of<float> fsmooth = brip_vil1_float_ops::gaussian(temp2, 0.8f);
  this->clear_input();
  switch(state_)
    {
    case NO_IMAGE:
      queue_.push_back(fsmooth);
      state_ = FIRST_IMAGE;
      break;
    case FIRST_IMAGE:
      queue_.push_back(fsmooth);
      state_ = IN_PROCESS;
      break;
    case IN_PROCESS:
      this->compute_lucas_kanade(fsmooth);
      this->update_queue(fsmooth);
      state_ = IN_PROCESS;
      break;
    default:
      vcl_cout << "In vpro_lucas_kanade_process::execute() - shouldn't happen\n";
      return false;
    }
  return true;
}

bool vpro_lucas_kanade_process::finish()
{
  queue_.clear();
  state_=NO_IMAGE;
  return true;
}
