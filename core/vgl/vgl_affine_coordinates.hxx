// This is core/vgl/vgl_affine_coordinates.hxx
#ifndef vgl_affine_coordinates_hxx_
#define vgl_affine_coordinates_hxx_
#include <cassert>
#include <cmath>
#include <vgl/vgl_affine_coordinates.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_tolerance.h>
// Points are all coplanar. The first three points in pts are the basis, pts[0] is the origin
template <class T>
void vgl_affine_coordinates_2d(std::vector<vgl_point_2d<T> > const& pts, std::vector<vgl_point_2d<T> >& affine_pts)
{
  assert(pts.size()>=3);
  vgl_vector_2d<T> v0 = pts[1]-pts[0];
  vgl_vector_2d<T> v1 = pts[2]-pts[0];
  T dv0v0 = dot_product(v0,v0);
  T dv0v1 = dot_product(v0,v1);
  T dv1v1 = dot_product(v1,v1);
  T det = dv0v0*dv1v1 - dv0v1*dv0v1;
  T tol = vgl_tolerance<T>::position;
  assert(fabs(det)>tol);
  for(unsigned i =0; i<pts.size(); ++i){
    vgl_vector_2d<T> vp = pts[i]-pts[0];
    T dvpv0 = dot_product(vp, v0);
    T dvpv1 = dot_product(vp, v1);
    T alpha = (dvpv0*dv1v1-dv0v1*dvpv1)/det;
    T beta  = (dv0v0*dvpv1-dvpv0*dv0v1)/det;
    affine_pts.push_back(vgl_point_2d<T>(alpha, beta));
  }
}
// The first four points in pts are the basis.
template <class T>
void vgl_affine_coordinates_3d(std::vector<vgl_point_3d<T> > const& pts, std::vector<vgl_point_3d<T> >& affine_pts)
{
    assert(pts.size()>=4);
    vgl_vector_3d<T> v0 = pts[1]-pts[0];
    vgl_vector_3d<T> v1 = pts[2]-pts[0];
    vgl_vector_3d<T> v2 = pts[3]-pts[0];
    T dv0v0 = dot_product(v0,v0);
    T dv0v1 = dot_product(v0,v1);
    T dv0v2 = dot_product(v0,v2);
    T dv1v1 = dot_product(v1,v1);
    T dv1v2 = dot_product(v1,v2);
    T dv2v2 = dot_product(v2,v2);
    T det = -dv0v2*dv0v2*dv1v1 + 2*dv0v1*dv0v2*dv1v2 - dv0v0*dv1v2*dv1v2 - dv0v1*dv0v1*dv2v2 + dv0v0*dv1v1*dv2v2;
    T tol = vgl_tolerance<T>::position;
    assert(fabs(det)>tol);
    for(unsigned i =0; i<pts.size(); ++i){
      vgl_vector_3d<T> vp = pts[i]-pts[0];
      T dvpv0 = dot_product(vp, v0);
      T dvpv1 = dot_product(vp, v1);
      T dvpv2 = dot_product(vp, v2);
      T alpha = (-dv1v2*dv1v2*dvpv0 + dv1v1*dv2v2*dvpv0 + dv0v2*dv1v2*dvpv1 -
                 dv0v1*dv2v2*dvpv1 - dv0v2*dv1v1*dvpv2 + dv0v1*dv1v2*dvpv2)/det;
      T beta = (dv0v2*dv1v2*dvpv0 - dv0v1*dv2v2*dvpv0 - dv0v2*dv0v2*dvpv1 +
              dv0v0*dv2v2*dvpv1 + dv0v1*dv0v2*dvpv2 - dv0v0*dv1v2*dvpv2)/det;
      T gamma = (-dv0v2*dv1v1*dvpv0 + dv0v1*dv1v2*dvpv0 + dv0v1*dv0v2*dvpv1 -
               dv0v0*dv1v2*dvpv1 - dv0v1*dv0v1*dvpv2 + dv0v0*dv1v1*dvpv2)/det;
      affine_pts.push_back(vgl_point_3d<T>(alpha, beta, gamma));
    }
}

// Two 2-d pointsets define the 3-d basis. The first four points in pts1 and pts2 are the basis.
template <class T>
void vgl_affine_coordinates_3d(std::vector<vgl_point_2d<T> > const& pts1, std::vector<vgl_point_2d<T> > const& pts2,
                               std::vector<vgl_point_3d<T> >& affine_pts)
{
  unsigned n1 = pts1.size(), n2 = pts2.size();
  assert(n1>=4);
  assert(n1==n2);
  T tol = vgl_tolerance<T>::position;
  //normalize 2-d pointsets
  T x1=T(0), x2=T(0), y1 = T(0), y2 = T(0);
  T dist_1 = T(0), dist_2 = T(0);
  for(unsigned i = 0; i<n1;++i){
    x1+=pts1[i].x(); y1+=pts1[i].y();
    x2+=pts2[i].x(); y2+=pts2[i].y();
  }
  vgl_point_2d<T> cent_1(x1/n1, y1/n1), cent_2(x2/n1, y2/n1);
  for(unsigned i = 0; i<n1;++i){
    dist_1+=(pts1[i]-cent_1).length();
    dist_2+=(pts2[i]-cent_2).length();
  }
  dist_1/=n1; dist_2/=n2;
  std::vector<vgl_point_2d<T> > norm_pts1, norm_pts2;
  for(unsigned i = 0; i<n1;++i){
    vgl_point_2d<T> np1(pts1[i].x()-cent_1.x(), pts1[i].y()-cent_1.y());
    if(dist_1>tol)
      np1.set(np1.x()/dist_1, np1.y()/dist_1);
    vgl_point_2d<T> np2(pts2[i].x()-cent_2.x(), pts2[i].y()-cent_2.y());
    if(dist_2>tol)
      np2.set(np2.x()/dist_2, np2.y()/dist_2);
    norm_pts1.push_back(np1);
    norm_pts2.push_back(np2);
  }
  // 2-d basis for pts1
  vgl_vector_2d<T> v01 = norm_pts1[1]-norm_pts1[0];
  vgl_vector_2d<T> v11 = norm_pts1[2]-norm_pts1[0];
  T dv01v01 = dot_product(v01,v01);
  T dv01v11 = dot_product(v01,v11);
  T dv11v11 = dot_product(v11,v11);
  T det1 = dv01v01*dv11v11 - dv01v11*dv01v11;
  assert(fabs(det1)>tol);
  //affine coordinates for V2' (pts1)
  vgl_vector_2d<T> v21 = norm_pts1[3]-norm_pts1[0];
  T dv21v01 = dot_product(v01, v21);
  T dv21v11 = dot_product(v11, v21);
  T alpha_1 = (dv21v01*dv11v11-dv01v11*dv21v11)/det1;
  T beta_1  = (dv01v01*dv21v11-dv21v01*dv01v11)/det1;
  // 2-d basis for pts2
  vgl_vector_2d<T> v02 = norm_pts2[1]-norm_pts2[0];
  vgl_vector_2d<T> v12 = norm_pts2[2]-norm_pts2[0];
  T dv02v02 = dot_product(v02,v02);
  T dv02v12 = dot_product(v02,v12);
  T dv12v12 = dot_product(v12,v12);
  T det2 = dv02v02*dv12v12 - dv02v12*dv02v12;
  assert(fabs(det2)>tol);
  //affine coordinates for V2 (pts2)
  vgl_vector_2d<T> v22 = norm_pts2[3]-norm_pts2[0];
  T dv22v02 = dot_product(v02, v22);
  T dv22v12 = dot_product(v12, v22);
  T alpha_2 = (dv22v02*dv12v12-dv02v12*dv22v12)/det2;
  T beta_2  = (dv02v02*dv22v12-dv22v02*dv02v12)/det2;
  //length of V2'V2 in pts2
  vgl_vector_2d<T> V2pV2 = (alpha_2-alpha_1)*v02 + (beta_2-beta_1)*v12;
  T L2 = V2pV2.length();
  assert(L2>tol);// otherwise the views are the same or the basis points are coplanar
  //determine sign of L2
  T s2 = (alpha_2-alpha_1), fs2 = fabs(s2);
  if(fs2<tol){
    s2 = (beta_2-beta_1);
    fs2 = fabs(s2);
    if(fs2<tol){
      s2 = 1.0;
      fs2 = 1.0;
    }
  }
  L2 *= s2/fs2;
  for(unsigned i = 0; i<n1; ++i){
    //affine coordinates for p in pts1
    vgl_vector_2d<T> vp1 = norm_pts1[i]-norm_pts1[0];
    T dvp1v01 = dot_product(vp1, v01);
    T dvp1v11 = dot_product(vp1, v11);
    T alpha_p1 = (dvp1v01*dv11v11-dv01v11*dvp1v11)/det1;
    T beta_p1  = (dv01v01*dvp1v11-dvp1v01*dv01v11)/det1;
    //affine coordinates for p in pts2
    vgl_vector_2d<T> vp2 = norm_pts2[i]-norm_pts2[0];
    T dvp2v02 = dot_product(vp2, v02);
    T dvp2v12 = dot_product(vp2, v12);
    T alpha_p2 = (dvp2v02*dv12v12-dv02v12*dvp2v12)/det2;
    T beta_p2  = (dv02v02*dvp2v12-dvp2v02*dv02v12)/det2;
    // length of PP' in pts 2
    vgl_vector_2d<T> PpP = (alpha_p2-alpha_p1)*v02 + (beta_p2-beta_p1)*v12;
    T Lp = PpP.length();
    //determine sign of PP'
    T sp = (alpha_p2-alpha_p1), fsp = fabs(sp);
    if(fsp<tol){
      sp = (beta_p2-beta_p1);
      fsp = fabs(sp);
      if(fsp<tol){
        sp = 1.0;
        fsp= 1.0;
      }
    }
    Lp *= sp/fsp;
    T gamma_p = Lp/L2;
    T alpha_p = (alpha_p1-gamma_p*alpha_1);
    T beta_p = (beta_p1-gamma_p*beta_1);
    affine_pts.push_back(vgl_point_3d<T>(alpha_p, beta_p, gamma_p));
  }
}
#undef VGL_AFFINE_COORDINATES_INSTANTIATE
#define VGL_AFFINE_COORDINATES_INSTANTIATE(T) \
  template void vgl_affine_coordinates_2d(std::vector<vgl_point_2d<T> > const& pts, std::vector<vgl_point_2d<T> >& affine_pts); \
  template void vgl_affine_coordinates_3d(std::vector<vgl_point_3d<T> > const& pts, std::vector<vgl_point_3d<T> >& affine_pts); \
  template void vgl_affine_coordinates_3d(std::vector<vgl_point_2d<T> > const& pts1, std::vector<vgl_point_2d<T> > const& pts2, \
                                          std::vector<vgl_point_3d<T> >& affine_pts)
#endif // vgl_affine_coordinates_h_
