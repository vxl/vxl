// This is mul/vimt3d/vimt3d_reconstruct_from_grid.txx
#ifndef vimt3d_reconstruct_from_grid_txx_
#define vimt3d_reconstruct_from_grid_txx_
//:
// \file
#include "vimt3d_reconstruct_from_grid.h"
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
                                    int nu, int nv, int nw,
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

//: Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in world coordinates)
//  Profile points are p+i.u+j.v+k.w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid(vimt3d_image_3d_of<imType>& image,
                                  const vnl_vector<vecType>& vec,
                                  const vgl_point_3d<double>& p,
                                  const vgl_vector_3d<double>& u,
                                  const vgl_vector_3d<double>& v,
                                  const vgl_vector_3d<double>& w,
                                  int nu, int nv, int nw, bool add_data)
{
  // convert to image coordinates
  vgl_point_3d<double> im_p0 = image.world2im()(p);
  vgl_vector_3d<double> im_u = image.world2im()(p+u)-im_p0;
  vgl_vector_3d<double> im_v = image.world2im()(p+v)-im_p0;
  vgl_vector_3d<double> im_w = image.world2im()(p+w)-im_p0;

  // call image coordinate version of grid sampler
  vimt3d_reconstruct_from_grid_ic(image.image(),vec,im_p0,im_u,im_v,im_w,nu,nv,nw,add_data);

  return;
}


//: Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in image coordinates)
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid_ic(vil3d_image_view<imType>& image,
                                     const vnl_vector<vecType>& vec,
                                     const vgl_point_3d<double>& im_p,
                                     const vgl_vector_3d<double>& im_u,
                                     const vgl_vector_3d<double>& im_v,
                                     const vgl_vector_3d<double>& im_w,
                                     int nu, int nv, int nw, bool add_data)
{
  if (vimt3d_grid_in_image_ic(im_p,im_u,im_v,im_w,nu,nv,nw,image))
    vimt3d_reconstruct_from_grid_ic_no_checks(image,vec,im_p,im_u,im_v,im_w,nu,nv,nw,add_data);
  else
    vimt3d_reconstruct_from_grid_ic_safe(image,vec,im_p,im_u,im_v,im_w,nu,nv,nw,add_data);

  return;
}

//: WITH NO CHECKS Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in image coordinates)
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid_ic_no_checks(vil3d_image_view<imType>& image,
                                               const vnl_vector<vecType>& vec,
                                               const vgl_point_3d<double>& p0,
                                               const vgl_vector_3d<double>& u,
                                               const vgl_vector_3d<double>& v,
                                               const vgl_vector_3d<double>& w,
                                               int nu, int nv, int nw, bool add_data)
{
  int np = image.nplanes();
  int ni = image.ni();
  int nj = image.nj();
  int nk = image.nk();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();

  const vecType* vc = vec.begin();
  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    imType* plane0 = image.origin_ptr();
    for (int i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (int j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (int k=0;k<nw;++k,p+=w,++vc)
        {
            vimt3d_reconstruct_ic_no_checks(*vc,p.x(),p.y(),p.z(),plane0,ni,nj,nk,istep,jstep,kstep,add_data);
        }
      }
    }
  }
  else
  {
    for (int i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (int j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (int l=0;l<nw;++l,p+=w)
          for (int k=0;k<np;++k,++vc)
          {
            vimt3d_reconstruct_ic_no_checks(*vc,p.x(),p.y(),p.z(),image.origin_ptr(),ni,nj,nk,istep,jstep,kstep,add_data);
          }
      }
    }
  }
}


//: WITH SAFETY CHECKS Reconstruct a smoothed image grid p+i.u+j.v+k.w from vector (in image coordinates)
//  Profile points are im_p+i.im_u+j.im_v+k.im_w, where i=[0..nu-1],j=[0..nv-1], k=[0..nw-1]
//  Vector v is resized to nu*nv*nw*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Reconstruction occurs along direction w first
template <class imType, class vecType>
void vimt3d_reconstruct_from_grid_ic_safe(vil3d_image_view<imType>& image,
                                          const vnl_vector<vecType>& vec,
                                          const vgl_point_3d<double>& p0,
                                          const vgl_vector_3d<double>& u,
                                          const vgl_vector_3d<double>& v,
                                          const vgl_vector_3d<double>& w,
                                          int nu, int nv, int nw, bool add_data)
{
  int np = image.nplanes();
  int ni = image.ni();
  int nj = image.nj();
  int nk = image.nk();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t kstep = image.kstep();

  const vecType* vc = vec.begin();
  vgl_point_3d<double> p1 = p0;

  if (np==1)
  {
    imType* plane0 = image.origin_ptr();
    for (int i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (int j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (int k=0;k<nw;++k,p+=w,++vc)
        {
            vimt3d_reconstruct_ic_safe(*vc,p.x(),p.y(),p.z(),plane0,ni,nj,nk,istep,jstep,kstep,add_data);
        }
      }
    }
  }
  else
  {
    for (int i=0;i<nu;++i,p1+=u)
    {
      vgl_point_3d<double> p2 = p1;
      for (int j=0;j<nv;++j,p2+=v)
      {
        vgl_point_3d<double> p = p2;
        // Sample each row (along w)
        for (int l=0;l<nw;++l,p+=w)
          for (int k=0;k<np;++k,++vc)
          {
            vimt3d_reconstruct_ic_safe(*vc,p.x(),p.y(),p.z(),image.origin_ptr(),ni,nj,nk,istep,jstep,kstep,add_data);
          }
      }
    }
  }
}


#define VIMT3D_RECONSTRUCT_FROM_GRID_INSTANTIATE( imType, vecType ) \
template void vimt3d_reconstruct_from_grid(vimt3d_image_3d_of<imType >& image, \
                                           const vnl_vector<vecType >& vec, \
                                           const vgl_point_3d<double >& p, \
                                           const vgl_vector_3d<double >& u, \
                                           const vgl_vector_3d<double >& v, \
                                           const vgl_vector_3d<double >& w, \
                                           int nu, int nv, int nw, bool add_data); \
template void vimt3d_reconstruct_from_grid_ic(vil3d_image_view<imType >& image, \
                                              const vnl_vector<vecType >& vec, \
                                              const vgl_point_3d<double >& im_p, \
                                              const vgl_vector_3d<double >& im_u, \
                                              const vgl_vector_3d<double >& im_v, \
                                              const vgl_vector_3d<double >& im_w, \
                                              int nu, int nv, int nw, bool add_data); \
template void vimt3d_reconstruct_from_grid_ic_safe(vil3d_image_view<imType>& image, \
                                                   const vnl_vector<vecType>& vec, \
                                                   const vgl_point_3d<double>& p0, \
                                                   const vgl_vector_3d<double>& u, \
                                                   const vgl_vector_3d<double>& v, \
                                                   const vgl_vector_3d<double>& w, \
                                                   int nu, int nv, int nw, bool add_data); \
template void vimt3d_reconstruct_from_grid_ic_no_checks(vil3d_image_view<imType>& image, \
                                                        const vnl_vector<vecType>& vec, \
                                                        const vgl_point_3d<double>& p0, \
                                                        const vgl_vector_3d<double>& u, \
                                                        const vgl_vector_3d<double>& v, \
                                                        const vgl_vector_3d<double>& w, \
                                                        int nu, int nv, int nw, bool add_data)

#endif // vimt3d_reconstruct_from_grid_txx_
