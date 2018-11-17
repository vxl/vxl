//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
#ifndef boxm2_ocl_hierarchical_reg_h_
#define boxm2_ocl_hierarchical_reg_h_
//:
// \file
// \brief A hierarchical cost function for registering volumes using mutual information
// \author Vishal Jain
// \date March 9, 2012
//
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <bocl/bocl_device.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>


//: A cost function for registering video frames by minimizing square difference in intensities.
class boxm2_ocl_hierarchical_reg : public boxm2_ocl_reg_mutual_info
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  boxm2_ocl_hierarchical_reg( boxm2_opencl_cache_sptr  & cache,
                               boxm2_scene_sptr sceneA,
                               boxm2_scene_sptr sceneB,
                               const bocl_device_sptr& device, int nbins,
                               bool do_vary_scale);

  //: initialize the monte carlo reg function
  bool init(vnl_vector<double> const& mu, vnl_vector<double> const & cov);
  bool likelihood(int num_iter,int depth = 3);
  bool update();
  bool exhaustive();
  vnl_vector<double> max_sample();

 // void convert_to_xform(vnl_vector<double> & x);

protected:
  bool generate_samples(int samplenum = 100);
  int numsamples_;
  vnl_vector<double> mu_;
  vnl_vector<double> cov_;
  std::vector< vnl_vector<double> > samples_;
  std::vector< vnl_vector<double> > parent_samples_;
  std::vector< double > mis;
  std::vector<std::pair<int, double> > pdf_;
  std::vector<std::pair<int, double> > cdf_;

  bool do_vary_scale_;

  double mu_cost_;
};


#endif // boxm2_ocl_hierarchical_reg_h_
