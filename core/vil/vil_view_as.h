// This is mul/vil2/vil2_view_as.h
#ifndef vil2_view_as_h_
#define vil2_view_as_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Various view conversion functions.
// vil2_image_view<T>::operator=() can automatically perform these
// conversions for you.
// \author Tim Cootes, Ian Scott - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>


//: Return a 3-plane view of an RGB image (or a 4-plane view if RGBA)
// \return an empty view if it can't do the conversion.
//  O(1).
// \relates vil2_image_view
// class T must be a compound pixel type.
template<class T>
inline vil2_image_view<typename T::value_type> vil2_view_as_planes(const vil2_image_view<T >& v)
{
  if (v.nplanes()!=1) return vil2_image_view<T>();
  const unsigned ncomponents = sizeof(T) / sizeof(T::value_type);

  // Image is RGBRGBRGB so i step = 3ncomponents*v.istep(), jstep=ncomponents*v.jstep()
#if VCL_VC60 || !VCL_HAS_TYPENAME
  return vil2_image_view<T::value_type>(
    v.memory_chunk(),reinterpret_cast<T::value_type const*>(v.top_left_ptr()),
#else
  return vil2_image_view<typename T::value_type>(
    v.memory_chunk(),reinterpret_cast<typename T::value_type const*>( v.top_left_ptr()),
#endif
    v.ni(),v.nj(),ncomponents,
    v.istep()*ncomponents,v.jstep()*ncomponents,1);
}

//: Return an RGB component view of a 3-plane image.
// \return an empty view if it can't do the conversion (e.g. planestep != 1)
//  O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& v)
{
  if ((v.nplanes()!=3) || (v.planestep()!=1) || (v.istep()!=3 && v.jstep()!=3))
    return vil2_image_view<vil_rgb<T> >();

  return vil2_image_view<vil_rgb<T> >(v.memory_chunk(),
                                      reinterpret_cast<vil_rgb<T> const*>( v.top_left_ptr()),
                                      v.ni(),v.nj(),1,
                                      v.istep()/3,v.jstep()/3,1);
}

//: Return an RGBA component view of a 4-plane image.
// \return an empty view if it can't do the conversion (e.g. planestep != 1)
//  O(1).
// \relates vil2_image_view
template<class T>
inline vil2_image_view<vil_rgba<T> > vil2_view_as_rgba(const vil2_image_view<T>& v)
{
  if ((v.nplanes()!=4) || (v.planestep()!=1) || (v.istep()!=4 && v.jstep()!=4))
    return vil2_image_view<vil_rgba<T> >();

  return vil2_image_view<vil_rgba<T> >(v.memory_chunk(),
                                       static_cast<vil_rgba<T> const*>( v.top_left_ptr()),
                                       v.ni(),v.nj(),1,
                                       v.istep()/3,v.jstep()/3,1);
}


#endif // vil2_view_as_h_
