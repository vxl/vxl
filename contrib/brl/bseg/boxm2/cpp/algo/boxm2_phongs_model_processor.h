// This is brl/bseg/boxm2/cpp/algo/boxm2_phongs_model_processor.h
#ifndef boxm2_phongs_model_processor_h_
#define boxm2_phongs_model_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor
//
// \author Vishal Jain
// \date   Dec 27, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <brad/brad_phongs_model_est.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class  boxm2_phongs_model_processor
{
  public:
    static float expected_color(brad_phongs_model & pmodel, vnl_double_3 view_dir, float sun_elev, float azim);

    static float prob_density(float obs, float expected_obs, float var);

    static brad_phongs_model  compute_phongs_model(float & var,
                                                   float sun_elev,
                                                   float sun_azim,
                                                   std::vector<float>        & obs,
                                                   std::vector<vnl_double_3> & view_dirs,
                                                   std::vector<float>        & vis);
};

#endif // boxm2_phongs_model_processor_h_
