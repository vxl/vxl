//:
// \file
// \brief A class for reading video files on windows platform
// \author Louise Butcher

#include <mvl2/mvl2_video_from_avi_windows.h>

mvl2_video_from_avi::mvl2_video_from_avi()
{
  is_initialized_=false;
  current_frame_=0;
}

mvl2_video_from_avi::~mvl2_video_from_avi()
{
}

vcl_string mvl2_video_from_avi::is_a() const
{
  return vcl_string("mvl2_video_from_avi");
}

mvl2_video_reader* mvl2_video_from_avi::clone() const
{
  return new mvl2_video_from_avi(*this);
}

// possible options : Grey
bool mvl2_video_from_avi::initialize( int width, int height, 
                                     vcl_string format, vcl_string file_name)
{
  if (!format.find(vcl_string("Grey"))) use_colour_=false;
  AVIFileInit();
  ppavi_=0;
  LPCTSTR szFile;
  szFile=(LPCTSTR)file_name.c_str();
  DWORD fccType=streamtypeVIDEO;
  LONG lParam=0;
  UINT mode=OF_READ;             
  CLSID * pclsidHandler=NULL;  
  bool success;
  current_frame_=0;
  success=!(AVIStreamOpenFromFile(&ppavi_,szFile,fccType,lParam,mode,pclsidHandler));
  if (success)
  {
    is_initialized_=true;
  }
  else
  {
    is_initialized_=false;
    AVIFileExit();
  }
  return success;
}

void mvl2_video_from_avi::uninitialize()
{
  AVIStreamRelease(ppavi_);
  AVIFileExit();
  is_initialized_=false;
}

int mvl2_video_from_avi::next_frame()
{
  if (!is_initialized_) return -1;
  
  return ++current_frame_;
}

bool mvl2_video_from_avi::get_frame(vimt_image_2d_of<vxl_byte>& image)
{
  
  if (!is_initialized_) return false;
  BITMAPINFO bmp_info;
  BITMAPINFO* img;
	 bool use_colour;
   
   getVideoFormat(bmp_info);
   PGETFRAME g_frame=AVIStreamGetFrameOpen(ppavi_,NULL);
   LPVOID img_data=AVIStreamGetFrame(g_frame,current_frame_);
   if (!img_data) return false;
   img=(BITMAPINFO* )img_data;
   
   int nx = bmp_info.bmiHeader.biWidth;
   int ny = bmp_info.bmiHeader.biHeight;
   int bpp= bmp_info.bmiHeader.biBitCount;
   int bplanes= bmp_info.bmiHeader.biBitCount;
   
   if (bplanes)
     use_colour=false;
   else
     use_colour=true;
   
   int ystep=(nx*(bpp/8) + 3)& -4;
   int xstep=bpp/8;
   
   unsigned char* data=(unsigned char*)(img_data)+img->bmiHeader.biSize+img->bmiHeader.biClrUsed*sizeof(RGBQUAD);
   vimt_image_2d_of<vxl_byte> temp_img;
   if (use_colour)
     temp_img.image().set_to_memory(data,nx,ny,bplanes,xstep,ystep,1);
   else
     temp_img.image().set_to_memory(data,nx,ny,bplanes,xstep,ystep,1);
   
   image.deep_copy(temp_img);
   
   AVIStreamGetFrameClose(g_frame);
   return true;
}

void mvl2_video_from_avi::reset_frame()
{
  current_frame_=0;
}

void mvl2_video_from_avi::set_frame_rate(double frame_rate)
{
}

double mvl2_video_from_avi::get_frame_rate()
{
 	AVISTREAMINFO avis; 
  AVIStreamInfo(ppavi_, &avis, sizeof(avis));
  frame_rate_=(avis.dwRate)/(avis.dwScale);
  return frame_rate_;
}

int mvl2_video_from_avi::get_width()
{
  BITMAPINFO bmp_info;
	 getVideoFormat(bmp_info);
   width_=bmp_info.bmiHeader.biWidth;
   return width_;
}

int mvl2_video_from_avi::get_height()
{
  BITMAPINFO bmp_info;
	 getVideoFormat(bmp_info);
   width_=bmp_info.bmiHeader.biHeight;
   return height_;
}

void mvl2_video_from_avi::set_capture_size(int width,int height)
{
} 

void mvl2_video_from_avi::getVideoFormat(BITMAPINFO& bmp_info )
{
  BITMAPINFO* vfmt=0;
  HRESULT hr;
  long lStreamSize;
  AVIStreamFormatSize(ppavi_, 0, &lStreamSize); 
  
  vfmt = (LPBITMAPINFO)LocalAlloc(LPTR, lStreamSize);
  
  hr = AVIStreamReadFormat(ppavi_, 0, vfmt, &lStreamSize); // Read format
  bmp_info=*vfmt;
  LocalFree(vfmt);
  return;
}
