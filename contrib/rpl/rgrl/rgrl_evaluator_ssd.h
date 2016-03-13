#ifndef rgrl_evaluator_ssd_h_
#define rgrl_evaluator_ssd_h_

//:
// \file
// \author Lee, Ying-Lin (Bess)
//
// \verbatim
// \endverbatim

#include <iostream>
#include <vector>
#include <rgrl/rgrl_evaluator.h>
#include <vcl_compiler.h>

class rgrl_evaluator_ssd
  : public rgrl_evaluator
{
public:
  rgrl_evaluator_ssd() {}
  ~rgrl_evaluator_ssd() {}

  double evaluate( std::vector< double > const& a,
                   std::vector< double > const& b,
                   std::vector< double > const& weight ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_evaluator_ssd, rgrl_evaluator );

};
#endif
