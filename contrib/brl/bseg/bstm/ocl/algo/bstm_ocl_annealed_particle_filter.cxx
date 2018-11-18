#include <iostream>
#include <fstream>
#include <sstream>
#include "bstm_ocl_annealed_particle_filter.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#define ALPHA 0.5
#define MAX_BETA 15
#define ENABLE_ROT

void bstm_ocl_annealed_particle_filter::track()
{
  for(unsigned t = start_t_ + 1; t <= end_t_; t++ )
  {
    std::cerr << "PROCESSING TIME: " << t << std::endl;

    //correct covariance, move this to propagate_particles
    //vnl_vector_fixed<double,3> t_covar((double)(t_sigma_ * t_sigma_) * ALPHA);
    //translation_sampler_.set_covar(t_covar);
    t_sigma_ = original_t_sigma_ * ALPHA;
    w_sigma_ = original_w_sigma_ * ALPHA;
    w_kappa_ = original_w_kappa_ / ALPHA;

    //populates bb_,R_,T_,blk_map_ at time t
    propagate_particles(t-1,t);

    float beta = 1.0f;
    for(unsigned m = num_annealing_layers_; m > 0; m--)
    {
      std::cerr << "LAYER: " << m << std::endl;

      //remove the mi from the prev layer, no need to store it...
      if(m != num_annealing_layers_)
        mi_.pop_back();

      //populates mi_ at time t
      eval_obs_density(t-1,t);

      //calculate the optimal beta to ensure alpha
      beta = scale_obs_density(t, beta,m);
      std::cerr << "SURVIVAL DIAGNOSTIC: " << bstm_ocl_particle_filter::survival_diagnostic(t) << " BETA: " << beta << std::endl;

      dump_particles(t,m);

      //print best bb and its score
      best_bb( bb_[t - start_t_],  mi_[t - start_t_],  T_[t - start_t_], R_[t - start_t_]);
      w_mean_bb(bb_[t - start_t_],  mi_[t - start_t_]);

      //repopulates particles by re-sampling to ensure low score particles are killed.
      resample(t-1,t);

      if(m != 1) //don't perturb at the last lvl
        perturb_particles(t-1,t,m); //perturbs the particles based on annealing lvl
    }
    //label the mean bb
    label(t);

    //clear gpu mem every once in a while
    if(t % 10 == 0)
      this->save_and_clear_gpu();
  }
}

void bstm_ocl_annealed_particle_filter::dump_particles(unsigned cur_time, unsigned cascade)
{
  std::stringstream filename;
  filename <<  pf_output_path_ << "/iter_" << cur_time << "_" << cascade << ".xyz";
  std::ofstream myfile(filename.str().c_str());

  std::vector<vgl_orient_box_3d<double> > bb = bb_[cur_time - start_t_];
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
#ifndef DUMP_BOX
    myfile << bb[particle_no].centroid().x() << " " << bb[particle_no].centroid().y() << " "<< bb[particle_no].centroid().z() << " " <<  mi_[cur_time - start_t_][particle_no] << std::endl;
#else
    std::vector<vgl_point_3d<double> > corners  = bb[particle_no].corners();
    for(int i = 0; i < corners.size(); i++)
      myfile << corners[i].x() << " " << corners[i].y() << " " << corners[i].z() << " " <<  mi_[cur_time - start_t_][particle_no] << std::endl;
#endif
  }
  myfile.close();
}

void bstm_ocl_annealed_particle_filter::perturb_particles(unsigned  /*prev_time*/, unsigned cur_time, unsigned  /*m*/)
{
  t_sigma_ *= ALPHA;
  w_sigma_ *= ALPHA;
  w_kappa_ /= ALPHA;
  std::cout << "Using t_sigma: " << t_sigma_ << std::endl;
  std::cout << "Using w_sigma: " << w_sigma_ << std::endl;
  std::cout << "Using w_kappa: " << w_kappa_ << std::endl;

  global_blk_map_.clear(); //make sure to empty the blk map

  //get the latest bb,R,T and blk_map to perturb
  std::vector<vgl_orient_box_3d<double> > new_bb = bb_[cur_time - start_t_];
  std::vector< vgl_rotation_3d<double> > new_R = R_[cur_time - start_t_];
  std::vector< vgl_vector_3d<double> >  new_T = T_[cur_time - start_t_];
  std::vector< vgl_rotation_3d<double> > new_infrot = infrot_[cur_time - start_t_];
  std::vector< vgl_vector_3d<double> >  new_inft = inft_[cur_time - start_t_];

  std::vector< std::map<bstm_block_id, std::vector<bstm_block_id> > > new_blk_map = blk_map_[cur_time - start_t_];

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    //sample infinitesimal R and T
    //vnl_vector_fixed<double,3> t = translation_sampler_.sample(rand_);
    vnl_vector_fixed<double,3> t(rand_.drand32(-t_sigma_, t_sigma_),rand_.drand32(-t_sigma_, t_sigma_),rand_.drand32(-t_sigma_, t_sigma_) );

#ifdef ENABLE_ROT
    vgl_rotation_3d<double> inf_rot( sample_rot(new_infrot[particle_no], w_kappa_,w_sigma_ ) );
#else
    vgl_rotation_3d<double> inf_rot;
#endif
    vgl_vector_3d<double> inf_t(t[0], t[1], t[2]);


    new_T[particle_no] = inf_rot * new_infrot[particle_no].transpose()  * (new_T[particle_no] - new_inft[particle_no]) + new_inft[particle_no] + inf_t;
    new_inft[particle_no] = new_inft[particle_no] + inf_t;
    new_R[particle_no] = inf_rot * new_infrot[particle_no].transpose() * new_R[particle_no] ;        //newinfrot * R_2^t * (R_2* R_1)
    new_infrot[particle_no] = inf_rot;

    //update box
    //look into vgl_box_3d to see why corners 0,1,2,4 are taken.
    vgl_orient_box_3d<double> orientedbox( initial_bb_, new_R[particle_no].as_quaternion() );
    std::vector<vgl_point_3d<double> > corners = orientedbox.corners();

    vgl_point_3d<double> box_min_pt = corners[0] + new_T[particle_no];
    vgl_point_3d<double> box_min_pt_plus_width  = corners[1] + new_T[particle_no];
    vgl_point_3d<double> box_min_pt_plus_depth  = corners[2] + new_T[particle_no];
    vgl_point_3d<double> box_min_pt_plus_height = corners[4] + new_T[particle_no];
    //create the oriented box from the four pts
    new_bb[particle_no] = vgl_orient_box_3d<double> ( box_min_pt,box_min_pt_plus_width,box_min_pt_plus_depth,box_min_pt_plus_height );


    //figure out mapping
    std::map<bstm_block_id, std::vector<bstm_block_id> > map;
    populate_blk_mapping( map, initial_bb_, new_R[particle_no], new_T[particle_no], start_t_, cur_time);

  }
  //erase the last elements, old info
  bb_.pop_back();
  R_.pop_back();
  T_.pop_back();
  blk_map_.pop_back();
  infrot_.pop_back();
  inft_.pop_back();

  //push back the new info
  bb_.push_back(new_bb);
  R_.push_back(new_R);
  T_.push_back(new_T);
  blk_map_.push_back(new_blk_map);
  infrot_.push_back(new_infrot);
  inft_.push_back(new_inft);
}

float bstm_ocl_annealed_particle_filter::survival_diagnostic(unsigned cur_time, double beta)
{
  double sum_weights = 0;
  double sum_weights_sq = 0;
  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
  {
    if(mi_[cur_time - start_t_][particle_no] > 0.0f)
    {
      sum_weights += std::pow(mi_[cur_time - start_t_][particle_no], beta);
      sum_weights_sq += std::pow(mi_[cur_time - start_t_][particle_no],beta) * std::pow(mi_[cur_time - start_t_][particle_no],beta);
    }
  }
  sum_weights_sq /= sum_weights * sum_weights;
  return 1 / (sum_weights_sq * num_particles_);
}


float bstm_ocl_annealed_particle_filter::scale_obs_density(unsigned cur_time, double cur_beta, unsigned m)
{
  float grad_step = 0.0001;
  if(m != num_annealing_layers_)
  {
    while(  survival_diagnostic(cur_time, cur_beta) - ALPHA > 0.01 && cur_beta < MAX_BETA)
      cur_beta +=grad_step;
  }
  else
  {
    while(  std::fabs(survival_diagnostic(cur_time, cur_beta) - ALPHA) > 0.01 && cur_beta < MAX_BETA)
      cur_beta += (survival_diagnostic(cur_time, cur_beta) - ALPHA) > 0 ? grad_step : -grad_step;
  }

  for(unsigned particle_no = 0; particle_no < num_particles_;particle_no++)
    if(mi_[cur_time - start_t_][particle_no] > 0.0f)
      mi_[cur_time - start_t_][particle_no] = std::pow(mi_[cur_time - start_t_][particle_no],cur_beta);


  return cur_beta;
}
