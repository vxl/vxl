#ifndef rgrl_est_proj_rad_func_h_
#define rgrl_est_proj_rad_func_h_

//:
// \file
// \author Gehua Yang
// \date   March 2007
// \brief  a generic class to estimate a homogeneous projection matrix with radial distortion parameter(s)  using L-M

#include <vnl/vnl_vector_fixed.h>
#include <rgrl/rgrl_fwd.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_est_proj_func.h>
#include <vcl_vector.h>

template <unsigned int Tdim, unsigned int Fdim>
class rgrl_est_proj_rad_func
: public rgrl_est_proj_func<Tdim, Fdim>
{
 public:
  //: ctor
  rgrl_est_proj_rad_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                          unsigned int camera_dof,
                          bool with_grad = true );

  //: ctor without matches
  rgrl_est_proj_rad_func( unsigned int camera_dof,
                          bool with_grad = true );

  //: estimate the projective transformation and the associated covariance
  bool
  projective_estimate( vnl_matrix_fixed<double, Tdim+1, Fdim+1>& proj,
                       vcl_vector<double>& rad_dist,
                       vnl_matrix<double>& full_covar,
                       vnl_vector_fixed<double, Fdim>& from_centre,
                       vnl_vector_fixed<double, Tdim>& to_centre,
                       vnl_vector_fixed<double, Tdim> const& camera_centre);

  //: obj func value
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Jacobian
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

  //: set image centre
  void
  set_image_centre( vnl_vector_fixed<double, Tdim> const& image_centre )
  {
    image_centre_ = image_centre;
  }

  //: map a location
  inline
  void
  map_loc( vnl_vector_fixed<double, Tdim>& mapped, 
           vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
           vcl_vector<double> const& rad_k,
           vnl_vector_fixed<double, Fdim> const& from  ) const
  {
  
    vnl_vector_fixed<double, Tdim> proj_mapped;
    rgrl_est_proj_map_inhomo_point<Tdim, Fdim>( proj_mapped, proj, from-this->from_centre_ );
    
    // apply radial distortion
    apply_radial_distortion( mapped, proj_mapped, rad_k );
    
    mapped += this->to_centre_;  
  }
  
protected:

  //: compute jacobian
  void
  reduced_proj_rad_jacobian( vnl_matrix<double>                            & base_jac,
                             vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                             vcl_vector<double>                       const& rad_k,
                             vnl_vector_fixed<double, Fdim>           const& from ) const;

  //: compute the full jacobian
  void
  full_proj_rad_jacobian( vnl_matrix<double>                            & base_jac,
                          vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj,
                          vcl_vector<double>                       const& rad_k,
                          vnl_vector_fixed<double, Fdim>           const& from ) const;

  //: convert parameters
  void
  convert_parameters( vnl_vector<double>& params,
                      vnl_matrix_fixed<double, Tdim+1, Fdim+1> const& proj_matrix,
                      vcl_vector<double>             const& rad_dist,
                      vnl_vector_fixed<double, Fdim> const& fc,
                      vnl_vector_fixed<double, Tdim> const& tc,
                      vnl_vector_fixed<double, Tdim> const& camera_centre );

  void
  apply_radial_distortion( vnl_vector_fixed<double, Tdim>      & mapped,
                           vnl_vector_fixed<double, Tdim> const& p,
                           vcl_vector<double> const& rad_k ) const;

  //: transfer parameters into the temp vector
  void
  transfer_radial_params_into_temp_storage( vnl_vector<double> const& params ) const;
  
protected:
  unsigned int                    camera_dof_;
  vnl_vector_fixed<double, Tdim>  image_centre_;
  mutable vcl_vector<double>  temp_rad_k_;
};

#endif //rgrl_est_proj_rad_func_h_
