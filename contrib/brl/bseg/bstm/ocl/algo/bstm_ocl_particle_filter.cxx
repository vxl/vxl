#include <iostream>
#include <algorithm>
#include <utility>
#include "bstm_ocl_particle_filter.h"

#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_intersection.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vul/vul_timer.h>
#include <bstm/cpp/algo/bstm_label_bb_function.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_sphere_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_det.h>

#define ENABLE_ROT

bstm_ocl_particle_filter::bstm_ocl_particle_filter(const bocl_device_sptr& device, const bstm_scene_sptr& scene, const bstm_cache_sptr& cache, const bstm_opencl_cache_sptr& opencl_cache,
                        unsigned start_t, unsigned end_t, vgl_box_3d<double> initial_bb, int num_particles, double t_sigma, double w_sigma,
                        std::string kernel_opt,int nbins, int track_label, double radius ):
                        device_(device), scene_(scene), cache_(cache), opencl_cache_(opencl_cache),num_particles_(num_particles), initial_bb_(initial_bb),
                        t_sigma_(t_sigma), w_sigma_(w_sigma), start_t_(start_t), end_t_(end_t), track_label_(track_label), radius_(radius),
                        kernel_opt_(std::move(kernel_opt)), app_nbins_(nbins), app_view_dir_num_(4), surf_nbins_(2), rand_(9667566)
{
  //get blocks that intersect the provided bounding box.
  //std::vector<bstm_block_id> relevant_blocks = scene_->get_block_ids(initial_bb, start_t);

  //initially, each particle is the same bb
  std::vector<vgl_orient_box_3d<double> > initial_bbs;

  std::vector< vgl_rotation_3d<double> > empty_r;
  std::vector< vgl_vector_3d<double> >  empty_t;

  std::vector< std::map<bstm_block_id, std::vector<bstm_block_id> > > empty_blk_map;
  std::vector< double > initial_mi;
  for(unsigned i = 0; i < num_particles_;i++) {
    initial_bbs.emplace_back(initial_bb );
    initial_mi.push_back(1.0f);
    empty_t.emplace_back(0,0,0 );
    empty_r.emplace_back( );
  }
  bb_.push_back(initial_bbs);
  R_.push_back(empty_r);
  T_.push_back(empty_t);
  blk_map_.push_back(empty_blk_map);
  mi_.push_back(initial_mi);
  mean_bb_.push_back(initial_bb);
  infrot_.push_back(empty_r);
  inft_.push_back(empty_t);

  //construct 3d gaussians used to propagate particles
  vnl_vector_fixed<double,3> zero(double(0));
  vnl_vector_fixed<double,3> t_covar((double)(t_sigma*t_sigma));
  translation_sampler_.set_mean(zero);
  translation_sampler_.set_covar(t_covar);


  //initialize buffs
  surf_joint_histogram_buff_ = new cl_float[4* num_particles_];
  app_joint_histogram_buff_ = new cl_uint[app_view_dir_num_ *app_nbins_*app_nbins_* num_particles_];
  translation_buff_ = new cl_float[4 * num_particles_];
  rotation_buff_= new cl_float[9 * num_particles_];
  bb_buf_ = new cl_float[6 * num_particles_];
  particle_nos_ = new cl_uint[ num_particles_];

  //int entropy buf
  app_hist_  = new float[app_nbins_* app_nbins_*app_view_dir_num_];
  app_histA_ = new float[app_nbins_];
  app_histB_ = new float[app_nbins_];

  //init ocl
  init_ocl_minfo();
  compile_kernel();

  label(start_t);
}


std::vector<double> bstm_ocl_particle_filter::compute_mi_from_hist()
{
  std::vector<double> all_mi;

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {

    /*
    cl_float sum_surf_joint_hist = 0.0f;
    for(int i = 0; i < surf_nbins_ * surf_nbins_; i++)
      sum_surf_joint_hist += surf_joint_histogram_buff_[ surf_nbins_*surf_nbins_* particle_no + i];
    for(int i = 0; i < surf_nbins_ * surf_nbins_; i++)
      surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + i] /= sum_surf_joint_hist;
    for (int k = 0; k<surf_nbins_; k++)
    {
      surf_histA_[k] = 0.0;  surf_histB_[k] = 0.0;
    }

    surf_histA_[0] = surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 0] + surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 1];
    surf_histA_[1] = surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 2] + surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 3];
    surf_histB_[0] = surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 0] + surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 2];
    surf_histB_[1] = surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 1] + surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + 3];

    float surf_entropyA = 0;
    for (int k = 0; k < surf_nbins_; k++) {
      surf_entropyA += -(surf_histA_[k]?surf_histA_[k]*std::log(surf_histA_[k]):0); // if prob=0 this value is defined as 0
    }
    float surf_entropyB = 0;
    for (int l = 0; l < surf_nbins_; l++) {
      surf_entropyB += -(surf_histB_[l]?surf_histB_[l]*std::log(surf_histB_[l]):0); // if prob=0 this value is defined as 0
    }

    float surf_entropyAB =  0.0; ;
    for (int k = 0; k < surf_nbins_; k++) {
      for (int l = 0; l < surf_nbins_; l++) {
        surf_entropyAB += -(surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + k*surf_nbins_+l]?surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + k*surf_nbins_+l]*std::log(surf_joint_histogram_buff_[surf_nbins_*surf_nbins_* particle_no + k*surf_nbins_+l]):0);
      }
    }
    float mi_surf  = ( (surf_entropyA + surf_entropyB ) - surf_entropyAB) /vnl_math::ln2;
    */


    float mi_app = 0;
    for(unsigned view_dir_num = 0; view_dir_num < app_view_dir_num_; view_dir_num++)
    {
      cl_float sum_app_joint_hist = 0.0f;
      for(int i = 0; i < app_nbins_* app_nbins_; i++) {
        sum_app_joint_hist += app_joint_histogram_buff_[ app_view_dir_num_ *app_nbins_*app_nbins_* particle_no + (app_nbins_* app_nbins_*view_dir_num) + i];
      }
      //if the num is below a threshold, no apps here.
      if(sum_app_joint_hist < 1e-07f)
        continue;


      for(int i = 0; i < app_nbins_* app_nbins_; i++)
        app_hist_[(app_nbins_* app_nbins_*view_dir_num) + i] =  app_joint_histogram_buff_[app_view_dir_num_ *app_nbins_*app_nbins_* particle_no +(app_nbins_* app_nbins_*view_dir_num) + i] / sum_app_joint_hist;


      for (int k = 0; k<app_nbins_; k++)
      {
        app_histA_[k] = 0.0;
        app_histB_[k] = 0.0;
      }

      for (int k = 0; k < app_nbins_; k++) {
        for (int l = 0; l < app_nbins_; l++) {
          app_histA_[k]+=app_hist_[(app_nbins_* app_nbins_*view_dir_num) + k*app_nbins_+l];
        }
      }
      for (int k = 0; k < app_nbins_; k++) {
        for (int l = 0; l < app_nbins_; l++) {
          app_histB_[k]+=app_hist_[(app_nbins_* app_nbins_*view_dir_num) + l*app_nbins_+k];
        }
      }

      float app_entropyA = 0;
      for (int k = 0; k < app_nbins_; k++) {
        app_entropyA += -(app_histA_[k]?app_histA_[k]*std::log(app_histA_[k]):0); // if prob=0 this value is defined as 0
      }


      float app_entropyB = 0;
      for (int l = 0; l < app_nbins_; l++) {
        app_entropyB += -(app_histB_[l]?app_histB_[l]*std::log(app_histB_[l]):0); // if prob=0 this value is defined as 0
      }

      float app_entropyAB =  0.0; ;
      for (int k = 0; k < app_nbins_; k++) {
        for (int l = 0; l < app_nbins_; l++) {
          float val = app_hist_[(app_nbins_* app_nbins_*view_dir_num) + k*app_nbins_+l];
          app_entropyAB += -(val?val*std::log(val):0);
        }
      }
      //float norm = app_entropyA /vnl_math::ln2;
      mi_app  +=  ( ( (app_entropyA + app_entropyB) - app_entropyAB) /vnl_math::ln2 ) ;

    }
    float mi =  mi_app;

    all_mi.push_back(mi);
    //std::cout << R_.back()[particle_no].as_euler_angles() << " " << mi << std::endl;
  }

  return all_mi;
}

void bstm_ocl_particle_filter::normalize_mi_score(unsigned t)
{
  double sum = 0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
    sum += mi_[t - start_t_][particle_no];

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
    mi_[t - start_t_][particle_no] = num_particles_ * mi_[t - start_t_][particle_no] / sum;
}


bool bstm_ocl_particle_filter::compile_kernel()
{
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "bit/time_tree_library_functions.cl");
  src_paths.push_back(source_dir     + "statistics_library_functions.cl");
  src_paths.push_back(source_dir     + "view_dep_app_helper_functions.cl");
  src_paths.push_back(source_dir +     "tracking/particle_filter.cl");

  this->kern_ = new bocl_kernel();
  return kern_->create_kernel(&device_->context(),device_->device_id(), src_paths, "estimate_mi", kernel_opt_, "MI");
}


void bstm_ocl_particle_filter::propagate_particles(unsigned prev_time, unsigned cur_time)
{
  global_blk_map_.clear(); //make sure to empty the blk map

  //make an estimate of the velocity of the object
  vgl_vector_3d<double> v = velocity_estimate(prev_time-1,prev_time);
  vgl_rotation_3d<double> w = angular_velocity_estimate(prev_time-1,prev_time);
  std::cerr << "VELOCITY ESTIMATE: " << v.x() << " " << v.y() << " " << v.z() << std::endl;
  std::cerr << "ANGULAR VELOCITY ESTIMATE: " << w.as_rodrigues() << std::endl;

  std::vector<vgl_orient_box_3d<double> > next_bb;
  std::vector< vgl_rotation_3d<double> > new_R;
  std::vector< vgl_rotation_3d<double> > new_infrot;
  std::vector< vgl_vector_3d<double> >  new_T;
  std::vector< vgl_vector_3d<double> > new_inft;

  std::vector< std::map<bstm_block_id, std::vector<bstm_block_id> > > new_blk_map;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    //sample infinitesimal R and T
    vnl_vector_fixed<double,3> t( rand_.drand32(-t_sigma_, t_sigma_),rand_.drand32(-t_sigma_, t_sigma_),rand_.drand32(-t_sigma_, t_sigma_) );

    /*
    //random axis
    vnl_vector_fixed<double,3> axis(rand_.normal(),rand_.normal(),rand_.normal() );
    axis.normalize();
    vnl_quaternion<double> q(axis,rand_.drand32(-w_sigma_, w_sigma_) );
    vgl_rotation_3d<double> inf_rot(q);
    if(rand_.drand32() < 0.5)
        w = vgl_rotation_3d<double>(q);
    */

#ifdef ENABLE_ROT
    vgl_rotation_3d<double> inf_rot( sample_rot(w , w_kappa_,w_sigma_));
#else
    vgl_rotation_3d<double> inf_rot;
#endif

    vgl_vector_3d<double> inf_t(t[0], t[1], t[2]);

    if(rand_.drand32() < 0.5 )
      inf_t += v; //add velocity estimate

    //update R,T estimates
    vgl_rotation_3d<double> new_rotation = inf_rot * R_[prev_time - start_t_][particle_no];
    //std::cout << "OVERALL ROT " << new_rotation.as_rodrigues()[0] << " " << new_rotation.as_rodrigues()[1] << " "<< new_rotation.as_rodrigues()[2] << std::endl;

    vgl_vector_3d<double> new_t = inf_rot * T_[prev_time - start_t_][particle_no] + inf_t;
    //orient the corners of bb to create new AABB

    //look into vgl_box_3d to see why corners 0,1,2,4 are taken.
    vgl_orient_box_3d<double> orientedbox( initial_bb_, new_rotation.as_quaternion() );
    std::vector<vgl_point_3d<double> > corners = orientedbox.corners();

    vgl_point_3d<double> box_min_pt = corners[0] + new_t;
    vgl_point_3d<double> box_min_pt_plus_width  = corners[1] + new_t;
    vgl_point_3d<double> box_min_pt_plus_depth  = corners[2] + new_t;
    vgl_point_3d<double> box_min_pt_plus_height = corners[4] + new_t;
    //create the oriented box from the four pts
    vgl_orient_box_3d<double> new_box( box_min_pt,box_min_pt_plus_width,box_min_pt_plus_depth,box_min_pt_plus_height );

    //figure out mapping
    std::map<bstm_block_id, std::vector<bstm_block_id> > map;
    populate_blk_mapping( map, initial_bb_, new_rotation, new_t, start_t_, cur_time);

    //save
    new_R.push_back(new_rotation);
    new_T.push_back(new_t);
    next_bb.push_back(new_box);
    new_blk_map.push_back(map);
    new_infrot.push_back(inf_rot);
    new_inft.push_back(inf_t);
  }
  bb_.push_back(next_bb);
  R_.push_back(new_R);
  T_.push_back(new_T);
  blk_map_.push_back(new_blk_map);
  infrot_.push_back(new_infrot);
  inft_.push_back(new_inft);
}


void bstm_ocl_particle_filter::eval_obs_density(unsigned prev_time, unsigned cur_time)
{
  int status=0;
  queue_ = clCreateCommandQueue(device_->context(),*(device_->device_id()), CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE ,&status);
  if (status!=0)
    std::cerr << "Command queue cannot be created..." << std::endl;

  vul_timer timer;
  std::cerr << "EVALUATING OBS DENSITY..."  << std::endl;
  gpu_time_ = 0.0f;

  write_particle_ocl_info(start_t_,cur_time);

  mi_.push_back(eval_mi(prev_time, cur_time) );
  std::cerr << "FINISHED EVALUATING OBS DENSITY...gpu time: " <<  gpu_time_ / (double)1000 << " seconds. Real time: " << timer.real() / double(1000) << " seconds." << std::endl;

  //normalize scores to avoid precision problem
  normalize_mi_score(cur_time);

  clReleaseCommandQueue(queue_);
}

void bstm_ocl_particle_filter::write_particle_ocl_info(unsigned prev_time,unsigned cur_time )
{

  std::vector<cl_float> translations;
  std::vector<cl_float> rotations;
  std::vector<cl_float> bbs;

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    //get its AABB, R and T and blk mapping
    vgl_rotation_3d<double> R = R_[cur_time - start_t_][particle_no];
    vgl_vector_3d<double> T = T_[cur_time  - start_t_][particle_no];

    translations.push_back((float)T.x() );
    translations.push_back((float)T.y() );
    translations.push_back((float)T.z() );
    translations.push_back( 0.0f );

    vnl_matrix_fixed<double, 3, 3> R_matrix = R.as_matrix();
    rotations.push_back( (float)R_matrix(0,0) );
    rotations.push_back( (float)R_matrix(0,1) );
    rotations.push_back( (float)R_matrix(0,2) );
    rotations.push_back( (float)R_matrix(1,0) );
    rotations.push_back( (float)R_matrix(1,1) );
    rotations.push_back( (float)R_matrix(1,2) );
    rotations.push_back( (float)R_matrix(2,0) );
    rotations.push_back( (float)R_matrix(2,1) );
    rotations.push_back( (float)R_matrix(2,2) );

    //bounding box buffer
    bbs.push_back( initial_bb_.min_x() );
    bbs.push_back( initial_bb_.min_y() );
    bbs.push_back( initial_bb_.min_z() );
    bbs.push_back( initial_bb_.max_x() );
    bbs.push_back( initial_bb_.max_y() );
    bbs.push_back( initial_bb_.max_z() );
  }
  //copy back vectors to cl_bufs
  std::copy(translations.begin(), translations.end(), translation_buff_);
  std::copy(rotations.begin(), rotations.end(), rotation_buff_);
  std::copy(bbs.begin(), bbs.end(), bb_buf_);



  //load prev_time and time 2 (local coordinates)
  double local_time1,local_time2;
  scene_->local_time((double)prev_time + 0.5,local_time1);
  scene_->local_time((double)cur_time + 0.5,local_time2);
  std::cout << "Prev time: " << local_time1 << " cur time: " << local_time2 << std::endl;
  times_[0] = (float) local_time1;
  times_[1] = (float) local_time2;
  times_[2] = prev_time;
  times_[3] = cur_time;
  times_mem_->write_to_buffer(queue_,true);


  //write back to ocl
  bb_mem_->write_to_buffer(queue_,true);
  R_mem_->write_to_buffer(queue_,true);
  T_mem_->write_to_buffer(queue_,true);

  //empty histograms
  app_joint_hist_mem_->zero_gpu_buffer(queue_,true);
  //surf_joint_hist_mem_->zero_gpu_buffer(queue_,true);
  //output_->zero_gpu_buffer(queue_,true);
}

vgl_rotation_3d<double> bstm_ocl_particle_filter::sample_rot(const vgl_rotation_3d<double>& rot, double kappa, double w_sigma )
{

  //first construct a random sample from a von-mises fisher distribution with mean [0,0,1] and concentration parameter kappa
  double v = 2 * vnl_math::pi * rand_.drand32();
  double u = rand_.drand32();
  double w = 1 + (1/kappa) * ( std::log(u)  + std::log(1 - ((u-1)/u)* std::exp(-2 * kappa) ) );
  vnl_vector_fixed<double,3> sample;
  sample[0] = std::sqrt(1-w*w) * std::cos(v);
  sample[1] = std::sqrt(1-w*w) * std::sin(v);
  sample[2] = w;


  //now rotate to center at mean of rot
  vnl_vector_fixed<double,3> z_dir; z_dir[0] = 0; z_dir[1] = 0; z_dir[2] = 1;
  vgl_rotation_3d<double> mean_transform(z_dir, rot.axis());
  vnl_vector_fixed<double,3> rotated_sample = mean_transform * sample;

  //add gaussian noise to angle
  double new_angle = rot.angle() + rand_.normal() * w_sigma/2;
  //double new_angle = rot.angle() + rand_.drand32(-w_sigma/2,w_sigma/2);

  //std::cout << "Axis is " << rot.axis() << " became " << rotated_sample << std::endl;
  //std::cout << "Angle is " << rot.angle() << " became " << new_angle << std::endl;
  return vgl_rotation_3d<double>(rotated_sample * new_angle);

}

void bstm_ocl_particle_filter::resample(unsigned  /*prev_time*/, unsigned cur_time)
{

  //resampled vectors to replace current ones
  std::vector<vgl_orient_box_3d<double> > resampled_bbs;
  std::vector< vgl_rotation_3d<double> > resampled_r;
  std::vector< vgl_vector_3d<double> >  resampled_t;
  std::vector< vgl_rotation_3d<double> > resampled_infrot;
  std::vector< vgl_vector_3d<double> >  resampled_inft;

  std::vector< std::map<bstm_block_id, std::vector<bstm_block_id> > > resampled_blk_map;
  std::vector< double > resampled_mi;

  //build cdf
  std::vector<double> cdf;
  double sum = 0.0f;
  cdf.push_back(sum);

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    cdf.push_back( cdf.back() + mi_[cur_time - start_t_][particle_no]);
    sum += mi_[cur_time - start_t_][particle_no];
  }

  float u = rand_.drand32();
  int j = 1;
  //resample
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    float u_i = (u + particle_no) / num_particles_;

    while( u_i > (cdf[j] / sum) )
      j++;

    //push in sample j-1
    resampled_bbs.push_back( bb_[cur_time - start_t_][j-1]);
    resampled_r.push_back( R_[cur_time - start_t_][j-1]);
    resampled_t.push_back( T_[cur_time - start_t_][j-1]);
    resampled_blk_map.push_back( blk_map_[cur_time - start_t_][j-1]);
    //resampled_mi.push_back( mi_[cur_time - start_t_][j-1]);
    resampled_mi.push_back( 1.0 /num_particles_);
    resampled_infrot.push_back( infrot_[cur_time - start_t_][j-1]);
    resampled_inft.push_back( inft_[cur_time - start_t_][j-1]);
  }

  //delete last
  bb_.pop_back();
  R_.pop_back();
  T_.pop_back();
  blk_map_.pop_back();
  mi_.pop_back();
  infrot_.pop_back();
  inft_.pop_back();

  bb_.push_back(resampled_bbs);
  R_.push_back(resampled_r);
  T_.push_back(resampled_t);
  blk_map_.push_back(resampled_blk_map);
  mi_.push_back(resampled_mi);
  infrot_.push_back(resampled_infrot);
  inft_.push_back(resampled_inft);
}

double bstm_ocl_particle_filter::survival_diagnostic(unsigned cur_time)
{
  double sum_weights = 0;
  double sum_weights_sq = 0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    if(mi_[cur_time - start_t_][particle_no] > 0.0f)
    {
      sum_weights += mi_[cur_time - start_t_][particle_no];
      sum_weights_sq += mi_[cur_time - start_t_][particle_no] * mi_[cur_time - start_t_][particle_no];
    }
  }
  sum_weights_sq /= sum_weights * sum_weights;
  return 1 / (sum_weights_sq * num_particles_);
}


vgl_box_3d<double> bstm_ocl_particle_filter::w_mean_bb( std::vector<vgl_orient_box_3d<double> > bb,  std::vector< double> weights)
{
  vgl_point_3d<double> min_pt,max_pt;
  float sum_weights = 0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    min_pt.x() += bb[particle_no].corners()[0].x() * weights[particle_no];
    min_pt.y() += bb[particle_no].corners()[0].y() * weights[particle_no];
    min_pt.z() += bb[particle_no].corners()[0].z() * weights[particle_no];

    max_pt.x() += bb[particle_no].corners()[7].x() * weights[particle_no];
    max_pt.y() += bb[particle_no].corners()[7].y() * weights[particle_no];
    max_pt.z() += bb[particle_no].corners()[7].z() * weights[particle_no];
    sum_weights += weights[particle_no];
  }
  min_pt.set(min_pt.x() / sum_weights,min_pt.y() / sum_weights,min_pt.z() / sum_weights  );
  max_pt.set(max_pt.x() / sum_weights,max_pt.y() / sum_weights,max_pt.z() / sum_weights  );

  vgl_box_3d<double> weighted_box(min_pt,max_pt);

  std::cerr << "MEAN BB: " << weighted_box.centroid() << " dim " << weighted_box.width() << " " << weighted_box.height() << " " << weighted_box.depth() << std::endl;

  return weighted_box;
}



vgl_vector_3d<double> bstm_ocl_particle_filter::velocity_estimate(unsigned  /*prev2_time*/, unsigned prev_time)
{
  if(prev_time == start_t_)
    return {0,0,0};

/*
  vgl_box_3d<double> box_prev2 = w_mean_bb(bb_[prev2_time - start_t_],  mi_[prev2_time - start_t_]);
  vgl_box_3d<double> box_prev = w_mean_bb(bb_[prev_time - start_t_],  mi_[prev_time - start_t_]);
  vgl_vector_3d<double> velocity = box_prev.centroid() - box_prev2.centroid();
*/

  vgl_vector_3d<double> sum_inft;
  float sum_weights = 0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    sum_inft  += inft_[prev_time - start_t_][particle_no] * mi_[prev_time - start_t_][particle_no];
    sum_weights +=  mi_[prev_time - start_t_][particle_no];
  }
  vgl_vector_3d<double> velocity = sum_inft / sum_weights;

  return velocity;
}

vgl_rotation_3d<double> bstm_ocl_particle_filter::angular_velocity_estimate(unsigned prev2_time, unsigned prev_time)
{
  if(prev_time == start_t_)
    return vgl_rotation_3d<double>(0.075175, -0.320105, -0.0650326); //HACK


  vgl_rotation_3d<double> rot_prev2 = mean_rot(R_[prev2_time - start_t_], mi_[prev2_time - start_t_]);
  std::cerr << "ROT PREV2 " << rot_prev2.as_rodrigues() << std::endl;
  vgl_rotation_3d<double> rot_prev = mean_rot(R_[prev_time - start_t_], mi_[prev_time - start_t_]);
  std::cerr << "ROT PREV " << rot_prev.as_rodrigues() << std::endl;
  return rot_prev * rot_prev2.transpose();

  /*
  float best_score_prev2,best_score_prev = 0;
  vgl_rotation_3d<double> best_r_prev2,best_r_prev;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    if(mi_[prev2_time - start_t_][particle_no] >= best_score_prev2) {
      best_score_prev2 = mi_[prev2_time - start_t_][particle_no];
      best_r_prev2 = R_[prev2_time - start_t_][particle_no];
    }
    if(mi_[prev_time - start_t_][particle_no] >= best_score_prev)
    {
      best_score_prev = mi_[prev_time - start_t_][particle_no];
      best_r_prev = R_[prev_time - start_t_][particle_no];
    }
  }
  std::cerr << "ROT PREV2 " << best_r_prev2.as_rodrigues() << std::endl;
  std::cerr << "ROT PREV " << best_r_prev.as_rodrigues() << std::endl;


  return best_r_prev * best_r_prev2.transpose();
  */
}

std::vector<double> bstm_ocl_particle_filter::eval_mi(unsigned  /*prev_time*/, unsigned cur_time)
{
  std::size_t local_threads[1]={128};
  std::size_t global_threads[1]={1};

  //snap thru each relevant blk and its mapping
  std::set< std::pair< bstm_block_id, bstm_block_id> >::const_iterator iter;
  for (iter = global_blk_map_.begin(); iter != global_blk_map_.end(); iter++)
  {
    bstm_block_id relevant_blk_id = iter->first;
    bstm_block_id target_blk_id = iter->second;

    //set global threads
    bstm_block_metadata mdata = scene_->get_block_metadata(relevant_blk_id);
    global_threads[0] = (unsigned) RoundUp(mdata.sub_block_num_.x()*mdata.sub_block_num_.y()*mdata.sub_block_num_.z(),(int)local_threads[0]);

    //load target mem
    bocl_mem* target_blk = opencl_cache_->get_block(target_blk_id);
    bocl_mem* target_blk_t = opencl_cache_->get_time_block(target_blk_id);
    bocl_mem* target_alpha = opencl_cache_->get_data(target_blk_id, bstm_data_traits<BSTM_ALPHA>::prefix());
    bocl_mem* target_app = opencl_cache_->get_data(target_blk_id, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());

    //load blk info and write appropriate info to several buffers
    bocl_mem* blk_info = opencl_cache_->loaded_block_info();
    auto* info_buffer = (bstm_scene_info*) blk_info->cpu_buffer();
    info_buffer->data_buffer_length = (int) (target_blk_t->num_bytes()/8);
    int target_time_tree_len = info_buffer->data_buffer_length;

    ////////
    //load ocl stuff about target blk

    //load scene origin
    target_blk_origin_buff[0] = info_buffer->scene_origin[0];
    target_blk_origin_buff[1] = info_buffer->scene_origin[1];
    target_blk_origin_buff[2] = info_buffer->scene_origin[2];
    target_blk_origin_buff[3] = info_buffer->scene_origin[3];
    target_blk_origin_->write_to_buffer(queue_,true);

    //loads scene dims (number of sub-blocks/trees in each block)
    target_subblk_num_buff[0] = info_buffer->scene_dims[0];
    target_subblk_num_buff[1] = info_buffer->scene_dims[1];
    target_subblk_num_buff[2] = info_buffer->scene_dims[2];
    target_subblk_num_buff[3] = info_buffer->scene_dims[3];
    target_subblk_num_->write_to_buffer(queue_,true);

    //load scene block len
    block_len_buff = info_buffer->block_len;
    scene_sub_block_len_->write_to_buffer(queue_,true);
    output_->zero_gpu_buffer(queue_,true);
    int status = clFinish(queue_);
    check_val(status, MEM_FAILURE, "Writing buffers failed: " + error_to_string(status));

    ////////

    //load current mem
    bocl_mem* blk = opencl_cache_->get_block(relevant_blk_id);
    bocl_mem* blk_t = opencl_cache_->get_time_block(relevant_blk_id);
    bocl_mem* alpha = opencl_cache_->get_data(relevant_blk_id, bstm_data_traits<BSTM_ALPHA>::prefix());
    bocl_mem* app = opencl_cache_->get_data(relevant_blk_id, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    blk_info = opencl_cache_->loaded_block_info();

    //opencl mem operations are done. begin kernel launches...

    for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
    {
      //first check if the particle needs this blk pair
      std::vector<bstm_block_id> particle_target_blks = blk_map_[cur_time  - start_t_][particle_no][relevant_blk_id] ;
      auto found = std::find(particle_target_blks.begin(), particle_target_blks.end(), target_blk_id);
      if(found != particle_target_blks.end())  //particle needs this blk mapping!
      {
        kern_->set_arg(blk_info);
        kern_->set_arg(centerX_.ptr());
        kern_->set_arg(centerY_.ptr());
        kern_->set_arg(centerZ_.ptr());
        kern_->set_arg(lookup_.ptr());

        kern_->set_arg(blk);
        kern_->set_arg(blk_t);
        kern_->set_arg(alpha);
        kern_->set_arg(app);

        kern_->set_arg(target_blk);
        kern_->set_arg(target_blk_t);
        kern_->set_arg(target_alpha);
        kern_->set_arg(target_app);

        kern_->set_arg(T_mem_.ptr());
        kern_->set_arg(R_mem_.ptr());
        kern_->set_arg(bb_mem_.ptr());


        kern_->set_arg(target_blk_origin_.ptr());
        kern_->set_arg(target_subblk_num_.ptr());
        kern_->set_arg(scene_sub_block_len_.ptr());
        kern_->set_arg(times_mem_.ptr());

        kern_->set_arg(num_bins_mem_.ptr());
        kern_->set_arg(app_joint_hist_mem_.ptr());
        //kern_->set_arg(surf_joint_hist_mem_.ptr());

        kern_->set_arg(particle_no_mems_[particle_no].ptr());

        kern_->set_arg(output_.ptr());

        kern_->set_local_arg(16*local_threads[0]*sizeof(cl_uchar)); // local trees
        kern_->set_local_arg(16*local_threads[0]*sizeof(cl_uchar )); // local trees
        kern_->set_local_arg(TT_NUM_BYTES*local_threads[0]*sizeof(cl_uchar)); // local time trees
        kern_->set_local_arg(app_view_dir_num_ *app_nbins_*app_nbins_*sizeof(cl_uint));
        kern_->execute(queue_, 1, local_threads, global_threads);

        //check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        //gpu_time_ += kern_->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern_->clear_args();
      }
    }

    status = clFinish(queue_);
    check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
  }

  //output_->read_to_buffer(queue_,true);

  app_joint_hist_mem_->read_to_buffer(queue_,true);
  //surf_joint_hist_mem_->read_to_buffer(queue_,true);
  return compute_mi_from_hist();
 }

void bstm_ocl_particle_filter::init_ocl_minfo()
{
  //load cell centers
  centerX_ = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  centerY_ = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  centerZ_ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


  //translation
  T_mem_ = new bocl_mem(device_->context(), translation_buff_, sizeof(cl_float)*4 * num_particles_, " translation " );
  T_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //rotation
  R_mem_ = new bocl_mem(device_->context(), rotation_buff_, sizeof(cl_float)*9* num_particles_, " rotation " );
  R_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //time buffer
  times_mem_ = new bocl_mem(device_->context(), times_, sizeof(cl_float)* 4, " time buf" );
  times_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //bb buffer
  bb_mem_ = new bocl_mem(device_->context(), bb_buf_, sizeof(cl_float)* 6* num_particles_, " bounding box buf" );
  bb_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  num_bins_mem_  = new bocl_mem(device_->context(), &app_nbins_, sizeof(cl_uint), " num_bins buf" );
  num_bins_mem_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );


  //target blk's origin
  target_blk_origin_ = new bocl_mem(device_->context(), target_blk_origin_buff, 4*sizeof(cl_float), " scene origin buff" );
  target_blk_origin_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //target blk's sub blk num
  target_subblk_num_ = new bocl_mem(device_->context(), target_subblk_num_buff, sizeof(cl_int)* 4, " scene sub block num" );
  target_subblk_num_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //target blk's tree len
  scene_sub_block_len_ = new bocl_mem(device_->context(), &(block_len_buff), sizeof(cl_float), " scene sub block len" );
  scene_sub_block_len_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //particle_no
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    particle_nos_[particle_no ] = particle_no;
    particle_no_mems_.push_back ( new bocl_mem(device_->context(), &(particle_nos_[particle_no ]), sizeof(cl_uint), " particle number" ) );
    particle_no_mems_.back()->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
  }


  // bit lookup buffer
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup_=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // output buffer for debugging
  for(float & i : output_buff) i = 0;
  output_ = new bocl_mem(device_->context(), output_buff, sizeof(cl_float)*1000, "output" );
  output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  //histograms
  app_joint_hist_mem_  = new bocl_mem(device_->context(), app_joint_histogram_buff_, sizeof(cl_uint)*app_nbins_*app_nbins_* app_view_dir_num_ * num_particles_, " app joint histogram" );
  app_joint_hist_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );

  surf_joint_hist_mem_  = new bocl_mem(device_->context(), surf_joint_histogram_buff_, sizeof(cl_float)*4 * num_particles_, " surf joint histogram" );
  surf_joint_hist_mem_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
}

void bstm_ocl_particle_filter::track()
{
  for(unsigned t = start_t_ + 1; t <= end_t_; t++ )
  {
    //populates bb_,R_,T_,blk_map_ at time t
    propagate_particles(t-1,t);

    //populates mi_ at time t
    eval_obs_density(t-1,t);

    //repopulates particles by re-sampling to ensure low score particles are killed.
    resample(t-1,t);

    //label the mean bb
    label(t);
  }

}

vgl_orient_box_3d<double> bstm_ocl_particle_filter::best_bb( std::vector<vgl_orient_box_3d<double> > bb,  std::vector< double> weights,  std::vector< vgl_vector_3d<double> > T,std::vector< vgl_rotation_3d<double> > R)
{
  float best_score = 0;
  vgl_orient_box_3d<double> best_bb;
  vgl_rotation_3d<double> best_r;
  vgl_vector_3d<double> best_t;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    if(weights[particle_no] >= best_score) {
      best_score = weights[particle_no];
      best_bb = bb[particle_no];
      best_t = T[particle_no];
      best_r = R[particle_no];
    }
  }
  std::cerr << "BEST BB " << best_bb.centroid() << " dim " << best_bb.width() << " " << best_bb.height() << " " << best_bb.depth() << " has MI: " << best_score
            << " and T: " << best_t << " and R: " << best_r.as_euler_angles() << std::endl;
  return best_bb;
}

vgl_rotation_3d<double> bstm_ocl_particle_filter::mean_rot( std::vector< vgl_rotation_3d<double> > rot, std::vector< double > weights )
{

  vnl_matrix_fixed<double,3,3> avg_rot;
  avg_rot.fill(0);
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
    avg_rot += rot[particle_no].as_matrix() * weights[particle_no];

  vnl_svd<double> svd(avg_rot);
  vnl_matrix_fixed<double,3,3> U = svd.U();
  vnl_matrix_fixed<double,3,3> V = svd.V();

  vnl_matrix_fixed<double,3,3> H;
  vnl_vector<double> diag(3);
  diag[0] = 1.0;
  diag[1] = 1.0;
  diag[2] = -1.0;
  H.set_diagonal( diag );

  if( vnl_det(U * V.transpose() ) >= 0.0 )
    return vgl_rotation_3d<double>( U * V.transpose() );
  else
    return vgl_rotation_3d<double>( U * H * V.transpose() );
}

void bstm_ocl_particle_filter::mean_state( unsigned t, vgl_vector_3d<double> & mean_T, vgl_rotation_3d<double> & mean_R)
{

  mean_T = vgl_vector_3d<double>(0,0,0);
  double sum_weights =0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    mean_T += T_[t- start_t_][particle_no] * mi_[t - start_t_][particle_no];
    sum_weights +=  mi_[t- start_t_][particle_no];
  }
  mean_T /= sum_weights;

  mean_R = mean_rot(R_[t- start_t_], mi_[t- start_t_]);
}


void bstm_ocl_particle_filter::save_and_clear_gpu()
{
  cache_->write_to_disk();
  opencl_cache_->clear_cache();
  cache_->clear_cache();
}


void bstm_ocl_particle_filter::label(unsigned t)
{
  //get initial box
  vgl_orient_box_3d<double> box;
  //label box at time start_t;
  if(t  == start_t_) {
    box = bb_[0][0]; //get the intial box
  }
  else
  {

    vgl_vector_3d<double>  mean_T;
    vgl_rotation_3d<double> mean_R;
    mean_state( t, mean_T, mean_R);
    std::cout << "MEAN T: " << mean_T << " and mean R: " << mean_R.as_euler_angles()[0] << " " << mean_R.as_euler_angles()[1] << " "<< mean_R.as_euler_angles()[2] << std::endl;

    vgl_orient_box_3d<double> orientedbox( initial_bb_, mean_R.as_quaternion() );
    std::vector<vgl_point_3d<double> > corners = orientedbox.corners();

    vgl_point_3d<double> box_min_pt = corners[0] + mean_T;
    vgl_point_3d<double> box_min_pt_plus_width  = corners[1] + mean_T;
    vgl_point_3d<double> box_min_pt_plus_depth  = corners[2] + mean_T;
    vgl_point_3d<double> box_min_pt_plus_height = corners[4] + mean_T;
    //create the oriented box from the four pts
    box = vgl_orient_box_3d<double> ( box_min_pt,box_min_pt_plus_width,box_min_pt_plus_depth,box_min_pt_plus_height );


    //box =  best_bb( bb_[t - start_t_ ], mi_[t - start_t_ ] , T_[t - start_t_ ], R_[t - start_t_ ]);
  }



  //get best box's aabb
  std::vector<vgl_point_3d<double> > corners = box.corners();
  vgl_box_3d<double> aabb;
  for(std::vector<vgl_point_3d<double> >::const_iterator pt_iter = corners.begin(); pt_iter != corners.end(); pt_iter++)
    aabb.add(*pt_iter);
  std::cout << "AABB: " << aabb.centroid() << " dim " << aabb.width() << " " << aabb.height() << " " << aabb.depth() << std::endl;


  //iterate over each block/metadata to check if bbox intersects the input bbox
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene_->blocks();
  std::map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;

    if(!vgl_intersection<double>( bstm_metadata.bbox(), aabb).is_empty() ) //if the two boxes intersect
    {
      double local_time;
      if(bstm_metadata.contains_t (t, local_time) ) //if the block box contains the given time
      {
        bstm_block* blk = cache_->get_block(bstm_metadata.id_);
        bstm_time_block* blk_t = cache_->get_time_block(bstm_metadata.id_);
        bstm_data_base * alph    = cache_->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
        bstm_data_base * label_data_base = cache_->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_LABEL>::prefix(),
                                        alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize() * bstm_data_traits<BSTM_LABEL>::datasize() );

        auto * alpha_data = (bstm_data_traits<BSTM_ALPHA>::datatype*) alph->data_buffer();
        auto * label_data = (bstm_data_traits<BSTM_LABEL>::datatype*) label_data_base->data_buffer();

        //if(radius_ > 0) { //if labeling a ball
        //  vgl_sphere_3d<double> sphere(aabb.centroid(), radius_);
        //  bstm_label_ball(blk, bstm_metadata, blk_t,label_data, alpha_data, local_time, track_label_, sphere, 0.1);
        //}
        //else //else labeling a box
          bstm_label_oriented_bb(blk, bstm_metadata, blk_t,label_data, alpha_data, local_time, track_label_, box, 0);
      }
    }
  }



  cache_->write_to_disk();

}

void bstm_ocl_particle_filter::populate_blk_mapping(std::map<bstm_block_id, std::vector<bstm_block_id> > & mapping, const vgl_box_3d<double>& aabb,
                                                          const vgl_rotation_3d<double>& r,const  vgl_vector_3d<double>& t, unsigned prev_time, unsigned curr_time)
{
  //get relevant blocks from prev time
  std::vector<bstm_block_id> relevant_blocks = scene_->get_block_ids(aabb, prev_time);

  std::vector<bstm_block_id>::const_iterator iter;
  for(iter = relevant_blocks.begin(); iter != relevant_blocks.end(); iter++)
  {
    //std::cout << "Processing blk " << *iter << std::endl;
    vgl_box_3d<double> block_bb =  scene_->get_block_metadata(*iter).bbox();

    //intersect with bounding bb
    vgl_box_3d<double> block_intersection_bb = vgl_intersection(block_bb, aabb);

    block_intersection_bb.scale_about_centroid(0.99);

    //now rotate and translate the box according to R,T
    vgl_orient_box_3d<double> oriented_intersection_box(block_intersection_bb, r.as_quaternion());
    //get the corners of resulting box and insert them to an axis aligned box.
    std::vector<vgl_point_3d<double> > corners = oriented_intersection_box.corners();
    vgl_box_3d<double> aa_intersection_box;
    std::vector<vgl_point_3d<double> >::const_iterator pt_iter;
    for(pt_iter = corners.begin(); pt_iter != corners.end(); pt_iter++) {
      aa_intersection_box.add(*pt_iter + t) ;
    }
    //get blocks intersecting this box
    std::vector<bstm_block_id> query_relevant_blocks = scene_->get_block_ids(aa_intersection_box, curr_time);
    std::vector<bstm_block_id>::const_iterator iter_query_blks;
    for(iter_query_blks = query_relevant_blocks.begin(); iter_query_blks != query_relevant_blocks.end(); iter_query_blks++) {
      std::pair<bstm_block_id,bstm_block_id> my_pair(*iter,  *iter_query_blks);
      global_blk_map_.insert(my_pair);
    }
    //insert into map
    mapping[*iter] = query_relevant_blocks;
  }

}
