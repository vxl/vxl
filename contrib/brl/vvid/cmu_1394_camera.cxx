//:
// \file
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
   current_ = -1;
}

cmu_1394_camera::~cmu_1394_camera()
{}

cmu_1394_camera::cmu_1394_camera(int node, const cmu_1394_camera_params& cp)
  : cmu_1394_camera_params(cp)
{
  if (!init(node))
    link_failed_ = true;

  //redundant?
  this->update_settings();
  running_ = false;
  image_valid_ = false;
   current_ = -1;
}

void cmu_1394_camera::print_control(C1394CameraControl const& c)
{
  vcl_cout << "Feature Information\n";

  if (c.m_present)
    vcl_cout << "Feature Present true\n";
  else
    vcl_cout << "Feature Present false\n";

  if (c.m_onePush)
    vcl_cout << "One Push true\n";
  else
    vcl_cout << "One Push false\n";

  if (c.m_readout)
    vcl_cout << "Readout true\n";
  else
    vcl_cout << "Readout false\n";

  if (c.m_onoff)
    vcl_cout << "On-Off  true\n";
  else
    vcl_cout << "On-Off false\n";

  if (c.m_auto)
    vcl_cout << "Auto  true\n";
  else
    vcl_cout << "Auto false\n";

  if (c.m_manual)
    vcl_cout << "Manual  true\n";
  else
    vcl_cout << "Manual false\n";

  vcl_cout <<"Min Value " << c.m_min
           <<"Max Value " << c.m_max;
}

void cmu_1394_camera::print_status(C1394CameraControl const& c)
{
  vcl_cout << " Status\n";

  if (c.m_statusOnePush)
    vcl_cout << "OnePush  true | ";
  else
    vcl_cout << "OnePush false | ";

  if (c.m_statusOnOff)
    vcl_cout << "On-Off  true | ";
  else
    vcl_cout << "On-Off false | ";

  if (c.m_statusAutoManual)
    vcl_cout << "AutoMan  true\n";
  else
    vcl_cout << "AutoMan false\n";

  vcl_cout << "Value 1 " << c.m_value1 << '\n'
           << "Value 2 " << c.m_value2 << '\n';
}

//----------------------------------------------------------------
// : update video configuration to current setting
//
void cmu_1394_camera::update_video_configuration()
{
  if (current_<0||!format_.size())
    return;
  video_format_ = format_[current_];
  video_mode_ = mode_[current_];
  frame_rate_ = rate_[current_];
}

//----------------------------------------------------------------------
//: is the camera resolution, frame rate and color sampling setting valid?
//  if so set the current index to point to the setting
//  if the suggested configuration is invalid then keep the original
//  setting.  If this is the first validation then set the index to 0,
//  i.e., the first valid configuration
//
void cmu_1394_camera::validate_default_configuration()
{
  int n = format_.size();
  if (!n)
    {
      vcl_cout << "In cmu_1394_camera::validate_configuration() -"
               << " no capabilities (shouldn't happen)\n";
      return;
    }
  bool valid = false;
  for (int i = 0; i<n&&!valid; i++)
    {
      if (video_format_==format_[i]&&
          video_mode_ == mode_[i]&&
          frame_rate_ == rate_[i])
        {
          current_ = i;
          valid = true;
        }
    }
  if (valid)
    return;
  if (current_<0)
    current_ = 0;
}

void cmu_1394_camera::update_settings()
{
   if (!camera_present_)
     return;
   this->update_video_configuration();
   cmu_1394_camera_params::constrain();
  //Shutter control
  if (!auto_exposure_)
    {
      C1394Camera::m_controlAutoExposure.TurnOn(false);
      C1394Camera::m_controlShutter.TurnOn(true);
    }
  else
    {
      C1394Camera::m_controlAutoExposure.TurnOn(true);
      C1394Camera::m_controlShutter.TurnOn(false);
    }
  if (!autowhitebalance_)
   {
      C1394Camera::m_controlWhiteBalance.SetAutoMode(false);
      C1394Camera::SetWhiteBalance(whitebalanceU_, whitebalanceV_);
   }
  else
  {
    C1394Camera::m_controlWhiteBalance.SetAutoMode(true);
  }
  if (onepushWBbalance_)
    {
        C1394Camera::m_controlWhiteBalance.SetAutoMode(false);      
        C1394Camera::m_controlWhiteBalance.SetOnePush();
    }
   if (!auto_gain_)
    {
      C1394Camera::m_controlGain.SetAutoMode(false);
      C1394Camera::m_controlGain.TurnOn(true);
    }
  C1394Camera::SetVideoFormat(video_format_);
  C1394Camera::SetVideoMode(video_mode_);
  C1394Camera::SetVideoFrameRate(frame_rate_);
  C1394Camera::SetShutter(shutter_);
  C1394Camera::SetBrightness(brightness_);
  if (auto_exposure_)
    C1394Camera::SetAutoExposure(exposure_);
  if (!auto_gain_)
    C1394Camera::SetGain(gain_);



}

//-----------------------------------------------------------------
//: find the standard 1394 capabilities of this camera
//
void cmu_1394_camera::init_capabilities()
{
  format_.clear();
  mode_.clear();
  rate_.clear();
  capability_desc_.clear();
  for (int i = 0; i<3; i++)
    for (int j = 0; j<6; j++)
      for (int k = 0; k<6; k++)
        if ((*this).m_videoFlags[i][j][k])
          {
            vcl_string temp = this->video_configuration(i,j);
            temp += " Fr/Sec(";
            temp += this->frame_rate(k);
            temp += ")";
            capability_desc_.push_back(temp);
            format_.push_back(i);
            mode_.push_back(j);
            rate_.push_back(k);
          }
}

//-----------------------------------------------------------------
//: Establish the control variables for the camera
//
void cmu_1394_camera::init_control()
{
  //get the values from the camera
  C1394Camera::InquireControlRegisters();
  //Electronic shutter control
  manual_shutter_control_ = C1394Camera::m_controlShutter.m_present;
  min_shutter_ = C1394Camera::m_controlShutter.m_min;
  max_shutter_ = C1394Camera::m_controlShutter.m_max;
  shutter_ = int((max_shutter_-min_shutter_)/2.0) + min_shutter_;
  //Gain control
  min_gain_ = C1394Camera::m_controlGain.m_min;
  max_gain_ = C1394Camera::m_controlGain.m_max;
  gain_ = int((max_gain_-min_gain_)/2.0) + min_gain_;

  //Brightness control (make this minimum by default since we don't usually
  // want an offset);
  min_brightness_ = C1394Camera::m_controlBrightness.m_min;
  max_brightness_ = C1394Camera::m_controlBrightness.m_max;
  brightness_ = min_brightness_;

  min_WB_=C1394Camera::m_controlWhiteBalance.m_min;
  max_WB_=C1394Camera::m_controlWhiteBalance.m_max;
  //Auto exposure
  auto_exposure_control_ = C1394Camera::m_controlAutoExposure.m_present;
  min_exposure_ = C1394Camera::m_controlAutoExposure.m_min;
  max_exposure_ = C1394Camera::m_controlAutoExposure.m_max;
  exposure_ = int((max_exposure_-min_exposure_)/2.0) + min_exposure_;
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
  C1394Camera::InitCamera();
  this->init_capabilities();
  this->init_control();
  this->validate_default_configuration();
  this->update_settings();
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
       if (C1394Camera::StartImageCapture())
       {
         vcl_cout<< "In cmu_1394_camera::start() - Problem Starting Capture\n";
         return false;
       }
     }
   else
    if (C1394Camera::StartImageAcquisition())
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
    if (file_capture_)
       {
        vp_->clear_input();
        vp_->add_input_image(temp);
        if (!vp_->execute())
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
  if (!file_capture_||!vp_)
    return false;
  file_capture_ = false;
  return vp_->finish();
}

vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& cp)
{
  os << "camera_vendor: " << (char*)cp.m_nameVendor << '\n'
     << "camera_model: " << (char*)cp.m_nameModel << '\n'
     << "width: " << cp.m_width << '\n'
     << "height " << cp.m_height << '\n'
     << "supported capabilities\n";
  for (vcl_vector<vcl_string>::const_iterator cit = cp.capability_desc_.begin();
       cit != cp.capability_desc_.end(); cit++)
    os << "  " << (*cit) << '\n';
  os << "link_checked: " << cp.m_linkChecked << '\n'
     << "camera_initialized: " << cp.m_cameraInitialized << '\n';
  return os;
}
