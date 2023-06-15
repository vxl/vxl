// This is core/vgl/algo/vgl_intersection.hxx
#ifndef vgl_algo_intersection_hxx_
#define vgl_algo_intersection_hxx_
//:
// \file
// \author Gamze Tunali

#include "vgl_intersection.h"

#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
vgl_point_3d<T> vgl_intersection(std::vector<vgl_plane_3d<T> > const& p)
{
  std::vector<vgl_homg_plane_3d<T> > planes;
  for (unsigned i=0; i<p.size(); ++i) {
    planes.push_back(vgl_homg_plane_3d<T> (p[i]));
  }

  return vgl_homg_operators_3d<T>::intersection(planes);
}


template <class T>
vgl_infinite_line_3d<T>
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes)
{
  if (planes.size() == 0)
    return vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),vgl_vector_3d<T>(0,0,0));

  // form the matrix of plane normal monomials
  vnl_matrix<double> Q(3,3,0.0);
  vnl_vector<double> vd(3,0.0);
  unsigned n = planes.size();
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = (*pit).a(), b = (*pit).b(), c = (*pit).c(),d = (*pit).d();
    Q[0][0] += a*a; Q[0][1] += a*b; Q[0][2] += a*c;
    Q[1][1] += b*b; Q[1][2] += b*c;
    Q[2][2] += c*c;
    vd[0]-=a*d; vd[1]-=b*d; vd[2]-=c*d;
  }
  Q[1][0]=  Q[0][1];   Q[2][0]= Q[0][2];   Q[2][1]=  Q[1][2];
  Q/=n;
  vd/=n;
  vnl_svd<double> svd(Q);

  // the direction of the resulting line
  vnl_vector<double> t = svd.nullvector();

  double tx = t[0], ty = t[1], tz = t[2];
  double atx = std::fabs(tx), aty = std::fabs(ty), atz = std::fabs(tz);
  // determine maximum component of t
  char component = 'x';
  if (aty>atx&&aty>atz)
    component = 'y';
  if (atz>atx&&atz>aty)
    component = 'z';
  vgl_point_3d<double> p0d;
  switch (component)
  {
    case 'x':
    {
      double det = Q[1][1]*Q[2][2] - Q[1][2]*Q[2][1];
      double neuy = vd[1]*Q[2][2]  - Q[1][2]*vd[2];
      double neuz = Q[1][1]*vd[2]  - vd[1]*Q[2][1];
      p0d.set(0.0, neuy/det, neuz/det);
      break;
    }
    case 'y':
    {
      double det = Q[0][0]*Q[2][2] - Q[0][2]*Q[2][0];
      double neux = vd[0]*Q[2][2]  - Q[0][2]*vd[2];
      double neuz = Q[0][0]*vd[2]  - vd[0]*Q[2][0];
      p0d.set(neux/det, 0.0, neuz/det);
      break;
    }
    case 'z':
    default:
    {
      double det = Q[0][0]*Q[1][1] - Q[0][1]*Q[1][0];
      double neux = vd[0]*Q[1][1]  - Q[0][1]*vd[1];
      double neuy = Q[0][0]*vd[1]  - vd[0]*Q[1][0];
      p0d.set(neux/det, neuy/det, 0.0);
      break;
    }
  }
  vgl_point_3d<T> pt(static_cast<T>(p0d.x()),
                     static_cast<T>(p0d.y()),
                     static_cast<T>(p0d.z()));

  vgl_vector_3d<T> tv(static_cast<T>(tx),
                      static_cast<T>(ty),
                      static_cast<T>(tz));

  return vgl_infinite_line_3d<T>(pt, tv);
}

template <class T>
bool
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes, std::vector<T> ws,
                 vgl_infinite_line_3d<T>& line, T &residual)
{
  if (planes.size() < 2)
    return false;
  // form the matrix of plane normal monomials
  vnl_matrix<double> Q(3,3,0.0);
  vnl_vector<double> vd(3,0.0);
  unsigned cnt=0;
  T sum_ws=0;
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = (*pit).a(), b = (*pit).b(), c = (*pit).c(), d = (*pit).d();
    T w=ws[cnt++];
    Q[0][0] += w*a*a;
    Q[0][1] += w*a*b; Q[1][1] += w*b*b;
    Q[0][2] += w*a*c; Q[1][2] += w*b*c; Q[2][2] += w*c*c;
    vd[0]-=w*a*d; vd[1]-=w*b*d; vd[2]-=w*c*d;
    sum_ws+=w;
  }
  Q[1][0]=  Q[0][1];   Q[2][0]= Q[0][2];   Q[2][1]=  Q[1][2];
  Q/=sum_ws;
  vd/=sum_ws;
  vnl_svd<double> svd(Q);
  // the direction of the resulting line
  vnl_vector<double> t = svd.nullvector();
  double tx = t[0], ty = t[1], tz = t[2];
  double atx = std::fabs(tx), aty = std::fabs(ty), atz = std::fabs(tz);
  // determine maximum component of t
  char component = 'x';
  if (aty>atx&&aty>atz)
    component = 'y';
  if (atz>atx&&atz>aty)
    component = 'z';
  vgl_point_3d<double> p0d;
  switch (component)
  {
    case 'x':
    {
      double det = Q[1][1]*Q[2][2] - Q[1][2]*Q[2][1];
      double neuy = vd[1]*Q[2][2]  - Q[1][2]*vd[2];
      double neuz = Q[1][1]*vd[2]  - vd[1]*Q[2][1];
      p0d.set(0.0, neuy/det, neuz/det);
      break;
    }
    case 'y':
    {
      double det = Q[0][0]*Q[2][2] - Q[0][2]*Q[2][0];
      double neux = vd[0]*Q[2][2]  - Q[0][2]*vd[2];
      double neuz = Q[0][0]*vd[2]  - vd[0]*Q[2][0];
      p0d.set(neux/det, 0.0, neuz/det);
      break;
    }
    case 'z':
    default:
    {
      double det = Q[0][0]*Q[1][1] - Q[0][1]*Q[1][0];
      double neux = vd[0]*Q[1][1]  - Q[0][1]*vd[1];
      double neuy = Q[0][0]*vd[1]  - vd[0]*Q[1][0];
      p0d.set(neux/det, neuy/det, 0.0);
      break;
    }
  }
  vgl_point_3d<T> pt(static_cast<T>(p0d.x()),
                     static_cast<T>(p0d.y()),
                     static_cast<T>(p0d.z()));

  vgl_vector_3d<T> tv(static_cast<T>(tx),
                      static_cast<T>(ty),
                      static_cast<T>(tz));
  residual=T(0);
  sum_ws=0;
  cnt=0;
  for (typename std::list<vgl_plane_3d<T> >::const_iterator pit = planes.begin();
       pit != planes.end(); ++pit)
  {
    double a = pit->normal().x(), b = pit->normal().y(), c = pit->normal().z();
    residual+=ws[cnt]*ws[cnt]*T(a*tx+b*ty+c*tz)*T(a*tx+b*ty+c*tz);
    sum_ws+=ws[cnt]*ws[cnt];
    cnt++;
  }

  if (cnt>0)
  {
    residual/=sum_ws;
    residual=std::sqrt(residual);
  }
  line=vgl_infinite_line_3d<T>(pt, tv);
  return true;
}


template <class T>
bool vgl_intersection(vgl_box_3d<T> const& b, std::list<vgl_point_3d<T> >& poly)
{
  // check if two bounding boxes intersect
  // find the bounding box of the polygon
  assert(poly.size() >= 3);

  vgl_box_3d<T> bb;
  typename std::list<vgl_point_3d<T> >::iterator it=poly.begin();
  for (; it != poly.end(); ++it)
    bb.add(*it);

  vgl_box_3d<T> inters = vgl_intersection(b, bb);
  if (inters.is_empty())
    return false;

  // check if the polygon corners inside the box
  for (it=poly.begin(); it != poly.end(); ++it) {
    if (b.contains(*it))
      return true;
  }

  it=poly.begin();
  // get the first 3 points to create a plane
  vgl_point_3d<T> p0=*it; ++it;
  vgl_point_3d<T> p1=*it; ++it;
  vgl_point_3d<T> p2=*it; ++it;
  // create a plane from polygon
  vgl_plane_3d<T> poly_plane(p0,p1,p2);
  if (!vgl_intersection<T>(b, poly_plane))
    return false;

  // now we do a 3D transformation of the polygon and the box center to the plane
  // where polygon resides, so that we can do 2D poly-point test
  vgl_vector_3d<T> n = poly_plane.normal();
  n=normalize(n);
  vgl_vector_3d<T> u(p1-p0);
  u=normalize(u);
  vgl_vector_3d<T> v = cross_product(n,u);

  vnl_matrix<T> M(3,3);
  M.put(0,0,u.x());
  M.put(1,0,u.y());
  M.put(2,0,u.z());
  M.put(0,1,v.x());
  M.put(1,1,v.y());
  M.put(2,1,v.z());
  M.put(0,2,n.x());
  M.put(1,2,n.y());
  M.put(2,2,n.z());

  vnl_matrix_inverse<T> R(M);

  // transform the polygon
  vgl_polygon<T> poly2d(1);  // with one sheet
  for (it=poly.begin(); it != poly.end(); ++it) {
    vgl_vector_3d<T> temp(*it-p0);
    vnl_matrix<T> tv(3,1);
    tv.put(0,0,temp.x());
    tv.put(1,0,temp.y());
    tv.put(2,0,temp.z());
    vnl_matrix<T> pi = R*tv;
    poly2d.push_back(pi.get(0,0), pi.get(1,0));
  }

  vgl_point_3d<T> c=b.centroid();
  vnl_matrix<T> tv(3,1);
  tv.put(0,0,c.x()-p0.x());
  tv.put(1,0,c.y()-p0.y());
  tv.put(2,0,c.z()-p0.z());
  vnl_matrix<T> ci(R*tv);
  return poly2d.contains(ci.get(0,0),ci.get(1,0));
}
//
// the linear least squares equation to solve is given by, Rp = q, where p is the unknown intersection point
// the 3x3 matrix R is Sum_i [ I - dir_i dir_i^T ], where dir_i is the unit direction vector of the ray.
// the 3x1 vector q is given by, Sum_i [ I - dir_i dir_i^T ]org_i, where org_i is the ray origin point.
//
template <class T>
bool vgl_intersection(std::vector<vgl_ray_3d<T> > const& rays, vgl_point_3d<T>& inter_pt){
  size_t n = rays.size();
  if(n<2){
    std::cout << "insufficient number of rays " << n << " to compute intersection" << std::endl;
    return false;
  }
  // compute R and q
  vnl_matrix_fixed<T, 3, 3> R(T(0)), I(T(0));
  vnl_matrix_fixed<T, 3, 1> q(T(0));
  I[0][0] = T(1);   I[1][1] = T(1);   I[2][2] = T(1);

  for(size_t i = 0; i<n; ++i){
    const vgl_ray_3d<T>& r = rays[i];
    vgl_point_3d<T> org_i = r.origin();
    vgl_vector_3d<T> dir_i = r.direction();
    vnl_matrix_fixed<T, 3, 1> ndir_i, norg_i;
    norg_i[0][0] = org_i.x(); norg_i[1][0] = org_i.y(); norg_i[2][0] = org_i.z();
    ndir_i[0][0] = dir_i.x(); ndir_i[1][0] = dir_i.y(); ndir_i[2][0] = dir_i.z();
    vnl_matrix_fixed<T, 3, 3> temp = ndir_i * (ndir_i.transpose()), temp2;
    temp2 = (I - temp);
    R += temp2;
    q += temp2*norg_i;
  }
  R/=T(n); q/=T(n);
  vnl_svd<T> svd(R.as_ref());
  size_t rank = svd.rank();
  if(rank < 3){
   std::cout << "insufficient svd rank " << rank << " to compute intersection" << std::endl;
    return false;
  }
  vnl_matrix<T> p = svd.solve(q.as_ref());
  inter_pt.set(p[0][0], p[1][0], p[2][0]);
  return true;
}
template <class T>
bool vgl_intersection(std::vector<vgl_ray_3d<T> > const& rays, vnl_matrix<T> const& covar, vgl_point_3d<T>& inter_pt){
  size_t n = rays.size(), nr = covar.rows(), nc = covar.cols();
  if(n<2){
    std::cout << "insufficient number of rays " << n << " to compute intersection" << std::endl;
    return false;
  }
  if(nr != nc || nr != 2*n){
    std::cout << "covariance matrix must be 2n x 2n where n is the number of rays" <<std::endl;
    return false;
  }
  vnl_svd<T> svd_cov(covar);
  if(svd_cov.rank() != nr){
   std::cout << "covariance matrix is singular" <<std::endl;
    return false;
  }
  vnl_matrix<T> cov_inv = svd_cov.inverse();
  // plane coordinate vectors for planes perpendicular to each ray
  // perp_T = [u_0, v_0, u_1, v_1, ... , u_n-1, v_n-1]
  vnl_matrix<T> perp_T(3, nr);
  vnl_matrix<T> ray_origins(3, n);
  size_t pidx = 0;
  for(size_t i = 0; i<n; i++, pidx+=2){
    const vgl_ray_3d<T>& r = rays[i];
    vgl_point_3d<T> org_i = r.origin();
    vgl_vector_3d<T> dir_i = r.direction();
    // define the plane perpendicular to ray i
    vgl_plane_3d<T> pl_perp(dir_i, org_i);
    vgl_vector_3d<T> uvec, vvec;
    // extract the unit vectors for the plane coordinate axes
    pl_perp.plane_coord_vectors(uvec, vvec);
    // convert to vnl
    vnl_vector_fixed<T,3> ui(uvec.x(), uvec.y(), uvec.z());
    vnl_vector_fixed<T,3> vi(vvec.x(), vvec.y(), vvec.z());
    perp_T.set_column(pidx, ui.as_vector());
    perp_T.set_column(pidx+1, vi.as_vector());
    // cache the ray origins for the next stage
    ray_origins[0][i] = org_i.x();
    ray_origins[1][i] = org_i.y();
    ray_origins[2][i] = org_i.z();
  }
  // the average ray projection matrix A
  vnl_matrix<T> A = perp_T * cov_inv * perp_T.transpose();
  vnl_svd<T> svd_A(A);
  if(svd_A.rank() != 3){
    std::cout << "matrix A has insufficient rank" << std::endl;
    return false;
  }
  vnl_matrix<T> A_inv = svd_A.inverse();
  vnl_matrix<T> perp = perp_T.transpose();

  // the origin vectors are projected onto the orthogonal ray planes
  vnl_matrix<T> proj(nr, 1);
  pidx = 0;
  for (size_t i = 0; i < n; ++i, pidx+=2) {
    vnl_matrix<T> temp_o(3, 1), temp_p(1,3), temp;
    temp_o.set_column(0, ray_origins.get_column(i));
    temp_p.set_row(0, perp.get_row(pidx));
    temp = temp_p * temp_o;
    proj[pidx][0] = temp[0][0];
    temp_p.set_row(0, perp.get_row(pidx+1));
    temp = temp_p * temp_o;
    proj[pidx+1][0] = temp[0][0];
  }
  // the intersection point based on weighted least squares
  vnl_matrix<T> X = A_inv * perp_T * cov_inv * proj;
  inter_pt.set(X[0][0], X[1][0], X[2][0]);
  return true;
}
// special case of two rays and also returns the closest distance between the rays
template <class T>
bool vgl_intersection(vgl_ray_3d<T> const& ray0, vgl_ray_3d<T> const& ray1, vgl_point_3d<T>& inter_pt, T& dist){
  const vgl_point_3d<T>& p0 = ray0.origin(), p1 = ray1.origin();
  const vgl_vector_3d<T> v0 = ray0.direction(), v1 = ray1.direction();
  vnl_matrix_fixed<T, 2, 2> A(0.0), Ainv;
  A[0][0] = dot_product(v0, v0); A[0][1] = -dot_product(v0, v1);
  A[1][0] = A[0][1];             A[1][1] = dot_product(v1, v1);
  T det = vnl_det(A);
  if(fabs(det)<1.0e-6)
    return false;
  Ainv = vnl_inverse(A);
  
  vnl_vector_fixed<T, 2> b, tvals;
  b[0] = -dot_product((p0-p1), v0);
  b[1] =  dot_product((p0-p1), v1);
  
  tvals = Ainv*b;
  vgl_point_3d<T> P0 = p0 + tvals[0]*v0, P1 = p1 + tvals[1]*v1;
  dist = (P0-P1).length();

  // intersection point is average of position on each ray
  inter_pt.set(0.5 * (P0.x() + P1.x()), 0.5 * (P0.y() + P1.y()), 0.5 * (P0.z() + P1.z()));
  return true;
}

#undef VGL_ALGO_INTERSECTION_INSTANTIATE
#define VGL_ALGO_INTERSECTION_INSTANTIATE(T) \
template vgl_point_3d<T > vgl_intersection(const std::vector<vgl_plane_3d<T > >&); \
template bool vgl_intersection(vgl_box_3d<T > const&, std::list<vgl_point_3d<T > >&); \
template vgl_infinite_line_3d<T > vgl_intersection(const std::list<vgl_plane_3d<T > >& planes); \
template bool vgl_intersection(const std::list<vgl_plane_3d<T > >& planes, std::vector<T > ws,vgl_infinite_line_3d<T >&,T& residual); \
template bool vgl_intersection(std::vector<vgl_ray_3d<T> > const& rays, vgl_point_3d<T>& inter_pt); \
template bool vgl_intersection(std::vector<vgl_ray_3d<T> > const& rays, vnl_matrix<T> const& covar, vgl_point_3d<T>& inter_pt); \
template bool vgl_intersection(vgl_ray_3d<T> const& ray0, vgl_ray_3d<T> const& ray1, vgl_point_3d<T>& inter_pt, T& dist)
#endif // vgl_algo_intersection_hxx_
