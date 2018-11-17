#include <iostream>
#include <algorithm>
#include "boxm2_ocl_hierarchical_points_to_volume_reg.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
boxm2_ocl_hierarchical_points_to_volume_reg::boxm2_ocl_hierarchical_points_to_volume_reg( boxm2_opencl_cache_sptr  & cache,
                                                   float *pts,
                                                   boxm2_scene_sptr sceneB,
                                                   int npts,
                                                   const bocl_device_sptr& device, bool do_vary_scale):  boxm2_ocl_reg_points_to_volume_mutual_info(cache,pts,sceneB,device,npts, do_vary_scale), do_vary_scale_(do_vary_scale)
{

}

bool boxm2_ocl_hierarchical_points_to_volume_reg::init(vnl_vector<double> const& mu, vnl_vector<double> const & cov)
{
    mu_ = mu ;
    cov_= cov;
    mu_cost_ = this->cost(mu_);
    std::cout<<"Mutual Information for current position is "<<mu_cost_<<std::endl;
    return true;
}

bool boxm2_ocl_hierarchical_points_to_volume_reg::exhaustive()
{
    samples_.clear();
    int nlevels = 4;
    unsigned int numsamples_search_width[4] = {3,3,3,3}; // should be odd
    unsigned int numbestparticales[4] = {1,1,1,1};
    int params_to_vary = mu_.size() ;
    if( !do_vary_scale_ )
        params_to_vary -=1;
    std::map<double,vnl_vector<double> > samples_sorted;
    vnl_vector<double> cov = cov_;
    samples_sorted[mu_cost_] =mu_;
    for (unsigned int level = 0; level <nlevels; level++)
    {
        std::cout<<"Level #"<<level<<std::endl;
        mis.clear();
        samples_.clear();
        auto iter = samples_sorted.rbegin();
        for(unsigned int j = 0 ; j < numbestparticales[level] && iter!=samples_sorted.rend(); j++,iter++)
        {
            std::cout<<".";
            std::cout.flush();
            int searchwidth = numsamples_search_width[level];
            vnl_vector<double> best_sample = iter->second;
            int numsamples_per_best_particle = (int) std::pow((float)searchwidth,(float)params_to_vary);
            for(unsigned int sampleno = 0 ; sampleno < numsamples_per_best_particle; sampleno++)
            {
                int cont=sampleno;
                vnl_vector<double> curr_sample = best_sample;
                for(unsigned k = 0; k <params_to_vary; k++)
                {
                    int t = params_to_vary - k - 1;
                    unsigned int var = cont/(unsigned int)std::pow((float)searchwidth,(float)t); // quotient
                    cont = cont - (unsigned int)std::pow((float)searchwidth,(float)t)*var;       // remainder
                    double offset = (2*((double)var/((double)searchwidth-1))-1)*cov[t];
                    curr_sample[t] +=  offset;
                }
                double minfo = this->cost(curr_sample, level) ;
                mis.push_back(minfo);
                samples_.push_back(curr_sample);
            }
        }
        std::cout<<std::endl;
        samples_sorted.clear();
        for(unsigned i = 0 ; i <mis.size(); i++)
            samples_sorted[mis[i]]=samples_[i];
        std::cout<<"Mutual Infor for Max Sample"<<this->max_sample()-mu_<<" is "<<this->cost(this->max_sample(),level)<<std::endl;
        cov = cov_/std::pow((float)2,(float)level+1);
    }
    return true;
}
vnl_vector<double> boxm2_ocl_hierarchical_points_to_volume_reg::max_sample()
{
    vnl_vector<double> max_sample( mu_.size(),0.0);
    double max_pdf = 0.0;
    for(unsigned i = 0 ; i < mis.size(); i++)
        if(mis[i] > max_pdf)
        {
            max_pdf = mis[i];
            max_sample = samples_[i];
        }
   return max_sample;
}
