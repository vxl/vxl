// This is mul/vimt3d/vimt3d_from_image_2d.h
#ifndef vimt3d_from_image_2d_h_
#define vimt3d_from_image_2d_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \author Kevin de Souza

#include <vcl_cassert.h>

#include <vil/vil_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_from_image_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt3d/vimt3d_image_3d_of.h>



//: Return a 3D image containing a single slice obtained from a 2D image.
//  result(x,y,0,p)=im(x,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
// \relates vimt_image
// \relates vimt3d_image
template <class T>
inline vimt3d_image_3d_of<T> vimt3d_from_image_2d(const vimt_image_2d_of<T>& img)
{
  // Do image view
  const vil_image_view<T>& imview2d = img.image();
  vil3d_image_view<T> imview3d = vil3d_from_image_2d(imview2d);

  // Do transform
  const vimt_transform_2d& transf2d =  img.world2im();
  vimt_transform_2d::Form form2d = transf2d.form();
  vimt3d_transform_3d transf3d;
  if (form2d==vimt_transform_2d::Identity)
  {
    transf3d.set_identity(); 
  }
  else if (form2d==vimt_transform_2d::ZoomOnly)
  {
    vnl_vector<double> v(4);
    transf2d.params(v);
    double sx=v(0), sy=v(1), sz=1.0, tx=v(2), ty=v(3), tz=0.0;
    transf3d.set_zoom_only(sx, sy, sz, tx, ty, tz);
  }
  else
  {
    // Undefined
  }

  // Return a 3D image constructed from the image_view + transform
  return vimt3d_image_3d_of<T>(imview3d, transf3d);
}


#endif // vimt3d_from_image_2d_h_
