// This is mul/vimt3d/vimt3d_slice.h
#ifndef vimt3d_slice_h_
#define vimt3d_slice_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Chris Wolstenholme

#include <vil/vil_image_view.h>
#include <vil3d/vil3d_slice.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt/vimt_transform_2d.h>
#include <vcl_cassert.h>

//: Return a 2D view of slice k of 3D image aligned as (j,i).
//  result(x,y,p)=im(y,x,k,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_ji(const vimt3d_image_3d_of<T> &im, unsigned k)
{
  // Check the slice is in range
  assert(k<im.image().nk());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_ji(im.image(),k);

  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(1,1),trans3.matrix()(0,0),trans3.matrix()(1,3),trans3.matrix()(0,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

//: Return a 2D view of slice k of 3D image aligned as (i,j)
//  result(x,y,p)=im(x,y,k,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_ij(const vimt3d_image_3d_of<T> &im, unsigned k)
{
  // Check the slice is in range
  assert(k<im.image().nk());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_ij(im.image(),k);

  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(0,0),trans3.matrix()(1,1),trans3.matrix()(0,3),trans3.matrix()(1,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

//: Return a 2D view of slice i of 3D image aligned as (j,k)
//  result(x,y,p)=im(i,x,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_jk(const vimt3d_image_3d_of<T> &im, unsigned i)
{
  // Check the slice is in range
  assert(i<im.image().ni());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_jk(im.image(),i);

  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(1,1),trans3.matrix()(2,2),trans3.matrix()(1,3),trans3.matrix()(2,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

//: Return a 2D view of slice i of 3D image aligned as (k,j)
//  result(x,y,p)=im(i,y,x,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_kj(const vimt3d_image_3d_of<T> &im, unsigned i)
{
  // Check the slice is in range
  assert(i<im.image().ni());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_kj(im.image(),i);
 
  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(2,2),trans3.matrix()(1,1),trans3.matrix()(2,3),trans3.matrix()(1,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

//: Return a 2D view of slice j of 3D image aligned as (k,i)
//  result(x,y,p)=im(y,i,x,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_ki(const vimt3d_image_3d_of<T> &im, unsigned j)
{
  // Check the slice is in range
  assert(j<im.image().nj());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_ki(im.image(),j);

  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(2,2),trans3.matrix()(0,0),trans3.matrix()(2,3),trans3.matrix()(0,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

//: Return a 2D view of slice j of 3D image aligned as (i,k)
//  result(x,y,p)=im(x,i,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
template<class T>
inline vimt_image_2d_of<T> vimt3d_slice_ik(const vimt3d_image_3d_of<T> &im, unsigned j)
{ 
  // Check the slice is in range
  assert(j<im.image().nj());

  // Get the 2d view
  vil_image_view<T> imview = vil3d_slice_ik(im.image(),j);

  // Check the transform
  vimt3d_transform_3d trans3 = im.world2im();
    assert(trans3.is_identity() || trans3.form()==vimt3d_transform_3d::ZoomOnly);

  // Get the appropriate transform details
  vimt_transform_2d trans2;
  trans2.set_zoom_only(trans3.matrix()(0,0),trans3.matrix()(2,2),trans3.matrix()(0,3),trans3.matrix()(2,3));

  return vimt_image_2d_of<T>(imview,trans2);
}

#endif // vimt3d_slice_h_
