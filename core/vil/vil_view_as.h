// This is core/vil/vil_view_as.h
#ifndef vil_view_as_h_
#define vil_view_as_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Various view conversion functions.
//
// vil_image_view<T>::operator=() can automatically perform these
// conversions for you.
// \author Tim Cootes, Ian Scott - Manchester

#include <vcl_complex.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>

//: Return a 3-plane view of an RGB image, or a 4-plane view of an RGBA, or a 2-plane view of a complex image.
// Class T must be a compound pixel type.
// \return an empty view if it can't do the conversion.
// O(1).
// \relates vil_image_view
template<class T>
inline vil_image_view<typename T::value_type> vil_view_as_planes(const vil_image_view<T >& v)
{
  typedef typename T::value_type comp_type;
  if (v.nplanes()!=1) return vil_image_view<T>();
  const unsigned ncomponents = sizeof(T) / sizeof(comp_type);

  // An RGB image is RGBRGBRGB, an RGBA image is RGBARGBARGBA, and a
  // complex image is RCRCRC, so istep = ncomponents*v.istep(), and
  // jstep = ncomponents*v.jstep().

  return vil_image_view<comp_type>(
    v.memory_chunk(),reinterpret_cast<comp_type const*>(v.top_left_ptr()),
    v.ni(),v.nj(),ncomponents,
    v.istep()*ncomponents,v.jstep()*ncomponents,1);
}

//: Return an RGB component view of a 3-plane image.
// \return an empty view if it can't do the conversion (e.g. planestep != 1)
// O(1).
// \relates vil_image_view
template<class T>
inline vil_image_view<vil_rgb<T> > vil_view_as_rgb(const vil_image_view<T>& v)
{
  if ((v.nplanes()!=3) || (v.planestep()!=1) || (v.istep()!=3 && v.jstep()!=3))
    return vil_image_view<vil_rgb<T> >();

  return vil_image_view<vil_rgb<T> >(v.memory_chunk(),
                                     reinterpret_cast<vil_rgb<T> const*>( v.top_left_ptr()),
                                     v.ni(),v.nj(),1,
                                     v.istep()/3,v.jstep()/3,1);
}

//: Return an RGBA component view of a 4-plane image.
// \return an empty view if it can't do the conversion (e.g. planestep != 1)
// O(1).
// \relates vil_image_view
template<class T>
inline vil_image_view<vil_rgba<T> > vil_view_as_rgba(const vil_image_view<T>& v)
{
  if ((v.nplanes()!=4) || (v.planestep()!=1) || (v.istep()!=4 && v.jstep()!=4))
    return vil_image_view<vil_rgba<T> >();

  return vil_image_view<vil_rgba<T> >(v.memory_chunk(),
                                      static_cast<vil_rgba<T> const*>( v.top_left_ptr()),
                                      v.ni(),v.nj(),1,
                                      v.istep()/3,v.jstep()/3,1);
}

//: Return an complex component view of a 2N-plane image.
// \return an empty view if it can't do the conversion (e.g. planestep != 1)
// O(1).
// \relates vil_image_view
template<class T>
inline vil_image_view<vcl_complex<T> >
vil_view_as_complex (const vil_image_view<T> & v)
{
  if ((v.nplanes()%2!=0) || (v.planestep()!=1) || (v.istep()!=2 && v.jstep()!=2))
      return vil_image_view<vcl_complex<T> >();

  return vil_image_view<vcl_complex<T> > (
      v.memory_chunk(),
      reinterpret_cast<vcl_complex<T> const *> (v.top_left_ptr()),
      v.ni(), v.nj(), v.nplanes()/2,
      v.istep()/2, v.jstep()/2, 1);
}

//: Base function to do the work for both vil_view_real/imag_part
// O(1).
// \relates vil_image_view
template <class T>
inline vil_image_view<T>
vil_view_part (vil_image_view<vcl_complex<T> > img, int pt)
{
  return vil_image_view<T> (
      img.memory_chunk(),
      reinterpret_cast<T *>(img.top_left_ptr()) + pt,
      img.ni(), img.nj(), img.nplanes(),
      2*img.istep(), 2*img.jstep(), 2*img.planestep());
}

//: Return a view of the real part of a complex image.
// O(1).
// \relates vil_image_view
template <class T>
inline vil_image_view<T>
vil_view_real_part (vil_image_view<vcl_complex<T> > img)
{
  return vil_view_part (img, 0);
}

//: Return a view of the imaginary part of a complex image.
// O(1).
// \relates vil_image_view
template <class T>
inline vil_image_view<T>
vil_view_imag_part (vil_image_view<vcl_complex<T> > img)
{
  return vil_view_part (img, 1);
}

#endif // vil_view_as_h_
