// This is core/vil/vil_clamp.h
#ifndef vil_clamp_h_
#define vil_clamp_h_
//:
// \file
// \author Ian Scott.
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier - added vil_clamp_below
// \endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Clamp an image view between two values.
// \relatesalso vil_image_view
template <class T>
inline void vil_clamp(const vil_image_view<T >&src, vil_image_view<T >&dest, T lo, T hi)
{
  assert (hi >= lo);
  assert (src.nplanes() == dest.nplanes() &&
          src.nj() == dest.nj() &&
          src.ni() == dest.ni());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
      {
        const T v = src(i,j,p);
        dest(i,j,p) = v<lo?lo:(v>hi?hi:v);
      }
}


//: Clamp an image resource between two values.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_clamp(const vil_image_resource_sptr &src, double low, double hi);


//: A generic_image adaptor that behaves like a clamped version of its input
// For implementation use only - use vil_clamp() to create one.
class vil_clamp_image_resource : public vil_image_resource
{
  vil_clamp_image_resource(vil_image_resource_sptr const&, double low, double high);
  friend vil_image_resource_sptr vil_clamp(const vil_image_resource_sptr &src, double low, double hi);
 public:

  unsigned nplanes() const override { return src_->nplanes(); }
  unsigned ni() const override { return src_->ni(); }
  unsigned nj() const override { return src_->nj(); }

  enum vil_pixel_format pixel_format() const override { return src_->pixel_format(); }


  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const override {
    return get_copy_view(i0, ni, j0, nj); }


  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& /*im*/, unsigned /*i0*/, unsigned /*j0*/) override { return false; }

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override;

 protected:
  //: Reference to underlying image source
  vil_image_resource_sptr src_;
  //: Lower clamp value
  double lo_;
  //: Upper clamp value
  double hi_;
};

//: Clamp an image view above a given value t, setting it to v if below or on t
// \relatesalso vil_image_view
template <class T>
inline void vil_clamp_below(vil_image_view<T>& src, T t, T v)
{
   std::ptrdiff_t istepA=src.istep(), jstepA=src.jstep(), pstepA=src.planestep();
   T* planeA = src.top_left_ptr();
   for (unsigned int p=0; p<src.nplanes(); ++p,planeA+=pstepA)
   {
     T* rowA = planeA;
     for (unsigned int j=0; j<src.nj(); ++j,rowA+=jstepA)
     {
       T* pixelA = rowA;
       for (unsigned int i=0; i<src.ni(); ++i,pixelA+=istepA)
         if (*pixelA <= t)
           *pixelA = v;
     }
  }
}

//: Clamp an image view below a given value t, setting it to v if above or on t
// \relatesalso vil_image_view
template <class T>
inline void vil_clamp_above(vil_image_view<T>& src, T t, T v)
{
   std::ptrdiff_t istepA=src.istep(), jstepA=src.jstep(), pstepA=src.planestep();
   T* planeA = src.top_left_ptr();
   for (unsigned int p=0; p<src.nplanes(); ++p,planeA+=pstepA)
   {
     T* rowA = planeA;
     for (unsigned int j=0; j<src.nj(); ++j,rowA+=jstepA)
     {
       T* pixelA = rowA;
       for (unsigned int i=0; i<src.ni(); ++i,pixelA+=istepA)
         if (*pixelA >= t)
           *pixelA = v;
     }
  }
}

//: Clamp an image view above a given value t, setting it to this t if below t
// \relatesalso vil_image_view
template <class T>
inline void vil_clamp_below(vil_image_view<T>& src, T t)
{
  vil_clamp_below(src, t, t);
}

//: Clamp an image view below a given value t, setting it to this t if above t
// \relatesalso vil_image_view
template <class T>
inline void vil_clamp_above(vil_image_view<T>& src, T t)
{
  vil_clamp_above(src, t, t);
}


#endif // vil_clamp_h_
