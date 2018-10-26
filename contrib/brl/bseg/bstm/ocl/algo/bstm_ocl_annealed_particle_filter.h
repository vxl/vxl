#ifndef bstm_ocl_annealed_particle_filter_h_
#define bstm_ocl_annealed_particle_filter_h_

#include <utility>

#include "bstm_ocl_particle_filter.h"
//:
// \file

class bstm_ocl_annealed_particle_filter : public bstm_ocl_particle_filter
{
 public:

  bstm_ocl_annealed_particle_filter(bocl_device_sptr device, bstm_scene_sptr scene, bstm_cache_sptr cache, bstm_opencl_cache_sptr opencl_cache,
                           unsigned start_t, unsigned end_t, vgl_box_3d<double> initial_bb, int num_particles, double t_sigma, double w_sigma, double w_kappa,
                           std::string kernel_opt, int nbins, int label, std::string pf_output_path, double radius = 0 ) :
                               bstm_ocl_particle_filter( device,  scene, cache, opencl_cache, start_t, end_t, initial_bb, num_particles, t_sigma, w_sigma,
                                                         kernel_opt, nbins, label,  radius ), pf_output_path_(std::move(pf_output_path)), original_t_sigma_(t_sigma), original_w_sigma_(w_sigma),original_w_kappa_(w_kappa) { }

  void track() override;

  void set_num_annealing_layers(unsigned num_annealing_layers) { num_annealing_layers_ = num_annealing_layers; };

 private:

  void perturb_particles(unsigned prev_time, unsigned cur_time, unsigned m);

  float scale_obs_density(unsigned cur_time, double cur_beta, unsigned m);

  float survival_diagnostic(unsigned cur_time, double beta);

  void dump_particles(unsigned cur_time, unsigned cascade);

  unsigned num_annealing_layers_;

  double original_t_sigma_;
  double original_w_sigma_;
  double original_w_kappa_;

  std::string pf_output_path_;
};


#endif //bstm_ocl_annealed_particle_filter_h_
