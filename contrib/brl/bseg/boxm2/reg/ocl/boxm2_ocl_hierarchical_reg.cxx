#include "boxm2_ocl_hierarchical_reg.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>
boxm2_ocl_hierarchical_reg::boxm2_ocl_hierarchical_reg( boxm2_opencl_cache2_sptr& cacheA,
                                                    boxm2_stream_scene_cache& cacheB,
                                                    bocl_device_sptr device,
                                                    int nbins,bool iscoarse, double scale, int numsamples):  boxm2_ocl_reg_mutual_info(cacheA,cacheB,device,nbins, scale), iscoarse_(iscoarse)
{
    numsamples_ = numsamples;
}

bool boxm2_ocl_hierarchical_reg::init(vnl_vector<double> const& mu, vnl_vector<double> const & cov)
{
    mu_ = mu ; 
    cov_= cov;
    mu_cost_ = this->mutual_info(mu_);
    mis.resize(numsamples_,0.0);
    vcl_cout<<"Cost of current poistion ois "<<mu_cost_<<vcl_endl;
    vcl_cout<<"# of Samples generated "<<numsamples_<<vcl_endl;
    vnl_random rand;
    for(unsigned i = 0 ; i < numsamples_; i++)
    {
        vnl_vector<double> sample( mu_.size() ) ;
        for(unsigned j = 0 ; j < sample.size(); j++)
            sample[j] = mu_[j] + (2*rand.drand32()-1)*cov_[j];
        samples_.push_back(sample) ; 
    }   
    return true;
}

bool boxm2_ocl_hierarchical_reg::exhaustive(int depth )
{
    samples_.clear();
    unsigned int numsamples = 4096;
    int n1 = 16;
    if( !iscoarse_)
    {
        numsamples = 64;
        n1 = 4;
    }
    mis.clear();
    vcl_map<double,vnl_vector<double> > samples_sorted;
    for(unsigned i = 0 ; i < numsamples; i++)
    {
        vnl_vector<double> sample( mu_.size() ) ;
        for(unsigned k = 0 ; k < sample.size(); k++)
        {
            unsigned int var = i >> (2*k);
            var = var & 3;
            double offset = (0.5*(double)var-0.75)*cov_[k];
            if(!iscoarse_)
            {
                var = i >> (k);
                var = var & 1;
                offset = ((double)var-0.5)*cov_[k];
            }
            sample[k] = mu_[k] + offset; 
        }
        double minfo = this->mutual_info(sample, 1) ;
        vcl_cout<<sample-mu_<<" "<<i<<vcl_endl;
        vcl_cout<<minfo<<vcl_endl;
        mis.push_back(minfo);
        samples_.push_back(sample);
        samples_sorted[minfo] = sample;
    }
    vcl_cout<<"Mutual Infor for Max Sample"<<this->max_sample()-mu_<<" is "<<this->mutual_info(this->max_sample(),1)<<vcl_endl;
    //: Pick n1 best samples ( 16 in this case )
    int count =0;
    numsamples = 64;
    mis.clear();
    samples_.clear();
    for(vcl_map<double,vnl_vector<double> >::reverse_iterator iter = samples_sorted.rbegin(); iter!=samples_sorted.rend() && count <n1; iter++, count++)
    {
        for(unsigned i = 0 ; i < numsamples; i++)
        {
            vnl_vector<double> sample( mu_.size() ) ;
            for(unsigned k = 0 ; k < sample.size(); k++)
            {
                unsigned int var = i >> (k);
                var = var & 1;
                double offset = ((double)var-0.5)*cov_[k]/4;
                sample[k] = iter->second[k] + offset; 
            }
            double minfo = this->mutual_info(sample, 2) ;
            vcl_cout<<"\nIteration 2 "<<sample-mu_<<" "<<minfo<<" "<<i+count*numsamples<<" "<<iter->first<<" "<<iter->second-mu_<<vcl_endl;
            mis.push_back(minfo);
            samples_.push_back(sample);
        }
    }
    samples_sorted.clear();
    for(unsigned i = 0 ; i <mis.size(); i++)
        samples_sorted[mis[i]]=samples_[i];
    count =0;
    numsamples = 64;
    mis.clear();
    samples_.clear();
    for(vcl_map<double,vnl_vector<double> >::reverse_iterator iter = samples_sorted.rbegin(); iter!=samples_sorted.rend() && count <n1; iter++, count++)
    {
        for(unsigned i = 0 ; i < numsamples; i++)
        {
            vnl_vector<double> sample( mu_.size() ) ;
            for(unsigned k = 0 ; k < sample.size(); k++)
            {
                unsigned int var = i >> (k);
                var = var & 1;
                double offset = ((double)var-0.5)*cov_[k]/8;
                sample[k] = iter->second[k] + offset; 
            }
            double minfo = this->mutual_info(sample, 3) ;
            vcl_cout<<"\nIteration 3 "<<sample-mu_<<" "<<minfo<<" "<<i+count*numsamples<<" "<<iter->first<<" "<<iter->second-mu_<<vcl_endl;
            mis.push_back(minfo);
            samples_.push_back(sample);
        }
    }
    vcl_cout<<"Mutual Infor for Max Sample"<<this->max_sample()-mu_<<" is "<<this->mutual_info(this->max_sample(),3)<<vcl_endl;
    return true;
}
vnl_vector<double> boxm2_ocl_hierarchical_reg::max_sample()
{
    vnl_vector<double> max_sample( mu_.size(),0.0);
    double max_pdf = 0.0;
    for(unsigned i = 0 ; i < mis.size(); i++)
    { 
        if(mis[i] > max_pdf)
        {
            max_pdf = mis[i];
            max_sample = samples_[i]; 
        }
    }
    return max_sample;
}


