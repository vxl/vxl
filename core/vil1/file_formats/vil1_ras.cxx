// This is core/vil1/file_formats/vil1_ras.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_ras.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

#include <vxl_config.h>

char const* vil1_ras_format_tag = "ras";

namespace {
#if VXL_LITTLE_ENDIAN
  inline void swap_endian( vxl_uint_32& word )
  {
    vxl_uint_8* bytes = (vxl_uint_8*)&word;
    vxl_uint_8 t = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = t;
    t = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = t;
  }
#else // VXL_BIG_ENDIAN: do nothing
  inline void swap_endian( vxl_uint_32& ) {}
#endif

  // Equivalent of ntoh
  bool read_uint_32( vil1_stream* vs, vxl_uint_32& word )
  {
    if ( vs->read( &word, 4 ) < 4 )
      return false;
    swap_endian( word );
    return true;
  }

  // Equivalent of hton
  bool write_uint_32( vil1_stream* vs, vxl_uint_32 word )
  {
    swap_endian( word );
    return vs->write( &word, 4 ) == 4;
  }

  // Compute the length of the data from the width, height and depth,
  // accounting for any padding that may be necessary to keep the scan
  // lines on 16-bit boundaries.
  vxl_uint_32 compute_length( vxl_uint_32 w, vxl_uint_32 h, vxl_uint_32 d )
  {
    w *= (d/8);
    w += (w%2);
    return h*w;
  }

  // From http://gmt.soest.hawaii.edu/gmt/doc/html/GMT_Docs/node111.html
  // and other documents on the web.
  const vxl_uint_8 RAS_MAGIC[] = { 0x59, 0xA6, 0x6A, 0x95 };
  const vxl_uint_32 RT_OLD = 0;          // Raw pixrect image in MSB-first order
  const vxl_uint_32 RT_STANDARD = 1;     // Raw pixrect image in MSB-first order
  const vxl_uint_32 RT_BYTE_ENCODED = 2; // (Run-length compression of bytes)
  const vxl_uint_32 RT_FORMAT_RGB = 3;   // ([X]RGB instead of [X]BGR)
  const vxl_uint_32 RMT_NONE = 0;        // No colourmap (ras_maplength is expected to be 0)
  const vxl_uint_32 RMT_EQUAL_RGB = 1;   // (red[ras_maplength/3],green[],blue[])
}


vil1_image_impl* vil1_ras_file_format::make_input_image(vil1_stream* vs)
{
  // Check the magic number
  vxl_uint_8 buf[4] = { 0, 0, 0, 0 };
  vs->read(buf, 4);
  if ( ! ( buf[0] == RAS_MAGIC[0] && buf[1] == RAS_MAGIC[1] &&
           buf[2] == RAS_MAGIC[2] && buf[3] == RAS_MAGIC[3]  ) )
    return 0;

  return new vil1_ras_generic_image(vs);
}

vil1_image_impl* vil1_ras_file_format::make_output_image(vil1_stream* vs, int planes,
                                                         int width,
                                                         int height,
                                                         int components,
                                                         int bits_per_component,
                                                         vil1_component_format format)
{
  return new vil1_ras_generic_image(vs, planes, width, height, components, bits_per_component, format);
}

char const* vil1_ras_file_format::tag() const
{
  return vil1_ras_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

vil1_ras_generic_image::vil1_ras_generic_image(vil1_stream* vs):
  vs_(vs)
{
  vs_->ref();
  read_header();
}

bool vil1_ras_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  // The default raw colour format is BGR. The default indexed colour
  // format is RGB. Go figure.
  if (0==vcl_strcmp(tag, vil1_property_component_order_is_BGR)) {
    if ( prop )
      (*(bool*)prop) = ( map_type_ == RMT_NONE && type_ != RT_FORMAT_RGB );
    return true;
  }

  return false;
}

char const* vil1_ras_generic_image::file_format() const
{
  return vil1_ras_format_tag;
}

vil1_ras_generic_image::vil1_ras_generic_image(vil1_stream* vs, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil1_component_format /*format*/)
  : vs_(vs)
{
  vs_->ref();
  width_ = width;
  height_ = height;

  if ( planes != 1 ) {
    vcl_cerr << __FILE__ << ": can only handle 1 plane\n";
    return;
  }
  if ( components != 3 && components != 1 ) {
    vcl_cerr << __FILE__ << ": can't handle " << components << " components\n";
    return;
  }
  if ( bits_per_component != 8 ) {
    vcl_cerr << __FILE__ << ": can't handle " << bits_per_component << " bits per component\n";
    return;
  }

  components_ = components;
  bits_per_component_ = bits_per_component;
  depth_ = components_ * bits_per_component_;

  if (components_ == 3) {
    type_ = RT_FORMAT_RGB;
  } else {
    type_ = RT_STANDARD;
  }
  map_type_ = RMT_NONE;
  map_length_ = 0;
  length_ = compute_length( width_, height_, depth_ );
  col_map_ = 0;

  write_header();
}

vil1_ras_generic_image::~vil1_ras_generic_image()
{
  delete[] col_map_;
  vs_->unref();
}


//: Read the header of a Sun raster file.
bool vil1_ras_generic_image::read_header()
{
  // Go to start of file
  vs_->seek(0);

  vxl_uint_8 buf[4];
  if (vs_->read(buf, 4) < 4) // at end-of-file?
    return false;
  if (! ( buf[0] == RAS_MAGIC[0] && buf[1] == RAS_MAGIC[1] &&
          buf[2] == RAS_MAGIC[2] && buf[3] == RAS_MAGIC[3]  ) )
    return false; // magic number isn't correct

  if (!( read_uint_32( vs_, width_ ) &&
         read_uint_32( vs_, height_ ) &&
         read_uint_32( vs_, depth_ ) &&
         read_uint_32( vs_, length_ ) &&
         read_uint_32( vs_, type_ ) &&
         read_uint_32( vs_, map_type_ ) &&
         read_uint_32( vs_, map_length_ ) ) )
    return false;

  // Do consistency checks of the header

  if (type_ != RT_OLD && type_ != RT_STANDARD &&
      type_ != RT_BYTE_ENCODED && type_ != RT_FORMAT_RGB ) {
    vcl_cerr << __FILE__ << ": unknown type " << type_ << vcl_endl;
    return false;
  }
  if ( map_type_ != RMT_NONE && map_type_ != RMT_EQUAL_RGB ) {
    vcl_cerr << __FILE__ << ": unknown map type " << map_type_ << vcl_endl;
    return false;
  }
  if ( map_type_ == RMT_NONE && map_length_ != 0 ) {
    vcl_cerr << __FILE__ << ": No colour map according to header, but there is a map!\n";
    return false;
  }

  if ( depth_ != 8 && depth_ != 24 ) {
    vcl_cerr << __FILE__ << ": depth " << depth_ << " not implemented\n";
    return false;
  }

  // The "old" format always has length set to zero, so we should compute it ourselves.
  if ( type_ == RT_OLD ) {
    length_ = compute_length( width_, height_, depth_ );
  }
  if ( length_ == 0 ) {
    vcl_cerr << __FILE__ << ": header says image has length zero\n";
    return false;
  }
  if ( type_ != RT_BYTE_ENCODED && length_ != compute_length( width_, height_, depth_ ) ) {
    vcl_cerr << __FILE__ << ": length " << length_ << " does not match wxhxd = "
             << compute_length( width_, height_, depth_ ) << vcl_endl;
    return false;
  }

  if ( map_length_ ) {
    assert( map_length_ % 3 == 0 );
    col_map_ = new vxl_uint_8[ map_length_ ];
    vs_->read( col_map_, (vil1_streampos)map_length_ );
  } else {
    col_map_ = 0;
  }

  start_of_data_ = vs_->tell();

  assert( depth_ == 8 || depth_ == 24 );

  if ( map_type_ != RMT_NONE || depth_ == 24 ) {
    components_ = 3;
  } else {
    components_ = 1;
  }
  bits_per_component_ = 8;

  return true;
}

bool vil1_ras_generic_image::write_header()
{
  vs_->seek(0);

  vs_->write( RAS_MAGIC, 4 );

  write_uint_32( vs_, width_ );
  write_uint_32( vs_, height_ );
  write_uint_32( vs_, depth_ );
  write_uint_32( vs_, length_ );
  write_uint_32( vs_, type_ );
  write_uint_32( vs_, map_type_ );
  write_uint_32( vs_, map_length_ );

  assert( map_length_ == 0 );

  start_of_data_ = vs_->tell();

  return true;
}

bool vil1_ras_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if ( type_ == RT_BYTE_ENCODED )
    return false; // not yet implemented

  int file_bytes_per_pixel = (depth_+7)/8;
  int buff_bytes_per_pixel = components_ * ( (bits_per_component_+7)/8 );
  int file_byte_width = width_ * file_bytes_per_pixel;
  file_byte_width += ( file_byte_width % 2 ); // each scan line ends on a 16bit boundary
  int file_byte_start = start_of_data_ + y0 * file_byte_width + x0 * file_bytes_per_pixel;
  int buff_byte_width = xs * buff_bytes_per_pixel;

  vxl_uint_8* ib = (vxl_uint_8*) buf;

  if ( !col_map_ ) {
    // No colourmap, so just read in the bytes. This could be RGB or
    // BGR, depending on type_.
    for (int y = 0; y < ys; ++y) {
      vs_->seek(file_byte_start + y * file_byte_width);
      vs_->read(ib + y * buff_byte_width, buff_byte_width);
    }

  } else {
    assert( file_bytes_per_pixel == 1 && buff_bytes_per_pixel == 3 );
    int col_len = map_length_ / 3;
    // Read a line, and map every index into an RGB triple
    vxl_uint_8* line = new vxl_uint_8[ xs ];
    for (int y = 0; y < ys; ++y) {
      vs_->seek( file_byte_start + y * file_byte_width );
      vs_->read( line, xs );
      vxl_uint_8* in_p = line;
      vxl_uint_8* out_p = ib + y * buff_byte_width;
      for ( int i=0; i < xs; ++i ) {
        assert( *in_p < col_len );
        *(out_p++) = col_map_[ *in_p ];
        *(out_p++) = col_map_[ *in_p + col_len ];
        *(out_p++) = col_map_[ *in_p + 2*col_len ];
        ++in_p;
      }
    }
    delete[] line;
  }

  return true;
}

bool vil1_ras_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  if ( col_map_ ) {
    vcl_cerr << __FILE__ << ": writing to file with a colour map is not implemented\n";
    return false;
  }
  if ( type_ == RT_BYTE_ENCODED ) {
    vcl_cerr << __FILE__ << ": writing to a run-length encoded file is not implemented\n";
    return false;
  }
  if ( components_ == 3 && type_ != RT_FORMAT_RGB ) {
    vcl_cerr << __FILE__ << ": writing BGR format is not implemented\n";
    return false;
  }

  // With the restrictions above, writing is simple. Just dump the bytes.

  int file_bytes_per_pixel = (depth_+7)/8;
  int buff_bytes_per_pixel = components_ * ( (bits_per_component_+7)/8 );
  int file_byte_width = width_ * file_bytes_per_pixel;
  file_byte_width += ( file_byte_width % 2 ); // each scan line ends on a 16bit boundary
  int file_byte_start = start_of_data_ + y0 * file_byte_width + x0 * file_bytes_per_pixel;
  int buff_byte_width = xs * buff_bytes_per_pixel;

  assert( file_bytes_per_pixel == buff_bytes_per_pixel );
  assert( file_byte_width >= buff_byte_width );

  // only pad if whole lines are written. Otherwise, assume the
  // previous contents are valid.
  bool need_pad = ( file_byte_width == buff_byte_width+1 );
  vxl_uint_8 zero = 0;

  vxl_uint_8 const* ob = (vxl_uint_8 const*) buf;

  for (int y = 0; y < ys; ++y) {
    vs_->seek( file_byte_start + y * file_byte_width );
    vs_->write( ob + y * buff_byte_width, buff_byte_width );
    if ( need_pad )
      vs_->write( &zero, 1 );
  }

  return true;
}

vil1_image vil1_ras_generic_image::get_plane(unsigned int plane) const
{
  assert(plane == 0);
  return const_cast<vil1_ras_generic_image*>(this);
}
