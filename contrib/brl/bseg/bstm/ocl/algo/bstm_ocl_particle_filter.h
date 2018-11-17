#ifndef bstm_ocl_particle_filter_h_
#define bstm_ocl_particle_filter_h_


//:
// \file
// \brief   This class implements a particle filter based on mutual information. At each time step, the algorithm samples
//          a new set of particles/hypothesis (axis aligned bounding boxes for simplicity) by perturbing the particles
//          from the previous time step and evaluates a score based on MI. Next, the hypothesis are resampled to ensure
//          low ranking particles are eliminated.


#include <iostream>
#include <set>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_random.h>
#include <bbas/bsta/bsta_gaussian_indep.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_orient_box_3d.h>

class bstm_ocl_particle_filter
{
 public:
  bstm_ocl_particle_filter(const bocl_device_sptr& device, const bstm_scene_sptr& scene, const bstm_cache_sptr& cache, const bstm_opencl_cache_sptr& opencl_cache,
                           unsigned start_t, unsigned end_t, vgl_box_3d<double> initial_bb, int num_particles, double t_sigma, double w_sigma,
                           std::string kernel_opt, int nbins, int label, double radius = 0 );

  virtual void track();


  ~bstm_ocl_particle_filter() { delete[] app_joint_histogram_buff_; delete[] app_hist_; delete[] app_histA_; delete[] app_histB_;
                                   delete[] translation_buff_; delete[] rotation_buff_; delete[] bb_buf_; delete[] particle_nos_; };


 protected:

  void label(unsigned time);

  void init_ocl_minfo();

  void resample(unsigned prev_time, unsigned cur_time);

  void eval_obs_density(unsigned prev_time, unsigned cur_time);

  void propagate_particles(unsigned prev_time, unsigned curr_time);

  std::vector<double> eval_mi(unsigned prev_time, unsigned cur_time);

  std::vector<double> compute_mi_from_hist();

  bool compile_kernel();

  vgl_vector_3d<double> velocity_estimate(unsigned prev2_time, unsigned prev_time);
  vgl_rotation_3d<double> angular_velocity_estimate(unsigned prev2_time, unsigned prev_time);

  void mean_state( unsigned t, vgl_vector_3d<double> & mean_T, vgl_rotation_3d<double>& mean_R);
  vgl_rotation_3d<double> mean_rot( std::vector< vgl_rotation_3d<double> > rot, std::vector< double > weights );
  vgl_rotation_3d<double> sample_rot(const vgl_rotation_3d<double>& rot, double kappa , double w_sigma );


  //helper
  void populate_blk_mapping(std::map<bstm_block_id, std::vector<bstm_block_id> > & mapping, const vgl_box_3d<double>& aabb, const vgl_rotation_3d<double>& r,
                            const  vgl_vector_3d<double>& t, unsigned prev_time, unsigned curr_time);

  void write_particle_ocl_info(unsigned prev_time,unsigned cur_time );

  vgl_box_3d<double> w_mean_bb( std::vector<vgl_orient_box_3d<double> > bb,  std::vector< double> weights);

  vgl_orient_box_3d<double> best_bb( std::vector<vgl_orient_box_3d<double> > bb,  std::vector< double> weights,  std::vector< vgl_vector_3d<double> > T, std::vector< vgl_rotation_3d<double> > R);

  double survival_diagnostic(unsigned cur_time);

  void save_and_clear_gpu();

  void normalize_mi_score(unsigned t);

  //members
  bocl_device_sptr device_;
  bstm_scene_sptr scene_;
  bstm_opencl_cache_sptr opencl_cache_;
  bstm_cache_sptr cache_;

  //start and end times (unsigned for convinience)
  unsigned end_t_;
  unsigned start_t_;

  vgl_box_3d<double> initial_bb_;

  //Particle filter param
  int num_particles_;
  double t_sigma_;
  double w_sigma_;
  double w_kappa_;
  int track_label_;
  double radius_;

  //MI param
  cl_uint surf_nbins_;
  cl_uint app_nbins_;
  cl_uint app_view_dir_num_;

  //bb
  std::vector< std::vector<vgl_orient_box_3d<double> > > bb_;
  //mean bounding box
  std::vector<vgl_box_3d<double> > mean_bb_;
  //MI
  std::vector< std::vector< double > > mi_;
  //Transformations
  std::vector< std::vector< vgl_rotation_3d<double> > > R_;
  std::vector< std::vector< vgl_rotation_3d<double> > > infrot_;
  std::vector< std::vector< vgl_vector_3d<double> > > T_ ;
  std::vector< std::vector< vgl_vector_3d<double> > > inft_ ;

  //blk mappings
  std::vector< std::vector< std::map<bstm_block_id, std::vector<bstm_block_id> > > > blk_map_;
  //store all possible blk mappings
  std::set< std::pair< bstm_block_id, bstm_block_id>  > global_blk_map_;

  //kernels
  std::string kernel_opt_;
  bocl_kernel * kern_;

  //3d gaussians
  bsta_gaussian_indep<double,3> translation_sampler_;



  //ocl stuff
  cl_command_queue queue_;
  cl_float target_blk_origin_buff[4];
  cl_int target_subblk_num_buff[4];
  cl_float output_buff[1000];
  cl_float block_len_buff;
  cl_uchar lookup_arr[256];
  cl_uint *app_joint_histogram_buff_;
  cl_float * surf_joint_histogram_buff_;
  cl_float* translation_buff_;
  cl_float* rotation_buff_;
  cl_float* bb_buf_;
  cl_float times_[4];
  cl_uint* particle_nos_;

  bocl_mem_sptr centerX_;
  bocl_mem_sptr centerY_;
  bocl_mem_sptr centerZ_;
  bocl_mem_sptr num_bins_mem_;
  bocl_mem_sptr app_joint_hist_mem_;
  bocl_mem_sptr surf_joint_hist_mem_;

  bocl_mem_sptr lookup_;
  bocl_mem_sptr output_;
  bocl_mem_sptr T_mem_;
  bocl_mem_sptr R_mem_;
  bocl_mem_sptr bb_mem_;
  bocl_mem_sptr times_mem_;
  bocl_mem_sptr target_blk_origin_;
  bocl_mem_sptr target_subblk_num_;
  bocl_mem_sptr scene_sub_block_len_;
  std::vector<bocl_mem_sptr> particle_no_mems_;

  //tmp param
  float * app_hist_;
  float * app_histA_;
  float * app_histB_;

  float  surf_hist_[4];
  float  surf_histA_[2];
  float  surf_histB_[2];

  float gpu_time_;

  vnl_random rand_;
};


#endif //bstm_ocl_particle_filter_h_
