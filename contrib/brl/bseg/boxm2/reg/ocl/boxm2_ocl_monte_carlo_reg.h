#ifndef boxm2_ocl_monte_carlo_reg_h_
#define boxm2_ocl_monte_carlo_reg_h_
//:
// \file
// \brief A cost function for registering volumes using mutual information
// \author Vishal Jain
// \date March 9, 2012
//
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache2.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache2.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>


//: A cost function for registering video frames by minimizing square difference in intensities.
class boxm2_ocl_monte_carlo_reg : public boxm2_ocl_reg_mutual_info
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_monte_carlo_reg( boxm2_opencl_cache2_sptr  & cacheA,
                             boxm2_stream_scene_cache & cacheB,
                             bocl_device_sptr device, int nbins,
                             double scale = 1.0, 
                             int numsamples = 100);

  //: initialize the monte carlo reg function
  bool init(vnl_vector<double> const& mu, vnl_vector<double> const & cov);
  bool likelihood(int num_iter,int depth = 3);
  bool sample_from_cdf(int depth );
  bool update();
  bool exhaustive(int depth =3 );
  ////: run simulated annealing
  bool run_annealing(); 
  vnl_vector<double> expected_sample();
  vnl_vector<double> max_sample();
 
protected:
  bool generate_samples(int samplenum = 100);
  int numsamples_;
  vnl_vector<double> mu_;
  vnl_vector<double> cov_;
  vcl_vector< vnl_vector<double> > samples_;
  vcl_vector< vnl_vector<double> > parent_samples_;
  vcl_vector< double > mis;
  vcl_vector<vcl_pair<int, double> > pdf_;
  vcl_vector<vcl_pair<int, double> > cdf_;


  double mu_cost_;
};


#endif // boxm2_ocl_monte_carlo_reg_h_
