// This is core/vil1/vil1_vil.h
#ifndef vil1_vil_h_
#define vil1_vil_h_
//:
// \file
// \brief Provide inline vil wrappers of vil1 and vice-versa.

#include <vil1/vil1_fwd.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vil/vil_memory_chunk.h>

//: Create a view of a vil1_memory_image_of<T>
// Warning, the created view does not share ownership of the underlying image. The
// vil1_memory_image_of<T> should not be destroyed while the view is in use.
// The function assumes that the addressing increments to the next column, row or plane
// are constant throughout the image.

template <class T>
vil_image_view<T> vil1_to_vil_image_view(const vil1_memory_image_of<T> &vil1_im)
{
  const T* tl = &vil1_im(0,0);

  vcl_ptrdiff_t planestep;

  if (vil1_im.planes() == 1)
    planestep = vil1_im.cols()* vil1_im.rows();
  else
    planestep = &((vil1_memory_image_of<T>(vil1_im.get_plane(1)))(0,0)) - tl;

  return vil_image_view<T>(tl,
                           vil1_im.cols(), vil1_im.rows(), vil1_im.planes(),
                           &vil1_im(1,0) - tl, &vil1_im(0,1) - tl, planestep);
}


//: Create a vil1 memory image from a vil image view.
// Warning, the created vil1 image doesn't not share ownership of the underlying image. The
// vil_image_view<T> should not be destroyed while the vil1 image is in use.
template <class T>
vil1_memory_image_of<T> vil1_from_vil_image_view(const vil_image_view<T> &vil_im)
{
  if (!vil_im.is_contiguous() || vil_im.nplanes() != 1)
  {
    vcl_cerr << "WARNING vil1_vil1_from_image_view(): Unable to create vil1_memory_image_of<T>\n";
    return vil1_memory_image_of<T>();
  }

  return vil1_memory_image_of<T>(const_cast<T*>(vil_im.top_left_ptr()),
                                                vil_im.ni(), vil_im.nj());
}

//: Create a vil_image_resource from a vil1_image.
inline vil_image_resource_sptr vil1_to_vil_image_resource(const vil1_image &vil1_im);


//: This class wraps a vil1_image to provide a vil style interface
class vil1_vil_image_resource: public vil_image_resource
{
  vil1_vil_image_resource(const vil1_image &src) : src_(src)
  {
    // can't cope with images organised RRR..GGG..BBB.. dues to vil1_images useless support for planes.
    assert(src_.planes() == 1);
    // can't cope with bool images because vil1_image packs the bits.
    assert(src_.bits_per_component() > 1);
  }

  friend vil_image_resource_sptr vil1_to_vil_image_resource(const vil1_image &vil1_im);
  vil1_image src_;

 public:
  virtual vil_pixel_format pixel_format() const
  {
    if (!src_)
      return VIL_PIXEL_FORMAT_UNKNOWN;

    switch(src_.component_format())
    {
     case VIL1_COMPONENT_FORMAT_UNKNOWN:
     case VIL1_COMPONENT_FORMAT_COMPLEX:
      return VIL_PIXEL_FORMAT_UNKNOWN;
     case VIL1_COMPONENT_FORMAT_UNSIGNED_INT:
      if (src_.bits_per_component() == 1) return VIL_PIXEL_FORMAT_BOOL;
      else if (src_.bits_per_component() <= 8) return VIL_PIXEL_FORMAT_BYTE;
      else if (src_.bits_per_component() <= 16) return VIL_PIXEL_FORMAT_UINT_16;
      else if (src_.bits_per_component() <= 32) return VIL_PIXEL_FORMAT_UINT_32;
      else return VIL_PIXEL_FORMAT_UNKNOWN;
     case VIL1_COMPONENT_FORMAT_SIGNED_INT:
      if (src_.bits_per_component() == 1) return VIL_PIXEL_FORMAT_BOOL;
      else if (src_.bits_per_component() <= 8) return VIL_PIXEL_FORMAT_SBYTE;
      else if (src_.bits_per_component() <= 16) return VIL_PIXEL_FORMAT_INT_16;
      else if (src_.bits_per_component() <= 32) return VIL_PIXEL_FORMAT_INT_32;
      else return VIL_PIXEL_FORMAT_UNKNOWN;
     case VIL1_COMPONENT_FORMAT_IEEE_FLOAT:
      if (src_.bits_per_component() == 32) return VIL_PIXEL_FORMAT_FLOAT;
      else if (src_.bits_per_component() <= 64) return VIL_PIXEL_FORMAT_DOUBLE;
      else return VIL_PIXEL_FORMAT_UNKNOWN;
     default: return VIL_PIXEL_FORMAT_UNKNOWN;
    }
  }

  virtual unsigned ni() const { if (src_) return src_.width(); else return 0; }
  virtual unsigned nj() const { if (src_) return src_.height(); else return 0; }
  virtual unsigned nplanes() const { if (!src_) return 0; else return src_.components(); }

  virtual bool get_property(char const *tag, void *property_value=0) const
  {
    if (src_)
      return src_.get_property(tag, property_value);
    else
      return false;
  }

  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, unsigned j0, unsigned nj) const
  {
    if (!src_)
      return 0;

    switch (pixel_format())
    {
#define macro( F , T ) \
    case F: { \
        vil_memory_chunk_sptr chunk = new \
          vil_memory_chunk(ni*nj*nplanes()*sizeof(T ), F ); \
        src_.get_section(chunk->data(), i0, j0, ni, nj); \
        return new vil_image_view<T >(chunk, (const T *)chunk->data(), \
                                      ni, nj, nplanes(), nplanes(), ni*nplanes(), 1); }

        macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
        macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
        macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
        macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
        macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
        macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
        macro(VIL_PIXEL_FORMAT_FLOAT , float )
        macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
    default: return 0;
    }
  }

  virtual bool put_view(const vil_image_view_base &im, unsigned i0, unsigned j0)
  {
    if (!view_fits(im, i0, j0)) return false;
    if (!src_) return false;

    switch (pixel_format())
    {
#define macro( F , T ) \
    case F: { \
        const vil_image_view<T > &view = \
          static_cast<const vil_image_view<T > &>(im); \
        assert(nplanes()==1 || view.planestep() == 1); \
        src_.put_section(view.top_left_ptr(), i0, j0, im.ni(), im.nj()); }

        macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
        macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
        macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
        macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
        macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
        macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
        macro(VIL_PIXEL_FORMAT_FLOAT , float )
        macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
    default: return false;
    }
  }
};

inline vil_image_resource_sptr vil1_to_vil_image_resource(const vil1_image &vil1_im)
{
  if (!vil1_im) return 0;
  return new vil1_vil_image_resource(vil1_im);
}

#endif // vil1_vil_h_
