#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_motion_process.h>

vpro_motion_process::vpro_motion_process(vpro_motion_params& vmp) : vpro_motion_params(vmp)
{
  state_ = NO_IMAGE;
}

vpro_motion_process::~vpro_motion_process()
{
}

void vpro_motion_process::compute_motion(vil1_image ix, vil1_image iy)
{
  //ix contains the current image not the gradient (maybe later)
  vil1_memory_image_of<float> fimg(ix);
  //Get sqrt (sigma0*sigma1) of the gradient matrix on a 3x3 neighborhood
  int n = 1;
  vil1_memory_image_of<float> sing =
    brip_vil1_float_ops::sqrt_grad_singular_values(fimg, n);
  //Get the time derivative
  vil1_memory_image_of<float> Im(queuex_[1]);
  vil1_memory_image_of<float> It = brip_vil1_float_ops::difference(Im, fimg);
  //form the motion image
  int w = fimg.width(), h = fimg.height();
  vil1_memory_image_of<float> out;
  out.resize(w,h);
  for (int y = 0; y<h; y++)
    for (int x = 0; x<w; x++)
      out(x,y) = vcl_fabs(It(x,y))*sing(x,y);
  output_image_ = brip_vil1_float_ops::convert_to_byte(out, low_range_, high_range_);
}

void vpro_motion_process::update_queue(vil1_image ix, vil1_image iy)
{
  queuex_[0]=queuex_[1];
  //  queuey_[0]=queuey_[1];
  queuex_[1] = ix;
  //  queuey_[1] = iy;
}

bool vpro_motion_process::execute()
{
  if (!this->get_N_input_images()==1)
    {
      vcl_cout << "In vpro_motion_process::execute() - not at exactly one"
               << " input image \n";
      return false;
    }
  vil1_image img = vpro_video_process::get_input_image(0);
  vil1_memory_image_of<float> fimg = brip_vil1_float_ops::convert_to_float(img);
  vil1_memory_image_of<float> fsmooth = brip_vil1_float_ops::gaussian(fimg, smooth_sigma_);
 // vil1_memory_image_of<float> fx = brip_vil1_float_ops::dx(fsmooth);
 // vil1_memory_image_of<float> fy = brip_vil1_float_ops::dx(fsmooth);
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
      vcl_cout << "In vpro_motion_process::execute() - shouldn't happen\n";
      return false;
    }
  return true;
}

bool vpro_motion_process::finish()
{
  queuex_.clear();
  queuey_.clear();
  state_=NO_IMAGE;
  return true;
}
