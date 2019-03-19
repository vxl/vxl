// This is core/vpgl/algo/vpgl_lens_warp_mapper.h
#ifndef vpgl_lens_warp_mapper_h_
#define vpgl_lens_warp_mapper_h_
//:
// \file
// \brief A lens distortion adaptor for vil_warp
// \author Matt Leotta
// \date August 22, 2005
// \verbatim
//  Modifications
//   March 3, 2019 J.L. Mundy added pixel coordinate versions.
//  Standard lens distortion is in the focal plane not pixel coordinates
//  A calibration matrix is needed for the pixel coordinate functions
// \endverbatim
#include <vpgl/vpgl_lens_distortion.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute a bounding box in the distorted space for an existing box
// This is computed by sampling along the box boundary edges
// \note in general this is not the inverse of vpgl_lens_unwarp_bounds
template <class DataT, class BoxT>
vgl_box_2d<BoxT>
vpgl_lens_warp_bounds(const vpgl_lens_distortion<DataT>& lens,
                      const vgl_box_2d<BoxT>& box,
                      BoxT step_size = BoxT(1))
{
  vgl_box_2d<BoxT> new_box;
  for (BoxT x=box.min_x(); x<=box.max_x(); x+=step_size){
    vgl_point_2d<DataT> p1(lens.distort(vgl_homg_point_2d<DataT>(x,box.min_y())));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.distort(vgl_homg_point_2d<DataT>(x,box.max_y())));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));
  }
  for (BoxT y=box.min_y(); y<=box.max_y(); y+=step_size){
    vgl_point_2d<DataT> p1(lens.distort(vgl_homg_point_2d<DataT>(box.min_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.distort(vgl_homg_point_2d<DataT>(box.max_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));;
  }
  return new_box;
}

//: Compute a bounding box in the distorted pixel space
// This is computed by sampling along the box boundary edges
// \note in general this is not the inverse of vpgl_lens_unwarp_bounds
template <class DataT, class BoxT>
vgl_box_2d<BoxT>
vpgl_lens_warp_pixel_bounds(const vpgl_lens_distortion<DataT>& lens,
                            const vgl_box_2d<BoxT>& box, const vpgl_calibration_matrix<DataT>& K,
                            BoxT step_size = BoxT(1))
{
  vgl_box_2d<BoxT> new_box;
  for (BoxT x=box.min_x(); x<=box.max_x(); x+=step_size){
    vgl_point_2d<DataT> p1(lens.distort_pixel(vgl_homg_point_2d<DataT>(x,box.min_y()),K));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.distort_pixel(vgl_homg_point_2d<DataT>(x,box.max_y()),K));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));
  }
  for (BoxT y=box.min_y(); y<=box.max_y(); y+=step_size){
    vgl_point_2d<DataT> p1(lens.distort_pixel(vgl_homg_point_2d<DataT>(box.min_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.distort_pixel(vgl_homg_point_2d<DataT>(box.max_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));;
  }
  return new_box;
}

//: Compute a bounding box for an existing box in the distorted space
// This is computed by sampling along the box boundary edges
// \note in general this is not the inverse of vpgl_lens_warp_bounds
template <class DataT, class BoxT>
vgl_box_2d<BoxT>
vpgl_lens_unwarp_bounds(const vpgl_lens_distortion<DataT>& lens,
                        const vgl_box_2d<BoxT>& box,
                        BoxT step_size = BoxT(1))
{
  vgl_box_2d<BoxT> new_box;
  for (BoxT x=box.min_x(); x<=box.max_x(); x+=step_size){
    vgl_point_2d<DataT> p1(lens.undistort(vgl_homg_point_2d<DataT>(x,box.min_y())));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.undistort(vgl_homg_point_2d<DataT>(x,box.max_y())));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));
  }
  for (BoxT y=box.min_y(); y<=box.max_y(); y+=step_size){
    vgl_point_2d<DataT> p1(lens.undistort(vgl_homg_point_2d<DataT>(box.min_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.undistort(vgl_homg_point_2d<DataT>(box.max_x(),y)));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));;
  }
  return new_box;
}

//: Compute an undistorted bounding box in pixel coordinates
//  for an existing box in distorted image pixel coordinates
// This result is computed by sampling along the box boundary edges
// \note in general this is not the inverse of vpgl_lens_warp_bounds
template <class DataT, class BoxT>
vgl_box_2d<BoxT>
vpgl_lens_unwarp_pixel_bounds(const vpgl_lens_distortion<DataT>& lens,
                              const vgl_box_2d<BoxT>& box,
                              const vpgl_calibration_matrix<DataT>& K,
                              BoxT step_size = BoxT(1))
{
  vgl_box_2d<BoxT> new_box;
  for (BoxT x=box.min_x(); x<=box.max_x(); x+=step_size){
    vgl_point_2d<DataT> p1(lens.undistort_pixel(vgl_homg_point_2d<DataT>(x,box.min_y())));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.undistort_pixel(vgl_homg_point_2d<DataT>(x,box.max_y()),K));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));
  }
  for (BoxT y=box.min_y(); y<=box.max_y(); y+=step_size){
    vgl_point_2d<DataT> p1(lens.undistort_pixel(vgl_homg_point_2d<DataT>(box.min_x(),y)),K);
    new_box.add(vgl_point_2d<BoxT>( BoxT(p1.x()), BoxT(p1.y()) ));
    vgl_point_2d<DataT> p2(lens.undistort_pixel(vgl_homg_point_2d<DataT>(box.max_x(),y),K));
    new_box.add(vgl_point_2d<BoxT>( BoxT(p2.x()), BoxT(p2.y()) ));;
  }
  return new_box;
}


//: A warping function to apply lens distortion to an image
// This function automatically sets the translation of the distortion function
// and computes the appropriate image size such that all distorted pixel
// lie in the resulting image
template <class sType, class dType, class T, class InterpFunctor>
vil_image_view<dType>
vpgl_lens_warp_resize(const vil_image_view<sType>& in,
                      dType /*out_dummy*/,
                      vpgl_lens_distortion<T>& ld,
                      InterpFunctor interp)
{
  vgl_box_2d<int> bounds = vpgl_lens_warp_bounds(ld, vgl_box_2d<int>(0,in.ni(),0,in.nj()));
  vgl_vector_2d<T> offset(T(-bounds.min_x()), T(-bounds.min_y()));
  ld.set_translation( offset, true );
  vil_image_view<dType> out(bounds.width(), bounds.height(), in.nplanes());
  vpgl_lens_warp(in, out, ld, interp);
  return out;
}

//: A warping function to apply lens distortion to an image
// This function automatically sets the translation of the distortion function
// and computes the appropriate image size such that all distorted pixel
// lie in the resulting image
template <class sType, class dType, class T, class InterpFunctor>
vil_image_view<dType>
vpgl_lens_warp_pixel_resize(const vil_image_view<sType>& in,
                            dType /*out_dummy*/,
                            vpgl_lens_distortion<T>& ld,
                            const vpgl_calibration_matrix<T>& K,
                            InterpFunctor interp)
{
  vgl_box_2d<int> bounds = vpgl_lens_warp_pixel_bounds(ld, vgl_box_2d<int>(0,in.ni(),0,in.nj()),K);
  vgl_vector_2d<T> offset(T(-bounds.min_x()), T(-bounds.min_y()));
  ld.set_translation( offset, true );
  vil_image_view<dType> out(bounds.width(), bounds.height(), in.nplanes());
  vpgl_lens_warp(in, out, ld, interp);
  return out;
}

//: A version of vil_warp specialized for lens warping with iterative undistort
// The algorithm uses the result at the previous pixel as an initial guess
// which should reduce iterations until convergence
template <class sType, class dType, class T, class InterpFunctor>
void vpgl_lens_warp(const vil_image_view<sType>& in,
                    vil_image_view<dType>& out,
                    const vpgl_lens_distortion<T>& ld,
                    InterpFunctor interp)
{
  unsigned const out_w = out.ni();
  unsigned const out_h = out.nj();

  assert(out.nplanes() == in.nplanes());

  vgl_homg_point_2d<T> init(T(0),T(0));
  for (unsigned oy = 0; oy < out_h; ++oy)
  {
    vgl_homg_point_2d<T> unwarp_pt = init;
    for (unsigned ox = 0; ox < out_w; ++ox)
    {
      // *** Find (ix, iy) from (ox,oy)
      double ix, iy;
      unwarp_pt = ld.undistort(vgl_homg_point_2d<T>(ox,oy),&unwarp_pt);
      if (oy == 0) init = unwarp_pt;
      ix = unwarp_pt.x()/unwarp_pt.w();
      iy = unwarp_pt.y()/unwarp_pt.w();
      for (unsigned p = 0; p < out.nplanes(); ++p)
      {
        out(ox, oy, p) = dType(interp(in, ix, iy, p));
      }
    }
  }
}

//: A version of vil_warp specialized for lens warping with iterative undistort
// The algorithm uses the result at the previous pixel as an initial guess
// which should reduce iterations until convergence
template <class sType, class dType, class T, class InterpFunctor>
void vpgl_lens_warp_pixel(const vil_image_view<sType>& in,
                          vil_image_view<dType>& out,
                          const vpgl_lens_distortion<T>& ld,
                          const vpgl_calibration_matrix<T>& K,
                          InterpFunctor interp)
{
  unsigned const out_w = out.ni();
  unsigned const out_h = out.nj();

  assert(out.nplanes() == in.nplanes());

  vgl_homg_point_2d<T> init(T(0),T(0));
  for (unsigned oy = 0; oy < out_h; ++oy)
  {
    vgl_homg_point_2d<T> unwarp_pt = init;
    for (unsigned ox = 0; ox < out_w; ++ox)
    {
      // *** Find (ix, iy) from (ox,oy)
      double ix, iy;
      unwarp_pt = ld.undistort_pixel(vgl_homg_point_2d<T>(ox,oy),K, &unwarp_pt);
      if (oy == 0) init = unwarp_pt;
      ix = unwarp_pt.x()/unwarp_pt.w();
      iy = unwarp_pt.y()/unwarp_pt.w();
      for (unsigned p = 0; p < out.nplanes(); ++p)
      {
        out(ox, oy, p) = dType(interp(in, ix, iy, p));
      }
    }
  }
}

//: A warping function to apply lens distortion to an image
// This function automatically sets the translation of the distortion function
// and computes the appropriate image size such that all distorted pixel
// lie in the resulting image
template <class sType, class dType, class T, class InterpFunctor>
vil_image_view<dType>
vpgl_lens_unwarp_resize(const vil_image_view<sType>& in,
                        dType /*out_dummy*/,
                        vpgl_lens_distortion<T>& ld,
                        InterpFunctor interp)
{
  vgl_box_2d<int> bounds = vpgl_lens_unwarp_bounds(ld, vgl_box_2d<int>(0,in.ni(),0,in.nj()));
  vgl_vector_2d<T> offset(T(-bounds.min_x()), T(-bounds.min_y()));
  ld.set_translation( offset, false );
  vil_image_view<dType> out(bounds.width(), bounds.height(), in.nplanes());
  vpgl_lens_unwarp(in, out, ld, interp);
  return out;
}

//: A warping function to apply lens distortion to an image
// This function automatically sets the translation of the distortion function
// and computes the appropriate image size such that all distorted pixel
// lie in the resulting image
template <class sType, class dType, class T, class InterpFunctor>
vil_image_view<dType>
vpgl_lens_unwarp_pixel_resize(const vil_image_view<sType>& in,
                        dType /*out_dummy*/,
                        vpgl_lens_distortion<T>& ld,
                        const vpgl_calibration_matrix<T>& K,
                        InterpFunctor interp)
{
  vgl_box_2d<int> bounds = vpgl_lens_unwarp_pixel_bounds(ld, vgl_box_2d<int>(0,in.ni(),0,in.nj()), K);
  vgl_vector_2d<T> offset(T(-bounds.min_x()), T(-bounds.min_y()));
  ld.set_translation( offset, false );
  vil_image_view<dType> out(bounds.width(), bounds.height(), in.nplanes());
  vpgl_lens_unwarp(in, out, ld, interp);
  return out;
}

//: A version of vil_warp specialized for lens unwarping
template <class sType, class dType, class T, class InterpFunctor>
void vpgl_lens_unwarp(const vil_image_view<sType>& in,
                      vil_image_view<dType>& out,
                      const vpgl_lens_distortion<T>& ld,
                      InterpFunctor interp)
{
  unsigned const out_w = out.ni();
  unsigned const out_h = out.nj();

  assert(out.nplanes() == in.nplanes());

  for (unsigned oy = 0; oy < out_h; ++oy)
  {
    for (unsigned ox = 0; ox < out_w; ++ox)
    {
      vgl_point_2d<T> pt = ld.distort(vgl_homg_point_2d<T>(ox,oy));
      for (unsigned p = 0; p < out.nplanes(); ++p)
      {
        out(ox, oy, p) = dType(interp(in, pt.x(), pt.y(), p));
      }
    }
  }
}

//: A version of vil_warp specialized for lens unwarping
template <class sType, class dType, class T, class InterpFunctor>
void vpgl_lens_unwarp_pixel(const vil_image_view<sType>& in,
                            vil_image_view<dType>& out,
                            const vpgl_lens_distortion<T>& ld,
                            const vpgl_calibration_matrix<T>& K,
                            InterpFunctor interp)
{
  unsigned const out_w = out.ni();
  unsigned const out_h = out.nj();

  assert(out.nplanes() == in.nplanes());

  for (unsigned oy = 0; oy < out_h; ++oy)
  {
    for (unsigned ox = 0; ox < out_w; ++ox)
    {
      vgl_point_2d<T> pt = ld.distort_pixel(vgl_homg_point_2d<T>(ox,oy), K);
      for (unsigned p = 0; p < out.nplanes(); ++p)
      {
        out(ox, oy, p) = dType(interp(in, pt.x(), pt.y(), p));
      }
    }
  }
}

#endif // vpgl_lens_warp_mapper_h_
