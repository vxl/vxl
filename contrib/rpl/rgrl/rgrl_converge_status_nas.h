#ifndef rgrl_converge_status_nas_h_
#define rgrl_converge_status_nas_h_

//:
// \file
// \brief Class to represent the registration status.
//   This class has knowledge of current Normal directions, 
//   Alignment error, and Scaling factors of the transformation
// 
// \author Gehua Yang
// \date  March 2005
//

#include "rgrl_converge_status.h"
#include <vnl/vnl_vector.h>

//: Represent the registration status
//
class rgrl_converge_status_nas
  : public rgrl_converge_status
{
public:
  //:
  rgrl_converge_status_nas( bool     has_converged,
                            bool     has_stagnated,
                            bool     is_good_enough,
                            bool     is_failed,
                            double   error,
                            unsigned oscillation_count,
                            double   error_diff );

  //: copy ctor
  rgrl_converge_status_nas( rgrl_converge_status const& old );
  
  virtual ~rgrl_converge_status_nas();

  //: get iteration number
  int iteration() const { return iteration_; }
  
  //: set iteration number
  void set_iteration( int i ) { iteration_ = i; }
  
  //: get initial scaling factors
  vnl_vector<double> const& init_scaling_factors() const 
  { return init_scaling_factors_; }
  
  //: set initial scaling factors
  void set_init_scaling_factors( vnl_vector<double> const& scaling );
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_converge_status_nas, rgrl_converge_status );

protected:
  int iteration_;
  
  vnl_vector<double> init_scaling_factors_;
  
};
  
#endif // rgrl_converge_status_nas_h_
