#ifndef vsrl_saliency_diffusion_h
#define vsrl_saliency_diffusion_h

#include <vsrl/vsrl_diffusion.h>
#include <vsrl/vsrl_token_saliency.h>

// This program will take the results of a step diffusion
// algorithm and token saliency object and use diffusion
// inorder to spread out the disparity from salient pixels
// to non salient pixels.

class vsrl_saliency_diffusion : public vsrl_diffusion
{
 protected:
  vsrl_token_saliency *saliency_; // the object that defines the salieny of each point

 public:

  // constructor
  vsrl_saliency_diffusion(vsrl_dense_matcher *matcher);

  // destructor
  ~vsrl_saliency_diffusion();

  // set the initial disparity based on a step diffusion mechanism
  void set_initial_disparity(vsrl_diffusion *diffusion);

  // set the saliency object
  void set_saliency(vsrl_token_saliency *saliency);

  void execute(int num_iter=150);

 protected:

  // diffuse over the steps
  void difuse_disparity(int num_iter);

  // modify based on the the pixel saliency measures
  void consider_saliency();
};

#endif
