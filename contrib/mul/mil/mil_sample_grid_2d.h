// This is mul/mil/mil_sample_grid_2d.h
#ifndef mil_sample_grid_2d_h_
#define mil_sample_grid_2d_h_
//:
// \file
// \brief Profile sampling functions for 2D images
// \author Tim Cootes

#include <mil/mil_image_2d_of.h>
#include <vnl/vnl_fwd.h>

//: Sample grid p+i.u+j.v using bilinear interpolation
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
template <class imType, class vecType>
void mil_sample_grid_2d(vnl_vector<vecType>& vec,
                        const mil_image_2d_of<imType>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        const vgl_vector_2d<double>& v,
                        int nu, int nv);

//: Sample grid, using safe bilinear interpolation
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_safe(vnl_vector<vecType>& vec,
                             const mil_image_2d_of<imType>& image,
                             const vgl_point_2d<double>& p0,
                             const vgl_vector_2d<double>& u,
                             const vgl_vector_2d<double>& v,
                             int nu, int nv);

//: Sample grid, using bilinear interpolation (no checks)
//  Profile points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_no_checks(vnl_vector<vecType>& vec,
                                  const mil_image_2d_of<imType>& image,
                                  const vgl_point_2d<double>& p0,
                                  const vgl_vector_2d<double>& u,
                                  const vgl_vector_2d<double>& v,
                                  int nu, int nv);

//: Sample grid, using safe bilinear interpolation (points in image co-ordinates)
//  Grid points are p+i.u+j.v, where i=[0..nu-1],j=[0..nv-1]
//  Vector v is resized to nu*nv*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Samples are taken along direction v first
//  Points outside image return zero.
template <class imType, class vecType>
void mil_sample_grid_2d_ic_safe(vnl_vector<vecType>& vec,
                                const mil_image_2d_of<imType>& image,
                                const vgl_point_2d<double>& p0,
                                const vgl_vector_2d<double>& u,
                                const vgl_vector_2d<double>& v,
                                int nu, int nv);

#define MIL_SAMPLE_GRID_2D_INSTANTIATE(imType, vecType) \
extern "please #include mil/mil_sample_grid_2d.txx instead"

#endif // mil_sample_grid_2d_h_
