// This is mul/vimt3d/vimt3d_sample_grid_trilin.txx
#ifndef vimt3d_sample_grid_trilin_txx_
#define vimt3d_sample_grid_trilin_txx_
//:
// \file
// \brief Profile sampling functions for 3D images
// \author Graham Vincent

#include "vimt3d_sample_grid_trilin.h"
#include <vimt3d/vimt3d_trilin_interp.h>
#include <vnl/vnl_vector.h>

//: True if p clearly inside the image
template<class T>
inline bool vimt3d_point_in_image(const vgl_point_3d<double>& p, const vil3d_image_view<T>& image)
{
  if (p.x()<1) return false;
  if (p.y()<1) return false;
  if (p.z()<1) return false;
  if (p.x()+2>image.ni()) return false;
  if (p.y()+2>image.nj()) return false;
  if (p.z()+2>image.nk()) return false;
  return true;
}

//: True if grid of size nu * nv * nw (in steps of u,v,w) is entirely in the image.
//  p defines centre of one size.
template<class T>
inline bool vimt3d_grid_in_image_ic(const vgl_point_3d<double>& im_p,
                                    const vgl_vector_3d<double>& im_u,
                                    const vgl_vector_3d<double>& im_v,
                                    const vgl_vector_3d<double>& im_w,
                                    unsigned nu, unsigned nv, unsigned nw,
                                    const vil3d_image_view<T>& image)
{
  vgl_vector_3d<double> u1=(nu-1)*im_u;
  vgl_vector_3d<double> v1=(nv-1)*im_v;
  vgl_vector_3d<double> w1=(nw-1)*im_w;
  if (!vimt3d_point_in_image(im_p,image)) return false;
  if (!vimt3d_point_in_image(im_p+u1,image)) return false;
  if (!vimt3d_point_in_image(im_p+v1,image)) return false;
  if (!vimt3d_point_in_image(im_p+w1,image)) return false;
  if (!vimt3d_point_in_image(im_p+u1+v1,image)) return false;
  if (!vimt3d_point_in_image(im_p+u1+w1,image)) return false;
  if (!vimt3d_point_in_image(im_p+v1+w1,image)) return false;
  if (!vimt3d_point_in_image(im_p+u1+v1+w1,image)) return false;

  return true;
}


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p.
//  Samples are taken along direction w first. Samples
//  outside the image are set to 0.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin(vnl_vector<vecType>& vec,
                               const vimt3d_image_3d_of<imType>& image,
                               const vgl_point_3d<double>& p,
                               const vgl_vector_3d<double>& u,
                               const vgl_vector_3d<double>& v,
                               const vgl_vector_3d<double>& w,
                               unsigned nu, unsigned nv, unsigned nw)
{
  // convert to image coordinates
  vgl_point_3d<double> im_p0 = image.world2im()(p);
  vgl_vector_3d<double> im_u = image.world2im()(p+u)-im_p0;
  vgl_vector_3d<double> im_v = image.world2im()(p+v)-im_p0;
  vgl_vector_3d<double> im_w = image.world2im()(p+w)-im_p0;

  // call image coordinate version of grid sampler
  vimt3d_sample_grid_trilin_ic(vec,image.image(),im_p0,im_u,im_v,im_w,nu,nv,nw);

  return;
}


//: Sample grid p+i.u+j.v+k.w in image coordinates using trilinear interpolation with NO CHECKS.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
inline void vimt3d_sample_grid_trilin_ic_no_checks(
  vnl_vector<vecType>& vec,
  const vil3d_image_view<imType>& image,
  const vgl_point_3d<double>& p0,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  unsigned nu, unsigned nv, unsigned nw)
{
  unsigned np = image.nplanes();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();
  vcl_ptrdiff_t pstep = image.planestep();

  vec.set_size(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.origin_ptr();
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned k=0;k<nw;++k,p+=w,++vc)
          *vc = vil3d_trilin_interp_raw(p.x(),p.y(),p.z(),
                                        plane0,istep,jstep,kstep);
      }
    }
  }
  else
  {
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned l=0;l<nw;++l,p+=w)
          for (unsigned k=0;k<np;++k,++vc)
            *vc = vil3d_trilin_interp_raw(p.x(),p.y(),p.z(),
              image.origin_ptr()+k*pstep,istep,jstep,kstep);
      }
    }
  }
}

//: Sample grid p+i.u+j.v+k.w safely in image coordinates using trilinear interpolation.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image return zero.
template <class imType, class vecType>
inline void vimt3d_sample_grid_trilin_ic_safe(
  vnl_vector<vecType>& vec,
  const vil3d_image_view<imType>& image,
  const vgl_point_3d<double>& p0,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  unsigned nu, unsigned nv, unsigned nw)
{
  unsigned np = image.nplanes();
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();
  vcl_ptrdiff_t pstep = image.planestep();

  vec.set_size(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.origin_ptr();
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned k=0;k<nw;++k,p+=w,++vc)
          *vc = vil3d_trilin_interp_safe(p.x(),p.y(),p.z(),plane0,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
  else
  {
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned l=0;l<nw;++l,p+=w)
          for (unsigned k=0;k<np;++k,++vc)
            *vc = vil3d_trilin_interp_safe(p.x(),p.y(),p.z(),
                                           image.origin_ptr()+k*pstep,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
}

//: Sample grid p+i.u+j.v+k.w safely in image coordinates using trilinear interpolation.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first
//  Points outside image are set to the nearest voxel's value.
template <class imType, class vecType>
inline void vimt3d_sample_grid_trilin_ic_extend(
  vnl_vector<vecType>& vec,
  const vil3d_image_view<imType>& image,
  const vgl_point_3d<double>& p0,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  unsigned nu, unsigned nv, unsigned nw)
{
  unsigned np = image.nplanes();
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();
  vcl_ptrdiff_t pstep = image.planestep();

  vec.set_size(nu*nv*nw*np);
  vecType* vc = vec.begin();

  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    const imType* plane0 = image.origin_ptr();
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned k=0;k<nw;++k,p+=w,++vc)
          *vc = vil3d_trilin_interp_safe_extend(p.x(),p.y(),
            p.z(),plane0,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
  else
  {
    for (unsigned i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (unsigned j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (unsigned l=0;l<nw;++l,p+=w)
          for (unsigned k=0;k<np;++k,++vc)
            *vc = vil3d_trilin_interp_safe_extend(p.x(),p.y(),p.z(),
              image.origin_ptr()+k*pstep,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
}


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in world coordinates.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p.
//  Samples are taken along direction w first. Samples
//  outside the image are set to the value of the nearest voxel's value.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin_extend(
  vnl_vector<vecType>& vec,
  const vimt3d_image_3d_of<imType>& image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  unsigned nu, unsigned nv, unsigned nw)
{
  // convert to image coordinates
  vgl_point_3d<double> im_p0 = image.world2im()(p);
  vgl_vector_3d<double> im_u = image.world2im()(p+u)-im_p0;
  vgl_vector_3d<double> im_v = image.world2im()(p+v)-im_p0;
  vgl_vector_3d<double> im_w = image.world2im()(p+w)-im_p0;

  // call image coordinate version of grid sampler
  if (vimt3d_grid_in_image_ic(im_p0,im_u,im_v,im_w,nu,nv,nw,image.image()))
    vimt3d_sample_grid_trilin_ic_no_checks(vec,image.image(),im_p0,im_u,im_v,im_w,nu,nv,nw);
  else
    vimt3d_sample_grid_trilin_ic_extend(vec,image.image(),im_p0,im_u,im_v,im_w,nu,nv,nw);

  return;
}


//: Sample grid p+i.u+j.v+k.w using trilinear interpolation in image coordinates.
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction w first.
//  Samples outside the image are set to 0.
template <class imType, class vecType>
void vimt3d_sample_grid_trilin_ic(vnl_vector<vecType>& vec,
                                  const vil3d_image_view<imType>& image,
                                  const vgl_point_3d<double>& im_p,
                                  const vgl_vector_3d<double>& im_u,
                                  const vgl_vector_3d<double>& im_v,
                                  const vgl_vector_3d<double>& im_w,
                                  unsigned nu, unsigned nv, unsigned nw)
{
  if (vimt3d_grid_in_image_ic(im_p,im_u,im_v,im_w,nu,nv,nw,image))
    vimt3d_sample_grid_trilin_ic_no_checks(vec,image,im_p,im_u,im_v,im_w,nu,nv,nw);
  else
    vimt3d_sample_grid_trilin_ic_safe(vec,image,im_p,im_u,im_v,im_w,nu,nv,nw);

  return;
}


#define VIMT3D_SAMPLE_GRID_TRILIN_INSTANTIATE( imType, vecType ) \
template void vimt3d_sample_grid_trilin( \
  vnl_vector<vecType >& vec, \
  const vimt3d_image_3d_of<imType >& image, \
  const vgl_point_3d<double >& p, \
  const vgl_vector_3d<double >& u, \
  const vgl_vector_3d<double >& v, \
  const vgl_vector_3d<double >& w, \
  unsigned nu, unsigned nv, unsigned nw); \
template void vimt3d_sample_grid_trilin_extend( \
  vnl_vector<vecType >& vec, \
  const vimt3d_image_3d_of<imType >& image, \
  const vgl_point_3d<double >& p, \
  const vgl_vector_3d<double >& u, \
  const vgl_vector_3d<double >& v, \
  const vgl_vector_3d<double >& w, \
  unsigned nu, unsigned nv, unsigned nw); \
template void vimt3d_sample_grid_trilin_ic( \
  vnl_vector<vecType >& vec, \
  const vil3d_image_view<imType >& image, \
  const vgl_point_3d<double >& im_p, \
  const vgl_vector_3d<double >& im_u, \
  const vgl_vector_3d<double >& im_v, \
  const vgl_vector_3d<double >& im_w, \
  unsigned nu, unsigned nv, unsigned nw)

#endif // vimt3d_sample_grid_trilin_txx_
