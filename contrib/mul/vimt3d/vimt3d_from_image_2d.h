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
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_from_image_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <mbl/mbl_exception.h>


//: Return a 3D image containing a single slice obtained from a 2D image.
//  result(x,y,0,p)=im(x,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
// \relates vimt_image
// \relates vimt3d_image
// \note Currently only implemented for some transform types i.e. Identity, Translation and ZoomOnly
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
  switch(form2d)
  {
  case vimt_transform_2d::Identity:
    transf3d.set_identity(); 
    break;

  case vimt_transform_2d::Translation:
    {
      vnl_vector<double> v(2);
      transf2d.params(v);
      double tx=v(0), ty=v(1), tz=0.0;
      transf3d.set_translation(tx, ty, tz);
    }
    break;

  case vimt_transform_2d::ZoomOnly:
    {
      vnl_vector<double> v(4);
      transf2d.params(v);
      double sx=v(0), sy=v(1), sz=1.0, tx=v(2), ty=v(3), tz=0.0;
      transf3d.set_zoom_only(sx, sy, sz, tx, ty, tz);
    }
    break;

  default:
    mbl_exception_error(mbl_exception_abort("vimt3d_from_image_2d(): "
      "Unable to handle transforms"));
    break;
  }

  // Return a 3D image constructed from the image_view + transform
  return vimt3d_image_3d_of<T>(imview3d, transf3d);
}


#endif // vimt3d_from_image_2d_h_
