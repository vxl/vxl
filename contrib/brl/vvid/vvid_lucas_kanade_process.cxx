#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vil/vil_rgb.h>
#include <brip/brip_float_ops.h>
#include <vvid/vvid_lucas_kanade_process.h>

vvid_lucas_kanade_process::vvid_lucas_kanade_process()
{
  state_ = NO_IMAGE;
}

vvid_lucas_kanade_process::~vvid_lucas_kanade_process()
{
}

void vvid_lucas_kanade_process::
compute_lucas_kanade(vil_memory_image_of<float>& image)
{
  int w = image.width(), h = image.height();
  vil_memory_image_of<float> vx, vy;
  vx.resize(w,h);
  vy.resize(w,h);
  //Hard code these parameters for now.
  int n = 2;
  int thresh = 20000;
  vil_memory_image_of<float> prev(queue_[0]);
  brip_float_ops::Lucas_KanadeMotion(image, prev, n, thresh, vx, vy);
  vil_memory_image_of< vil_rgb<unsigned char> > output;
  output.resize(w,h);
  vil_rgb<unsigned char> z(0,0,0);
  for(int y = 0; y<h; y++)
    for(int x = 0; x<w; x++)
      {
        double  fx = vx(x,y), fy = vy(x,y);
        if(!fx&&!fy)
          {
            output(x,y)= z;
            continue;
          }
        double ang = atan2(fy, fx);
        double red = 63*cos(ang), grn = 63*sin(ang);
        unsigned char  r = (unsigned char)(red+127);
        unsigned char  g = (unsigned char)(grn+127);
        vil_rgb<unsigned char> v(r,g, 127);
        output(x,y) = v;
      }
  output_image_ = output;
}

void vvid_lucas_kanade_process::update_queue(vil_image image)
{
  queue_[0]=queue_[1];
  queue_[1]=image;
}

bool vvid_lucas_kanade_process::execute()
{
  if (!this->get_N_inputs()==1)
    {
      vcl_cout << "In vvid_lucas_kanade_process::execute() - not at exactly one"
               << " input image \n";
      return false;
    }
  vil_image img = vvid_video_process::get_input_image(0);
  vil_memory_image_of<unsigned char> temp(img);
  vil_memory_image_of<float> fimg = brip_float_ops::convert_to_float(temp);
  vil_memory_image_of<float> fsmooth = brip_float_ops::gaussian(fimg, 0.8f);
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
      vcl_cout << "In vvid_lucas_kanade_process::execute() - shouldn't happen\n";
      return false;
    }
  return true;
}

bool vvid_lucas_kanade_process::finish()
{
  queue_.clear();
  state_=NO_IMAGE;
  return true;
}
