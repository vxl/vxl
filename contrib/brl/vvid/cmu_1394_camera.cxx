#include <vil/vil_byte.h>
#include <vvid/cmu_1394_camera.h>
cmu_1394_camera::cmu_1394_camera()
  : cmu_1394_camera_params()
{
 int node = 0;
 if(!init(node))
   _link_failed = true;
   _running = false;
   _image_valid = false;
}
cmu_1394_camera::~cmu_1394_camera()
{
  if(_camera_present)
    C1394Camera::ResetLink(true);
}

cmu_1394_camera::cmu_1394_camera(int node, const cmu_1394_camera_params& cp)
  : cmu_1394_camera_params(cp)
{
  if(!init(node))
    _link_failed = true;

  //redundant?
  //  this->update_settings();
  _running = false;
  _image_valid = false;
}  
void cmu_1394_camera::update_settings()
{
   if(!_camera_present)
     return;
  cmu_1394_camera_params::constrain();
  C1394Camera::SetVideoFormat(_video_format);
  C1394Camera::SetVideoMode(_video_mode);
  C1394Camera::SetVideoFrameRate(_frame_rate);
  C1394Camera::SetBrightness(_brightness);
  C1394Camera::SetAutoExposure(_exposure);
  C1394Camera::SetGain(_gain);
}  
bool cmu_1394_camera::init(int node)
{
  if(C1394Camera::CheckLink())
    {
      vcl_cout << "In cmu_1394_camera::init() - link bad" << vcl_endl;
      _camera_present = false;
      return false;
    }
  C1394Camera::SelectCamera(node);
  this->update_settings();
  C1394Camera::InitCamera();
  _camera_present = true;
  _image_valid = false;
  return true;
}

void cmu_1394_camera::start()
{
  if(!_camera_present)
    {
      vcl_cout << "In cmu_1394_camera::start() -- no camera link" << vcl_endl;
      return;
    }

 if(!_running)
   {
     if(_capture)
       C1394Camera::StartImageCapture();
     else
       C1394Camera::StartImageAcquisition(); 
     if(this->get_frame())
       _image_valid = true;
     _running = true;
   }
}

void cmu_1394_camera::stop()
{
  if(!_camera_present)
    {
      vcl_cout << "In cmu_1394_camera::stop() -- no camera link" << vcl_endl;
      return;
    }
    
  if(!_running)
    {
      vcl_cout << "In cmu_1394_camera::stop() -- already stopped" << vcl_endl;
      return;
    }
  if(_capture)
    C1394Camera::StopImageCapture(); 
  else
    C1394Camera::StopImageAcquisition();
  _running = false;
  _image_valid = false;
}

bool cmu_1394_camera::get_frame()
{
  if(!_camera_present)
    {
      vcl_cout << "In cmu_1394_camera::get_frame() -- no camera link" 
               << vcl_endl;
      return false;
    }
    if (_running)
      {
        if(_capture)
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
  if(reread||!_image_valid)
    _image_valid = get_frame();
  
  if (_image_valid&&_running)
    {
      im.resize(xsize, ysize);
      unsigned char *p; 
      int size = C1394Camera::m_width*C1394Camera::m_height; 
      p = im.get_buffer(); 
      int offset = 0, yoffset, ooffset, oyoffset =0;
      for(int y=0, oy=0; y<C1394Camera::m_height;
          y+=pixel_sample_interval, oy++)
        {
          yoffset = C1394Camera::m_width*y;
          oyoffset = xsize*oy;
          for(int x = 0, ox=0; x<C1394Camera::m_width;
              x+=pixel_sample_interval, ox++)
            {
              offset = x+yoffset;
              ooffset = ox+ oyoffset;
              *(p + ooffset) = *(C1394Camera::m_pData + offset);
            }
        }
      return true;
    }
  else{
    vcl_cout << " cmu_1394_camera:get_monocrome_image -> couldn't get frame " 
             << vcl_endl;
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
  if(reread||!_image_valid)
    _image_valid = get_frame();
  
  if (_image_valid&&_running){
    temp.resize(C1394Camera::m_width, C1394Camera::m_height);
    C1394Camera::getRGB((unsigned char*)temp.get_buffer());
    im.resize(xsize, ysize);
    for(int y=0, yi=0; y<C1394Camera::m_height; y+=pixel_sample_interval, yi++)
        for(int x=0, xi=0; x<C1394Camera::m_width;  x+=pixel_sample_interval, xi++)
        im(xi, yi) = temp(x,y);
    return true;
  }
  else{
    vcl_cout << " cmu_1394_camera:get_rgb_image -> couldn't get frame " 
             << vcl_endl;
    return false;
  }
}

vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& cp)
{
  os << "camera_vendor: " << (char*)cp.m_nameVendor << vcl_endl;
  os << "camera_model: " << (char*)cp.m_nameModel << vcl_endl;
  os << "width: " << cp.m_width << vcl_endl;
  os << "height " << cp.m_height << vcl_endl;  
  os << "link_checked: " << cp.m_linkChecked << vcl_endl;
  os << "camera_initialized: " << cp.m_cameraInitialized << vcl_endl;
  return os;
}
