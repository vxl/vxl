// This is core/vgui/internals/vgui_generic_vil_image_view.cxx

//:
// \file
// \author Amitha Perera

#include "vgui_generic_vil_image_view.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>


// =============================================================================
//                                                               HELPER ROUTINES
// =============================================================================

namespace {

// The process of converting to and from compile-time type
// information to run-time type information is somewhat complex. The
// idea is to use the pixel format and type information stored in
// vil_pixel_format along with template recursion to get the
// compiler to write all the cases.
//
// When given a vil_image_base object, we need to case it to a
// concrete vil_image_view<T> object to extract the data pointers
// and such. Therefore, we need to examine the pixel_format of the
// input image and cast it to the appropriate vil_image_view. Now,
// for any pixel format enumeration fmt,
// vil_pixel_format_type_of<fmt>::type is the corresponding C++
// type. What we have to do is write a series of if statements that
// compare the input image format against each enumeration in
// vil_pixel_format and do the appropriate cast. Writing the list
// explicitly will create the possibility of inconsistency in this
// list and the list in vil_pixel_format. So, we write a recursive
// template that will generate the list for us. This template is in
// the class extract. The terminanting condition is when the
// enumeration reaches VIL_PIXEL_FORMAT_ENUM_END; we specialize the
// class to indicate this. The function extract_data starts off the
// recursion at pixel_format 1.
//
// A wrinkle is that not every integer between 1 and
// VIL_PIXEL_FORMAT_ENUM_END corresponds to a valid pixel format. We
// can detect this at compile time by examining the corresponding C++
// type: the invalid formats will have a C++ type of
// vil_pixel_format_invalid_type. The function cast_and_extract does
// this check by specializing for vil_pixel_format_invalid_type, and
// doing nothing in the specialization. The specialization should
// never be called, since an image should never have an invalid pixel
// format. However, the code is necessary because the compiler must
// generate code for these cases too, since it has no guarantee that
// these pixel types will not occur at run-time.
//
// Similarly, when creating a concrete vil_image_view<T> from the
// stored information, we need to find the concrete C++ type
// corresponding to the stored pixel format. We use the same
// recurvsive template idea to implement this.
//
// One practical issue that arises with this compile time recursion is
// that some compilers have a maximum recursion depth, which we will
// exceed. (17 seems to be a magic limit, while we have about 30 file
// formats.) We work around this problem by creating "breaks" in the
// recursion, by specializing the template at steps of 10. The idea is
// that if the template f<n> calls f<n+1>, then we provide
// specializations f<10>, f<20>, etc. When the compiler tries to
// generate code for f<1>, it'll generate f<1> through f<9>. f<9> will
// call the specialzied f<10>, so the recursive generation will
// stop. Maximum recursive instantiation depth: about 10. When it
// tries to generate the code for f<10>, it'll instantiate f<11>
// through f<19>. And so on. Now, the body of these special "break"
// specialization is the same as the normal template, except for a few
// syntactic changes. So, we define the normal recursive body in a
// macro, and use that macro for generating both the normal template
// and for the breaks. This workaround is used for the extract and
// try_view_of classes below.
//
// We use classes to implement the recursion because some compilers
// (e.g. MSVC 6) is broken when it comes to functions and integral
// template parameters.


// We use this below to pass in a "blank" parameter to macros. At
// least one compiler (MSVC6) requires a token between the commas in
// the macro call.
//
#define NoParameter


// ---------------------------------------------------------------------------
//                                                                   view data

// This stores the pertinent information extracted from the concrete
// vil_image_view<T>
//
struct view_data
{
  void const* top_left;
  vcl_ptrdiff_t istep;
  vcl_ptrdiff_t jstep;
  vcl_ptrdiff_t pstep;
  vil_memory_chunk_sptr ptr;

  view_data( void const * tl, vcl_ptrdiff_t is, vcl_ptrdiff_t js,
             vcl_ptrdiff_t ps, vil_memory_chunk_sptr const& p )
    : top_left( tl ), istep( is ), jstep( js ),
      pstep( ps ), ptr( p )
    { }
};


// ---------------------------------------------------------------------------
//                                                            cast and extract


// Cast if the pixel format is valid. See comment above.
//
template < class T >
view_data
cast_and_extract( vil_image_view_base const& base_view,
                  T* /*dummy*/ )
{
  vil_image_view<T> const& img =
    static_cast< vil_image_view<T> const& >( base_view );
  return view_data( img.top_left_ptr(), img.istep(), img.jstep(),
                    img.planestep(), img.memory_chunk() );
}

// Cast if the pixel format is valid. See comment above.
//
VCL_DEFINE_SPECIALIZATION
view_data
cast_and_extract( vil_image_view_base const& base_view,
                  vil_pixel_format_invalid_type* /*dummy*/ )
{
  vcl_cerr << __FILE__ << ": " << __LINE__ << ": "
           << "Error: image pixel format " << base_view.pixel_format()
           << " does not have a valid C++ type\n";
  return view_data( 0, 0, 0, 0, 0 );
}


// ---------------------------------------------------------------------------
//                                                                     extract

// Extract the data if the image pixel format is fmt or
// higher. Terminate the recursion at VIL_PIXEL_FORMAT_ENUM_END, since
// there are no valid pixels formats after that.
//

// See the comments above on why we have these macros

// fmt is the current format enumeration. fmt_specialization is
// nothing for the normal template, and is <fmt> for a
// specialization. Typename is either the keyword "typename" (for a
// normal tempalate) or is nothing (for a specialization)
//
#define ExtractDefinition( fmt, fmt_specialization, Typename )                  \
  struct extract fmt_specialization                                             \
  {                                                                             \
    static view_data                                                            \
    process( vil_image_view_base const& other )                                 \
      {                                                                         \
        if ( other.pixel_format() == fmt ) {                                    \
          typedef Typename vil_pixel_format_type_of<fmt>::type pixel_type;      \
          return cast_and_extract( other, (pixel_type*)0 );                     \
        } else {                                                                \
          return extract< vil_pixel_format(fmt+1) >::process( other );          \
        }                                                                       \
      }                                                                         \
  }

#define ExtractRecursionBreak( fmt )                    \
  VCL_DEFINE_SPECIALIZATION                             \
  ExtractDefinition( vil_pixel_format( fmt ),           \
                     < vil_pixel_format( fmt ) >,       \
                     NoParameter /*no typename*/ )


// This defines the normal, recucursively instantiating template
template < vil_pixel_format fmt >
ExtractDefinition( fmt, NoParameter /*no specialization*/, typename );

// Terminating condition.
//
VCL_DEFINE_SPECIALIZATION
struct extract<VIL_PIXEL_FORMAT_ENUM_END>
{
  static view_data
  process( vil_image_view_base const& other )
    {
      vcl_cerr << __FILE__ << ": " << __LINE__ << ": "
               << "Error: image pixel format " << other.pixel_format()
               << " is not known\n";
      return view_data( 0, 0, 0, 0, 0 );
    }
};

// These specialization form "breaks" in the recursion. Must list in
// descending order.
ExtractRecursionBreak(30);
ExtractRecursionBreak(20);
ExtractRecursionBreak(10);


// ---------------------------------------------------------------------------
//                                                                extract data

// Find extract the image view data from the object.
//
view_data
extract_data( vil_image_view_base const& other )
{
  // Start at format number 1 since number 0 is the unknown pixel format.
  assert( VIL_PIXEL_FORMAT_UNKNOWN == vil_pixel_format(0) );
  return extract< vil_pixel_format(1) >::process( other );
}


// ---------------------------------------------------------------------------
//                                                              create view of

// Create a view for C++ type T
//
template < class T >
vil_image_view_base_sptr
create_view_of( vgui_generic_vil_image_view const& generic,
                T* /*dummy*/ )
{
  typedef vil_image_view<T> image_type;

  return new image_type( generic.memory_chunk(),
                         (const T*)generic.top_left_ptr(),
                         generic.ni(), generic.nj(), generic.nplanes(),
                         generic.istep(), generic.jstep(), generic.planestep() );
}

// Don't create a view for vil_pixel_format_invalid_type, since it
// represents an invalid pixel format.
//
VCL_DEFINE_SPECIALIZATION
vil_image_view_base_sptr
create_view_of( vgui_generic_vil_image_view const& generic,
                vil_pixel_format_invalid_type* /*dummy*/ )
{
  vcl_cerr << __FILE__ << ": " << __LINE__ << ": "
           << "Error: image pixel format " << generic.pixel_format()
           << " does not have a valid C++ type\n";
  return 0;
}


// ---------------------------------------------------------------------------
//                                                                 try view of

// See if a pixel format >= fmt will fit the stored pixel format.
//

// See the comments above on why we have these macros

// fmt is the current format enumeration. fmt_specialization is
// nothing for the normal template, and is <fmt> for a
// specialization. Typename is either the keyword "typename" (for a
// normal template) or is nothing (for a specialization).
//
#define TryViewOfDefinition( fmt, fmt_specialization, Typename )                \
struct try_view_of fmt_specialization                                           \
{                                                                               \
  static vil_image_view_base_sptr                                               \
  process( vgui_generic_vil_image_view const& generic )                         \
    {                                                                           \
      if ( generic.pixel_format() == fmt ) {                                    \
        typedef Typename vil_pixel_format_type_of< fmt >::type pixel_type;      \
        return create_view_of( generic, (pixel_type*)0 );                       \
      } else {                                                                  \
        return try_view_of< vil_pixel_format( fmt + 1 ) >::process( generic );  \
      }                                                                         \
    }                                                                           \
};

#define TryViewOfRecursionBreak( fmt )                  \
  VCL_DEFINE_SPECIALIZATION                             \
  TryViewOfDefinition( vil_pixel_format( fmt ),         \
                       < vil_pixel_format( fmt ) >,     \
                       NoParameter /*no typename*/ )

// This defines the normal, recucursively instantiating template
template < vil_pixel_format fmt >
TryViewOfDefinition( fmt, NoParameter /*no specialization*/, typename );

// Terminating condition. There are no pixel formats >=
// VIL_PIXEL_FORMAT_ENUM_END.
//
VCL_DEFINE_SPECIALIZATION
struct try_view_of<VIL_PIXEL_FORMAT_ENUM_END>
{
  static vil_image_view_base_sptr
  process( vgui_generic_vil_image_view const& generic )
    {
      vcl_cerr << __FILE__ << ": " << __LINE__ << ": "
               << "Error: image pixel format " << generic.pixel_format()
               << " is not known\n";
      return 0;
    }
};

// These specialization form "breaks" in the recursion. Must list in
// descending order.
TryViewOfRecursionBreak(30);
TryViewOfRecursionBreak(20);
TryViewOfRecursionBreak(10);


// ---------------------------------------------------------------------------
//                                                             make image view

// Create a vil_image_view<T> object with an appropriate T for the
// stored pixel format, and return a smart pointer to this object.
//
vil_image_view_base_sptr
make_image_view( vgui_generic_vil_image_view const& generic )
{
  // Start at format number 1 since number 0 is the unknown pixel format.
  assert( VIL_PIXEL_FORMAT_UNKNOWN == vil_pixel_format(0) );
  return try_view_of< vil_pixel_format(1) >::process( generic );
}

} // anonymous namespace


// =============================================================================
//                                                   VGUI GENERIC VIL IMAGE VIEW
// =============================================================================


// ---------------------------------------------------------------------------
//                                                                constructors

vgui_generic_vil_image_view::
vgui_generic_vil_image_view( )
  : top_left_( 0 ),
    istep_( 0 ),
    jstep_( 0 ),
    planestep_( 0 ),
    ptr_( 0 ),
    pixel_format_( VIL_PIXEL_FORMAT_UNKNOWN )
{
}


vgui_generic_vil_image_view::
vgui_generic_vil_image_view( vil_image_view_base const& img )
  : ni_( img.ni() ),
    nj_( img.nj() ),
    nplanes_( img.nplanes() ),
    pixel_format_( img.pixel_format() )
{
  view_data data = extract_data( img );
  top_left_ = data.top_left;
  istep_ = data.istep;
  jstep_ = data.jstep;
  planestep_ = data.pstep;
  ptr_ = data.ptr;
}


// ---------------------------------------------------------------------------
//                                                                   operator=

void
vgui_generic_vil_image_view::
operator=( vil_image_view_base const& img )
{
  // sets the dimensions
  ni_ = img.ni();
  nj_ = img.nj();
  nplanes_ = img.nplanes();
  pixel_format_ = img.pixel_format();

  view_data data = extract_data( img );
  top_left_ = data.top_left;
  istep_ = data.istep;
  jstep_ = data.jstep;
  planestep_ = data.pstep;
  ptr_ = data.ptr;
}


// ---------------------------------------------------------------------------
//                                                                   make view

vil_image_view_base_sptr
vgui_generic_vil_image_view::
make_view() const
{
  // call on helper routine
  return make_image_view( *this );
}


// ---------------------------------------------------------------------------
//                                                                pixel format

vil_pixel_format
vgui_generic_vil_image_view::
pixel_format() const
{
  return pixel_format_;
}
