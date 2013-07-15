// This is brl/bbas/bil/bil_raw_image_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew Miller
// \date   19 Dec 2011
//
//-----------------------------------------------------------------------------

#include "bil_raw_image_istream.h"
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_chunk.h>

//--------------------------------------------------------------------------------

//: The initial frame index
// \note the initial frame index is invalid until advance() is called
static const unsigned int INIT_INDEX = unsigned(-1);


//: Constructor
bil_raw_image_istream::
bil_raw_image_istream()
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(NULL),
    time_stamp_(-1) {}


//: Constructor
bil_raw_image_istream::
bil_raw_image_istream(const vcl_string& glob)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(NULL),
    time_stamp_(-1)
{
  open(glob);
}


//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
bil_raw_image_istream::
open(const vcl_string& rawFile)
{
  //open up raw file, and read 20 byte header
  this->raw_file_ = rawFile;
  raw_.open(raw_file_.c_str(), vcl_ios::in | vcl_ios::binary);

  int ni, nj, pixelSize;
  int numFrames;
  raw_.read( (char*) &ni, sizeof(ni) );
  raw_.read( (char*) &nj, sizeof(nj) );
  raw_.read( (char*) &pixelSize, sizeof(pixelSize) );
  raw_.read( (char*) &numFrames, sizeof(numFrames) );
  //vj modification
  //vxl_int_64 timestamp;
  //for (int i = 0; i < numFrames; ++i)
  //  raw_.read( (char*) &timestamp, sizeof(timestamp) );

  vcl_cout<<"Raw file header:\n"
          <<"  size: "<<ni<<','<<nj<<'\n'
          <<"  pixel:"<<pixelSize<<'\n'
          <<"  #fram:"<<numFrames<<vcl_endl;

  //store in member vars
  ni_ = ni;
  nj_ = nj;
  num_frames_ = (unsigned int)numFrames; // possible overflow...
  pixel_size_ = pixelSize;
  if (pixelSize==24)
    format_ = VIL_PIXEL_FORMAT_RGB_BYTE;
  else if (pixelSize==8)
    format_ = VIL_PIXEL_FORMAT_BYTE;
  else if (pixelSize<=16 && pixelSize > 8)
  {
      pixel_size_ =16;
    format_ = VIL_PIXEL_FORMAT_UINT_16;
  }

  //index is invalid until advance is called
  index_ = INIT_INDEX;
  current_frame_ = NULL;
  return true;
}

//: Close the stream
void
bil_raw_image_istream::
close()
{
  //image_paths_.clear();
  index_ = INIT_INDEX;
  current_frame_ = NULL;
  ni_ = 0;
  nj_ = 0;
  format_ = VIL_PIXEL_FORMAT_UNKNOWN;
  raw_.close();
  vcl_cout<<"bil_raw_image_istream closed"<<vcl_endl;
}


//: Advance to the next frame (but do not load the next image)
bool
bil_raw_image_istream::
advance()
{
  current_frame_ = NULL;
  if (index_ < num_frames_ || index_ == INIT_INDEX )
    return ++index_ < num_frames_;

  return false;
}


//: Read the next frame from the stream
vil_image_view_base_sptr
bil_raw_image_istream::read_frame()
{
  advance();
  return current_frame();
}


//: Return the current frame in the stream
vil_image_view_base_sptr
bil_raw_image_istream::current_frame()
{
  //hack way to clean up current memory
  if (is_valid()) {
    if (!current_frame_)
    {
      //calc image size, seek to offset
      unsigned int imgSize = ni_*nj_*pixel_size_/8;
      //  vj Modification
      long long loc = 20+8*index_ + (long long) index_* ( (long long)imgSize );
      raw_.seekg(loc, vcl_ios::beg);
      vcl_cout<<"The location is "<<loc<<" and image size is "<<ni_<<" "<<nj_<<" "<<pixel_size_<<vcl_endl;
      //allocate vil memory chunk
      vil_memory_chunk_sptr mem_chunk = new vil_memory_chunk(imgSize,VIL_PIXEL_FORMAT_BYTE);

      //read image from byte stream
      if (pixel_size_==24) {
        raw_.read( (char*) mem_chunk->data(), imgSize );
        current_frame_ = new vil_image_view<vxl_byte>(mem_chunk, (vxl_byte*) mem_chunk->data(), ni_, nj_, 3, 3, 3*ni_, 1);
      }
      else if (pixel_size_==8){
        raw_.read( (char*) mem_chunk->data(), imgSize );
        current_frame_ = new vil_image_view<vxl_byte>(mem_chunk, (vxl_byte*) mem_chunk->data(), ni_, nj_, 1, 1, ni_, ni_*nj_);
      }
      else if (pixel_size_==16) {
        raw_.read( (char*) mem_chunk->data(), imgSize );
        current_frame_ = new vil_image_view<unsigned short>(mem_chunk, (unsigned short*) mem_chunk->data(), ni_, nj_, 1, 1, ni_, ni_*nj_);
      }
      //  vj Modification
      //read timestamp
      vxl_int_64 timeStamp;
      raw_.read( (char*) &timeStamp, sizeof(timeStamp) );
      time_stamp_ = timeStamp;
    }
    return current_frame_;
  }
  return NULL;
}


//: Return the path to the current image in the stream
vcl_string
bil_raw_image_istream::current_path() const
{
  return raw_file_;
}


//: Seek to the given frame number (but do not load the image)
// \returns true if successful
bool
bil_raw_image_istream::
seek_frame(unsigned int frame_nr)
{
  if (is_open() && frame_nr < num_frames_) {
    if (index_ != frame_nr)
      current_frame_ = NULL;
    index_ = frame_nr;
    vcl_cout<<"Index is "<<index_<<vcl_endl;
    return true;
  }
  return false;
}

//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream const& scene) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream& os, const bil_raw_image_istream* &p) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream_sptr& sptr) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream_sptr const& sptr) {}

//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream &scene) {}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream* p) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream_sptr& sptr) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream_sptr const& sptr) {}


