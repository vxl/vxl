// This is core/vgui/vgui_section_buffer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_section_buffer.h for a description of this file.
//
// \verbatim
// Modifications
//  16-AUG-2000  Marko Bacic, Oxford RRG -- Added support for multiple textures
//               Many cheap graphics cards do not support texture
//               maps bigger than 256x256. To support images greater then 256x256
//               it was necessary to add support for multiple textures. Hence an
//               image is rendered by tiling together several textures containing
//               different parts of it.
//
//  the_same_day fsm
//               Imposed my rigid ways on Marko's changes.
//               Fixes for SolarisGL.
//  05-AUG-2003  Amitha Perera
//               Added support for rendering vil_image_views, and cleaned up
//               the macros for selecting the pixel types and doing the data
//               conversion.
// \endverbatim

#include "vgui_section_buffer.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_pixel.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_pixel_traits.h>
#include "vgui_macro.h"
#include "vgui_pixel.h"
#include "vgui_section_render.h"
#include "vgui_range_map_params.h"
#include "vgui_range_map.h"
#include "internals/vgui_gl_selection_macros.h"
#include "internals/vgui_accelerate.h"

static bool debug = false;

namespace
{
  // These two helper functions are used in the apply() methods. See
  // the comments in apply(vil_image_view_base).

  // Converts the section of \a in marked by (x,y)-(x+w-1,y+h-1) into
  // the output GL buffer \a out.
  //
  // This handles multi-plane images with scalar-valued pixels.
  //
  // This is a helper routine for vgui_section_buffer::apply()
  template <class InT, class OutT>
  bool
  convert_buffer( vil_image_view<InT> const& in,
                  vgui_range_map_params_sptr const& rmp,
                  OutT* out, vcl_ptrdiff_t hstep )
  {
    bool params_but_not_mappable = false;
    if (rmp&&rmp->n_components_==in.nplanes())
    {
      vgui_range_map<InT> rm(*rmp);
      if (rm.table_mapable())
      {
        //offset for signed types
        InT O = rm.offset();
        switch ( in.nplanes() )
        {
         case 1: {
          vbl_array_1d<vxl_byte> Lmap = rm.Lmap();
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( Lmap[(unsigned)(in(i,j)+O)],
                                  *(out+i+j*hstep) );
          return true; }
         case 3: {
          vbl_array_1d<vxl_byte> Rmap = rm.Rmap();
          vbl_array_1d<vxl_byte> Gmap = rm.Gmap();
          vbl_array_1d<vxl_byte> Bmap = rm.Bmap();
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( Rmap[(unsigned)(in(i,j,0)+O)],
                                  Gmap[(unsigned)(in(i,j,1)+O)],
                                  Bmap[(unsigned)(in(i,j,2)+O)],
                                  *(out+i+j*hstep) );
          return true; }
         case 4: {
          vbl_array_1d<vxl_byte> Rmap = rm.Rmap();
          vbl_array_1d<vxl_byte> Gmap = rm.Gmap();
          vbl_array_1d<vxl_byte> Bmap = rm.Bmap();
          vbl_array_1d<vxl_byte> Amap = rm.Amap();
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( Rmap[(unsigned)(in(i,j,0)+O)],
                                  Gmap[(unsigned)(in(i,j,1)+O)],
                                  Bmap[(unsigned)(in(i,j,2)+O)],
                                  Amap[(unsigned)(in(i,j,3)+O)],
                                  *(out+i+j*hstep) );
          return true; }
         default:
          return false;
        } // end case
      }
      if (rm.mapable())//have to compute the mapping on the fly,e.g. for float
      {
        switch ( in.nplanes() )
        {
         case 1:
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( rm.map_L_pixel(in(i,j)),
                                  *(out+i+j*hstep) );
          return true;
         case 3:
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( rm.map_R_pixel(in(i,j,0)),
                                  rm.map_G_pixel(in(i,j,1)),
                                  rm.map_B_pixel(in(i,j,2)),
                                  *(out+i+j*hstep) );
          return true;
         case 4:
          for ( unsigned j=0; j < in.nj(); ++j )
            for ( unsigned i=0; i < in.ni(); ++i )
              vgui_pixel_convert( rm.map_R_pixel(in(i,j,0)),
                                  rm.map_G_pixel(in(i,j,1)),
                                  rm.map_B_pixel(in(i,j,2)),
                                  rm.map_A_pixel(in(i,j,3)),
                                  *(out+i+j*hstep) );
          return true;
         default:
          return false;
        } // end switch
      }
      params_but_not_mappable = true;
    }

    //otherwise, just clamp the values as originally done
    if (!rmp||rmp->n_components_!=in.nplanes()||params_but_not_mappable)
    {
      switch ( in.nplanes() )
      {
       case 1:
        for ( unsigned j=0; j < in.nj(); ++j )
          for ( unsigned i=0; i < in.ni(); ++i )
            vgui_pixel_convert( in(i,j), *(out+i+j*hstep) );
        return true;
       case 3:
        for ( unsigned j=0; j < in.nj(); ++j )
          for ( unsigned i=0; i < in.ni(); ++i )
            vgui_pixel_convert( in(i,j,0), in(i,j,1), in(i,j,2),
                                *(out+i+j*hstep) );
        return true;
       case 4:
        for ( unsigned j=0; j < in.nj(); ++j )
          for ( unsigned i=0; i < in.ni(); ++i )
            vgui_pixel_convert( in(i,j,0), in(i,j,1), in(i,j,2),
                                in(i,j,3), *(out+i+j*hstep) );
        return true;
       default:
        return false;
      } // end case
    }
    return false;
  }

  // Given the input image type, determine the output image type (GL
  // pixel type) and call convert_buffer() to do the actual conversion
  //
  // Used in the vgui_section_buffer::apply().
  //
  template <class InT>
  bool
  convert_image( vil_image_view<InT> const& in,
                 vgui_range_map_params_sptr const& rmp,
                 void* out, vcl_ptrdiff_t hstep,
                 GLenum format, GLenum type )
  {
    bool result = false;

#define Code(BufferType) result=convert_buffer(in,rmp,(BufferType*)out,hstep);
    ConditionListBegin;
    ConditionListBody( format, type );
    ConditionListFail {
      // shouldn't fail here. If we don't know this format and type, the
      // constructor would've failed.
      assert( false );
    }
#undef Code

    return result;
  }
} // end anonymous namespace


  // ==============================================================================
  //                                                            VGUI SECTION BUFFER
  // ==============================================================================


  // ---------------------------------------------------------------------------
  //                                                                 constructor

vgui_section_buffer::
vgui_section_buffer( unsigned in_x, unsigned in_y,
                     unsigned in_w, unsigned in_h,
                     GLenum in_format,
                     GLenum in_type )
  : format_( in_format ),
    type_( in_type ),
    x_( in_x ),
    y_( in_y ),
    w_( in_w ),
    h_( in_h ),
    allocw_( w_ ),
    alloch_( h_ ),
    buffer_( 0 ),
    buffer_ok_( false )
{
  assert( w_ > 0 && h_ > 0 );
#if 0 //These variables are not used and cause warnings
  GLenum format1 = GL_RGBA;
  GLenum type1 = GL_UNSIGNED_SHORT, type2 = GL_SHORT, type3 = GL_BYTE, type4 = GL_UNSIGNED_BYTE;
#endif
  // It doesn't seem to make any sense to specify only one of the 'format' and
  // 'type' parameters. Until we decide if it makes sense, it's not allowed.
  if      ( format_ == GL_NONE && type_ == GL_NONE )
    vgui_accelerate::instance()->vgui_choose_cache_format( &format_, &type_ );
  else if ( format_ == GL_NONE || type_ == GL_NONE )
    assert(false);

  // make sure allocw_ and alloch_ have been initialized.
  assert( allocw_*alloch_ >= w_*h_ );

  // To add a new format, you need to:
  // - create a new pixel type in vgui_pixel.h. Make sure the size of
  //   that pixel type is the same as that of the corresponding GL type.
  // - add the format type to
  //   internals/vgui_gl_selection_macros.h. Make sure to only
  //   conditionally include your type unless you are certain that all
  //   OpenGL implementations will support that type.

  // This will generate code for every GL pixel type we know about.
#define Code(BufferType) buffer_=new BufferType[allocw_*alloch_];
  ConditionListBegin;
  ConditionListBody( format_, type_ );
  ConditionListFail {
    vcl_cerr << __FILE__ << ": " << __LINE__ << ": unknown GL format ("
             << format_ << ") and type (" << type_ << ").\n"
             << "You can probably easily add support here.\n";
    assert( false );
  }
#undef Code
}


// ---------------------------------------------------------------------------
//                                                                  destructor

vgui_section_buffer::
~vgui_section_buffer()
{
  // We need to cast back to the correct type before we delete to make
  // sure the correct things happen. Since the data types are POD, it
  // doesn't really matter, because no destructors need to be
  // called. However, it's always good to do it correctly.
  //
#define Code(BufferType) delete[] static_cast<BufferType*>(buffer_);
  ConditionListBegin;
  ConditionListBody( format_, type_ );
  ConditionListFail {
    assert( false );
  }
#undef Code
}


// ---------------------------------------------------------------------------
//                                                  apply (vil image resource)

void
vgui_section_buffer::
apply( vil_image_resource_sptr const& image_in,
       vgui_range_map_params_sptr const& rmp)
{
  // In order to display the image, we need to convert the pixels from
  // the input image format to the OpenGL buffer format (given by
  // format_ and type_). So, there are two "run-time types" that we
  // need to handle: the input image pixel type, and the OpenGL buffer
  // pixel type. This function determines the first, and based on
  // that, calls the appropriate convert_image template instance. That
  // function will figure out the current OpenGL pixel type and call
  // convert_buffer to actually convert the pixels.

#define DoCase( T )                                                                \
      case T:                                                                      \
      {                                                                            \
        typedef vil_pixel_format_type_of<T>::type Type;                            \
        vil_image_view<Type> img = image_in->get_view( x_, w_, y_, h_ );           \
        assert( img );                                                             \
        conversion_okay = convert_image( img, rmp, buffer_, allocw_, format_, type_ );  \
        break;                                                                     \
      }

  bool conversion_okay = false;
  vil_pixel_format component_format =
    vil_pixel_format_component_format( image_in->pixel_format() );

  switch ( component_format ) {
    DoCase( VIL_PIXEL_FORMAT_UINT_32 )
    DoCase( VIL_PIXEL_FORMAT_INT_32 )
    DoCase( VIL_PIXEL_FORMAT_UINT_16 )
    DoCase( VIL_PIXEL_FORMAT_INT_16 )
    DoCase( VIL_PIXEL_FORMAT_BYTE )
    DoCase( VIL_PIXEL_FORMAT_SBYTE )
    DoCase( VIL_PIXEL_FORMAT_FLOAT )
    DoCase( VIL_PIXEL_FORMAT_DOUBLE )
    DoCase( VIL_PIXEL_FORMAT_BOOL )
    default:
      vcl_cerr << __FILE__ << ": " << __LINE__
               << ": can't handle image pixel format "
               << component_format << '\n';
  }

#undef DoCase

  if ( !conversion_okay ) {
    vcl_cerr << __FILE__ << ": " << __LINE__ << ": conversion failed\n";
  }

  buffer_ok_ = conversion_okay;
}


// ---------------------------------------------------------------------------
//                                                          apply (vil1 image)

void
vgui_section_buffer::
apply( vil1_image const& image,
       vgui_range_map_params_sptr const& rmp)
{
  // See comment in the other apply().

  assert( image.planes() == 1 ); // TODO: 3-plane RGB or 4-plane RGBA
  vil1_pixel_format_t pixel_format = vil1_pixel_format( image );

  bool conversion_ok = false;
  bool section_ok = false;

#define DoCase( PixelFormat, DataType, NComp )                                     \
      case PixelFormat:                                                            \
      {                                                                            \
        DataType* temp_buffer = new DataType[ w_ * h_ * NComp ];                   \
        section_ok = image.get_section( temp_buffer, x_, y_, w_, h_ );             \
        if ( section_ok ) {                                                        \
          vil_image_view<DataType> view( temp_buffer, w_, h_, NComp,               \
                                         NComp, NComp*w_, 1 );                     \
          conversion_ok = convert_image( view, rmp, buffer_, allocw_, format_, type_ ); \
        }                                                                          \
        delete[] temp_buffer;                                                      \
        break;                                                                     \
      }

  switch ( pixel_format ) {
    DoCase( VIL1_BYTE,       vxl_byte,    1 )
      DoCase( VIL1_UINT16,     vxl_uint_16, 1 )
      DoCase( VIL1_UINT32,     vxl_uint_32, 1 )
      DoCase( VIL1_FLOAT,      float,       1 )
      DoCase( VIL1_DOUBLE,     double,      1 )
      DoCase( VIL1_RGB_BYTE,   vxl_byte,    3 )
      DoCase( VIL1_RGB_UINT16, vxl_uint_16, 3 )
      DoCase( VIL1_RGB_FLOAT,  float,       3 )
      DoCase( VIL1_RGB_DOUBLE, double,      3 )
      DoCase( VIL1_RGBA_BYTE,  vxl_byte,    4 )
      default:
    vcl_cerr << __FILE__ << ": " << __LINE__
             << ": can't handle image pixel format "
             << vil1_print( pixel_format ) << '\n';
  }

#undef DoCase

  if ( !conversion_ok ) {
    vcl_cerr << __FILE__ << ": " << __LINE__ << ": conversion failed\n";
  }

  if (debug || !section_ok)
    vcl_cerr << (section_ok ? "section ok" : "section bad") << vcl_endl;

  buffer_ok_ = section_ok && conversion_ok;
}

// ---------------------------------------------------------------------------
//                                                           draw as rectangle

bool
vgui_section_buffer::
draw_as_rectangle( float x0, float y0,  float x1, float y1 ) const
{
  glColor3i( 0, 1, 0 ); // is green good for everyone?
  glLineWidth( 1 );
  glBegin( GL_LINE_LOOP );
  glVertex2f( x0, y0 );
  glVertex2f( x1, y0 );
  glVertex2f( x1, y1 );
  glVertex2f( x0, y1 );
  glEnd();
  return true;
}


bool
vgui_section_buffer::
draw_as_rectangle() const
{
  return draw_as_rectangle( x_, y_, x_+w_, y_+h_ );
}


// ---------------------------------------------------------------------------
//                                                               draw as image

bool
vgui_section_buffer::
draw_as_image( float x0, float y0,  float x1, float y1 ) const
{
  if ( !buffer_ok_ ) {
    vgui_macro_warning << "bad buffer in draw_as_image()\n";
    return draw_as_rectangle( x0, y0, x1, y1 );
  }

  return vgui_section_render( buffer_,
                              allocw_, alloch_,
                              x0,y0, x1, y1,
                              format_, type_ ,0/*, true*/ );
}

bool
vgui_section_buffer::
draw_as_image() const
{
  return draw_as_image( x_, y_, x_+w_, y_+h_ );
}

