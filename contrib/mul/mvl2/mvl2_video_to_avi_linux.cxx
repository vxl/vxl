#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief A class for writing videos
// \author Franck Bettinger

#include "mvl2_video_to_avi_linux.h"
#include <vcl_cstring.h>

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
  // TO DO : verify if the codec is supported for video writting.
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
    upside_down_=true;
  }
  else
  {
    upside_down_=false;
  }

  return true;
}

void mvl2_video_to_avi::set_quality(int qual)
{
  quality_=qual;
}

bool mvl2_video_to_avi::open( int width, int height,
                              vcl_string format, vcl_string file_name)
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

int mvl2_video_to_avi::get_width()
{
  return width_;
}

int mvl2_video_to_avi::get_height()
{
  return height_;
}

void mvl2_video_to_avi::set_frame_rate(double frame_rate)
{
  frame_rate_=frame_rate;
}

void mvl2_video_to_avi::write_frame(vimt_image_2d_of<vxl_byte>& image)
{
  if (width_==image.image().ni() && height_==image.image().nj() &&
      image.image().pixel_format()==VIL2_PIXEL_FORMAT_RGB_BYTE )
  {
    BitmapInfo bi=BitmapInfo(width_,height_,24);
    CImage* im24;
    im24 = new CImage(&bi,image.image().top_left_ptr(),false);
    moviestream_->AddFrame(im24);
  }
  else
  {
    vimt_transform_2d trans;
    double xratio=(double)width_/image.image().ni();
    double yratio=(double)height_/image.image().nj();
    double ratio=yratio<xratio?yratio:xratio;
    double x_lo=yratio<xratio?width_/2.0-image.image().ni()/2.0*ratio:0.0;
    double y_lo=yratio<xratio?0.0:height_/2.0-image.image().nj()/2.0*ratio;
    double x_hi=x_lo+image.image().ni()*ratio;
    double y_hi=y_lo+image.image().nj()*ratio;
    trans.set_similarity(ratio,0.0,x_lo,y_lo);

    trans=trans.inverse();

    vcl_cout << "write frame "<< current_frame_+1 << " ... "
             << image.image().ni()<<"x"<<image.image().nj()<<" -> "
             << width_<<"x"<<height_<< vcl_endl;

    uint8_t data[width_*height_*3];
    vcl_memset(data,0,width_*height_*3*sizeof(uint8_t));

    BitmapInfo bi=BitmapInfo(width_,height_,24);
    CImage* im24;
    im24 = new CImage(&bi,data,false);

    for (int y=(int)y_lo;y<(int)y_hi;++y)
      for (int x=(int)x_lo;x<(int)x_hi;++x)
      {
        int xn=(int)trans(x,y).x();
        int yn=(int)trans(x,y).y();
        if (upside_down_)
        {
          xn=(int)trans(x,y_hi+y_lo-y).x();
          yn=(int)trans(x,y_hi+y_lo-y).y();
        }
        if (xn<0) xn=0;
        if (yn<0) yn=0;
        if (xn>=image.image().ni()-1) xn=image.image().ni()-1;
        if (yn>=image.image().nj()-1) yn=image.image().nj()-1;
        if (image.image().nplanes()==3)
        {
          im24->At(x,y)[bgr_?2:0] = image.image()(xn,yn,2);
          im24->At(x,y)[1] = image.image()(xn,yn,1);
          im24->At(x,y)[bgr_?0:2] = image.image()(xn,yn,0);
        }
        else
        {
          im24->At(x,y)[bgr_?2:0] = image.image()(xn,yn,0);
          im24->At(x,y)[1] = image.image()(xn,yn,0);
          im24->At(x,y)[bgr_?0:2] = image.image()(xn,yn,0);
        }
      }

    moviestream_->AddFrame(im24);
  }

  current_frame_++;
}

vcl_string mvl2_video_to_avi::is_a() const
{
  return vcl_string("mvl2_video_to_avi");
}

mvl2_video_writer* mvl2_video_to_avi::clone() const
{
  return new mvl2_video_to_avi(*this);
}
