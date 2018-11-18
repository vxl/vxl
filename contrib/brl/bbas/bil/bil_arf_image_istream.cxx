// This is brl/bbas/bil/bil_arf_image_istream.cxx
//:
// \file
// \author Vishal Jain
// \date   Aug 31, 2012
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <algorithm>
#include <sstream>
#include "bil_arf_image_istream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_chunk.h>

//--------------------------------------------------------------------------------
void endian_swap(unsigned int& x)
{
  // Int is 4 bytes on this machine

  x = (x >> 24) |
  ((x<<8) & 0x00FF0000) |
  ((x>>8) & 0x0000FF00) |
  (x<<24);
}

void endian_swap_short(unsigned short& x)
{
  // Int is 4 bytes on this machine
  x = (x >> 8) |((x<<8) & 0x0000FF00) ;
}

//: The initial frame index
// \note the initial frame index is invalid until advance() is called
static const unsigned int INIT_INDEX = unsigned(-1);


//: Constructor
bil_arf_image_istream::
bil_arf_image_istream()
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIL_PIXEL_FORMAT_UNKNOWN ),
    current_frame_(nullptr),
    time_stamp_(-1) {}


//: Constructor
bil_arf_image_istream::
bil_arf_image_istream(const std::string& glob)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(nullptr),
    time_stamp_(-1)
{
  open(glob);
}


//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
bil_arf_image_istream::
open(const std::string& rawFile)
{
  //open up raw file, and read 20 byte header
  this->raw_file_ = rawFile;
  raw_.open(raw_file_.c_str(), std::ios::in | std::ios::binary);

  unsigned int magic_num, version, arf_image_type, ni, nj, num_images, image_offset, subheader_flags;

  raw_.read( (char*) &magic_num, sizeof(magic_num) );
  raw_.read( (char*) &version, sizeof(version) );
  raw_.read( (char*) &nj, sizeof(nj) );
  raw_.read( (char*) &ni, sizeof(ni) );
  raw_.read( (char*) &arf_image_type, sizeof(arf_image_type) );
  raw_.read( (char*) &num_images, sizeof(num_images) );
  raw_.read( (char*) &image_offset, sizeof(image_offset) );
  raw_.read( (char*) &subheader_flags, sizeof(subheader_flags) );

  endian_swap(magic_num);
  endian_swap(version);
  endian_swap(ni);
  endian_swap(nj);
  endian_swap(arf_image_type);
  endian_swap(num_images);
  endian_swap(image_offset);
  endian_swap(subheader_flags);

  //vj: modification
  std::cout<<"Raw file header:\n"
          <<"Magic Num "<<magic_num<<'\n'
          <<"version "<<version<<'\n'
          <<"size: "<<ni<<','<<nj<<'\n'
          <<"pixel:"<<arf_image_type<<'\n'
          <<"#fram:"<<num_images<<'\n'
          <<"Image Offset "<<image_offset<<'\n'
          <<"Subheader "<<subheader_flags<<std::endl;

  //store in member vars
  ni_ = ni;
  nj_ = nj;
  num_frames_ = (unsigned int)num_images; // possible overflow...

  if (arf_image_type==5)
  {
    format_ = VIL_PIXEL_FORMAT_UINT_16;
    pixel_size_ = 16;
  }
  image_offset_ = image_offset;

  //index is invalid until advance is called
  index_ = INIT_INDEX;
  current_frame_ = nullptr;
  return true;
}

//: Close the stream
void
bil_arf_image_istream::
close()
{
  //image_paths_.clear();
  index_ = INIT_INDEX;
  current_frame_ = nullptr;
  ni_ = 0;
  nj_ = 0;
  format_ = VIL_PIXEL_FORMAT_UNKNOWN;
  raw_.close();
  std::cout<<"bil_arf_image_istream closed"<<std::endl;
}


//: Advance to the next frame (but do not load the next image)
bool
bil_arf_image_istream::
advance()
{
  current_frame_ = nullptr;
  if (index_ < num_frames_ || index_ == INIT_INDEX )
    return ++index_ < num_frames_;
  else
    return false;
}


//: Read the next frame from the stream
vil_image_view_base_sptr
bil_arf_image_istream::read_frame()
{
  advance();
  return current_frame();
}


//: Return the current frame in the stream
vil_image_view_base_sptr
bil_arf_image_istream::current_frame()
{
  //hack way to clean up current memory
  if (is_valid()) {
    if (!current_frame_)
    {
      //calc image size, seek to offset
      unsigned int imgSize = ni_*nj_*pixel_size_/8;
      //  vj Modification
      long long loc = image_offset_+((long long) index_ -1)* ( (long long)imgSize +40 );

      std::cout<<"location "<<loc <<' '<<index_<<std::endl;
      raw_.seekg(loc, std::ios::beg);

      //allocate vil memory chunk
      vil_memory_chunk_sptr mem_chunk = new vil_memory_chunk(imgSize,VIL_PIXEL_FORMAT_BYTE);

      //read image from byte stream
      if (pixel_size_==24) {
        raw_.read( (char*) mem_chunk->data(), imgSize );
        current_frame_ = new vil_image_view<vxl_byte>(mem_chunk, (vxl_byte*) mem_chunk->data(), ni_, nj_, 3, 3, 3*ni_, 1);
      }
      else if (pixel_size_==8) {
        raw_.read( (char*) mem_chunk->data(), imgSize );
        current_frame_ = new vil_image_view<vxl_byte>(mem_chunk, (vxl_byte*) mem_chunk->data(), ni_, nj_, 1, 1, ni_, ni_*nj_);
      }
      else if (pixel_size_==16) {
        raw_.read( (char*) mem_chunk->data(), imgSize );
        auto * current_frame = new vil_image_view<unsigned short>(mem_chunk, (unsigned short*) mem_chunk->data(), ni_, nj_, 1, 1, ni_, ni_*nj_);

        for (unsigned i = 0 ; i < current_frame->ni(); i++)
        {
          for (unsigned j = 0 ; j < current_frame->nj(); j++)
          {
            unsigned short x = (*current_frame)(i,j) ;
            endian_swap_short(x);
            (*current_frame)(i,j) = x;
          }
        }
        current_frame_ = current_frame ;
      }
    }

    return current_frame_;
  }
  return nullptr;
}


//: Return the path to the current image in the stream
std::string
bil_arf_image_istream::current_path() const
{
  return raw_file_;
}


//: Seek to the given frame number (but do not load the image)
// \returns true if successful
bool
bil_arf_image_istream::
seek_frame(unsigned int frame_nr)
{
  if (is_open() && frame_nr < num_frames_) {
    if (index_ != frame_nr)
      current_frame_ = nullptr;
    index_ = frame_nr;
    return true;
  }
  return false;
}

//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bil_arf_image_istream const&  /*scene*/) {}
//: Binary write boxm2_data_base to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, const bil_arf_image_istream* & /*p*/) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bil_arf_image_istream_sptr&  /*sptr*/) {}
//: Binary write boxm2_data_base_sptr to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, bil_arf_image_istream_sptr const&  /*sptr*/) {}

//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bil_arf_image_istream & /*scene*/) {}
//: Binary load boxm2_data_base from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bil_arf_image_istream*  /*p*/) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bil_arf_image_istream_sptr&  /*sptr*/) {}
//: Binary load boxm2_data_base_sptr from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bil_arf_image_istream_sptr const&  /*sptr*/) {}
