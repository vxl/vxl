// This bbas/bpgl/ihog/ihog_world_roi.h
#ifndef ihog_world_roi_h_
#define ihog_world_roi_h_
//:
// \file
// \brief A class to represent region of interest in world coordinates.
// \author Matt Leotta
// \date April 13, 2004
//
// \verbatim
//  Modifications
//  dependency on vimt removed
//  G. Tunali 8/2010
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_image.h>

//:A class to represent a region of interest in world coordinates.
// This class holds the parameters of a uniform grid in the world
// coordinate system.  The roi is used to resample the images.

class ihog_world_roi : public vbl_ref_count
{
 public:
  //: Constructor
  ihog_world_roi( unsigned int n_u,  unsigned int n_v,
                  const vgl_point_2d< double > &  p0 = vgl_point_2d< double >(0.0, 0.0),
                  const vgl_vector_2d< double > &  u = vgl_vector_2d< double >(1.0, 0.0),
                  const vgl_vector_2d< double > &  v = vgl_vector_2d< double >(0.0, 1.0) );

  //: Sample the image in the ROI and return a vector of values
  vnl_vector<double> sample( const ihog_image<float>& image) const;

  //: Create a vector of weights for the sample of \p image
  //  Weights are based on distance from the image boundaries
  vnl_vector<double> sample_weights( const ihog_image<float>& image) const;

  //: Sample the image in the ROI and return an image
  vil_image_view<float> resample( const ihog_image<float>& image) const;

  //: Return the size of the grid in the u direction
  unsigned int size_in_u() const { return n_u_; }

  //: Return the size of the grid in the v direction
  unsigned int size_in_v() const { return n_v_; }

  //: Return the origin of the grid
  vgl_point_2d< double > origin() const { return p0_; }

  //: Return the u direction of sampling
  vgl_vector_2d< double > dir_in_u() const { return u_; }

  //: Return the v direction of sampling
  vgl_vector_2d< double > dir_in_v() const { return v_; }

  //: Set the size of the grid in the u direction
  void set_size_in_u(unsigned int n_u) { n_u_ = n_u; }

  //: Set the size of the grid in the v direction
  void set_size_in_v(unsigned int n_v) { n_v_ = n_v; }

  //: Set the origin of the grid
  void set_origin(const vgl_point_2d<double>& p0) { p0_ = p0; }

  //: Set the u direction of sampling
  void set_dir_in_u(const vgl_vector_2d<double>& u) { u_ = u; }

  //: Set the v direction of sampling
  void set_dir_in_v(const vgl_vector_2d<double>& v) { v_ = v; }

 protected:
  //: Default Constructor
  ihog_world_roi();

  //: The number of steps in u
  unsigned int  n_u_;
  //: The number of steps in v
  unsigned int  n_v_;
  //: The grid origin
  vgl_point_2d< double >   p0_;
  //: The step size and direction in u
  vgl_vector_2d< double >  u_;
  //: The step size and direction in v
  vgl_vector_2d< double >  v_;
};

#endif // ihog_world_roi_h_
