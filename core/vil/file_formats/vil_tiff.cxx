//This is core/vil/file_formats/vil_tiff.cxx
#include <cstring>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "vil_tiff.h"
//:
// \file
// See vil_tiff.h for a description of this file.
//
// \author  awf@robots.ox.ac.uk
//
// \verbatim
//  Modifications:
//   2001-11-09 K.Y.McGaul  Use dflt value for orientation when it can't be read
//   2005-12-xx J.L. Mundy  Essentially a complete rewrite to support blocking.
//                          Cleaner struct: hdr params moved to vil_tiff_header
// \endverbatim

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream.h>
#include <vil/vil_property.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_copy.h>
#include <vil/vil_image_list.h>
#include "vil_tiff_header.h"
#include <vil/vil_exception.h>
//#define DEBUG

// Constants
char const* vil_tiff_format_tag = "tiff";

static unsigned nimg(TIFF* tif)
{
  if (!tif)
    return 0;
  TIFFSetDirectory(tif, 0);
  unsigned int dircount = 0;
  do {
    dircount++;
  } while (TIFFReadDirectory(tif));
  return dircount;
}


bool vil_tiff_file_format_probe(vil_stream* is)
{
  // The byte ordering in a TIFF image (usually) depends on the byte-order
  // of the writing host. The header is always 4 bytes.

  char hdr[4];
  auto read = (unsigned)is->read(hdr, sizeof hdr);
  if (read < sizeof hdr)
    return false;

  // First two bytes specify the file byte-order (0x4D4D=big, 0x4949=little).
  // Second two bytes specify the TIFF version (we expect 0x2A for tiff and 0x2B for bigtiff).
  // For information about BigTIFF refers to http://www.remotesensing.org/libtiff/bigtiffdesign.html
  // So,
  //   0x4D 0x4D 0x00 0x2A

  // and
  //   0x49 0x49 0x2A 0x00
  // or
  //   0x49 0x49 0x2B 0x00
  // are invalid TIFF headers.
  if (hdr[0]==0x4D && hdr[1]==0x4D &&
      hdr[2]==0x00 && (hdr[3]==0x2A || hdr[3] == 0x2B) )
    return true;

  else if (hdr[0]==0x49 && hdr[1]==0x49 &&
           (hdr[2]==0x2A || hdr[2] == 0x2B) && hdr[3]==0x00)
    return true;

  else if ( ((hdr[0]==0x4D && hdr[1]==0x4D) || (hdr[0]==0x49 && hdr[1]==0x49)) &&
            ((hdr[2]==0x00 && hdr[3]==0x2A) || (hdr[2]==0x2A && hdr[3]==0x00)) )  {
    std::cerr << __FILE__ ": suspicious TIFF header\n";
    return true; // allow it.
  }

  else
    return false;
}

struct tif_stream_structures
{
  tif_stream_structures(vil_stream *vs_)
    : vs(vs_), filesize(0) /*, sample_format( SAMPLEFORMAT_VOID ), buf(0) */
  { if (vs) vs->ref(); }

  ~tif_stream_structures() { /* delete[] buf; */ if (vs) vs->unref(); }

  TIFF* tif;
  vil_stream* vs;
  vil_streampos filesize;
};

static tsize_t vil_tiff_readproc(thandle_t h, tdata_t buf, tsize_t n)
{
  auto* p = (tif_stream_structures*)h;
  long long no = static_cast<long long>(n);
  if (no > p->filesize) p->filesize= n;
  //there should be no problem with this case because n
  //is also of type tsize_t
  auto ret = (tsize_t)p->vs->read(buf, n);
  return ret;
}

static tsize_t vil_tiff_writeproc(thandle_t h, tdata_t buf, tsize_t n)
{
  auto* p = (tif_stream_structures*)h;
  //there should be no problem with this case because n
  //is also of type tsize_t
  auto ret = (tsize_t)p->vs->write(buf, n);
  vil_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  return ret;
}

static toff_t vil_tiff_seekproc(thandle_t h, toff_t offset, int whence)
{
  auto* p = (tif_stream_structures*)h;
  if      (whence == SEEK_SET) p->vs->seek(offset);
  else if (whence == SEEK_CUR) p->vs->seek(p->vs->tell() + offset);
  else if (whence == SEEK_END) p->vs->seek(p->filesize + offset);
  vil_streampos s = p->vs->tell();
  if (s > p->filesize)
    p->filesize = s;
  return (toff_t)s;
}

static int vil_tiff_closeproc(thandle_t h)
{
  auto* p = (tif_stream_structures*)h;
  p->vs->unref();
  p->vs = nullptr;
  delete p;
  return 0;
}

static toff_t vil_tiff_sizeproc(thandle_t)
{
  // TODO
#ifdef DEBUG
  std::cerr << "Warning: vil_tiff_sizeproc() not yet implemented\n";
#endif
  return (toff_t)(-1); // could be unsigned - avoid compiler warning
}

static int vil_tiff_mapfileproc(thandle_t, tdata_t*, toff_t*)
{
  // TODO: Add mmap support to vil_tiff_mapfileproc
#ifdef DEBUG
  std::cerr << "Warning: mmap support not yet in vil_tiff_mapfileproc()\n";
#endif
  return 0;
}

static void vil_tiff_unmapfileproc(thandle_t, tdata_t, toff_t)
{
}


static TIFF* open_tiff(tif_stream_structures* tss, const char* mode)
{
  tss->vs->seek(0L);
#if HAS_GEOTIFF
  TIFF* tiff = XTIFFClientOpen("unknown filename",
                               mode, // read, enable strip chopping
                               (thandle_t)tss,
                               vil_tiff_readproc, vil_tiff_writeproc,
                               vil_tiff_seekproc, vil_tiff_closeproc,
                               vil_tiff_sizeproc,
                               vil_tiff_mapfileproc, vil_tiff_unmapfileproc);
#else // this file is only included if HAS TIFF is defined vil_file_format.cxx
  TIFF* tiff = TIFFClientOpen("unknown filename",
                              mode, // read, enable strip chopping
                              (thandle_t)tss,
                              vil_tiff_readproc, vil_tiff_writeproc,
                              vil_tiff_seekproc, vil_tiff_closeproc,
                              vil_tiff_sizeproc,
                              vil_tiff_mapfileproc, vil_tiff_unmapfileproc);
#endif // HAS_GEOTIFF

  if (!tiff)
    return nullptr;
  else
    return tiff;
}

vil_image_resource_sptr vil_tiff_file_format::make_input_image(vil_stream* is)
{
  if (!vil_tiff_file_format_probe(is))
    return nullptr;
  auto* tss = new tif_stream_structures(is);

  tss->tif = open_tiff(tss, "rC");

  if (!tss->tif)
    return nullptr;
  auto* h = new vil_tiff_header(tss->tif);

  if (!h->format_supported)
  {
#if HAS_GEOTIFF
    XTIFFClose(tss->tif);
#else
    TIFFClose(tss->tif);
#endif // HAS_GEOTIFF
    delete h;
    return nullptr;
  }
  unsigned n = nimg(tss->tif);
  tif_smart_ptr tif_sptr = new tif_ref_cnt(tss->tif);
  return new vil_tiff_image(tif_sptr, h, n);
}

vil_pyramid_image_resource_sptr
vil_tiff_file_format::make_input_pyramid_image(char const* file)
{
  bool trace = false;
  if (vil_image_list::vil_is_directory(file))
    return nullptr;
  TIFF* in  = TIFFOpen(file, "rC");
  if (!in)
    return nullptr;
  bool open_for_reading = true;
  if (trace) // find test failure
    std::cerr << "make_input_pyramid_image::opening multi-image tiff pyramid resource\n";
  tif_smart_ptr tif_sptr = new tif_ref_cnt(in);
  vil_pyramid_image_resource_sptr pyr =
    new vil_tiff_pyramid_resource(tif_sptr, open_for_reading);
  if (pyr->nlevels()<=1)
    return nullptr;
  else
    return pyr;
}

static std::string level_filename(std::string& directory, std::string& filename,
                                 unsigned level)
{
  std::string slash;

#ifdef _WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  std::stringstream cs;
  cs << level;
  return directory + slash + filename + cs.str();
}

vil_pyramid_image_resource_sptr vil_tiff_file_format::
make_pyramid_image_from_base(char const* file,
                             vil_image_resource_sptr const& base_image,
                             unsigned nlevels,
                             char const* temp_dir)
{
  {//scope for writing the resources
    vil_pyramid_image_resource_sptr pyr = make_pyramid_output_image(file);
    pyr->put_resource(base_image);
    //Create the other pyramid levels
    {//scope for resource files
      std::string d = temp_dir;
      std::string fn = "tempR";
      vil_image_resource_sptr image = base_image;
      for (unsigned L = 1; L<nlevels; ++L)
      {
        std::cout << "Decimating Level " << L << std::endl;
        std::string full_filename = level_filename(d, fn, L) + ".tif";
        image =
          vil_pyramid_image_resource::decimate(image, full_filename.c_str());
      }
    }//end program scope to close resource files

    //reopen them for reading
    {//scope for il resources
      vil_image_list il(temp_dir);
      std::vector<vil_image_resource_sptr> rescs = il.resources();
      for (auto & resc : rescs)
        pyr->put_resource(resc);
    }//close il resources
  }//close pyr

  //clean up the temporary directory
  vil_image_list vl(temp_dir);
  if (!vl.clean_directory())
  {
    std::cout <<"Warning: In vil_tiff::make_pyramid_from_base(..) -"
             << " temporary directory not cleaned\n";
  }
  //reopen for reading
  return make_input_pyramid_image(file);
}

vil_blocked_image_resource_sptr
vil_tiff_file_format::make_blocked_output_image(vil_stream* vs,
                                                unsigned nx,
                                                unsigned ny,
                                                unsigned nplanes,
                                                unsigned size_block_i,
                                                unsigned size_block_j,
                                                enum vil_pixel_format format)
{
  if (size_block_i%16!=0||size_block_j%16!=0)
  {
    std::cerr << "In vil_tiff_file_format - Block dimensions must be a multiple of 16\n";
    return nullptr;
  }

  auto* tss = new tif_stream_structures(vs);
  tss->filesize = 0;
  std::string mode("w");
  vxl_uint_64 size_needed = vxl_uint_64(nx) * vxl_uint_64(ny) * vxl_uint_64(nplanes) * vil_pixel_format_sizeof_components(format) * vil_pixel_format_num_components(format);
  bool const bigtiff_needed = size_needed >= vxl_uint_64(0x7FFFFFFF);
  if (bigtiff_needed)
    mode += '8';   // enable bigtiff
  tss->tif = open_tiff(tss, mode.c_str());
  if (!tss->tif)
    return nullptr;

  //size_block_i==0 && size_block_j==0 specifies strips of one scanline
  //this constructor for h defines that the resource is to
  //be setup for writing
  auto* h = new vil_tiff_header(tss->tif, nx, ny, nplanes,
                                           format, size_block_i, size_block_j);
  if (!h->format_supported)
  {
#if HAS_GEOTIFF
    XTIFFClose(tss->tif);
#else
    TIFFClose(tss->tif);
#endif // HAS_GEOTIFF
    delete h;
    return nullptr;
  }
  tif_smart_ptr tsptr = new tif_ref_cnt(tss->tif);
  return new vil_tiff_image(tsptr, h);
}


vil_image_resource_sptr
vil_tiff_file_format::make_output_image(vil_stream* vs,
                                        unsigned ni,
                                        unsigned nj,
                                        unsigned nplanes,
                                        enum vil_pixel_format format)
{
  return make_blocked_output_image(vs, ni, nj, nplanes, 0, 0, format).ptr();
}

vil_pyramid_image_resource_sptr
vil_tiff_file_format::make_pyramid_output_image(char const* filename)
{
  TIFF* out  = TIFFOpen(filename, "w");
  if (!out)
    return nullptr;
  bool open_for_reading = false;
  tif_smart_ptr tsptr = new tif_ref_cnt(out);
  return new vil_tiff_pyramid_resource(tsptr, open_for_reading);
}

char const* vil_tiff_file_format::tag() const
{
  return vil_tiff_format_tag;
}

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////


vil_tiff_image::vil_tiff_image(tif_smart_ptr const& tif_sptr,
                               vil_tiff_header* th, const unsigned nimages):
    t_(tif_sptr), h_(th), index_(0), nimages_(nimages)
{
}

bool vil_tiff_image::get_property(char const * tag, void * value) const
{
  if (std::strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned*>(value) = h_->bits_per_sample.val;
    return true;
  }
  if (std::strcmp(vil_property_size_block_i, tag)==0)
  {
    if (!h_->is_tiled())
      return false;
    if (value)
      *static_cast<unsigned*>(value) = this->size_block_i();
    return true;
  }

  if (std::strcmp(vil_property_size_block_j, tag)==0)
  {
    if (!h_->is_tiled())
      return false;
    if (value)
      *static_cast<unsigned*>(value) = this->size_block_j();
    return true;
  }

  return false;
}

bool vil_tiff_image::set_compression_method(compression_methods cm)
{
  TIFF* const tif = t_.tif();
  if (tif)
  {
    int status = TIFFSetField(tif, TIFFTAG_COMPRESSION, int(cm));
    return bool(status);
  }
  else
    return false;
}

bool vil_tiff_image::set_compression_quality(int quality)
{
  TIFF* const tif = t_.tif();
  if (tif)
  {
    int status = TIFFSetField(tif, TIFFTAG_JPEGQUALITY, quality);
    return bool(status);
  }
  else
    return false;
}

#if HAS_GEOTIFF
vil_geotiff_header* vil_tiff_image::get_geotiff_header()
{
  auto* gtif = new vil_geotiff_header(t_.tif());
  if (gtif->gtif_number_of_keys() == 0) {
    delete gtif;
    return nullptr;
  }

  return gtif;
}
#endif

vil_pixel_format vil_tiff_image::pixel_format() const
{
  return h_->pix_fmt;
}

vil_tiff_image::~vil_tiff_image()
{
  delete h_;
}

//////
//Lifted from nitf2.  Maybe generalize to support other file formats
//////
char const* vil_tiff_image::file_format() const
{
  return vil_tiff_format_tag;
}

static void tif_swap16(vxl_byte *a, unsigned n)
{
  for (unsigned i = 0; i < n * 2; i += 2)
    std::swap( a[i+0], a[i+1] );
}

static void tif_swap32(vxl_byte *a, unsigned n)
{
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    std::swap( a[i+0], a[i+3] );
    std::swap( a[i+1], a[i+2] );
  }
}

static void endian_swap( vxl_byte* a, unsigned n_bytes,
                         unsigned bytes_per_sample)
{
  switch ( bytes_per_sample ) {
   case 1: break; // do nothing
   case 2: tif_swap16( a, n_bytes / 2 ); break; //16 bit
   case 4: tif_swap32( a, n_bytes / 4 ); break; //32 bit
   default: assert(!"Unsupported number of bytes per sample.");
  }
}

template<> bool* tiff_byte_align_data<bool>(bool* in_data, unsigned num_samples, unsigned in_bits_per_sample, bool* out_data)
{
  switch (sizeof(bool))
  {
   case 1:
    tiff_byte_align_data((vxl_byte*)in_data, num_samples, in_bits_per_sample, (vxl_byte*)out_data);
    break;
   case 2:
    tiff_byte_align_data((vxl_uint_16*)in_data, num_samples, in_bits_per_sample, (vxl_uint_16*)out_data);
    break;
   case 4:
    tiff_byte_align_data((vxl_uint_32*)in_data, num_samples, in_bits_per_sample, (vxl_uint_32*)out_data);
    break;
   default:
    assert(!"Unsupported size of bool in tiff file format.");
  }
  return out_data;
}

// the sample is an integral data type
bool integral_type(unsigned bits_per_sample)
{
  switch (bits_per_sample)
  {
   case  8:
   case 16:
   case 32: return true;
   default: break;
  }
  return false;
}

template< class T >
vil_memory_chunk_sptr
tiff_maybe_byte_align_data(vil_memory_chunk_sptr in_data,
                           unsigned num_samples,
                           unsigned in_bits_per_sample,
                           unsigned bytes_per_block)
{
  if (!integral_type(in_bits_per_sample))
  {
    vil_memory_chunk_sptr new_memory = new vil_memory_chunk(bytes_per_block, in_data->pixel_format());
#ifdef DEBUG
    std::cout << "Debug tiff_byte_align_data:"
             << "  Num Samples = " << num_samples
             << "  Input Bits/Sample = " << in_bits_per_sample
             << "  Bytes/Block = " << bytes_per_block
             << "  Output Bytes/Sample = " << vil_pixel_format_sizeof_components(in_data->pixel_format())
             << std::flush;
#endif
    T* out_ptr = reinterpret_cast<T*>(new_memory->data());
    T* in_ptr = reinterpret_cast<T*>(in_data->data());
    tiff_byte_align_data(in_ptr, num_samples, in_bits_per_sample, out_ptr );
#ifdef DEBUG
    std::cout << " .\n" << std::flush;
#endif
    return new_memory;
  }
  return in_data;
}

// don't do anything for float and double (bit shifting isn't allowed)
template<> vil_memory_chunk_sptr tiff_maybe_byte_align_data<float>
           ( vil_memory_chunk_sptr in_data    ,
             unsigned /* num_samples */       ,
             unsigned /* in_bits_per_sample */,
             unsigned /* bytes per block */   )
{ return in_data; }

template<> vil_memory_chunk_sptr tiff_maybe_byte_align_data<double>
           ( vil_memory_chunk_sptr in_data    ,
             unsigned /* num_samples */       ,
             unsigned /* in_bits_per_sample */,
             unsigned /* bytes per block */   )
{ return in_data; }

////////// End of lifted material //////

// simple virtual methods on vil_image_resource
unsigned vil_tiff_image::nplanes() const
{
  return h_->nplanes;
}

unsigned vil_tiff_image::ni() const
{
  if (h_->image_width.valid)
    return h_->image_width.val;
  return 0;
}

unsigned vil_tiff_image::nj() const
{
  if (h_->image_length.valid)
    return h_->image_length.val;
  return 0;
}

//: block size in cols
unsigned vil_tiff_image::size_block_i() const
{
  if (h_->tile_width.valid)
    return static_cast<unsigned>(h_->tile_width.val);
  if (h_->image_width.valid)
    return static_cast<unsigned>(h_->image_width.val);
  return 0;
}

//: block size in rows.
// For strips, the number of rows per strip can be larger
// than the image length but data is only valid for the number of actual
// image rows. For images with multiple strips, the last strip may be
// cropped by the actual number of image rows.
unsigned vil_tiff_image::size_block_j() const
{
  if (h_->tile_length.valid)
    return static_cast<unsigned>(h_->tile_length.val);

  auto bps = static_cast<unsigned>(h_->bytes_per_strip());
  auto bpl = static_cast<unsigned>(h_->bytes_per_line());
  unsigned size = bps/bpl;
  return size;
  return 0;
}

//: Number of blocks in image width
unsigned vil_tiff_image::n_block_i() const
{
  if (h_->tile_width.valid)
    return static_cast<unsigned>(h_->tiles_across());
  return 1;
}

//: Number of blocks in image height
unsigned vil_tiff_image::n_block_j() const
{
  if (h_->tile_length.valid&&h_->image_length.valid)
    return static_cast<unsigned>(h_->tiles_down());
  return static_cast<unsigned>(h_->strips_per_image());
}

///// end of simple virtual methods

unsigned vil_tiff_image::
block_index(unsigned block_i, unsigned block_j) const
{
  return block_j*n_block_i() + block_i;
}

// the number of samples per block, irrespective of bit resolution
unsigned vil_tiff_image::samples_per_block() const
{
  if (h_->samples_per_pixel.valid)
    return static_cast<unsigned>(h_->samples_per_pixel.val*
                                 size_block_i()*size_block_j());
  return 0;
}

//: Transfer data from block to memory chunk, row by row
// Since view and block are the same we can just blast across
void vil_tiff_image::copy_byte_block(vxl_byte* data, const vxl_uint_32 nbytes, vil_memory_chunk_sptr& cnk) const
{
  if (nbytes==0)
    return;
  auto* c_data = reinterpret_cast<vxl_byte*>(cnk->data());
  std::memcpy(c_data, data, nbytes);
}

//: map the input buffer into the view.
// Note strips won't trigger byte
// alignment, because they are already aligned at this point.
vil_image_view_base_sptr vil_tiff_image::
view_from_buffer(vil_pixel_format& fmt, vil_memory_chunk_sptr const& buf,
                 unsigned samples_per_block, unsigned bits_per_sample
                ) const
{
  vil_image_view_base_sptr view = nullptr;
  vil_memory_chunk_sptr  buf_out;
  unsigned spp = h_->samples_per_pixel.val;
  switch (fmt)
  {
#define GET_BLOCK_CASE(FORMAT, T) \
   case FORMAT: { \
    vil_image_view_base_sptr view; \
    buf_out = tiff_maybe_byte_align_data<T>(buf, samples_per_block, \
                                            bits_per_sample, \
                                            samples_per_block*vil_pixel_format_sizeof_components(fmt)); \
    view = new vil_image_view<T>(buf_out, reinterpret_cast<T*>(buf_out->data()), \
                                 size_block_i(), size_block_j(), \
                                 spp, spp, size_block_i()*spp, 1); \
    return view; }
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
#if VXL_HAS_INT_64
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
    GET_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef GET_BLOCK_CASE
   default:
    assert(!"Unknown vil data type in tiff file format");
    break;
  }
  return view;
}

// this internal block accessor is used for both tiled and
// striped encodings
vil_image_view_base_sptr
vil_tiff_image::get_block( unsigned block_index_i,
                           unsigned block_index_j ) const
{
  // the only two possibilities
  assert(h_->is_tiled() || h_->is_striped());
  //
  // If there are multiple images in the file it is
  // necessary to set the TIFF directory and file header corresponding to
  // this resource according to the index
  //
  if (nimages_>1)
  {
    if (TIFFSetDirectory(t_.tif(), index_)<=0)
      return nullptr;
    auto* h = new vil_tiff_header(t_.tif());
    //Cast away const
    auto* ti = (vil_tiff_image*)this;
    delete h_;
    ti->h_=h;
  }

  vil_image_view_base_sptr view = nullptr;

  //allocate input memory
  // input memory
  unsigned encoded_block_size = h_->encoded_bytes_per_block();
  assert(encoded_block_size>0);
  //vxl_byte* data = new vxl_byte[encoded_block_size];

  //compute the block index
  unsigned blk_indx = this->block_index(block_index_i, block_index_j);


  vil_pixel_format fmt = vil_pixel_format_component_format(h_->pix_fmt);

  // input memory chunk
  vil_memory_chunk_sptr buf =
    new vil_memory_chunk(encoded_block_size, fmt);
  unsigned expanded_sample_bytes = vil_pixel_format_sizeof_components(fmt);


  if (h_->is_tiled())
  {
	auto* data = new vxl_byte[encoded_block_size];
    if (TIFFReadEncodedTile(t_.tif(), blk_indx, data, (tsize_t) -1)<=0)
    {
      delete [] data;
      return view;
    }
    this->copy_byte_block(data, encoded_block_size, buf);
    delete [] data;
    if (h_->need_byte_swap())
      endian_swap( reinterpret_cast<vxl_byte*>(buf->data()),
                   encoded_block_size,
                   expanded_sample_bytes);
    return this->fill_block_from_tile(buf);
  }

  if (h_->is_striped()&&h_->planar_config.val ==1)
  {
	auto* data = new vxl_byte[encoded_block_size];
    if (TIFFReadEncodedStrip(t_.tif(), blk_indx, data, (tsize_t) -1)<=0)
    {
      delete [] data;
      return view;
    }
    this->copy_byte_block(data, encoded_block_size, buf);
    delete [] data;
    if (h_->need_byte_swap())
      endian_swap( reinterpret_cast<vxl_byte*>(buf->data()),
                   encoded_block_size,
                   expanded_sample_bytes);
    return this->fill_block_from_strip(buf);
  }else if (h_->is_striped()&&h_->planar_config.val ==2)
    {
      // planes are storted in multiple strips (n_planes) for planar_config == 2
      // the layout of the file is sequential by band (plane)
      //  b0 ---- ...  ---- b0|b1 ---- ...  ---- b1|b2 ---- ...  ---- b2| ... |b_np_1 ---- ...  ---- b_np-1|
      //  |strip 0|srip 1|... |strip_spp|...       |strip_2*spp|...           |strip_(np-1)*spp| ...
      //
      //  b0 -> b_np-1 - bands 0 through band np-1;
      //  np - number of planes,  spp - number of strips per plane
      //
      size_t nplanes = h_->nplanes;
      size_t strip_data_size = encoded_block_size/nplanes;
      size_t n_rows = h_->image_length.val;
      size_t rows_per_strip = h_->rows_per_strip.val;
      size_t strips_per_plane = n_rows/rows_per_strip;
      if(strips_per_plane*rows_per_strip < n_rows)
        strips_per_plane++;
      size_t bytes_per_row = encoded_block_size/rows_per_strip;
      size_t samples_per_pixel = h_->samples_per_pixel.val;//same as nplanes
      size_t bytes_per_row_per_sample = bytes_per_row/samples_per_pixel;
      size_t ni = h_->image_width.val;// pixels per row
      size_t bytes_per_pixel = bytes_per_row/ni;
      size_t bytes_per_sample = h_->bytes_per_sample();

      // hold the strips extracted from each plane section of the file
      std::vector<vxl_byte*> strip_plane_data(nplanes);

      // read the multiple strips -  one from each plane in the file
      for(size_t s = 0; s<samples_per_pixel; ++s){
        strip_plane_data[s] = new vxl_byte[strip_data_size];
        size_t strip_indx = blk_indx + s*strips_per_plane;
        size_t strip_size = TIFFReadEncodedStrip(t_.tif(), strip_indx, strip_plane_data[s], (tsize_t) -1);
        if (strip_size <=0)// if the read fails, bail--
          {
            for(size_t d = 0; d<=s; ++d)
              delete [] strip_plane_data[d];
            std::cout << "Error in reading tiff strip - " << strip_size
                      << " bytes returned instead of " << strip_data_size << std::endl;
            return view;
          }
      }
      //the start of block buffer to be filled from the cached strips
      auto* buf_adr = reinterpret_cast<vxl_byte*>(buf->data());

      // iterate over rows of the band strips and interleave bands as:
      //      |b0|b1|..b_np-1||b0|b1|..b_np-1||b0|b1|..b_np-1||b0|b1|..b_np-1|
      //row r |    pix 0     ||    pix1      ||  pix2 ...    ||  pix (ni-1)
      //  where np is number of samples (planes) per pixel, ni pixels per row
      //
      for(size_t r = 0; r< rows_per_strip; ++r){
        size_t rb_off = r*bytes_per_row, rs_off =r*bytes_per_row_per_sample;
        for(size_t i = 0; i<ni; ++i){
          size_t ib_off = i*bytes_per_pixel, is_off = i*bytes_per_sample;
          for(size_t s =0; s<samples_per_pixel; ++s){
            size_t sb_off = s*bytes_per_sample;
            for(size_t b = 0; b<bytes_per_sample; ++b){
            size_t buf_off =  rb_off +  ib_off + sb_off + b;
            size_t strip_off = rs_off + is_off +b;
            *(buf_adr + buf_off) = *(strip_plane_data[s] + strip_off);
            }
          }
        }
      }
      // delete the chached strips
      for(size_t p = 0; p<nplanes; ++p)
        delete [] strip_plane_data[p];

      //might need to correct an endian mismatch
      if (h_->need_byte_swap())
        endian_swap( reinterpret_cast<vxl_byte*>(buf->data()), encoded_block_size,  expanded_sample_bytes);

      // transfer the buffer to the block view
      return this->fill_block_from_strip(buf);
    }
  return view;
}

//decode tiles: the tile is a contiguous raster scan of potentially
//interleaved samples. This is an easy case since the tile is a
//contiguous raster scan.
vil_image_view_base_sptr vil_tiff_image::
fill_block_from_tile(vil_memory_chunk_sptr const & buf) const
{
  vil_image_view_base_sptr view = nullptr;

  //the size of the buffer when expanded to byte representation
  unsigned samples_per_block = this->samples_per_block();
  assert(samples_per_block>0);

  vil_pixel_format fmt = vil_pixel_format_component_format(h_->pix_fmt);
  view = view_from_buffer(fmt, buf, samples_per_block, h_->bits_per_sample.val);
  return view;
}

// decode strips.  The strip is somewhat different from the tile in that
// it is organized around scan lines. If bits_per_pixel is not an integral
// number of bytes then the last packed byte in the scan line will be only
// partially filled. The header function, bytes_per_line() gives the actual
// size of a scan line in the packed strip. The total size of the strip
// in bytes is normally size_block_j()*bytes_per_line() but the last strip
// may be truncated.
vil_image_view_base_sptr vil_tiff_image::fill_block_from_strip(vil_memory_chunk_sptr const & buf) const
{
  vil_image_view_base_sptr view = nullptr;
  vxl_uint_32 tl = size_block_j();

  unsigned bpl = h_->bytes_per_line();
  unsigned bytes_per_strip = h_->bytes_per_strip();
  unsigned lines_per_strip = bytes_per_strip/bpl;
  vil_pixel_format fmt = vil_pixel_format_component_format(h_->pix_fmt);
  unsigned expanded_bytes_per_sample =
    vil_pixel_format_sizeof_components(fmt);
  unsigned spl = h_->samples_per_line();
  unsigned bytes_expanded_line = spl*expanded_bytes_per_sample;
  //note here we make the last strip a full sized block to avoid
  //the messyness of multiple block sizes
  unsigned expanded_bytes_per_strip = tl*bytes_expanded_line;

  //pointer into the input packed strip buffer
  auto* buf_ptr = reinterpret_cast<vxl_byte*>(buf->data());

  //buffer for each scan line
  vil_memory_chunk_sptr line_buf = new vil_memory_chunk(bpl, fmt);

  //a buffer of zeros for filling partial strips to tile size
  vil_memory_chunk_sptr zero_buf = new vil_memory_chunk(bytes_expanded_line, fmt);
  auto* zero_ptr = reinterpret_cast<vxl_byte*>(zero_buf->data());
  for (unsigned i = 0; i<bytes_expanded_line; ++i)
    zero_ptr[i]=0;

  //buffer for the final unpacked output block
  vil_memory_chunk_sptr block_buf =
    new vil_memory_chunk(expanded_bytes_per_strip, fmt);
  auto* block_ptr = reinterpret_cast<vxl_byte*>(block_buf->data());
  //read scan lines from the strip and paste into the block
  for (unsigned j = 0; j<tl; ++j, buf_ptr+=bpl,
       block_ptr+=bytes_expanded_line)
  {
    if (j<lines_per_strip)
    {
      // get a row from the input buffer
      copy_byte_block(buf_ptr, bpl, line_buf);
      vil_memory_chunk_sptr out_line_buf;
      switch (fmt)
      {
#define GET_LINE_CASE(FORMAT, T)               \
       case FORMAT:\
        out_line_buf = \
          tiff_maybe_byte_align_data<T>(line_buf,\
                                        spl, h_->bits_per_sample.val,\
                                        bytes_expanded_line); \
        break
        GET_LINE_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
#if VXL_HAS_INT_64
        GET_LINE_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
        GET_LINE_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
        GET_LINE_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef GET_LINE_CASE
       default:
        assert(!"Unknown vil data type in tiff file format");
        break;
      }
      //now we have the unpacked scan line in out_buf so copy to the view
      //buffer.
      auto* out_line_buf_ptr =
        reinterpret_cast<vxl_byte*>(out_line_buf->data());

      std::memcpy(block_ptr, out_line_buf_ptr, bytes_expanded_line);
    }
    else
      std::memcpy(block_ptr, zero_ptr, bytes_expanded_line);
  }
  return this->view_from_buffer(fmt, block_buf, spl*tl, expanded_bytes_per_sample*8);
}

void vil_tiff_image::pad_block_with_zeros(unsigned ioff, unsigned joff,
                                          unsigned iclip, unsigned jclip,
                                          unsigned bytes_per_pixel,
                                          vxl_byte* block_buf)
{
  unsigned jstep = size_block_i()*bytes_per_pixel;
  unsigned row_start = ioff*bytes_per_pixel;
  unsigned bptr = 0;
  //fill leading part with zeroes
  if (ioff>0||joff>0)
    for (unsigned j = 0; j<joff-1; ++j)
    {
      unsigned row_ptr = row_start;
      for (unsigned i = 0; i<ioff-1; ++i)
      {
        for (unsigned p = 0; p<nplanes(); ++p)
          *(block_buf + bptr + row_ptr + p) = 0;
        row_ptr += bytes_per_pixel;
      }
      bptr += jstep;
    }
  bptr = jstep*jclip;
  row_start = iclip*bytes_per_pixel;
  if (iclip>0||jclip>0)
    for (unsigned j = jclip; j<size_block_j(); ++j)
    {
      unsigned row_ptr = row_start;
      for (unsigned i = iclip; i<size_block_i(); ++i)
      {
        for (unsigned p = 0; p<nplanes(); ++p)
          *(block_buf + bptr + row_ptr + p) = 0;
        row_ptr += bytes_per_pixel;
      }
      bptr += jstep;
    }
}

void vil_tiff_image::fill_block_from_view(unsigned bi, unsigned bj,
                                          unsigned i0, unsigned j0,
                                          unsigned ioff, unsigned joff,
                                          unsigned iclip, unsigned jclip,
                                          const vil_image_view_base& im,
                                          vxl_byte*& block_buf)
{
  unsigned bytes_per_sample = h_->bytes_per_sample();
  unsigned bytes_per_pixel = bytes_per_sample*nplanes();
  unsigned sbi = size_block_i(), sbj = size_block_j();
  unsigned bytes_per_block=bytes_per_pixel*sbi*sbj;
  unsigned view_i0 = bi*sbi-i0, view_j0 = bj*sbj-j0;
  unsigned block_jstep = sbi*bytes_per_pixel;
#if 0
  //Causes warnings. Leave here to document default values
  unsigned view_istep = 1, view_jstep = im.ni()*bytes_per_pixel, view_pstep = 1;
#endif
  std::ptrdiff_t view_istep, view_jstep, view_pstep;
  vxl_byte* view_buf;
  //Cast the pixel type and reinterpret upper_left_ptr as a byte array.
  switch (h_->pix_fmt)
  {
#define GET_VIEW_PTR(FORMAT, T) \
   case FORMAT: { \
    vil_image_view<T> view = static_cast<const vil_image_view<T>& >(im);\
    view_istep = view.istep(); view_jstep = view.jstep(); view_pstep = view.planestep(); \
    view_buf = reinterpret_cast<vxl_byte*>(view.top_left_ptr());\
   } break
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
#if VXL_HAS_INT_64
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_BOOL, bool);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_FLOAT, float);
    GET_VIEW_PTR(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef GET_VIEW_PTR
   default:
    assert(!"Unknown vil data type.");
    return;
  }
  //initial index into block buffer
  unsigned bptr = joff*block_jstep;
  unsigned ibstart = ioff*bytes_per_pixel;
  std::ptrdiff_t vistp = view_istep*bytes_per_sample;
  std::ptrdiff_t vjstp = view_jstep*bytes_per_sample;
  std::ptrdiff_t vpstp = view_pstep*bytes_per_sample;
  //initial index into view buffer
  // note that it is necessary to add the offset to the start of the
  // current block within the view, (view_i0, view_j0)
  std::ptrdiff_t vptr = (view_j0 + joff)*vjstp;
  unsigned ivstart = (view_i0 + ioff)*bytes_per_pixel;
  for (unsigned j = joff; j<jclip; ++j)
  {
    std::ptrdiff_t vrow_ptr = ivstart;
    std::ptrdiff_t brow_ptr = ibstart;
    for (unsigned i = ioff; i<iclip; ++i)
    {
      std::ptrdiff_t bpptr = 0, vpptr = 0;
      for (unsigned p = 0; p<nplanes(); ++p)
      {
        for (unsigned b = 0; b<bytes_per_sample; ++b)
          *(block_buf + bptr + brow_ptr + bpptr + b) =
            *(view_buf + vptr + vrow_ptr + vpptr + b);
        bpptr += bytes_per_sample; vpptr += vpstp;
      }
      brow_ptr += bytes_per_pixel; vrow_ptr += vistp;
    }
    bptr += block_jstep; vptr += vjstp;
  }

  //handle the case of bool  (other packed formats not supported for writing)
  if (this->pixel_format() == VIL_PIXEL_FORMAT_BOOL)
  {
    unsigned outsize = (bytes_per_block+7*sizeof(bool))/(8*sizeof(bool));
    auto* outbuf = new vxl_byte[outsize];
    this->bitpack_block(bytes_per_block, block_buf, outbuf);
    delete [] block_buf;
    block_buf = outbuf; // bytes_per_block=outsize;
  }
}

bool vil_tiff_image::write_block_to_file(unsigned bi, unsigned bj,
                                         unsigned block_size_bytes,
                                         vxl_byte* block_buf)
{
  unsigned blk_indx = this->block_index(bi, bj);
  if (h_->is_tiled())
    return TIFFWriteEncodedTile(t_.tif(), blk_indx, block_buf,
                                block_size_bytes)>0;
  if (h_->is_striped())
    return TIFFWriteEncodedStrip(t_.tif(), blk_indx, block_buf,
                                 block_size_bytes ) > 0;
  return false;
}

// Just support packing of bool data for now
// ultimately we need the opposite of maybe_byte_align_data
void vil_tiff_image::bitpack_block(unsigned bytes_per_block,
                                   const vxl_byte* in_block_buf,
                                   vxl_byte* out_block_buf)
{
  unsigned bytes_per_bool = sizeof(bool);
  auto* bl = new vxl_byte[bytes_per_bool];
  unsigned bitctr = 0;
  unsigned outctr = 0;
  vxl_byte packed_byte=0;
  for (unsigned i = 0; i<bytes_per_block; )
  {
    //test for a completed packed byte
    if (bitctr==8)
    {
      bitctr = 0;
      out_block_buf[outctr] = packed_byte;
      packed_byte = 0;
      ++outctr;
    }
    //pack a bool into the next bit
    for (unsigned b = 0; b<bytes_per_bool; ++b)
      bl[b] = *(in_block_buf + i + b);
    bool blv = *(reinterpret_cast<bool*>(bl));
    if (blv)
      packed_byte |= vxl_byte(1<<(7-bitctr)); //set a "1"
    else
      packed_byte &= vxl_byte(~(1<<(7-bitctr)));//set a "0"
    ++bitctr;

    i+=bytes_per_bool;
    if (i>=bytes_per_block) //output last (partial) byte
      out_block_buf[outctr] = packed_byte;
  }
  delete [] bl;
}

//an internal form of put_block for convenience
//write the indicated block to file, padding with zeros if necessary
//image view im is an arbitrary region of image that has to be decomposed into
//blocks. The resource is written with zeros if the input view doesn't
//correspond to exact block boundaries.  Subsequent put_view calls could
//fill in the missing image data.
bool vil_tiff_image::put_block(unsigned bi, unsigned bj, unsigned i0,
                               unsigned j0, const vil_image_view_base& im)
{
  //Get the block offset and clipping parameters

  //ioff and joff are the offsets within a block to the start of valid data
  unsigned ioff =0, joff = 0;
  unsigned sbi = size_block_i(), sbj = size_block_j();
  unsigned iclip =sbi , jclip = sbj;
  //column offset into block. fill [0->ioff-1]
  if (bi*sbi<i0&&(bi+1)*sbi>i0)
    if (!block_i_offset(bi, i0, ioff))
      return false;
  //row offset into block fill [0->joff-1]
  if (bj*sbj<j0&&(bj+1)*sbj>j0)
    if (!block_j_offset(bj, j0, joff))
      return false;

  //iclip and jclip are the start of invalid data at the right and
  //bottom of partially filled blocks

  //right block margin to be padded [iclip -> size_block_i()-1]
  if ( (bi+1)*sbi > (im.ni()+i0) )
  {
    iclip = (i0+im.ni())-bi*sbi;
    if (iclip > sbi)
      return false;
  }

  //bottom block margin to be padded [jclip -> size_block_j()-1]
  if ( (bj+1)*sbj > (im.nj()+j0) )
  {
    jclip = (j0+im.nj())-bj*sbj;
    if (jclip > sbj)
      return false;
  }
  unsigned bps = h_->bytes_per_sample();
  unsigned bytes_per_pixel = bps*nplanes();

  unsigned bytes_per_block = bytes_per_pixel*sbi*sbj;


  //the data buffer for the block
  auto* block_buf = new vxl_byte[bytes_per_block];

  this->pad_block_with_zeros(ioff, joff, iclip, jclip,
                             bytes_per_pixel, block_buf);


  this->fill_block_from_view(bi, bj, i0, j0, ioff, joff, iclip, jclip,
                             im, block_buf);
  //write the block to the tiff file
  bool good_write = write_block_to_file(bi, bj, bytes_per_block, block_buf);
  delete [] block_buf;
  return good_write;
}

bool vil_tiff_image::put_view(const vil_image_view_base& im,
                              unsigned i0, unsigned j0)
{
  if (!vil_image_resource::view_fits(im, i0, j0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_tiff_image::put_view"));
    return false;
  }

  unsigned tw = size_block_i(), tl = size_block_j();
  if (tw==0||tl==0)
    return false;
  unsigned  bi_start = i0/tw, bi_end = (i0+im.ni()-1)/tw;
  unsigned  bj_start = j0/tl, bj_end = (j0+im.nj()-1)/tl;
  for (unsigned bi = bi_start; bi<=bi_end; ++bi)
    for (unsigned bj = bj_start; bj<=bj_end; ++bj)
      if (!this->put_block(bi, bj, i0, j0, im))
        return false;
  return true;
}

// The virtual put_block method. In this case the view is a complete block
bool vil_tiff_image::put_block( unsigned  block_index_i,
                                unsigned  block_index_j,
                                const vil_image_view_base& blk )
{
  if (blk.ni()==0||blk.nj()==0)
    return false;
  unsigned sbi = this->size_block_i(), sbj = this->size_block_j();
  unsigned bps = h_->bytes_per_sample();
  unsigned bytes_per_pixel = bps*nplanes();

  unsigned bytes_per_block = bytes_per_pixel*sbi*sbj;

  //the data buffer for the block
  auto* block_buf = new vxl_byte[bytes_per_block];

  this->fill_block_from_view(0, 0, 0, 0, 0, 0,sbi, sbj, blk, block_buf);

  //write the block to the tiff file
  bool good_write = write_block_to_file(block_index_i, block_index_j, bytes_per_block, block_buf);
  delete [] block_buf;
  return good_write;
}

//Begin pyramid resource
static bool level_compare(tiff_pyramid_level* const l1, tiff_pyramid_level* const l2)
{
  assert(l1&&l2);
  return l1->ni_ > l2->ni_;
}

//: Assumes that the image in level 0 is the largest
void vil_tiff_pyramid_resource::normalize_scales()
{
  unsigned nlevels = this->nlevels();
  if (nlevels==0)
    return;
  levels_[0]->scale_ = 1.0f;
  if (nlevels==1)
    return;
  auto ni0 = static_cast<float>(levels_[0]->ni_);
  for (unsigned i = 1; i<nlevels; ++i)
    levels_[i]->scale_ = static_cast<float>(levels_[i]->ni_)/ni0;
}

//:find the level closest to the specified scale
tiff_pyramid_level* vil_tiff_pyramid_resource::closest(const float scale) const
{
  unsigned nlevels = this->nlevels();
  if (nlevels == 0)
    return nullptr;
  if (nlevels == 1)
    return levels_[0];
  float mind = 1.0e08f;//huge scale;
  unsigned lmin = 0;
  for (unsigned i = 0; i<nlevels; ++i)
  {
    float ds = std::fabs(scale - levels_[i]->scale_);
    if (ds<mind)
    {
      mind = ds;
      lmin = i;
    }
  }
  tiff_pyramid_level* pl = levels_[lmin];
  if (pl)
    pl->cur_level_ = lmin;
  return pl;
}

vil_tiff_pyramid_resource::vil_tiff_pyramid_resource()
  : read_(true), t_(nullptr)
{
}

vil_tiff_pyramid_resource::
vil_tiff_pyramid_resource(tif_smart_ptr const& t, bool read)
  : read_(read), t_(t)
{
  bool trace = false;
  if (!read)
    return;
  //for reading we need to set up the levels
  while (true)
  {
    vil_tiff_header h(t_.tif());
    if (trace)
      std::cerr << "In vil_tiff_pyramid_resource constructor"
               << " constructed header\n"
               << "n-levels = " << this->nlevels() << '\n';
    auto* pl = new tiff_pyramid_level(this->nlevels(),
                                                    h.image_width.val,
                                                    h.image_length.val,
                                                    h.nplanes,
                                                    h.pix_fmt);
    levels_.push_back(pl);
    if (trace)
      std::cerr << "In vil_tiff_pyramid_resource constructor"
               << " constructed level\n";
    int status = TIFFReadDirectory(t_.tif());
    if (trace)
      std::cerr << "In vil_tiff_pyramid_resource constructor"
               << " Read new directory\n";

    if (!status)
      break;
  }
  if (trace)
    std::cerr << "In vil_tiff_pyramid_resource constructor"
             << " Begin sorting\n";
  //sort the pyramid
  std::sort(levels_.begin(), levels_.end(), level_compare);
  //normalize the scales
  this->normalize_scales();
}

vil_tiff_pyramid_resource::~vil_tiff_pyramid_resource()
{
  for (unsigned L = 0; L<this->nlevels(); ++L)
    delete levels_[L];
}

//:Get a partial view from the image from a specified pyramid level
vil_image_view_base_sptr
vil_tiff_pyramid_resource::get_copy_view(unsigned i0, unsigned n_i,
                                         unsigned j0, unsigned n_j,
                                         unsigned level) const
{
  unsigned nl = this->nlevels();
  if (level>=nl)
    return vil_image_view_base_sptr();
  vil_image_resource_sptr resc = this->get_resource(level);
  //scale input coordinates to the scale of the level
  float scale = levels_[level]->scale_;
  float fi0 = std::floor(scale*i0), fj0 = std::floor(scale*j0);
  float fni = std::floor(scale*n_i), fnj = std::floor(scale*n_j);
  auto si0 = static_cast<unsigned>(fi0);
  auto sj0 = static_cast<unsigned>(fj0);
  auto sni = static_cast<unsigned>(fni);
  if (sni == 0) sni = 1;//can't have less than one pixel
  auto snj = static_cast<unsigned>(fnj);
  if (snj == 0) snj = 1;//can't have less than one pixel
  vil_image_view_base_sptr view = resc->get_copy_view(si0, sni, sj0, snj);
#if 0 //DON'T NEED CLEAR?
  resc->clear_TIFF();
#endif
  return view;
}

//:Get a partial view from the image in the pyramid closest to scale.
// The origin and size parameters are in the coordinate system
// of the base image. The scale factor is with respect to the base
// image (base scale = 1.0).
vil_image_view_base_sptr
vil_tiff_pyramid_resource::get_copy_view(unsigned i0, unsigned n_i,
                                         unsigned j0, unsigned n_j,
                                         const float scale,
                                         float& actual_scale) const
{
  // Get the closest scale
  tiff_pyramid_level* pl  = this->closest(scale);
  if (!pl)
    return nullptr;
  actual_scale = pl->scale_;
  return this->get_copy_view(i0, n_i, j0, n_j, pl->cur_level_);
}

//: Put the data in this view back into the image source at specified level.
// Only can be written once.
bool vil_tiff_pyramid_resource::put_resource(vil_image_resource_sptr const& ir)
{
  unsigned level = this->nlevels();
  unsigned ni = ir->ni(), nj = ir->nj();
  unsigned nplanes = ir->nplanes();
  vil_pixel_format fmt = ir->pixel_format();
  vil_blocked_image_resource_sptr bir = blocked_image_resource(ir);
  unsigned sbi = 0, sbj = 0;
  if (bir) { sbi = bir->size_block_i(); sbj = bir->size_block_j(); }
  // setup the image header for the level
  auto* h = new vil_tiff_header(t_.tif(), ni, nj, nplanes,
                                           fmt, sbi, sbj);

  /* We are writing single page of the multipage file */
  TIFFSetField(t_.tif(), TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
  /* Set the page number */
  TIFFSetField(t_.tif(), TIFFTAG_PAGENUMBER,level, 3);
  auto* ti = new vil_tiff_image(t_, h, level);
  vil_image_resource_sptr resc = ti;
  if (!vil_copy_deep(ir, resc))
    return false;
#if 0 //DON'T NEED CLEAR?
  ti->clear_TIFF();
#endif
  auto* pl = new tiff_pyramid_level((unsigned int)(levels_.size()), ni, nj, nplanes, fmt);
  levels_.push_back(pl);
  int status = TIFFWriteDirectory(t_.tif());
  return status == 1 ;
}
//: returns the pyramid resource at the specified level
vil_image_resource_sptr vil_tiff_pyramid_resource::get_resource(const unsigned level) const
{
  unsigned nl = this->nlevels();
  if (level>=nl)
    return nullptr;
  // setup the image header for the level
  unsigned header_index = levels_[level]->header_index_;
  // The status value should be checked here
  if (TIFFSetDirectory(t_.tif(), header_index)<=0)
    return nullptr;
  auto* h = new vil_tiff_header(t_.tif());
  auto* i = new vil_tiff_image(t_, h, nl);
  i->set_index(header_index);
  return i;
}
