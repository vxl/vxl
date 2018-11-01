//:
// \file
// \brief A class for writing videos
// \author Franck Bettinger

#include <iostream>
#include <cstring>
#include "mvl2_video_to_avi_linux.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_resample_bilin.h>

mvl2_video_to_avi::mvl2_video_to_avi()
{
  is_initialized_=false;
  current_frame_=0;
  fourcc_=fccDIVX;
  frame_rate_=25.0;
  quality_=10000;
  bgr_=false;
  upside_down_=true;
}

mvl2_video_to_avi::~mvl2_video_to_avi()
{
  close();
}

bool mvl2_video_to_avi::set_codec(char a, char b, char c, char d)
{
  // TO DO : verify if the codec is supported for video writing.
  fourcc_=mmioFOURCC(a,b,c,d);

  // hack to get correct color with mjpg codec
  if (fourcc_==fccmjpg)
  {
    bgr_=true;
  }
  else
  {
    bgr_=false;
  }

  // hack to get correct orientation with DIVX codec
  if (fourcc_==fccDIVX)
  {
    upside_down_=false;
  }
  else
  {
    upside_down_=true;
  }

  return true;
}

void mvl2_video_to_avi::set_quality(int qual)
{
  quality_=qual;
}

bool mvl2_video_to_avi::open( int width, int height,
                              std::string format, std::string file_name)
{
  if (is_initialized_) return false;

  width_=width%2?width+1:width;
  height_=height;

  moviefile_ = CreateIAviWriteFile(file_name.c_str());

  BitmapInfo bh(width_,height_,24);
  moviestream_ = moviefile_->AddVideoStream(
            fourcc_, &bh,
            (int)(1000000.0/frame_rate_));

  moviestream_->SetQuality( quality_ );
  moviestream_->SetKeyFrame(1);
  moviestream_->Start();

  is_initialized_=true;

  return is_initialized_;
}

void mvl2_video_to_avi::close()
{
  if (!is_initialized_) return;

  moviestream_->Stop();
  delete moviefile_;
  is_initialized_=false;
}

int mvl2_video_to_avi::get_width() const
{
  return width_;
}

int mvl2_video_to_avi::get_height() const
{
  return height_;
}

void mvl2_video_to_avi::set_frame_rate(double frame_rate)
{
  frame_rate_=frame_rate;
}

void mvl2_video_to_avi::write_frame(vil_image_view<vxl_byte>& image)
{
  if (width_==(int)image.ni() && height_==(int)image.nj() &&
      image.pixel_format()==VIL_PIXEL_FORMAT_RGB_BYTE )
  {
    BitmapInfo bi=BitmapInfo(width_,height_,24);
    CImage* im24;
    im24 = new CImage(&bi,image.top_left_ptr(),false);
    moviestream_->AddFrame(im24);
  }
  else
  {
    double xratio=(double)width_/image.ni();
    double yratio=(double)height_/image.nj();
    double ratio=1.0/(yratio>xratio?yratio:xratio);

    vil_image_view<vxl_byte> resampled_image(width_,height_,image.nplanes());
    vil_resample_bilin(image,resampled_image,0.0,0.0,
                       ratio,0.0,0.0,ratio,width_,height_);

    std::cout << "write frame "<< current_frame_+1 << " ... "
             << image.ni()<<'x'<<image.nj()<<" -> "
             << width_<<'x'<<height_<< std::endl;

    uint8_t data[width_*height_*3];
    std::memset(data,0,width_*height_*3*sizeof(uint8_t));

    BitmapInfo bi=BitmapInfo(width_,height_,24);
    CImage* im24;
    im24 = new CImage(&bi,data,false);

    for (int y=0;y<height_;++y)
      for (int x=0;x<width_;++x)
      {
        if (image.nplanes()==3)
        {
          im24->At(x,y)[bgr_?2:0] = resampled_image(x,
              upside_down_?y:(height_-y-1),2);
          im24->At(x,y)[1] = resampled_image(x,
              upside_down_?y:(height_-y-1),1);
          im24->At(x,y)[bgr_?0:2] = resampled_image(x,
              upside_down_?y:(height_-y-1),0);
        }
        else
        {
          im24->At(x,y)[bgr_?2:0] = resampled_image(x,
              upside_down_?y:(height_-y-1),0);
          im24->At(x,y)[1] = resampled_image(x,
              upside_down_?y:(height_-y-1),0);
          im24->At(x,y)[bgr_?0:2] = resampled_image(x,
              upside_down_?y:(height_-y-1),0);
        }
      }

    moviestream_->AddFrame(im24);
  }

  current_frame_++;
}

std::string mvl2_video_to_avi::is_a() const
{
  return std::string("mvl2_video_to_avi");
}

mvl2_video_writer* mvl2_video_to_avi::clone() const
{
  return new mvl2_video_to_avi(*this);
}
