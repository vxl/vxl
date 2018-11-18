//==========
// Kitware (c) 2010
//
// Restrictions applicable to use by the US Government:
//
// UNLIMITED RIGHTS
//
// Restrictions applicable for all other users:
//
// This software and all information and expression are the property of Kitware, Inc. All Rights Reserved.
//==========
//:
// \file
// \brief Image I/O for JPEG2000 imagery using OpenJPEG
// \author Chuck Atkins

#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vil/vil_stream.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_smart_ptr.hxx>
#include <vil/vil_image_view.hxx>

// TODO: How can we avoid using the "deprecated" functions?
#define USE_OPJ_DEPRECATED
extern "C" {
  #include <openjpeg.h>
}

#include "vil_openjpeg.h"


typedef vbl_smart_ptr<vil_stream> vil_stream_sptr;

//------------------------------------------------------------------------------
// Taken from
//------------------------------------------------------------------------------
// class vil_openjpeg_file_format
//

vil_image_resource_sptr
vil_openjpeg_file_format
::make_input_image(vil_stream* vs, vil_openjpeg_format opjfmt)
{
  auto *im = new vil_openjpeg_image(vs, opjfmt);

  if ( !im->is_valid() )
  {
    delete im;
    return nullptr;
  }
  else
    return im;
}


vil_image_resource_sptr
vil_openjpeg_file_format
::make_output_image(vil_stream* /*vs*/,
                    unsigned int /*ni*/, unsigned int /*nj*/, unsigned int /*nplanes*/,
                    vil_pixel_format /*format*/, vil_openjpeg_format /*opjfmt*/)
{
  assert(!"openjpeg write support is currently not implemented");
  return nullptr;
}


//------------------------------------------------------------------------------
// Various OpenJPEG parameters and data structures
//
struct opj_header
{
  vxl_uint_32 tile_width_;
  vxl_uint_32 tile_height_;
  vxl_uint_32 num_tiles_x_;
  vxl_uint_32 num_tiles_y_;
  vxl_int_32  x0_;
  vxl_int_32  y0_;
  vxl_uint_32 num_reductions_;
};


//------------------------------------------------------------------------------
// OpenJPEG decoder helper class
//
class vil_openjpeg_decoder
{
 public:
  vil_openjpeg_decoder(OPJ_CODEC_FORMAT opj_codec_format);
  ~vil_openjpeg_decoder(void);

  bool error(void) const;
  void silence(void);

  bool init_from_stream(unsigned int reduction, void *stream);
  bool set_decode_area(unsigned int x, unsigned int y,
                       unsigned int w, unsigned int h);
  opj_image_t * take_image(void);
  opj_image_t * decode(void);

  const opj_header * header(void) const;

 private:
  opj_dparameters_t params_;
  opj_codec_t *codec_;
  opj_image_t *image_;
  opj_stream_t *stream_;
  opj_header header_;
  OPJ_CODEC_FORMAT opj_codec_format_;

  bool error_;
  bool silent_;

  bool init_decoder(unsigned int reduction);
  bool init_stream(void *stream);
  bool read_header(void);

  // OpenJPEG I/O helper functions
  static vxl_uint_32 opj_vil_stream_read(void *p_buffer,
                                         vxl_uint_32 p_nb_bytes,
                                         void *p_user_data);
  static vxl_uint_32 opj_vil_stream_write(void *p_buffer,
                                          vxl_uint_32 p_nb_bytes,
                                          void *p_user_data);
  static vxl_uint_32 opj_vil_stream_skip(vxl_uint_32 p_nb_bytes,
                                         void *p_user_data);
  static bool opj_vil_stream_seek(vxl_uint_32 p_nb_bytes,
                                  void *p_user_data);

  // OpenJPEG logging functions
  static void opj_event_info(const char *msg, void *data);
  static void opj_event_warning(const char *msg, void *data);
  static void opj_event_error(const char *msg, void *data);
};


//------------------------------------------------------------------------------
// OpenJPEG image internal implementation
//
struct vil_openjpeg_image_impl
{
  // OpenJPEG data structures
  opj_cparameters_t encode_params_;
  opj_codec_t *encode_codec_;
  opj_image_t *image_;
  opj_header header_;
  OPJ_CODEC_FORMAT opj_codec_format_;

  // Fields needed for the vil implementation
  vil_stream_sptr vstream_;
  vil_streampos vstream_start_;
  bool is_valid_;
  bool error_;

  vil_openjpeg_image_impl(void)
  : encode_codec_(nullptr), image_(nullptr), vstream_(nullptr), vstream_start_(0),
    is_valid_(false), error_(false)
  {
    std::memset(&this->encode_params_, 0, sizeof(opj_cparameters_t));
    std::memset(&this->header_, 0, sizeof(opj_header));
  }
};


//------------------------------------------------------------------------------
// class vil_openjpeg_decoder
//


vil_openjpeg_decoder
::vil_openjpeg_decoder(OPJ_CODEC_FORMAT opj_codec_format)
: codec_(nullptr), image_(nullptr), stream_(nullptr), opj_codec_format_(opj_codec_format),
  error_(false), silent_(false)
{
  std::memset(&this->params_, 0, sizeof(opj_dparameters_t));
  std::memset(&this->header_, 0, sizeof(opj_header));
}


vil_openjpeg_decoder
::~vil_openjpeg_decoder(void)
{
  // De-allocate any necessary OpenJPEG data structures
  if ( this->stream_ )
  {
    opj_stream_destroy(this->stream_);
    this->stream_ = nullptr;
  }
  if ( this->codec_ )
  {
    opj_destroy_codec(this->codec_);
    this->codec_ = nullptr;
  }
  if ( this->image_ )
  {
    opj_image_destroy(this->image_);
    this->image_ = nullptr;
  }
}


bool
vil_openjpeg_decoder
::error(void) const
{
  return this->error_;
}


void
vil_openjpeg_decoder
::silence(void)
{
  this->silent_ = true;
}


bool
vil_openjpeg_decoder
::init_from_stream(unsigned int reduction, void *stream)
{
  if ( !init_stream(stream) )
    return false;

  if ( !init_decoder(reduction) )
    return false;

  if ( !read_header() )
    return false;

  return true;
}


bool
vil_openjpeg_decoder
::init_stream(void *stream)
{
  if ( this->stream_ )
  {
    opj_stream_destroy(this->stream_);
    this->stream_ = nullptr;
  }

  // Setup the input stream
  this->stream_ = opj_stream_default_create(true);
  if ( !this->stream_ )
    return false;

  // Configure the I/O methods for the opj stream using vil operations
  opj_stream_set_user_data(this->stream_, stream);
  opj_stream_set_read_function(this->stream_,
                               vil_openjpeg_decoder::opj_vil_stream_read);
  opj_stream_set_write_function(this->stream_,
                                vil_openjpeg_decoder::opj_vil_stream_write);
  opj_stream_set_skip_function(this->stream_,
                               vil_openjpeg_decoder::opj_vil_stream_skip);
  opj_stream_set_seek_function(this->stream_,
                               vil_openjpeg_decoder::opj_vil_stream_seek);

  return true;
}


bool
vil_openjpeg_decoder
::init_decoder(unsigned int reduction)
{
  if ( this->codec_ )
  {
    opj_destroy_codec(this->codec_);
    this->codec_ = nullptr;
  }

  // Set decoder parameters
  std::memset(&this->params_, 0, sizeof(opj_dparameters_t));
  opj_set_default_decoder_parameters(&this->params_);
  this->params_.cp_reduce = reduction;
  this->params_.cp_layer = 0;

  // Create the decoder
  this->codec_ = opj_create_decompress(this->opj_codec_format_);
  if ( !this->codec_ )
    return false;

  // Configure the OpenJPEG event manager
  opj_set_info_handler(this->codec_,
                       vil_openjpeg_decoder::opj_event_info, this);
  opj_set_warning_handler(this->codec_,
                          vil_openjpeg_decoder::opj_event_warning, this);
  opj_set_error_handler(this->codec_,
                        vil_openjpeg_decoder::opj_event_error, this);

  // Initialize the decoder
  if ( !opj_setup_decoder( this->codec_, &this->params_) )
    return false;
  if ( this->error_ )
    return false;

  return true;
}


bool
vil_openjpeg_decoder
::read_header(void)
{
  if ( this->image_ )
  {
    opj_image_destroy(this->image_);
    this->image_ = nullptr;
  }

  return opj_read_header( this->codec_,
                         &this->image_,
                         &this->header_.x0_,
                         &this->header_.y0_,
                         &this->header_.tile_width_,
                         &this->header_.tile_height_,
                         &this->header_.num_tiles_x_,
                         &this->header_.num_tiles_y_,
                          this->stream_) && !this->error_;
}


bool
vil_openjpeg_decoder
::set_decode_area(unsigned int x, unsigned int y,
                  unsigned int w, unsigned int h)
{
  this->error_ = false;
  return opj_set_decode_area( this->codec_, x, y, w, h ) && !this->error_;
}


opj_image_t *
vil_openjpeg_decoder
::take_image(void)
{
  opj_image_t *image = this->image_;
  this->image_ = nullptr;
  return image;
}


opj_image_t *
vil_openjpeg_decoder
::decode(void)
{
  this->error_ = false;
  return opj_decode(this->codec_, this->stream_);
}


const opj_header *
vil_openjpeg_decoder
::header(void) const
{
  return &this->header_;
}


//------------------------------------------------------------------------------
// Helper functions to read from vil_stream objects
//

vxl_uint_32
vil_openjpeg_decoder
::opj_vil_stream_read(void *p_buffer,
                      vxl_uint_32 p_nb_bytes,
                      void *p_user_data)
{
  auto *stream = reinterpret_cast<vil_stream*>(p_user_data);
  vil_streampos b = stream->read(p_buffer, p_nb_bytes);
  if ( b == 0 || !stream->ok() )
  {
    return static_cast<vxl_uint_32>(-1);
  }
  if ( b > static_cast<vil_streampos>(std::numeric_limits<vxl_uint_32>::max()) )
  {
    throw std::runtime_error("Stream position outof range");
  }
  return static_cast<vxl_uint_32>(b);
}


vxl_uint_32
vil_openjpeg_decoder
::opj_vil_stream_write(void *p_buffer,
                       vxl_uint_32 p_nb_bytes,
                       void *p_user_data)
{
  auto *stream = reinterpret_cast<vil_stream*>(p_user_data);
  vil_streampos b = stream->write(p_buffer, p_nb_bytes);
  if ( b == 0 || !stream->ok() )
  {
    return static_cast<vxl_uint_32>(-1);
  }
  if ( b > static_cast<vil_streampos>(std::numeric_limits<vxl_uint_32>::max()) )
  {
    throw std::runtime_error("Stream position outof range");
  }
  return static_cast<vxl_uint_32>(b);
}


vxl_uint_32
vil_openjpeg_decoder
::opj_vil_stream_skip(vxl_uint_32 p_nb_bytes,
                      void *p_user_data)
{
  auto *stream = reinterpret_cast<vil_stream*>(p_user_data);
  vil_streampos start = stream->tell();
  stream->seek(start+p_nb_bytes);
  if ( !stream->ok() )
  {
    return static_cast<vxl_uint_32>(-1);
  }
  vil_streampos end = stream->tell();
  vil_streampos b = end-start;
  if ( b > static_cast<vil_streampos>(std::numeric_limits<vxl_uint_32>::max()) )
  {
    throw std::runtime_error("Stream position outof range");
  }
  return static_cast<vxl_uint_32>(b);
}


bool
vil_openjpeg_decoder
::opj_vil_stream_seek( vxl_uint_32 p_nb_bytes,
                       void *p_user_data)
{
  auto *stream = reinterpret_cast<vil_stream*>(p_user_data);
  stream->seek(p_nb_bytes);
  if ( !stream->ok() )
  {
    return false;
  }
  vil_streampos pos = stream->tell();
  if ( pos > static_cast<vil_streampos>(std::numeric_limits<vxl_uint_32>::max()) )
  {
    throw std::runtime_error("Stream position outof range");
  }
  return p_nb_bytes == static_cast<vxl_uint_32>(pos);
}


//------------------------------------------------------------------------------
// Helper functions for OpenJPEG error handling
//

void
vil_openjpeg_decoder
::opj_event_info(const char *msg, void * /*data*/)
{
  std::clog << "vil_openjpeg_decoder::INFO  : " << msg << std::endl;
}


void
vil_openjpeg_decoder
::opj_event_warning(const char *msg, void * /*data*/)
{
  std::clog << "vil_openjpeg_decoder::WARN  : " << msg << std::endl;
}


void
vil_openjpeg_decoder
::opj_event_error(const char *msg, void *data)
{
  auto *decoder = reinterpret_cast<vil_openjpeg_decoder*>(data);
  if ( !decoder->silent_ )
    std::cerr << "vil_openjpeg_decoder::ERROR : " << msg << std::endl;
  decoder->error_ = true;
}


//------------------------------------------------------------------------------
// class vil_openjpeg_image
//

vil_openjpeg_image
::vil_openjpeg_image (vil_stream* /*is*/,
                      unsigned int /*ni*/, unsigned int /*nj*/, unsigned int /*nplanes*/,
                      vil_pixel_format /*format*/, vil_openjpeg_format /*opjfmt*/)
: impl_(new vil_openjpeg_image_impl)
{
  assert(!"openjpeg write support is currently not implemented");
}


vil_openjpeg_image
::vil_openjpeg_image(vil_stream* is, vil_openjpeg_format opjfmt)
: impl_(new vil_openjpeg_image_impl)
{
  switch ( opjfmt )
  {
    case VIL_OPENJPEG_JP2: this->impl_->opj_codec_format_ = CODEC_JP2; break;
    case VIL_OPENJPEG_JPT: this->impl_->opj_codec_format_ = CODEC_JPT; break;
    case VIL_OPENJPEG_J2K: this->impl_->opj_codec_format_ = CODEC_J2K; break;
    default: return;
  }

  this->impl_->vstream_ = is;
  this->impl_->vstream_start_ = is->tell();

  if ( !this->validate_format() )
    return;

  this->impl_->vstream_->seek(this->impl_->vstream_start_);
  vil_openjpeg_decoder decoder(this->impl_->opj_codec_format_);
  if ( !decoder.init_from_stream(0, this->impl_->vstream_.as_pointer()) )
    return;

  // Copy headers and image from decoder
  std::memcpy(&this->impl_->header_, decoder.header(), sizeof(opj_header));
  this->impl_->image_ = decoder.take_image();

  // Delay num reduction computation until requested
  this->impl_->header_.num_reductions_ = static_cast<vxl_uint_32>(-1);

#if 0 // Move to a lazy evaluation
  // Find out how many reductions are available
  // There ought to be a better way to do this, but I haven't found one yet :-(
  decoder.silence();
  for (;;)
  {
    // This will fail when we ask for a reduction that doesn't exist
    this->impl_->vstream_->seek(this->impl_->vstream_start_);
    if ( !decoder.init_from_stream(this->impl_->header_.num_reductions_ + 1,
                                  this->impl_->vstream_.as_pointer()) )
      break;
    // If init_from_stream succeeded, that reduction is available
    ++this->impl_->header_.num_reductions_;
  }
#endif // 0

  // No errors have occurred (except expected one) so mark success
  this->impl_->is_valid_ = true;
}


vil_openjpeg_image
::~vil_openjpeg_image(void)
{
  // De-allocate any necessary OpenJPEG data structures
  if ( this->impl_->image_ )
  {
    opj_image_destroy(this->impl_->image_);
    this->impl_->image_ = nullptr;
  }
  delete this->impl_;
}


bool
vil_openjpeg_image
::validate_format(void)
{
  vil_streampos pos_start = this->impl_->vstream_->tell();

  switch ( this->impl_->opj_codec_format_ )
  {
  case CODEC_JP2 :
  {
    // See specification ISO/IEC 15444-1 Part 1
    unsigned char sig[12] =
      {0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A};
    unsigned char sig_file[12];
    this->impl_->vstream_->read(sig_file, 12);
    if ( std::memcmp( sig, sig_file, 12) == 0 )
    {
      this->impl_->vstream_->seek(pos_start);
      return true;
    }
    break;
  }
  case CODEC_JPT: break; // Although supported by OpenJPEG, the JPT codec
                         // (JPEG2000 JPIP) is not yet implemented by the
                         // vil implementation
  case CODEC_J2K:
  {
    // See specification ISO/IEC 15444-1 Part 1
    unsigned char sig[2] = {0xFF, 0x4F};
    unsigned char sig_file[2];
    this->impl_->vstream_->read(sig_file, 2);
    if ( std::memcmp( sig, sig_file, 2) == 0 )
    {
      this->impl_->vstream_->seek(pos_start);
      return true;
    }
    break;
  }
  default : break;
  }

  return false;
}


bool
vil_openjpeg_image
::is_valid(void) const
{
  return this->impl_->is_valid_;
}


unsigned int
vil_openjpeg_image
::nreductions(void) const
{
  if ( !this->impl_->is_valid_ )
    return static_cast<unsigned int>(-1);
  if ( this->impl_->header_.num_reductions_ == static_cast<vxl_uint_32>(-1) )
  {
    vil_openjpeg_decoder decoder(this->impl_->opj_codec_format_);
    // Find out how many reductions are available
    // There ought to be a better way to do this, but I haven't found one yet
    decoder.silence();
    unsigned int num_reductions = 0;
    for (;;)
    {
      // This will fail when we ask for a reduction that doesn't exist
      this->impl_->vstream_->seek(this->impl_->vstream_start_);
      if ( !decoder.init_from_stream(num_reductions + 1,
                                    this->impl_->vstream_.as_pointer()) )
        break;
      // If init_from_stream succeeded, that reduction is available
      ++num_reductions;
    }
    this->impl_->header_.num_reductions_ = num_reductions;
  }
  return this->impl_->header_.num_reductions_;
}


unsigned int
vil_openjpeg_image
::nplanes() const
{
  if ( !this->impl_->is_valid_ )
    return static_cast<unsigned int>(-1);
  return this->impl_->image_->numcomps;
}


unsigned int
vil_openjpeg_image
::ni() const
{
  if ( !this->impl_->is_valid_ )
    return static_cast<unsigned int>(-1);
  return this->impl_->image_->comps[0].w;
}


unsigned int
vil_openjpeg_image
::nj() const
{
  if ( !this->impl_->is_valid_ )
    return static_cast<unsigned int>(-1);
  return this->impl_->image_->comps[0].h;
}


int
vil_openjpeg_image
::maxbpp(void) const
{
  if ( !this->impl_->is_valid_ )
    return -1;

  int maxbpp = this->impl_->image_->comps[0].prec;
  for ( unsigned int i = 1; i < this->impl_->image_->numcomps; ++i )
  {
    if ( static_cast<unsigned int>(maxbpp) < this->impl_->image_->comps[i].prec )
      maxbpp = this->impl_->image_->comps[i].prec;
  }
  return maxbpp;
}


enum vil_pixel_format
vil_openjpeg_image
::pixel_format() const
{
  // NOTE:
  // 1.  Up to 32 bit pixels are supported
  // 2.  An assumption is also made by OpenJPEG that all components are integral
  // 3.  This current vil implementation will always generate unsigned
  //     components
  switch ( this->maxbpp() )
  {
    case 8  : return VIL_PIXEL_FORMAT_BYTE;
    case 16 : return VIL_PIXEL_FORMAT_UINT_16;
    case 32 : return VIL_PIXEL_FORMAT_UINT_32;
    default : return VIL_PIXEL_FORMAT_UNKNOWN;
  }
}


const char *
vil_openjpeg_image
::file_format() const
{
  switch ( this->impl_->opj_codec_format_ )
  {
    case CODEC_JP2: return "jp2";
    case CODEC_JPT: return "jpt";
    case CODEC_J2K: return "j2k";
    default: return "openjpeg";
  }
}


vil_image_view_base_sptr
vil_openjpeg_image
::get_copy_view(unsigned int i0, unsigned int ni,
                unsigned int j0, unsigned int nj) const
{
  return this->get_copy_view_reduced(i0, ni, j0, nj, 0);
}


vil_image_view_base_sptr
vil_openjpeg_image
::get_copy_view_reduced(unsigned int i0, unsigned int ni,
                        unsigned int j0, unsigned int nj,
                        unsigned int reduction) const
{
  if ( !this->impl_->is_valid_ )
    return nullptr;

  if ( reduction > this->impl_->header_.num_reductions_ )
    return nullptr;

  vil_pixel_format pixel_format = this->pixel_format();
  if ( pixel_format == VIL_PIXEL_FORMAT_UNKNOWN )
    return nullptr;

  // Set up decoder
  this->impl_->vstream_->seek(this->impl_->vstream_start_);
  vil_openjpeg_decoder decoder(this->impl_->opj_codec_format_);
  if ( !decoder.init_from_stream(reduction, this->impl_->vstream_.as_pointer()) )
    return nullptr;

  // Configure the ROI
  int adj_mask = ~( (1 << reduction) - 1);
  i0 &= adj_mask; j0 &= adj_mask;
  ni &= adj_mask; nj &= adj_mask;
  if ( !decoder.set_decode_area( i0, j0, i0 + ni, j0 + nj ) )
    return nullptr;

  // Decode the JPEG2000 data
  opj_image_t *opj_view = decoder.decode();
  if ( !opj_view || decoder.error() )
    return nullptr;

  // Adjust ROI for reduction
  i0 >>= reduction;
  j0 >>= reduction;
  ni >>= reduction;
  nj >>= reduction;

  // Copy pixels
  switch ( pixel_format )
  {
  case VIL_PIXEL_FORMAT_BYTE :
    return this->opj2vil<vxl_byte>(opj_view, i0, ni, j0, nj);
  case VIL_PIXEL_FORMAT_UINT_16 :
    return this->opj2vil<vxl_uint_16>(opj_view, i0, ni, j0, nj);
  case VIL_PIXEL_FORMAT_UINT_32 :
    return this->opj2vil<vxl_uint_32>(opj_view, i0, ni, j0, nj);
  default: return nullptr;
  }
}


template<typename T_PIXEL>
vil_image_view_base_sptr
vil_openjpeg_image
::opj2vil(
  void *opj_view,
  unsigned int i0, unsigned int ni, unsigned int j0, unsigned int nj) const
{
  auto *opj_view_t = reinterpret_cast<opj_image_t*>(opj_view);
  unsigned int np = opj_view_t->numcomps;

  vil_memory_chunk_sptr chunk =
    new vil_memory_chunk(ni*nj*np*sizeof(T_PIXEL), this->pixel_format());

  auto *vil_view_t = new vil_image_view<T_PIXEL>(
    chunk, reinterpret_cast<T_PIXEL*>(chunk->data()),
    ni, nj, np, 1, ni, ni*nj);

  for ( unsigned int p = 0; p < np; ++p )
  {
    T_PIXEL sign = opj_view_t->comps[p].sgnd ?
                   1 << (opj_view_t->comps[p].prec - 1) : 0;

    int *src_plane = opj_view_t->comps[p].data;
    T_PIXEL *dst_plane = vil_view_t->begin() + p*vil_view_t->planestep();

    for ( unsigned int j = 0; j < nj; ++j)
    {
      int *src_row = src_plane + (j0+j)*opj_view_t->comps[p].w + i0;
      T_PIXEL *dst_row = dst_plane + j*vil_view_t->jstep();

      for ( unsigned int i = 0; i < ni; ++i )
      {
        *(dst_row + i*vil_view_t->istep()) = src_row[i] + sign;
      }
    }
  }

  return vil_view_t;
}


bool
vil_openjpeg_image
::put_view(const vil_image_view_base& /*im*/, unsigned int /*i0*/, unsigned int /*j0*/)
{
  assert(!"openjpeg write support is currently not implemented");
  return false;
}


bool
vil_openjpeg_image
::get_property(char const* /*tag*/, void* /*property_value*/) const
{
  return false;
}
