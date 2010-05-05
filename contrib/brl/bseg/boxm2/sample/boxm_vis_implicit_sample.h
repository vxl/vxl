#ifndef boxm_vis_implicit_sample_h_
#define boxm_vis_implicit_sample_h_
//:
// \file

class boxm_vis_implicit_sample
{
 public:
  //: default constructor
  boxm_vis_implicit_sample() : max_vis(0.0f), n_obs(0) {}

  float max_vis;
  unsigned int n_obs;
};

#endif // boxm_vis_implicit_sample_h_
