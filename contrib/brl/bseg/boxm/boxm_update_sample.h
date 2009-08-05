#ifndef boxm_update_sample_h_
#define boxm_update_sample_h_


class boxm_update_sample
{
public:
  //: default constructor
  boxm_update_sample() : update_multiplier(0.0f), do_update(false) {}

  // the (unnormalized) update multiplier
  float update_multiplier;
  bool do_update;
};

#endif

