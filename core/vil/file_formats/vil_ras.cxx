// This is core/vil/file_formats/vil_ras.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_ras.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_memory_chunk.h>

#include <vxl_config.h>

char const* vil_ras_format_tag = "ras";


// ===========================================================================
//                                                             helper routines


namespace
{
#if VXL_LITTLE_ENDIAN
  //: Change the byte order on a little endian machine.
  // Do nothing on a big endian machine.
  inline
  void swap_endian( vxl_uint_32& word )
  {
    vxl_uint_8* bytes = reinterpret_cast<vxl_uint_8*>( &word );
    vxl_uint_8 t = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = t;
    t = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = t;
  }
#endif

  //: Equivalent of ntoh
  // Read a big-endian word from the stream, storing it in the
  // native format.
  bool read_uint_32( vil_stream* vs, vxl_uint_32& word )
  {
    if ( vs->read( &word, 4 ) < 4 )
      return false;
#if VXL_LITTLE_ENDIAN
    swap_endian( word );
#endif
    return true;
  }

  //: Equivalent of hton
  // Write a host-format word as to a big-endian formatted stream.
  bool write_uint_32( vil_stream* vs, vxl_uint_32 word )
  {
#if VXL_LITTLE_ENDIAN
    swap_endian( word );
#endif
    return vs->write( &word, 4 ) == 4;
  }

  //: Compute the length of the data.
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
  const vxl_uint_32 RT_OLD = 0;          //< Raw pixrect image in MSB-first order
  const vxl_uint_32 RT_STANDARD = 1;     //< Raw pixrect image in MSB-first order
  const vxl_uint_32 RT_BYTE_ENCODED = 2; //< (Run-length compression of bytes)
  const vxl_uint_32 RT_FORMAT_RGB = 3;   //< ([X]RGB instead of [X]BGR)
  const vxl_uint_32 RMT_NONE = 0;        //< No colourmap (ras_maplength is expected to be 0)
  const vxl_uint_32 RMT_EQUAL_RGB = 1;   //< (red[ras_maplength/3],green[],blue[])
}


// ===========================================================================
//                                                        vil_ras_file_format


vil_image_resource_sptr
vil_ras_file_format::
make_input_image( vil_stream* vs )
{
  // Check the magic number
  vxl_uint_8 buf[4] = { 0, 0, 0, 0 };
  vs->read( buf, 4 );
  if ( ! ( buf[0] == RAS_MAGIC[0] && buf[1] == RAS_MAGIC[1] &&
           buf[2] == RAS_MAGIC[2] && buf[3] == RAS_MAGIC[3]  ) )
    return 0;

  return new vil_ras_image( vs );
}


vil_image_resource_sptr
vil_ras_file_format::
make_output_image( vil_stream* vs,
                   unsigned ni,
                   unsigned nj,
                   unsigned nplanes,
                   vil_pixel_format format )
{
  return new vil_ras_image(vs, ni, nj, nplanes, format );
}


char const*
vil_ras_file_format::
tag() const
{
  return vil_ras_format_tag;
}


// ===========================================================================
//                                                              vil_ras_image


vil_ras_image::
vil_ras_image( vil_stream* vs ):
  vs_(vs)
{
  vs_->ref();
  read_header();
}


bool
vil_ras_image::
get_property( char const* /*tag*/, void* /*prop*/ ) const
{
  // This is not an in-memory image type, nor is it read-only:
  return false;
}


char const*
vil_ras_image::
file_format() const
{
  return vil_ras_format_tag;
}


vil_ras_image::
vil_ras_image(vil_stream* vs,
              unsigned ni,
              unsigned nj,
              unsigned nplanes,
              vil_pixel_format format )
{
  vs_ = vs; vs_->ref();
  width_ = ni;
  height_ = nj;

  components_ = nplanes * vil_pixel_format_num_components( format );
  if ( components_ != 1 && components_ != 3 )
  {
    vcl_cerr << __FILE__ << ": can't handle "
             << nplanes << " x "
             << vil_pixel_format_num_components( format ) << " components\n";
    return;
  }

  bits_per_component_ = 8 * vil_pixel_format_sizeof_components( format );

  if ( bits_per_component_ != 8 ) {
    vcl_cerr << __FILE__ << ": can't handle " << bits_per_component_ << " bits per component\n";
    return;
  }

  depth_ = components_ * bits_per_component_;

  if (components_ == 3)
    type_ = RT_FORMAT_RGB;
  else
    type_ = RT_STANDARD;
  map_type_ = RMT_NONE;
  map_length_ = 0;
  length_ = compute_length( width_, height_, depth_ );
  col_map_ = 0;

  write_header();
}


vil_ras_image::
~vil_ras_image( )
{
  delete[] col_map_;
  vs_->unref();
}


//: Read the header of a Sun raster file.
bool
vil_ras_image::
read_header( )
{
  // Go to start of file
  vs_->seek(0);

  vxl_uint_8 buf[4];
  if ( vs_->read(buf, 4) < 4 ) // at end-of-file?
    return false;
  if (! ( buf[0] == RAS_MAGIC[0] && buf[1] == RAS_MAGIC[1] &&
          buf[2] == RAS_MAGIC[2] && buf[3] == RAS_MAGIC[3]  ) )
    return false; // magic number isn't correct

  if ( !( read_uint_32( vs_, width_ ) &&
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
  if ( map_length_ % 3 != 0 ) {
    vcl_cerr << __FILE__ << ": color map length is not a multiple of 3\n";
    return false;
  }

  if ( map_length_ ) {
    col_map_ = new vxl_uint_8[ map_length_ ];
    vs_->read( col_map_, (vil_streampos)map_length_ );
  } else {
    col_map_ = 0;
  }

  start_of_data_ = vs_->tell();

  if ( map_type_ != RMT_NONE || depth_ == 24 ) {
    components_ = 3;
  } else {
    components_ = 1;
  }
  bits_per_component_ = 8;

  return true;
}


bool
vil_ras_image::
write_header()
{
  vs_->seek(0);

  vs_->write( RAS_MAGIC, 4 );

  // no color map for the files we write
  assert( map_length_ == 0 );

  write_uint_32( vs_, width_ );
  write_uint_32( vs_, height_ );
  write_uint_32( vs_, depth_ );
  write_uint_32( vs_, length_ );
  write_uint_32( vs_, type_ );
  write_uint_32( vs_, map_type_ );
  write_uint_32( vs_, map_length_ );

  start_of_data_ = vs_->tell();

  return true;
}


unsigned
vil_ras_image::
nplanes() const
{
  return components_;
}


unsigned
vil_ras_image::
ni() const
{
  return width_;
}


unsigned
vil_ras_image::
nj() const
{
  return height_;
}


vil_pixel_format
vil_ras_image::
pixel_format() const
{
  assert( bits_per_component_ == 8 );
  return VIL_PIXEL_FORMAT_BYTE;
}


vil_image_view_base_sptr
vil_ras_image::
get_copy_view( unsigned i0, unsigned ni,
               unsigned j0, unsigned nj ) const
{
  if ( type_ == RT_BYTE_ENCODED )
    return 0; // not yet implemented

  unsigned file_bytes_per_pixel = (depth_+7)/8;
  unsigned buff_bytes_per_pixel = components_ * ( (bits_per_component_+7)/8 );
  unsigned file_byte_width = width_ * file_bytes_per_pixel;
  file_byte_width += ( file_byte_width % 2 ); // each scan line ends on a 16bit boundary
  unsigned file_byte_start = start_of_data_ + j0 * file_byte_width + i0 * file_bytes_per_pixel;
  unsigned buff_byte_width = ni * buff_bytes_per_pixel;

  vil_memory_chunk_sptr buf = new vil_memory_chunk(ni * nj * components_, VIL_PIXEL_FORMAT_BYTE );
  vxl_uint_8* ib = reinterpret_cast<vxl_uint_8*>( buf->data() );

  if ( !col_map_ ) {
    // No colourmap, so just read in the bytes.
    // Make the component order RGB to avoid surprising the user.
    for ( unsigned j = 0; j < nj; ++j ) {
      vs_->seek( file_byte_start + j * file_byte_width );
      vs_->read( ib + j * buff_byte_width, buff_byte_width );

      if ( type_ != RT_FORMAT_RGB && components_ == 3 ) {
        vxl_uint_8* pixel = ib + j * buff_byte_width;
        for ( unsigned i = 0; i < ni; ++i ) {
          vxl_uint_8* rp = pixel+2;
          vxl_uint_8 t = *pixel;
          *pixel = *rp;
          *rp = t;
          pixel += 3;
        }
      }
    }
  } else {
    assert( file_bytes_per_pixel == 1 && buff_bytes_per_pixel == 3 );
    unsigned col_len = map_length_ / 3;
    // Read a line, and map every index into an RGB triple
    vcl_vector<vxl_uint_8> line( ni );
    for ( unsigned j = 0; j < nj; ++j ) {
      vs_->seek( file_byte_start + j * file_byte_width );
      vs_->read( &line[0], line.size() );
      vxl_uint_8* in_p = &line[0];
      vxl_uint_8* out_p = ib + j * buff_byte_width;
      for ( unsigned i=0; i < ni; ++i ) {
        assert( *in_p < col_len );
        *(out_p++) = col_map_[ *in_p ];
        *(out_p++) = col_map_[ *in_p + col_len ];
        *(out_p++) = col_map_[ *in_p + 2*col_len ];
        ++in_p;
      }
    }
  }

  return new vil_image_view<vxl_byte>( buf, ib,
                                       width_, height_, components_,
                                       components_, components_*width_, 1 );
}


bool
vil_ras_image::
put_view( const vil_image_view_base& view, unsigned i0, unsigned j0 )
{
  // Get a 3-plane view of the section
  vil_image_view<vxl_uint_8> section( view );

  if ( ! this->view_fits( section, i0, j0 ) ) {
    vcl_cerr << "ERROR: " << __FILE__ << ": view does not fit\n";
    return false;
  }

  if ( section.nplanes() != components_ ) {
    vcl_cerr << "ERROR: " << __FILE__ << ": data parameters of view don't match\n";
    return false;
  }

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

  unsigned file_bytes_per_pixel = (depth_+7)/8;
  unsigned buff_bytes_per_pixel = components_ * ( (bits_per_component_+7)/8 );
  unsigned file_byte_width = width_ * file_bytes_per_pixel;
  file_byte_width += ( file_byte_width % 2 ); // each scan line ends on a 16bit boundary
  unsigned file_byte_start = start_of_data_ + j0 * file_byte_width + i0 * file_bytes_per_pixel;
  unsigned buff_byte_width = view.ni() * buff_bytes_per_pixel;

  assert( file_bytes_per_pixel == buff_bytes_per_pixel );
  assert( file_byte_width >= buff_byte_width );

  // If we are writing full line widths, then make sure the padding
  // byte is set to zero. Otherwise, assume that the current data is
  // valid, and therefore that the padding byte is already zero.
  //
  vcl_vector<vxl_uint_8> data_buffer;
  if ( file_byte_width == buff_byte_width+1 ) {
    data_buffer.resize( file_byte_width );
    data_buffer[ file_byte_width-1 ] = 0;
  } else {
    data_buffer.resize( buff_byte_width );
  }

  for ( unsigned j = 0; j < section.nj(); ++j ) {

    // Copy a line of the image into a contiguous buffer. No need to
    // optimize for the case with section is also appropriately
    // contiguous because the disk writing process will likely be the
    // bottleneck.
    //
    vxl_uint_8* ptr = &data_buffer[0];
    for ( unsigned i = i0; i < section.ni(); ++i ) {
      for ( unsigned p = 0; p < section.nplanes(); ++p ) {
        *ptr = section(i,j,p);
        ++ptr;
      }
    }

    // Write the line to disk.
    vs_->seek( file_byte_start + j * file_byte_width );
    vs_->write( &data_buffer[0], data_buffer.size() );
  }

  return true;
}
