#include <vvid/cmu_1394_camera.h>

cmu_1394_camera::cmu_1394_camera()
  : cmu_1394_camera_params()
{
 int node = 0;
 if (!init(node))
   link_failed_ = true;
   running_ = false;
   image_valid_ = false;
}
cmu_1394_camera::~cmu_1394_camera()
{
  if (camera_present_)
    C1394Camera::ResetLink(true);
}

cmu_1394_camera::cmu_1394_camera(int node, const cmu_1394_camera_params& cp)
  : cmu_1394_camera_params(cp)
{
  if (!init(node))
    link_failed_ = true;

  //redundant?
  //  this->update_settings();
  running_ = false;
  image_valid_ = false;
}

void cmu_1394_camera::update_settings()
{
   if (!camera_present_)
     return;
  cmu_1394_camera_params::constrain();
  C1394Camera::SetVideoFormat(video_format_);
  C1394Camera::SetVideoMode(video_mode_);
  C1394Camera::SetVideoFrameRate(frame_rate_);
  C1394Camera::SetBrightness(brightness_);
  C1394Camera::SetAutoExposure(exposure_);
  C1394Camera::SetGain(gain_);
}

bool cmu_1394_camera::init(int node)
{
  if (C1394Camera::CheckLink())
  {
    vcl_cout << "In cmu_1394_camera::init() - link bad\n";
    camera_present_ = false;
    return false;
  }
  C1394Camera::SelectCamera(node);
  this->update_settings();
  C1394Camera::InitCamera();
  camera_present_ = true;
  image_valid_ = false;
  return true;
}

void cmu_1394_camera::start()
{
  if (!camera_present_)
  {
    vcl_cout << "In cmu_1394_camera::start() -- no camera link\n";
    return;
  }

 if (!running_)
 {
   if (capture_)
     C1394Camera::StartImageCapture();
   else
     C1394Camera::StartImageAcquisition();
   if (this->get_frame())
     image_valid_ = true;
   running_ = true;
 }
}

void cmu_1394_camera::stop()
{
  if (!camera_present_)
    {
      vcl_cout << "In cmu_1394_camera::stop() -- no camera link\n";
      return;
    }

  if (!running_)
    {
      vcl_cout << "In cmu_1394_camera::stop() -- already stopped\n";
      return;
    }
  if (capture_)
    C1394Camera::StopImageCapture();
  else
    C1394Camera::StopImageAcquisition();
  running_ = false;
  image_valid_ = false;
}

bool cmu_1394_camera::get_frame()
{
  if (!camera_present_)
    {
      vcl_cout << "In cmu_1394_camera::get_frame() -- no camera link\n";
      return false;
    }
    if (running_)
      {
        if (capture_)
          C1394Camera::CaptureImage();
        else
          C1394Camera::AcquireImage();
        return true;
      }
    else
      return false;
}

bool
cmu_1394_camera::get_monochrome_image(vil_memory_image_of<unsigned char>& im,
                                      int pixel_sample_interval, bool reread)

{
  int xsize = C1394Camera::m_width/pixel_sample_interval,
      ysize = C1394Camera::m_height/pixel_sample_interval;

  //potentially reread the frame
  if (reread||!image_valid_)
    image_valid_ = get_frame();

  if (image_valid_&&running_)
    {
      im.resize(xsize, ysize);
      unsigned char *p;
      int size = C1394Camera::m_width*C1394Camera::m_height;
      p = im.get_buffer();
      int offset = 0, yoffset, ooffset, oyoffset =0;
      for (int y=0, oy=0; y<C1394Camera::m_height;
          y+=pixel_sample_interval, oy++)
        {
          yoffset = C1394Camera::m_width*y;
          oyoffset = xsize*oy;
          for (int x = 0, ox=0; x<C1394Camera::m_width;
              x+=pixel_sample_interval, ox++)
            {
              offset = x+yoffset;
              ooffset = ox+ oyoffset;
              *(p + ooffset) = *(C1394Camera::m_pData + offset);
            }
        }
      return true;
    }
  else {
    vcl_cout << " cmu_1394_camera:get_monocrome_image -> couldn't get frame\n";
    return false;
  }
}

bool cmu_1394_camera::
get_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
              int pixel_sample_interval, bool reread)
{
  int xsize = C1394Camera::m_width/pixel_sample_interval,
    ysize = C1394Camera::m_height/pixel_sample_interval;
  vil_memory_image_of< vil_rgb<unsigned char> > temp;

  //potentially reread the frame
  if (reread||!image_valid_)
    image_valid_ = get_frame();

  if (image_valid_&&running_){
    temp.resize(C1394Camera::m_width, C1394Camera::m_height);
    C1394Camera::getRGB((unsigned char*)temp.get_buffer());
    im.resize(xsize, ysize);
    for (int y=0, yi=0; y<C1394Camera::m_height; y+=pixel_sample_interval, yi++)
        for (int x=0, xi=0; x<C1394Camera::m_width;  x+=pixel_sample_interval, xi++)
        im(xi, yi) = temp(x,y);
    return true;
  }
  else {
    vcl_cout << " cmu_1394_camera:get_rgb_image -> couldn't get frame\n";
    return false;
  }
}

vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& cp)
{
  os << "camera_vendor: " << (char*)cp.m_nameVendor << vcl_endl
     << "camera_model: " << (char*)cp.m_nameModel << vcl_endl
     << "width: " << cp.m_width << vcl_endl
     << "height " << cp.m_height << vcl_endl
     << "link_checked: " << cp.m_linkChecked << vcl_endl
     << "camera_initialized: " << cp.m_cameraInitialized << vcl_endl;
  return os;
}
