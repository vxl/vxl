#ifndef rgrl_evaluator_h_
#define rgrl_evaluator_h_
//:
// \file
// \author Lee, Ying-Lin (Bess)

#include <vcl_vector.h>
#include <rgrl/rgrl_object.h>

class rgrl_evaluator : public rgrl_object
{
 public:
  // Default constructor
  rgrl_evaluator() {}
  // Destructor
  virtual ~rgrl_evaluator() {}

  virtual
  double evaluate( vcl_vector< double > const& a,
                   vcl_vector< double > const& b,
                   vcl_vector< double > const& weight ) const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_evaluator, rgrl_object );
};

#endif
