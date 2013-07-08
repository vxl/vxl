#include "boxm2_ocl_monte_carlo_reg.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>

//#define COARSE 
boxm2_ocl_monte_carlo_reg::boxm2_ocl_monte_carlo_reg( boxm2_opencl_cache2_sptr& cacheA,
                                                    boxm2_stream_scene_cache& cacheB,
                                                    bocl_device_sptr device,
                                                    int nbins,double scale, int numsamples):  boxm2_ocl_reg_mutual_info(cacheA,cacheB,device,nbins, scale)
{
    numsamples_ = numsamples;
    

}

/*
bool boxm2_ocl_monte_carlo_reg::init(vnl_vector<double> const& mu, vnl_vector<double> const & cov)
{
    mu_ = mu ; 
    cov_= cov;
    mu_cost_ = this->mutual_info(mu_);
    mis.resize(numsamples_,0.0);
    vcl_cout<<"# of Samples generated "<<numsamples_<<vcl_endl;
    vnl_random rand;
    for(unsigned i = 0 ; i < numsamples_; i++)
    {
        vnl_vector<double> sample( mu_.size() ) ;
        for(unsigned j = 0 ; j < sample.size(); j++)
            sample[j] = mu_[j] + (2*rand.drand32()-1)*cov_[j]; 
        samples_.push_back(sample) ; 
    }

    //cov_[0] = 25;
    //cov_[1] = 25;
    //cov_[2] = 25;
    //cov_[3] = 0.15;
    //cov_[4] = 0.15;
    //cov_[5] = 0.15;
    
    return true;
}*/
bool boxm2_ocl_monte_carlo_reg::init(vnl_vector<double> const& mu, vnl_vector<double> const & cov)
{
    mu_ = mu ; 
    cov_= cov;
    mu_cost_ = this->mutual_info(mu_);
    mis.resize(numsamples_,0.0);
    vcl_cout<<"# of Samples generated "<<numsamples_<<vcl_endl;
    vnl_random rand;
    for(unsigned i = 0 ; i < numsamples_; i++)
    {
        vnl_vector<double> sample( mu_.size() ) ;
        for(unsigned j = 0 ; j < sample.size(); j++)
            sample[j] = mu_[j] + (2*rand.drand32()-1)*cov_[j]; 
        samples_.push_back(sample) ; 
    }

    //cov_[0] = 25;
    //cov_[1] = 25;
    //cov_[2] = 25;
    //cov_[3] = 0.15;
    //cov_[4] = 0.15;
    //cov_[5] = 0.15;
    
    return true;
}
bool boxm2_ocl_monte_carlo_reg::sample_from_cdf(int depth)
{
    vcl_vector< vnl_vector<double> > samples;
    vnl_random rand_params;
    
    vcl_vector<bool> hasretained(numsamples_,false);
    vcl_vector<double> mis_new(mis.size(),0.0);
    for(unsigned i = 0 ; i < samples_.size() ; i++)
    {
        double particle = (double)i/(double)numsamples_;

        if(pdf_[i].second > (double)1/(double)numsamples_)
        {
            mis_new[i] = mis[i];
            continue;
        }
        vcl_cout<<"Particle #"<<i<<" "<<mis[i]<<" "<<pdf_[i].second<<" "<<cdf_[i].second<<" ";
        for(unsigned j = 0; j<cdf_.size(); j++)
        {
            if(cdf_[j].second > particle )
            {
                vnl_vector<double> sample( mu_.size() ) ;
                for(unsigned k = 0 ; k < sample.size();)
                {
                    double offset = (2*rand_params.drand32()-1)*cov_[k];
                    sample[k] = samples_[j][k] + offset; 
                    k++;
                }
                double minfo = this->mutual_info(sample, depth) ;
                vcl_cout<<particle<<" "<<mis[j]<<" "<<minfo<<" "<<sample-mu_;
                if(minfo > mis_new[j])
                {
                    if(minfo > mis[j]  )
                    {
                        samples_[i] = sample ; 
                        mis_new[i] = minfo;
                    }
                    else 
                    {
                        samples_[i] = samples_[j] ;
                        mis_new[i] = mis[j] ;
                    }
                }
                vcl_cout<<" "<<mis_new[i]<<vcl_endl;
                break;
            }
        }
    }
    mis = mis_new;
    return true;
}
bool boxm2_ocl_monte_carlo_reg::exhaustive(int depth )
{
    samples_.clear();

    unsigned int numsamples = 64; //4096
    //unsigned int numsamples = 4096;
    mis.clear();
    vcl_map<double,vnl_vector<double> > samples_sorted;
    for(unsigned i = 0 ; i < numsamples; i++)
    {
        vnl_vector<double> sample( mu_.size() ) ;
        for(unsigned k = 0 ; k < sample.size(); k++)
        {
#ifdef COARSE
            unsigned int var = i >> (2*k);
            var = var & 3;
            double offset = (0.5*(double)var-0.75)*cov_[k];

#else 
            unsigned int var = i >> (k);
            var = var & 1;
            double offset = ((double)var-0.5)*cov_[k];
#endif
            sample[k] = mu_[k] + offset; 
        }
        vcl_cout<<sample-mu_<<" "<<i<<vcl_endl;
        double minfo = this->mutual_info(sample, 1) ;
        vcl_cout<<minfo<<vcl_endl;
        mis.push_back(minfo);
        samples_.push_back(sample);
        samples_sorted[minfo] = sample;
    }
    vcl_cout<<"Mutual Infor for Max Sample"<<this->max_sample()-mu_<<" is "<<this->mutual_info(this->max_sample(),1)<<vcl_endl;

    //: Pick n1 best samples ( 16 in this case )
    int count =0;
    //int n1 = 16;
    int n1 = 4;
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


bool boxm2_ocl_monte_carlo_reg::likelihood(int num_iter, int depth  )
{
    pdf_.clear();
    cdf_.clear();
    double sum = 0;
    vcl_vector<double> zs;
    
    for(unsigned i = 0 ; i < numsamples_ ; i++)
    {
        zs.push_back(samples_[i][0]-mu_[0]) ;
        double minfo = vcl_pow(mis[i],(double) num_iter+1);
        sum+= minfo;
        pdf_.push_back(vcl_make_pair(i,minfo));
        cdf_.push_back(vcl_make_pair(i,sum));

    }
    vcl_cout<<vcl_endl;
    vcl_cout<<"[[";
    for(unsigned i = 0 ; i < numsamples_ ; i++)
    {
        pdf_[i].second /= sum;
        cdf_[i].second /= sum; 
        vcl_cout<<cdf_[i].second<<" "<<pdf_[i].second<<" "<<zs[i]<<" ";
    }
    vcl_cout<<"]]"<<vcl_endl;
    return true;
}

bool boxm2_ocl_monte_carlo_reg::update()
{
    vcl_cout<<"Covariance ";
    for(unsigned k = 0 ; k < cov_.size(); k++)
     {
         cov_[k]*=0.5;
         vcl_cout<<cov_[k]<<" ";
    }
    vcl_cout<<vcl_endl;
    return true;
}

vnl_vector<double> boxm2_ocl_monte_carlo_reg::expected_sample()
{
    vnl_vector<double> exp( mu_.size(),0.0);
    for(unsigned i = 0 ; i < samples_.size(); i++)
        exp = exp + pdf_[i].second * (samples_[i] ); 
    return exp;
}
vnl_vector<double> boxm2_ocl_monte_carlo_reg::max_sample()
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

bool boxm2_ocl_monte_carlo_reg::run_annealing()
{
    int num_iter = 4;
    for(unsigned k = 3 ; k < 4; k++)
    {
        for(unsigned i = 0 ; i < samples_.size(); i++)
        {
                mis[i] = this->mutual_info(samples_[i],k);

        }
        vcl_cout<<vcl_endl;
        this->likelihood(0,k);
        //vcl_cout<<"Max Sample at depth "<<k<<" is "<<this->mutual_info(this->max_sample(),k)<<vcl_endl;
        for(unsigned i = 0 ; i < num_iter; i++)
        {
            vcl_cout<<"======================================="<<vcl_endl;
            vcl_cout<<"Iteration # "<<i<<" at depth "<<k<<vcl_endl;
            this->sample_from_cdf(k);
            this->likelihood(i,k);
            vcl_cout<<"Max Sample "<<this->max_sample()-mu_<<" minfo is "<<this->mutual_info(this->max_sample(),k)<<vcl_endl;
            vcl_cout<<"Expected Sample "<<this->expected_sample()-mu_<<" minfo is "<<this->mutual_info(this->expected_sample(),k)<<vcl_endl;
        }
        this->update();
    }
    return true;
}
