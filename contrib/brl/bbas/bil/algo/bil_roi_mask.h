// This is contrib/brl/bbas/bil/algo/bil_roi_mask.h
#ifndef bil_roi_mask_h_
#define bil_roi_mask_h_
//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 12/07/2006
// \verbatim
//  Modifications
// \endverbatim

#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_ellipse_scan_iterator.h>

// ------------------------------------------------------------------
//: Add a polygon to a ROI mask
// (assumption on mask: true for ROI pixels, false otherwise)
template <class T>
inline void bil_roi_mask_add_polygon(vil_image_view<bool >& mask,
                   const vgl_polygon<T >& polygon
                   )
{
  // do nothing for empty view
  if (!mask) return;

  // bounding box of image
  vgl_box_2d<T> box(T(0), T(mask.ni()-1), T(0), T(mask.nj()-1));

  // use polygon scan iterator to extract image points in the polygons
  vgl_polygon_scan_iterator<T > psi(polygon,  true, box);
  for (psi.reset(); psi.next();) 
  {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x)
    {
      mask(x, y) = true;
    }
  }
  return;
}


// ------------------------------------------------------------------
//: Remove a polygon from a ROI mask
// (assumption on mask: true for ROI pixels, false otherwise)
template <class T>
inline void bil_roi_mask_remove_polygon(vil_image_view<bool >& mask,
                   const vgl_polygon<T >& polygon
                   )
{
  // do nothing for empty view
  if (!mask) return;

  // bounding box of image
  vgl_box_2d<T> box(T(0), T(mask.ni()-1), T(0), T(mask.nj()-1));

  // use polygon scan iterator to extract image points in the polygons
  vgl_polygon_scan_iterator<T > psi(polygon,  true, box);
  for (psi.reset(); psi.next();) 
  {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x)
    {
      mask(x, y) = false;
    }
  }
  return;
}




// ------------------------------------------------------------------
//: Toggle a polygon to a ROI mask
// (turn true to false and vice versa)
// (assumption on mask: true for ROI pixels, false otherwise)
template <class T>
inline void bil_roi_mask_toggle_polygon(vil_image_view<bool >& mask,
                   const vgl_polygon<T >& polygon
                   )
{
  // do nothing for empty view
  if (!mask) return;

  // bounding box of image
  vgl_box_2d<T> box(T(0), T(mask.ni()-1), T(0), T(mask.nj()-1));

  // use polygon scan iterator to extract image points in the polygons
  vgl_polygon_scan_iterator<T > psi(polygon,  true, box);
  for (psi.reset(); psi.next();) 
  {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x)
    {
      mask(x, y) = !mask(x,y);
    }
  }
  return;
}



// ------------------------------------------------------------------
//: Add an ellipse to a ROI mask
// (assumption on mask: true for ROI pixels, false otherwise)
//  The ellipse is parameterised by (\a xc, \a yc) (the centre), by \a
//  rx and \a ry (the radii along the principle axes) and by \a theta,
//  the rotation of the main axis (in radians) about the centre of the
//  ellipse w.r.t\. the horizontal direction (X-axis).
template <class T>
inline void bil_roi_mask_add_ellipse(vil_image_view<bool >& mask,
                   T xc, T yc, T rx, T ry, T theta)
{
  // do nothing for empty view
  if (!mask) return;

  // bounding box of image
  vgl_box_2d<T> box(T(0), T(mask.ni()-1), T(0), T(mask.nj()-1));

  // use polygon scan iterator to extract image points in the polygons
  vgl_ellipse_scan_iterator<T> esi(xc, yc, rx, ry, theta, true, box);
  for (esi.reset(); esi.next();) 
  {
    int y = esi.scany();
    for (int x = esi.startx(); x <= esi.endx(); ++x)
    {
      mask(x, y) = true;
    }
  }
  return;
}



// ------------------------------------------------------------------
//: Remove an ellipse to a ROI mask
// (assumption on mask: true for ROI pixels, false otherwise)
//  The ellipse is parameterised by (\a xc, \a yc) (the centre), by \a
//  rx and \a ry (the radii along the principle axes) and by \a theta,
//  the rotation of the main axis (in radians) about the centre of the
//  ellipse w.r.t\. the horizontal direction (X-axis).
template <class T>
inline void bil_roi_mask_remove_ellipse(vil_image_view<bool >& mask,
                   T xc, T yc, T rx, T ry, T theta)
{
  // do nothing for empty view
  if (!mask) return;

  // bounding box of image
  vgl_box_2d<T> box(T(0), T(mask.ni()-1), T(0), T(mask.nj()-1));

  // use polygon scan iterator to extract image points in the polygons
  vgl_ellipse_scan_iterator<T> esi(xc, yc, rx, ry, theta, true, box);
  for (esi.reset(); esi.next();) 
  {
    int y = esi.scany();
    for (int x = esi.startx(); x <= esi.endx(); ++x)
    {
      mask(x, y) = false;
    }
  }
  return;
}


#endif 
