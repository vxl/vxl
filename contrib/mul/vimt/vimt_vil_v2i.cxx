//:
// \file

#include "vimt_vil_v2i.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_fstream.h>

#include <vxl_config.h> // for VXL_BIG_ENDIAN and vxl_byte

#include <vil/vil_exception.h>
#include <vil/vil_property.h>
#include <vil/vil_copy.h>
#include <vil/vil_stream.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vimt/vimt_image_2d_of.h>

// v2i magic number
const unsigned V2I_MAGIC = 987123872U;


class vimt_vil_fstream: vil_stream_fstream
{
protected:
  vcl_fstream& underlying_stream() { return vil_stream_fstream::underlying_stream(); }
  friend class vimt_vil_v2i_format;
  friend class vimt_vil_v2i_image;
private:
  vimt_vil_fstream(): vil_stream_fstream("", "") {} // Will never be constructed
};



vil_image_resource_sptr vimt_vil_v2i_format::make_input_image(vil_stream* vs)
{
  // First of all hack an vil_stream to get at the underlying
  // vcl_stream, in order to create a vsl_b_stream
  if (typeid(*vs) != typeid(vil_stream_fstream))
  {
    vcl_cerr << "vimt_vil_v2i_format::make_input_image() WARNING\n"
             << "  Unable to deal with stream type" << vcl_endl;

    return 0;
  }
  vsl_b_istream vslstream( &reinterpret_cast<vimt_vil_fstream *>(vs)->underlying_stream() );
  if (!vslstream) return 0; // Not even a vsl file.
  unsigned magic;
  vil_streampos start = vs->tell();
  vsl_b_read(vslstream, magic);
  if (magic == V2I_MAGIC)
    return new vimt_vil_v2i_image(vs);

  // This vsl stream may hav been created with vsl_b_wrte(vimt_image) - with no
  //  v2i magic number. Check if that is true.
  vs->seek(start);
  int v_i;
  bool v_b;
  vsl_b_read(vslstream, v_i);  // vimt_image_2d_of.version
  vsl_b_read(vslstream, v_i);  // vil_image_view.version
  if (v_i != 1) return 0; // can only handle version 1.
  vsl_b_read(vslstream, v_i);  // vil_image_view.ni
  if (v_i == 0) return 0; // can't handle empty images.
  vsl_b_read(vslstream, v_i);  // vil_image_view.nj
  if (v_i == 0) return 0; // can't handle empty images.
  vsl_b_read(vslstream, v_i);  // vil_image_view.nplanes
  if (v_i == 0) return 0; // can't handle empty images.
  vsl_b_read(vslstream, v_i);  // vil_image_view.istep
  vsl_b_read(vslstream, v_i);  // vil_image_view.jstep
  vsl_b_read(vslstream, v_i);  // vil_image_view.planestep
  vsl_b_read(vslstream, v_i);  // vil_smart_ptr.version
  if (v_i != 2) return 0; // can only handle version 2.
  vsl_b_read(vslstream, v_b);  // vil_smart_ptr.firsttime
  if (!v_b) return 0; // can't handle multiple images.
  vsl_b_read(vslstream, v_i);  // vil_smart_ptr.id
  if (v_i != 1) return 0; // can't handle multiple images.
  vsl_b_read(vslstream, v_b);  // vil_memory_chunk*.nonnull
  if (!v_b) return 0; // can't handle empty images.
  vsl_b_read(vslstream, v_i);  // vil_memory_chunk.version
  if (v_i != 2) return 0; // can only handle version 2.
  vsl_b_read(vslstream, v_i);  //  chunk.pixel_format

  vil_pixel_format f = static_cast<vil_pixel_format>(v_i);
  vs->seek(start);
  switch(f)
  {
#define macro( F , T ) \
    case  F : \
    { vimt_image_2d_of< T > im; vsl_b_read(vslstream, im); if (!vslstream) return 0; } \
    break;
// Don't include versions for which there is no template instantian of vimt_image_2d_of.
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
//macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
    default: return 0; // Unknown pixel type - or more likely not a v2i image.
  }
  vs->seek(start);
  return new vimt_vil_v2i_image(vs, f);
}

vil_image_resource_sptr vimt_vil_v2i_format::make_output_image(vil_stream* vs,
                                                               unsigned ni,
                                                               unsigned nj,
                                                               unsigned nplanes,
                                                               vil_pixel_format format)
{
  // First of all hack an vil_stream to get at the underlying
  // ccl_stream, in order to create a vsl_b_stream
  if (typeid(*vs) != typeid(vil_stream_fstream))
  {
    vcl_cerr << "vimt_vil_v2i_format::make_output_image() WARNING\n"
             << "  Unable to deal with stream type" << vcl_endl;

    return 0;
  }
  if ( format != VIL_PIXEL_FORMAT_BYTE && format != VIL_PIXEL_FORMAT_SBYTE &&
       format != VIL_PIXEL_FORMAT_UINT_32 && format != VIL_PIXEL_FORMAT_INT_32 &&
       format != VIL_PIXEL_FORMAT_UINT_16 && format != VIL_PIXEL_FORMAT_INT_16 &&
       format != VIL_PIXEL_FORMAT_FLOAT && format != VIL_PIXEL_FORMAT_DOUBLE &&
       format != VIL_PIXEL_FORMAT_BOOL)
  {
    vcl_cerr << "vimt_vil_v2i_format::make_output_image() WARNING\n"
             << "  Unable to deal with file format : " << format << vcl_endl;
    return 0;
  }
  return new vimt_vil_v2i_image(vs, ni, nj, nplanes, format);
}


/////////////////////////////////////////////////////////////////////////////

vimt_vil_v2i_image::vimt_vil_v2i_image(vil_stream* vs):
  vs_(vs), im_(0), dirty_(false)
{
  vs_->ref();
  vs_->seek(0L);
  vsl_b_istream vslstream(& reinterpret_cast<vimt_vil_fstream *>(vs_)->underlying_stream());

  unsigned magic;
  vsl_b_read(vslstream, magic);
  assert(magic == V2I_MAGIC);

  short version;
  vsl_b_read(vslstream, version);

  switch (version)
  {
  case 1:
    {
      vimt_image *p_im=0;
      vsl_b_read(vslstream, p_im);
      im_ = dynamic_cast<vimt_image_2d *>(p_im);
      break;
    }
  default:
    vcl_cerr << "I/O ERROR: vimt_vil_v2i_image::vimt_vil_v2i_image()\n"
      << "           Unknown version number "<< version << '\n';
    return;
  }
}


//: Constructor to deal with directly created v2i files.
vimt_vil_v2i_image::vimt_vil_v2i_image(vil_stream* vs, vil_pixel_format f):
  vs_(vs), im_(0), dirty_(false)
{
  vs_->ref();
  vs_->seek(0L);
  vsl_b_istream vslstream(& reinterpret_cast<vimt_vil_fstream *>(vs_)->underlying_stream());

  
  switch(f)
  {
#define macro( F , T ) \
    case  F : \
    im_ = new vimt_image_2d_of< T >(); \
    vsl_b_read(vslstream, *static_cast<vimt_image_2d_of< T >*>(im_)); \
    break;
// Don't include versions for which there is no template instantian of vimt_image_2d_of.
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
//macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
    default: throw vil_exception_image_io("vimt_vil_v2i_image::vimt_vil_v2i_image",
      "v2i", "");
  }

  // Fiddle factor - most directly created vsl files have transforms in mm.
  vimt_transform_2d tr;
  tr.set_zoom_only(1000.0, 0.0, 0.0);
  im_->world2im() = im_->world2im() * tr;
  assert(! !vslstream);
  
}

bool vimt_vil_v2i_image::get_property(char const * key, void * value) const
{
  const vimt_transform_2d &tr = im_->world2im();

  if (vcl_strcmp(vil_property_pixel_size, key)==0)
  {
    vgl_vector_2d<double> p11 = tr.inverse()(1.0, 1.0) - tr.inverse().origin();
    //Assume no rotation or shearing.

    float* array =  static_cast<float*>(value);
    array[0] = (float) p11.x();
    array[1] = (float) p11.y();
    return true;
  }

  if (vcl_strcmp(vil_property_offset, key)==0)
  {
    vgl_point_2d<double> origin = tr.origin();
    float* array =  static_cast<float*>(value);
    array[0] = (float)(origin.x());
    array[1] = (float)(origin.y());
    return true;
  }

  return false;
}

vimt_vil_v2i_image::vimt_vil_v2i_image(vil_stream* vs, unsigned ni, unsigned nj,
                                       unsigned nplanes, vil_pixel_format format):
  vs_(vs), im_(0), dirty_(true)
{
  vs_->ref();
  switch (format)
  {
#define macro( F , T ) \
   case  F : \
    im_ = new vimt_image_2d_of< T > (ni, nj, nplanes); \
    break;
// Don't include versions for which there is no template instantian of vimt_image_2d_of.
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
//macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    vcl_cerr << "I/O ERROR: vimt_vil_v2i_image::vimt_vil_v2i_image()\n"
             << "           Unknown vil_pixel_format "<< format << '\n';
    vcl_abort();
  }
}


vimt_vil_v2i_image::~vimt_vil_v2i_image()
{
  if (dirty_)
  {
    vs_->seek(0l);
    vsl_b_ostream vslstream(& reinterpret_cast<vimt_vil_fstream *>(vs_)->underlying_stream());
 
    vsl_b_write(vslstream, V2I_MAGIC);

    const short version = 1;
    vsl_b_write(vslstream, version);


    vimt_image *p_im=im_;
    vsl_b_write(vslstream, p_im);
  }
  //delete vs_;
  vs_->unref();
  delete im_;
}

//: Dimensions:  nplanes x ni x nj.
// This concept is treated as a synonym to components.
unsigned vimt_vil_v2i_image::nplanes() const
{
  return im_->image_base().nplanes();
}

//: Dimensions:  nplanes x ni x nj.
// The number of pixels in each row.
unsigned vimt_vil_v2i_image::ni() const
{
  return im_->image_base().ni();
}

//: Dimensions:  nplanes x ni x nj.
// The number of pixels in each column.
unsigned vimt_vil_v2i_image::nj() const
{
  return im_->image_base().nj();
}

//: Pixel Format.
enum vil_pixel_format vimt_vil_v2i_image::pixel_format() const
{
  return im_->image_base().pixel_format();
}


const vimt_transform_2d & vimt_vil_v2i_image::world2im() const
{
  return im_->world2im();
}

void vimt_vil_v2i_image::set_world2im(const vimt_transform_2d & tr)
{
  im_->set_world2im(tr);
  dirty_ = true;
}

//: Set the size of the each pixel in the i,j,k directions.
void vimt_vil_v2i_image::set_pixel_size(float si, float sj)
{
  const vimt_transform_2d &tr = im_->world2im();

// Try to adjust pixel size without modifying rest of transform
  vgl_vector_2d<double> w11 = tr(1.0, 1.0) - tr.origin();

  vimt_transform_2d zoom;
  zoom.set_zoom_only (w11.x()/si, w11.y()/sj, 0.0, 0.0);

  im_->set_world2im(tr*zoom);
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil_image_view_base_sptr vimt_vil_v2i_image::get_copy_view(unsigned i0, unsigned ni,
                                                           unsigned j0, unsigned nj) const
{
  const vil_image_view_base &view = im_->image_base();

  if (i0 + ni > view.ni() || j0 + nj > view.nj() ) return 0;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case  F : { \
    const vil_image_view< T > &v = \
      static_cast<const vil_image_view< T > &>(view); \
    vil_image_view< T > w(v.memory_chunk(), &v(i0,j0), \
                          ni, nj, v.nplanes(), \
                          v.istep(), v.jstep(), v.planestep()); \
    return new vil_image_view< T >(vil_copy_deep(w)); }
// Don't include versions for which there is no template instantian of vimt_image_2d_of.
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
//macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return 0;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil_image_view_base_sptr vimt_vil_v2i_image::get_view(unsigned i0, unsigned ni,
                                                      unsigned j0, unsigned nj) const
{
  const vil_image_view_base &view = im_->image_base();


  if (i0 + ni > view.ni() || j0 + nj > view.nj()) return 0;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case  F : { \
    const vil_image_view< T > &v = \
      static_cast<const vil_image_view< T > &>(view); \
    return new vil_image_view< T >(v.memory_chunk(), &v(i0,j0), \
                                   ni, nj, v.nplanes(), \
                                   v.istep(), v.jstep(), v.planestep()); }
// Don't include versions for which there is no template instantian of vimt_image_2d_of.
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
//macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return 0;
  }
}


//: Set the contents of the volume.
bool vimt_vil_v2i_image::put_view(const vil_image_view_base& vv,
                                  unsigned i0, unsigned j0)
{
  if (!view_fits(vv, i0, j0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  if (vv.pixel_format() != im_->image_base().pixel_format())
  {
    vcl_cerr << "ERROR: vimt_vil_v2i_image::put_view(). Pixel formats do not match\n";
    return false;
  }


  dirty_ = true;

  switch (vv.pixel_format())
  {
#define macro( F , T ) \
   case  F : \
    vil_copy_to_window(static_cast<vil_image_view< T >const&>(vv), \
                       static_cast<vimt_image_2d_of< T >&>(*im_).image(), i0, j0); \
    return true;

// Don't include versions for which there is no template instantian of vimt_image_2d_of.
    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//  macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//  macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//  macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
    macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
    macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//  macro(VIL_PIXEL_FORMAT_BOOL , bool )
    macro(VIL_PIXEL_FORMAT_FLOAT , float )
//  macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return false;
  }
}
