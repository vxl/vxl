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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_evaluator_ssd
  : public rgrl_evaluator
{
public:
  rgrl_evaluator_ssd() = default;
  ~rgrl_evaluator_ssd() override = default;

  double evaluate( std::vector< double > const& a,
                   std::vector< double > const& b,
                   std::vector< double > const& weight ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_evaluator_ssd, rgrl_evaluator );

};
#endif
