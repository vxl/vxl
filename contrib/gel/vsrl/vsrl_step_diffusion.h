#ifndef vsrl_step_diffusion_h
#define vsrl_step_diffusion_h
//:
// \file

#include <vsrl/vsrl_diffusion.h>

//: this class will take the results of a dense matcher and use diffusion to get rid of "steps" in the data

class vsrl_step_diffusion : public vsrl_diffusion
{
 public:

  // constructor
  vsrl_step_diffusion(vsrl_dense_matcher *matcher);

  // destructor
  ~vsrl_step_diffusion();

  void execute();

  // get the initial disparity
  void set_initial_disparity();

  // interpolate the disparity
  void interpolate_disparity();

  // reset borders
  void clear_borders(int width);

  // diffuse over the steps
  void diffuse_disparity();
};

#endif
