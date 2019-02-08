// this is bbax/bil/algo/bil_warp.h
#ifndef bil_warp_h
#define bil_warp_h
//:
// \file
// \brief warp an image according to a homography transform
// \author J.L. Mundy
// \date Feburary 7, 2019
//

#include <iostream>
#include <limits>
#include <vil/vil_image_view.h>
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_det.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bool bil_warp_image_bilinear(vil_image_view<T> const& in_image, vnl_matrix_fixed<double, 3, 3> const& H,
                             vil_image_view<T>& warp_image, bool clip_not_translate = false,
                             T fill_value = T(0), size_t min_size = 5){
  size_t ni = in_image.ni(), nj = in_image.nj();
  double singular_tol = 1e-6;
  double ideal_tol = 1e-8;
  size_t np = in_image.nplanes();
  std::vector<vil_image_view<T> > planes;
  for(size_t p = 0; p<np; ++p){
    vil_image_view<T> view(ni, nj);
    for(size_t j = 0; j<nj; ++j)
      for(size_t i = 0; i<ni; ++i)
        view(i,j) = in_image(i,j,p);
    planes.push_back(view);
  }
  // see if H is singular
  double d = vnl_det(H);
  if(fabs(d) < singular_tol){
    std::cout << "singular homography in warp" << std::endl;
    return false;
  }
  // transform image bounds
  double dni = static_cast<double>(ni), dnj = static_cast<double>(nj);
  vnl_vector_fixed<double, 3> ul(0,0,1), ul_wrp, ur(ni-1.0,0,1), ur_wrp,
    ll(0,nj-1.0,1), ll_wrp, lr(ni-1.0,nj-1.0,1), lr_wrp;
  ul_wrp = H*ul;   ur_wrp = H*ur; ll_wrp = H*ll; lr_wrp = H*lr;
  if(fabs(ul_wrp[2])<ideal_tol ||fabs(ur_wrp[2])<ideal_tol
     || fabs(ll_wrp[2])<ideal_tol|| fabs(lr_wrp[2])<ideal_tol)
  {
    std::cout << "homography produces ideal points" << std::endl;
    return false;
  }
  ul_wrp/=ul_wrp[2];  ur_wrp/=ur_wrp[2]; ll_wrp/=ll_wrp[2];lr_wrp/=lr_wrp[2];
  vgl_box_2d<double> warp_bb;
  warp_bb.add(vgl_point_2d<double>(ul_wrp[0],ul_wrp[1]));
  warp_bb.add(vgl_point_2d<double>(ur_wrp[0],ur_wrp[1]));
  warp_bb.add(vgl_point_2d<double>(ll_wrp[0],ll_wrp[1]));
  warp_bb.add(vgl_point_2d<double>(lr_wrp[0],lr_wrp[1]));
   // translation to shift output coordinate system
  // initialize to identity
  vnl_matrix_fixed<double, 3, 3> tr(0.0);
  tr[0][0] = 1.0;   tr[1][1] = 1.0;   tr[2][2] = 1.0;
  size_t warp_ni = 0, warp_nj = 0;

  if(clip_not_translate){
    // don't shift the output coordinate frame to include the entire mapped input image
    double w = warp_bb.max_x(), h = warp_bb.max_y();
    warp_ni = static_cast<size_t>(w+0.5)+1;
    warp_nj = static_cast<size_t>(h+0.5)+1;
  } else{
    // shift the output coordinates be positive definite,
    // i.e., the minimum bound maps to (0, 0)
    double w = warp_bb.width(), h = warp_bb.height();
    tr[0][2] = -warp_bb.min_x();   tr[1][2] = -warp_bb.min_y();
    warp_ni = static_cast<size_t>(w+0.5)+1;
    warp_nj = static_cast<size_t>(h+0.5)+1;
  }
  if(warp_ni < min_size || warp_nj < min_size){
    std::cout << "warped image dimensions too small (" << warp_ni << ' ' << warp_nj << ")" << std::endl;
    return false;
  }
  vnl_matrix_fixed<double, 3, 3> Hinv = vnl_inverse(tr*H);
  warp_image.set_size(warp_ni, warp_nj, np);
  warp_image.fill(fill_value);
  double vmax = static_cast<double>(std::numeric_limits<T>::max());
  bool integer_val = std::numeric_limits<T>::is_integer;
  // obtain the interpolated values from the input image.
  for(size_t p = 0; p<np; ++p){
    for(size_t j = 0; j<warp_nj; ++j){
      double dj = j;
      for(size_t i = 0; i<warp_ni; ++i){
        double di = i;
        vnl_vector_fixed<double, 3> w_pix(di, dj, 1), in_pix;
        in_pix = Hinv*w_pix;
        double du_in = in_pix[0]/in_pix[2], dv_in = in_pix[1]/in_pix[2];
        if(du_in <0.0 || du_in >= dni || dv_in<0.0 ||dv_in>=dnj)
          continue;
        double dval = vil_bilin_interp_safe_extend(planes[p], du_in, dv_in);
        if(integer_val)
          dval += 0.5;//for rounding
        if (dval > vmax) dval = vmax;
        T val = static_cast<T>(dval);
        warp_image(i, j, p) = val;
      }
    }
  }
  return true;
}

#endif // bil_warp_h
