//:
// \file
// \brief A class for reading video files on linux platform
// \author Franck Bettinger

#include <mvl2/mvl2_video_from_avi_linux.h>
#include <avifile.h>
#include <videodecoder.h>
#include <infotypes.h>
#include <except.h>
#include <version.h>
#include <avm_default.h>
#include <avm_fourcc.h>
#include <vcl_cstring.h>

mvl2_video_from_avi::mvl2_video_from_avi()
{
  is_initialized_=false;
  current_frame_=0;
  buffer_=NULL;
}

mvl2_video_from_avi::~mvl2_video_from_avi()
{
  uninitialize();
  if (buffer_) delete buffer_;
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
  firstcall_=true;
  current_frame_=0;

  moviefile_=CreateIAviReadFile(file_name.c_str());
  if (moviefile_==NULL || moviefile_->VideoStreamCount()==0) return false;
  moviestream_=moviefile_->GetStream(0,AviStream::Video);
  if (moviestream_==NULL) return false;

  BITMAPINFOHEADER bh;
  moviestream_->GetVideoFormat(&bh, sizeof(bh));
  width_ = bh.biWidth;
  height_ = bh.biHeight;
  frame_rate_=(double)moviestream_->GetLength()/moviestream_->GetLengthTime();

  moviestream_->StartStreaming();
  use_colour_=true;
  if (!format.find(vcl_string("Grey"))) use_colour_=false;
  is_initialized_=true;

  return is_initialized_;
}

void mvl2_video_from_avi::uninitialize()
{
  if (!is_initialized_) return;
  moviestream_->StopStreaming();
  delete moviefile_;
  is_initialized_=false;
}

int mvl2_video_from_avi::next_frame()
{
  if (!is_initialized_) return -1;

  moviestream_->ReadFrame();

  // hack for GetPos bug with MJPG codec
  current_frame_= moviestream_->GetPos()==0 ? current_frame_+1 : moviestream_->GetPos();
  return current_frame_;
}

bool mvl2_video_from_avi::get_frame(vil_image_view<vxl_byte>& image)
{
  if (!is_initialized_) return false;

  CImage* cim;

  cim=moviestream_->GetFrame();

  if (cim==0) return false;
  CImage* im24;
  if (cim->Depth()==24)
    {
    im24=cim;
    }
  else
    {
    im24=new CImage(cim,24);
    }
  if (firstcall_)
    {
    if (buffer_) delete buffer_;
    buffer_=NULL;
    firstcall_=false;
    }
  if (!buffer_) buffer_=(vxl_byte*)malloc(sizeof(vxl_byte)*im24->Bytes());
  if (use_colour_)
    {
    image.set_size(im24->Width(),im24->Height(),3);
    vcl_memcpy(buffer_,im24->At(0,0),sizeof(vxl_byte)*im24->Bytes());
    image.set_to_memory(buffer_+2,im24->Width(),im24->Height(),3,
        3,3*im24->Width(),-1);
    }
  else
    {
    image.set_size(im24->Width(),im24->Height(),1);
      // takes the Y componant in the YUV space
    for (unsigned int y=0;y<image.nj();++y)
      for (unsigned int x=0;x<image.ni();++x)
        image(x,y,0) = (int)(0.299*(double)im24->At(x,y)[1]+
            0.587*(double)im24->At(x,y)[2]+
            0.114*(double)im24->At(x,y)[0]);
    }
  if (cim->Depth()!=24) delete im24;
  return true;
}

void mvl2_video_from_avi::reset_frame()
{
  if (!is_initialized_) return;

  current_frame_=0;
  moviestream_->Seek(current_frame_);
}

void mvl2_video_from_avi::set_frame_rate(double frame_rate)
{
}

double mvl2_video_from_avi::get_frame_rate()
{
  return frame_rate_;
}

int mvl2_video_from_avi::get_width()
{
   return width_;
}

int mvl2_video_from_avi::get_height()
{
   return height_;
}

void mvl2_video_from_avi::set_capture_size(int width,int height)
{
}

int mvl2_video_from_avi::length()
{
  if (!is_initialized_) return -1;

  return moviestream_->GetLength();
}

int mvl2_video_from_avi::seek(unsigned int frame_number)
{
  if (!is_initialized_ || frame_number>moviestream_->GetLength())
    return -1;
  if (frame_number==0)
  {
    reset_frame();
    next_frame();
    current_frame_=0;
    return 0;
  }
  if (frame_number==current_frame_) return current_frame_;
  if (frame_number==current_frame_+1) return next_frame();

  moviestream_->Seek(frame_number);
  moviestream_->SeekToPrevKeyFrame();
  unsigned int key_frame=moviestream_->GetPos();
  vcl_cout << "[mvl2_video_from_avi::seek] key frame " << key_frame
           << "  -> uncompress " << frame_number-key_frame << " frames\n";
  for (unsigned int i=key_frame; i<frame_number; ++i)
  {
    moviestream_->ReadFrame();
    /* CImage* cim = */ moviestream_->GetFrame();
  }
  moviestream_->ReadFrame();
  current_frame_ = moviestream_->GetPos();
  // hack for GetPos bug with MJPG codec
  if (current_frame_ == 0) current_frame_ = frame_number;
  return current_frame_;
}
