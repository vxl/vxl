#include "boxm2_ocl_monte_carlo_reg.h"
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <vcl_where_root_dir.h>
#include <vnl/vnl_random.h>

boxm2_ocl_monte_carlo_reg::boxm2_ocl_monte_carlo_reg( boxm2_opencl_cache_sptr& cacheA,
                                                    boxm2_stream_scene_cache& cacheB,
                                                    bocl_device_sptr device,
                                                    int nbins,double scale, int numsamples):  boxm2_ocl_reg_mutual_info(cacheA,cacheB,device,nbins, scale)
{
    numsamples_ = numsamples;


}


bool boxm2_ocl_monte_carlo_reg::init(vnl_vector<double> const& mu, vnl_vector<double> const & cov)
{
    mu_ = mu ; 
    cov_= cov;
    mu_cost_ = this->mutual_info(mu_);
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

bool boxm2_ocl_monte_carlo_reg::sample_from_cdf()
{
    vcl_vector< vnl_vector<double> > samples;
    vnl_random rand_samples( 120202 );
    vnl_random rand_params;

    for(unsigned i = 0 ; i < samples_.size() ; i++)
    {
        double particle = rand_samples.drand32();
        for(unsigned j = 0; j<cdf_.size(); j++)
        {
            if(cdf_[j].second > particle )
            {
                vnl_vector<double> sample( mu_.size() ) ;
                for(unsigned k = 0 ; k < sample.size(); k++)
                    sample[k] = samples_[i][k] + (2*rand_params.drand32()-1)*cov_[k];
                samples.push_back(sample);
                break;
            }
        }
    }

    samples_ = samples;
    return true;
}
bool boxm2_ocl_monte_carlo_reg::likelihood(int num_iter, int depth  )
{
    pdf_.clear();
    cdf_.clear();
    double sum = 0;
    vcl_cout<<"Cost of Initial Position "<<mu_cost_<<vcl_endl;
    for(unsigned i = 0 ; i < numsamples_ ; i++)
    {
        double minfo = this->mutual_info(samples_[i], depth) ;
        minfo = vcl_pow(minfo,(double) num_iter);
        sum+= minfo;
        //if( minfo > mu_cost_ ) 
        //    vcl_cout<<"Greater than "<<samples_[i]-mu_<<" "<<minfo<<vcl_endl;

        //if( i % 10 == 0)
        //    vcl_cout<<i<<" "<<minfo<<vcl_endl;
        pdf_.push_back(vcl_make_pair(i,minfo));
        cdf_.push_back(vcl_make_pair(i,sum));
    }

    for(unsigned i = 0 ; i < numsamples_ ; i++)
    {
        pdf_[i].second /= sum;
        cdf_[i].second /= sum; // vcl_make_pair(i,pdf_[i].second/sum));
        //vcl_cout<<cdf_[i].second<<vcl_endl;
    }
    return true;
}

bool boxm2_ocl_monte_carlo_reg::update()
{
    for(unsigned k = 0 ; k < cov_.size(); k++)
        cov_[k]/=2.0;
 
    return true;
}

vnl_vector<double> boxm2_ocl_monte_carlo_reg::expected_sample()
{
    vnl_vector<double> exp( mu_.size(),0.0);
    for(unsigned i = 0 ; i < samples_.size(); i++)
        exp = exp + pdf_[i].second * (samples_[i] ); 

    return exp;
}
bool boxm2_ocl_monte_carlo_reg::run_annealing()
{
    int num_iter = 9;
    int k = 0;
    //for(unsigned k = 3 ; k >= 0 ; k--)
    for(unsigned i = 0 ; i < num_iter; i++)
    {
        if ( i > 0 && i %3 == 0 )
            k ++;
        vcl_cout<<"======================================="<<vcl_endl;
        vcl_cout<<"Iteration # "<<i<<vcl_endl;
        this->likelihood(i,k);


        vcl_cout<<"Expected Sample "<<this->expected_sample()-mu_<<" minfo is "<<this->mutual_info(this->expected_sample())<<vcl_endl;
        this->update();
        this->sample_from_cdf();

    }



    return true;
}
