//:
// \file

#include "vidl_avifile_avicodec.h"

#include <avifile.h>
#include <videodecoder.h>
#include <infotypes.h>
#include <avm_except.h>
#include <version.h>
#include <avm_default.h>
#include <avm_fourcc.h>

#include <vidl/vidl_frame.h>
#include <vidl/vidl_movie.h>
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>

#include <vcl_iostream.h>
#include <vcl_cstdio.h>


//: Constructor
vidl_avicodec::vidl_avicodec()
: current_frame_(0), moviefile_(NULL), moviestream_(NULL)
{
}



//: Destructor
vidl_avicodec::~vidl_avicodec()
{
  if (moviestream_){
    moviestream_->StopStreaming();
  }
  if (moviefile_) delete moviefile_;
  //if (buffer_) delete buffer_;
}


//-----------------------------------------------------------------------------
//: Probe the file fname, open it as an AVI file. If it works, return true, false otherwise.

bool vidl_avicodec::probe(const char* fname)
{
  IAviReadFile* avi_file;
  IAviReadStream* avi_stream;
  avi_file = CreateIAviReadFile(fname);
  if ( avi_file && avi_file->VideoStreamCount()!=0){
    avi_stream = avi_file->GetStream(0,AviStream::Video);
    if ( avi_stream ){
      delete avi_file;
      return true;
    }
    delete avi_file;
  }
  return false;
}


//: Returns a clone of 'this' into which the given avi file is loaded.
//  This function creates a clone of 'this' (in order to allow
//  loading multiple avi videos at once) and loads the avi
//  into the cloned codec. The cloned codec is the one that is returned
//  by this function.
vidl_codec_sptr
vidl_avicodec::load(const char* fname, char mode)
{
  vidl_avicodec *cloned_avi_codec = new vidl_avicodec;

  if (!cloned_avi_codec->load_avi(fname,mode)){
    delete cloned_avi_codec;
    return NULL;
  }

  return vidl_codec_sptr(cloned_avi_codec);
}


bool
vidl_avicodec::load_avi(const char* fname, char mode )
{
  current_frame_=-1;

  moviefile_ = CreateIAviReadFile(fname);
  if( !moviefile_ ) return false;

  if( moviefile_->VideoStreamCount() == 0 ){
    delete moviefile_;
    moviefile_ = NULL;
    return false;
  }
  
  moviestream_ = moviefile_->GetStream(0,AviStream::Video);
  if ( !moviestream_ ){
    delete moviefile_;
    moviefile_ = NULL;
    return false;
  }

  BITMAPINFOHEADER bh;
  moviestream_->GetVideoFormat(&bh, sizeof(bh));
  this->set_width(bh.biWidth);
  this->set_height(bh.biHeight);
  this->set_number_frames(moviestream_->GetLength());
  this->set_format('L');
  this->set_image_class('C');
  this->set_name(vul_file::basename(fname).c_str());
  this->set_description(fname);
  
  //frame_rate_=(double)moviestream_->GetLength()/moviestream_->GetLengthTime();

  moviestream_->StartStreaming();
  
  return true;
}


vil_image_view_base_sptr
vidl_avicodec::get_view( int position,
                         int x0, int xs,
                         int y0, int ys ) const
{
  if (!moviestream_) return NULL;

  if( const_cast<vidl_avicodec *>(this)->seek(position) < 0) return NULL;
  
  CImage* cim;

  cim=moviestream_->GetFrame();
  if (cim==0) return NULL;

  CImage* im24;
  if (cim->Depth()==24){
    im24=cim;
  }
  else{
    im24=new CImage(cim,24);
  }

  // Allocate the memory chunck for the frame
  vil_pixel_format fmt = vil_pixel_format_of(vxl_byte());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(im24->Width()*im24->Height()*3*sizeof(vxl_byte),
                                                     vil_pixel_format_component_format(fmt));

  // Copy the data into the memory chunk
  vcl_memcpy(chunk->data(), im24->At(0,0),sizeof(vxl_byte)*im24->Bytes());

  // Create a vil_image_view of this memory chunk
  vil_image_view_base_sptr image = new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data())+2,
                                                                im24->Width(), im24->Height(), 3,
                                                                3, im24->Width()*3, -1);                                                               
  if (cim->Depth()!=24) delete im24;
  
  return image;
}


bool
vidl_avicodec::put_view( int /*position*/,
                         const vil_image_view_base &/*im*/,
                         int /*x0*/, int /*y0*/)
{
  vcl_cerr << "vidl_avicodec::put_section not implemented\n";
  return false;
}


int
vidl_avicodec::seek(int frame_number)
{
  if (!moviestream_ || (unsigned int)frame_number > moviestream_->GetLength())
    return -1;
    
  if (frame_number==current_frame_)
    return current_frame_;
    
  if (frame_number==current_frame_+1){
    current_frame_ = next_frame();
    return current_frame_;
  }
    
  if (frame_number==0){
    current_frame_=0;
    moviestream_->Seek(current_frame_);
    moviestream_->ReadFrame();
    return current_frame_;
  }
  
  moviestream_->Seek(frame_number);
  moviestream_->SeekToPrevKeyFrame();
  int key_frame = moviestream_->GetPos();
  vcl_cout << "[vidl_avicodec::seek] key frame " << key_frame
           << "  -> uncompress " << frame_number-key_frame << " frames" << vcl_endl;
  for (int i=key_frame; i<frame_number; ++i)
  {
    moviestream_->ReadFrame();
    moviestream_->GetFrame();
  }
  moviestream_->ReadFrame();
  current_frame_ = moviestream_->GetPos();
  // hack for GetPos bug with MJPG codec
  if (current_frame_ == 0)
    current_frame_ = frame_number;
  return current_frame_;
}


int
vidl_avicodec::next_frame()
{
  if (!moviestream_) return -1;

  moviestream_->ReadFrame();

  // hack for GetPos bug with MJPG codec
  return moviestream_->GetPos()==0 ? current_frame_+1 : moviestream_->GetPos();
}
