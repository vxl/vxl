#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <brip/brip_float_ops.h>
#include <vvid/vvid_motion_process.h>

vvid_motion_process::vvid_motion_process()
{
  state_ = NO_IMAGE;
}

vvid_motion_process::~vvid_motion_process()
{
}

void vvid_motion_process::compute_motion(vil_image ix, vil_image iy)
{
  //ix contains the current image not the gradient (maybe later)
  vil_memory_image_of<float> fimg(ix);

  //Get sqrt (sigma0*sigma1) of the gradient matrix on a 3x3 neighborhood
  int n = 1;
  vil_memory_image_of<float> sing = 
    brip_float_ops::sqrt_grad_singular_values(fimg, n);
  //Get the time derivative
  vil_memory_image_of<float> Im(queuex_[1]);
  vil_memory_image_of<float> It = brip_float_ops::difference(Im, fimg);
  //form the motion image
  int w = fimg.width(), h = fimg.height();
  vil_memory_image_of<float> out;
  out.resize(w,h);
  for(int y = 0; y<h; y++)
    for(int x = 0; x<w; x++)
      out(x,y) = fabs(It(x,y))*sing(x,y);
  output_image_ = brip_float_ops::convert_to_byte(out, 0, 10000.0);
}

void vvid_motion_process::update_queue(vil_image ix, vil_image iy)
{
  queuex_[0]=queuex_[1];
  //  queuey_[0]=queuey_[1];
  queuex_[1] = ix;
  //  queuey_[1] = iy;
}

bool vvid_motion_process::execute()
{
  if (!this->get_N_inputs()==1)
    {
      vcl_cout << "In vvid_motion_process::execute() - not at exactly one"
               << " input image \n";
      return false;
    }
  vil_image img = vvid_video_process::get_input_image(0);
  vil_memory_image_of<unsigned char> temp(img);
  vil_memory_image_of<float> fimg = brip_float_ops::convert_to_float(temp);
  vil_memory_image_of<float> fsmooth = brip_float_ops::gaussian(fimg, 1.0);
 // vil_memory_image_of<float> fx = brip_float_ops::dx(fsmooth);
 // vil_memory_image_of<float> fy = brip_float_ops::dx(fsmooth);
  this->clear_input();
  switch(state_)  
    {
    case NO_IMAGE:
      queuex_.push_back(fsmooth);
      //      queuey_->push_back(fy);
      state_ = FIRST_IMAGE;
      break;
    case FIRST_IMAGE:
      queuex_.push_back(fsmooth);
      //      queuey_->push_back(fy);
      state_ = IN_PROCESS;
      break;
    case IN_PROCESS:
      this->compute_motion(fsmooth, fsmooth);
      this->update_queue(fsmooth, fsmooth);
      state_ = IN_PROCESS;
      break;
    default:
      vcl_cout << "In vvid_motion_process::execute() - shouldn't happen\n";
      return false;
    }
  return true;
}

bool vvid_motion_process::finish()
{
  queuex_.clear();
  queuey_.clear();
  state_=NO_IMAGE;
  return true;
}
