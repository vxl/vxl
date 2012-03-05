#ifndef vimt_sample_grid_bilin_h_
#define vimt_sample_grid_bilin_h_
//:
// \file
// \brief Grid sampling functions for 2D images
// \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>

//: Sample grid from image, using bilinear interpolation
//  Grid points are p+i.u+j.v where i=[0..n1-1], j=[0..n2-1]
//  Vector vec is resized to n1*n2*np elements, where np=image.nplanes().
//  vec[0]..vec[np-1] are the values from point p
//  Samples are taken along direction v first, then along u.
//  Points outside image return zero.
// \relatesalso vimt_image_2d_of
template <class imType, class vecType>
void vimt_sample_grid_bilin(vnl_vector<vecType>& vec,
                            const vimt_image_2d_of<imType>& image,
                            const vgl_point_2d<double>& p,
                            const vgl_vector_2d<double>& u,
                            const vgl_vector_2d<double>& v,
                            int n1, int n2);

//: Sample grid from image, using bilinear interpolation
//  Grid points are p+i.u+j.v where i=[0..n1-1], j=[0..n2-1]
//  Vector vec is resized to n1*n2*np elements, where np=image.nplanes().
//  vec[0]..vec[np-1] are the values from point p
//  Samples are taken along direction v first, then along u.
//  Points outside image return NA.
// \relatesalso vimt_image_2d_of
template <class imType, class vecType>
void vimt_sample_grid_bilin_edgena(vnl_vector<vecType>& vec,
                                   const vimt_image_2d_of<imType>& image,
                                   const vgl_point_2d<double>& p,
                                   const vgl_vector_2d<double>& u,
                                   const vgl_vector_2d<double>& v,
                                   int n1, int n2);
#endif // vimt_sample_grid_bilin_h_
