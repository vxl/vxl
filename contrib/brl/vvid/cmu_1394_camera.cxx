#include <vpro/vpro_capture_process.h>
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
void cmu_1394_camera::print_control(C1394CameraControl const& c)
{
  vcl_cout << "Feature Information \n";
  if(c.m_present)
    vcl_cout << "Feature Present true\n";
  else
    vcl_cout << "Feature Present false\n";

  if(c.m_onePush)
    vcl_cout << "One Push true\n";
  else
    vcl_cout << "One Push false\n";

  if(c.m_readout)
    vcl_cout << "Readout true\n";
  else
    vcl_cout << "Readout false\n";

  if(c.m_onoff)
    vcl_cout << "On-Off  true\n";
  else
    vcl_cout << "On-Off false\n";

  if(c.m_auto)
    vcl_cout << "Auto  true\n";
  else
    vcl_cout << "Auto false\n";

  if(c.m_manual)
    vcl_cout << "Manual  true\n";
  else
    vcl_cout << "Manual false\n";

  vcl_cout <<"Min Value " << c.m_min;
  vcl_cout <<"Max Value " << c.m_max;

  vcl_cout << "\nStatus Information \n";
  if(c.m_statusOnePush)
    vcl_cout << "OnePush  true\n";
  else
    vcl_cout << "OnePush false\n";

  if(c.m_statusOnOff)
    vcl_cout << "On-Off  true\n";
  else
    vcl_cout << "On-Off false\n";

  if(c.m_statusAutoManual)
    vcl_cout << "AutoManual  true\n";
  else
    vcl_cout << "AutoManual false\n";

  vcl_cout << "Value 1 " << c.m_value1 << "\n";
  vcl_cout << "Value 2 " << c.m_value2 << "\n";
}  

void cmu_1394_camera::print_status(C1394CameraControl const& c)
{
  vcl_cout << " Status \n";

  if(c.m_statusOnePush)
    vcl_cout << "OnePush  true | ";
  else
    vcl_cout << "OnePush false | ";

  if(c.m_statusOnOff)
    vcl_cout << "On-Off  true | ";
  else
    vcl_cout << "On-Off false | ";

  if(c.m_statusAutoManual)
    vcl_cout << "AutoMan  true\n";
  else
    vcl_cout << "AutoMan false\n";

  vcl_cout << "Value 1 " << c.m_value1 << "\n";
  vcl_cout << "Value 2 " << c.m_value2 << "\n";
}  

void cmu_1394_camera::update_settings()
{
   if (!camera_present_)
     return;
  cmu_1394_camera_params::constrain();
  C1394Camera::m_controlAutoExposure.SetAutoMode(false);
  C1394Camera::SetShutter(shutter_);
  C1394Camera::SetVideoFormat(video_format_);
  C1394Camera::SetVideoMode(video_mode_);
  C1394Camera::SetVideoFrameRate(frame_rate_);
  C1394Camera::SetBrightness(brightness_);
  C1394Camera::SetGain(gain_);
  C1394Camera::StatusControlRegisters();
  vcl_cout << "AutoExposure";
  this->print_status(m_controlAutoExposure);
  vcl_cout << "Shutter ";
  this->print_status(m_controlShutter);
  vcl_cout << "Gain ";
  this->print_status(m_controlGain);
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
  vp_ = 0;
  file_capture_ = false;
  capture_ = false;
  return true;
}

bool cmu_1394_camera::start()
{
  if (!camera_present_)
  {
    vcl_cout << "In cmu_1394_camera::start() -- no camera link\n";
    return false;
  }

 if (!running_)
 {
   if (capture_)
     {
       if(C1394Camera::StartImageCapture())
       {
         vcl_cout<< "In cmu_1394_camera::start() - Problem Starting Capture\n";
         return false;
       }
     }
   else
    if(C1394Camera::StartImageAcquisition())
       {
         vcl_cout<< "In cmu_1394_camera::start() - Problem Starting Aquisition\n";
         return false;
       }
   if (this->get_frame())
     image_valid_ = true;
   running_ = true;
   return true;
 }
 return true;
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
void cmu_1394_camera::start_capture(vcl_string const & video_file_name)
{
  vp_ = new vpro_capture_process(video_file_name);
  file_capture_ = true;
}

bool
cmu_1394_camera::get_monochrome_image(vil1_memory_image_of<unsigned char>& im,
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
get_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im,
              int pixel_sample_interval, bool reread)
{
  int xsize = C1394Camera::m_width/pixel_sample_interval,
    ysize = C1394Camera::m_height/pixel_sample_interval;
  vil1_memory_image_of< vil1_rgb<unsigned char> > temp;

  //potentially reread the frame
  if (reread||!image_valid_)
    image_valid_ = get_frame();

  if (image_valid_&&running_){
    temp.resize(C1394Camera::m_width, C1394Camera::m_height);
    C1394Camera::getRGB((unsigned char*)temp.get_buffer());
    //cache the frame for live video capture
    if(file_capture_)
       {
        vp_->clear_input();
        vp_->add_input_image(temp);
        if(!vp_->execute())
          {
            vcl_cout << "In cmu_1394_camera::get_rbg_image(..) - capture failed\n";
            file_capture_ = false;
         }
     }
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
bool cmu_1394_camera::stop_capture()
{
  if(!file_capture_||!vp_)
    return false;
  file_capture_ = false;
  return vp_->finish();
}

vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& cp)
{
  os << "camera_vendor: " << (char*)cp.m_nameVendor << vcl_endl;
  os << "camera_model: " << (char*)cp.m_nameModel << vcl_endl;
  os << "width: " << cp.m_width << vcl_endl;
  os << "height " << cp.m_height << vcl_endl;
  os << "supported capabilities " << vcl_endl;
  for(int i = 0; i<3; i++)
    for(int j = 0; j<8; j++)
      for(int k = 0; k<6; k++)
        if(cp.m_videoFlags[i][j][k])
          vcl_cout << cp.video_configuration(i,j) << " Fr/Sec(" 
                   << cp.frame_rate(k) << ")\n";
  os << "link_checked: " << cp.m_linkChecked << vcl_endl;
  os << "camera_initialized: " << cp.m_cameraInitialized << vcl_endl;
  return os;
}
