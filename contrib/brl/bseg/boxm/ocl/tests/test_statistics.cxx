#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "stat_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_stat_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/algo/bsta_adaptive_updater.h>
static bool near_eq(float x, float y, float tol)
{ return vcl_fabs(x - y) <= tol;}

template <class T>
static void test_gaussian_pdf(stat_test_driver<T>& driver)
{
  boxm_stat_manager<T>::instance()->set_gauss_1d(0.5f, 1.0f);
  vcl_vector<float> data(1); data[0]=0.1f;
  driver.setup_data(data);
  driver.setup_result_data(1);
   
  if (driver.create_kernel("test_gaussian_pdf")!=SDK_SUCCESS) {
    TEST("Create Kernel test_gaussian", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_gaussian", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {
    float test[]={0.36827f};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && near_eq(results[i],test[i], 0.0001f);
    TEST("test_gaussian", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i++)
        vcl_cout << "test_gaussian_result["<< i <<"](" << results[i] << ")\n";
  }
  driver.clean_io_data();
  driver.release_kernel();
}
template <class T>
static void test_gaussian_3_mixture_pdf(stat_test_driver<T>& driver)
{
  boxm_stat_manager<T>::instance()->
    set_gauss_3_mixture_1d(0.5f, 0.05f, 1.0f/3.0f, 
                           0.25f, 0.01f, 1.0f/3.0f, 
                           0.75f, 0.2f, 1.0f/3.0f);

  vcl_vector<float> data(1); data[0]=0.5f;
  driver.setup_data(data);

  driver.setup_result_data(3);
  
  if (driver.create_kernel("test_gaussian_3_mixture_pdf")!=SDK_SUCCESS) {
    TEST("Create Kernel test_gaussian_3_mixture", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_gaussian_3_mixture", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {
    float test[]={2.96403f, 3.98942f, 7.97885f};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && near_eq(results[i],test[i], 0.0001f);
    TEST("test_gaussian_3_mixture", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i++)
        vcl_cout << "test_gaussian_3_mixture_result["<< i <<"](" << results[i] << ")\n";
  }
  driver.clean_io_data();
  driver.release_kernel();
}
template <class T>
static void test_update_gauss(stat_test_driver<T>& driver)
{
  float mu_init = 0.0f, sigma_init = 0.5f, min_sigma = 0.05f, rho = 0.1f;
  boxm_stat_manager<T>::instance()->
    set_init_gauss_1d(mu_init, sigma_init, min_sigma, rho);

  // Samples from a Gaussian
  unsigned nsamp = 20;
  //
  
  float samples[] = {0.509497f, 0.271204f, 0.519466f, 0.669653f, 0.292289f, 
                     0.534911f, 0.706098f, 0.468127f, 0.493323f, 0.57566f, 
                     0.500368f, 0.434049f, 0.414085f, 0.482841f, 0.560543f, 
                     0.536531f, 0.48683f, 0.510946f, 0.552359f, 0.60396f};
  vcl_vector<float> data(nsamp);
  for(unsigned i = 0; i<nsamp; ++i)
    data[i]=samples[i];// data size 
  driver.setup_data(data);
  driver.setup_result_data(2*nsamp);
  
  if (driver.create_kernel("test_update_gauss")!=SDK_SUCCESS) {
    TEST("Create Kernel test_update_gauss", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_update_gauss", false, true);
    return;
  }
  bool good = true;

  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();
  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {
    // comparison with bsta
    bsta_gaussian_sphere<float, 1> bgauss(mu_init, sigma_init*sigma_init);
		for (vcl_size_t i= 0; i<nsamp; i++){
      bsta_update_gaussian(bgauss, rho, data[i], min_sigma*min_sigma);
#if 0
      vcl_cout << i << ' ' << bgauss.mean() << ' ' << vcl_sqrt(bgauss.var())
               << '\n';
#endif
		  good = good && near_eq(results[2*i], bgauss.mean(), 0.0001f) &&
        near_eq(results[2*i+1], vcl_sqrt(bgauss.var()), 0.001f);
    }            
    TEST("test_update_gaussian", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<2*nsamp; i+=2)
        vcl_cout << "test_update_gauss["<< i <<"](" << results[i] << ' ' << results[i+1] << ")\n";
  }
  driver.clean_io_data();
  driver.release_kernel();
}
template <class T>
static void test_mixture_sort(stat_test_driver<T>& driver)
{
  boxm_stat_manager<T>::instance()->
    set_gauss_3_mixture_1d(0.75f, 0.2f, 1.0f/3.0f, 
                           0.25f, 0.01f, 1.0f/3.0f,
                           0.5f, 0.05f, 1.0f/3.0f
                           );
  driver.setup_result_data(9);

  if (driver.create_kernel("test_mixture_sort")!=SDK_SUCCESS) {
    TEST("Create Kernel test_mixture_sort", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_mixture_sort", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {
    float test[]={0.25f,
                  0.01f,
                  0.333333f,
                  0.5f,
                  0.05f,
                  0.333333f,
                  0.75f,
                  0.2f,
                  0.333333f};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && near_eq(results[i],test[i], 0.0001f);
    TEST("test_mixture_sort", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i++)
        vcl_cout << "test_mixture_sort["<< i <<"](" << results[i] << ")\n";
  }
  driver.clean_io_data();
  driver.release_kernel();
}

template <class T>
static void test_insert(stat_test_driver<T>& driver)
{

  boxm_stat_manager<T>::instance()->
    set_insert_gauss_3_mix_1d(0.2f, 0.25f, 
                              0.25f, 0.01f, 1.0f/3.0f,
                              0.5f, 0.05f, 1.0f/3.0f,
                              0.75f, 0.2f, 1.0f/3.0f 
                              );
  vcl_vector<float> data(1); data[0]=0.28f;
  driver.setup_data(data);

  driver.setup_result_data(39);

  if (driver.create_kernel("test_insert")!=SDK_SUCCESS) {
    TEST("Create Kernel test_insert", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_insert", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {

    float test[]={0.25f,0.01f,0.4f,3.0f,0.5f,0.05f,0.4f,4.0f,0.28f,0.25f,
                  0.2f,1.0f,2.0f,0.25f,0.01f,0.8f,3.0f,0.28f,0.25f,0.2f,1.0f,
                  0.28f,0.25f,0.0f,0.0f,1.0f,0.28f,0.25f,1.0f,1.0f,0.28f,
                  0.25f,0.0f,0.0f,0.28f,0.25f,0.0f,0.0f,0.0f};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && near_eq(results[i],test[i], 0.0001f);
    TEST("test_insert", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i++)
        vcl_cout << "test_insert["<< i <<"](" << results[i] << ")\n";
  }
  driver.release_kernel();
}
static void print_mix(unsigned b, float* results)
{
  float mu0=results[b],sigma0=results[b+1],
    w0=results[b+2],Nobs0=results[b+3];
  float mu1=results[b+4],sigma1=results[b+5],
    w1=results[b+6],Nobs1=results[b+7];
  float mu2=results[b+8],sigma2=results[b+9],
    w2=results[b+10],Nobs2=results[b+11];
  float Nobs_mix = results[b+12];
  unsigned ncomp = 0; 
  if(w0>0.0f) ncomp++;
  if(w1>0.0f) ncomp++;
  if(w2>0.0f) ncomp++;
  if(ncomp == 0)
    vcl_cout << "no components \n";
  if(ncomp == 1)
    vcl_cout <<mu0 << ' ' <<sigma0*sigma0 << ' ' << w0 << ' ' 
             << Nobs0 << '\n' << Nobs_mix << '\n';
  if(ncomp == 2){
    vcl_cout <<mu0 << ' ' <<sigma0*sigma0 << ' ' << w0 << ' ' 
             << Nobs0 << '\n';
    vcl_cout <<mu1 << ' ' <<sigma1*sigma1 << ' ' << w1 << ' ' 
             << Nobs1 << '\n' << Nobs_mix << '\n';
  }
  if(ncomp == 3){
    vcl_cout <<mu0 << ' ' <<sigma0*sigma0 << ' ' << w0 << ' ' 
             << Nobs0 << '\n';
    vcl_cout <<mu1 << ' ' <<sigma1*sigma1 << ' ' << w1 << ' ' 
             << Nobs1 << '\n';
    vcl_cout <<mu2 << ' ' <<sigma2*sigma2 << ' ' << w2 << ' ' 
             << Nobs2 << '\n' << Nobs_mix << '\n';
  } 
}
static bool mix_eq(bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3> > const& mix, unsigned b, float* results, float tol = 0.001)
{
  float mu0=results[b],sigma0=results[b+1],w0=results[b+2],Nobs0=results[b+3];
  float mu1=results[b+4],sigma1=results[b+5],w1=results[b+6],Nobs1=results[b+7];
  float mu2=results[b+8],sigma2=results[b+9],w2=results[b+10],Nobs2=results[b+11];
  float Nobs_mix = results[b+12];
  unsigned ncomp = 0; 
  if(w0>0.0f) ncomp++;
  if(w1>0.0f) ncomp++;
  if(w2>0.0f) ncomp++;  
  if(ncomp!=mix.num_components())
    return false;
  if(mix.num_observations != Nobs_mix)
    return false;
  if(ncomp >=1){
    float vw0 = mix.weight(0);
    float vmu0 = mix.distribution(0).mean();
    float vsigma0 = vcl_sqrt(mix.distribution(0).var());
    short vNobs0 = static_cast<short>(mix.distribution(0).num_observations);
    bool good = near_eq(mu0, vmu0, tol);
    good = good && near_eq(sigma0, vsigma0, tol);
    good = good && near_eq(w0, vw0, tol);
    good = good && vNobs0 == static_cast<short>(Nobs0);
    if(!good)
      return false;
  }
  if(ncomp >=2){
    float vw1 = mix.weight(1);
    float vmu1 = mix.distribution(1).mean();
    float vsigma1 = vcl_sqrt(mix.distribution(1).var());
    short vNobs1 = static_cast<short>(mix.distribution(1).num_observations);
    bool good = near_eq(mu1, vmu1, tol);
    good = good && near_eq(sigma1, vsigma1, tol);
    good = good && near_eq(w1, vw1, tol);
    good = good && vNobs1 == static_cast<short>(Nobs1);
    if(!good)
      return false;
  }
  if(ncomp ==3){
    float vw2 = mix.weight(2);
    float vmu2 = mix.distribution(2).mean();
    float vsigma2 = vcl_sqrt(mix.distribution(2).var());
    short vNobs2 = static_cast<short>(mix.distribution(2).num_observations);
    bool good = near_eq(mu2, vmu2, tol);
    good = good && near_eq(sigma2, vsigma2, tol);
    good = good && near_eq(w2, vw2, tol);
    good = good && vNobs2 == (short)Nobs2;
    if(!good)
      return false;
  }
 return true;
}
template <class T>
static void test_update_mix(stat_test_driver<T>& driver)
{
  float mu_init = 0.0f, sigma_init = 0.1f, min_sigma = 0.05f,
    t_match = 1.0f, w = 1.0f;
  float mu0 = 0.0f, sigma0 = 0.0f, w0 = 0.0f;
  float mu1 = 0.0f, sigma1 = 0.0f, w1 = 0.0f;
  float mu2 = 0.0f, sigma2 = 0.0f, w2 = 0.0f;
  boxm_stat_manager<T>::instance()->
    set_update_gauss_3_mix_1d(w, sigma_init, min_sigma, t_match,
                              mu0, sigma0, w0,
                              mu1, sigma1, w1,
                              mu2, sigma2, w2
                              );
  unsigned nsamp = 30;
  float samples[] = {0.242128f, 0.48103f, 0.445362f, 0.257038f, 0.492382f,
                     1.07666f, 0.249464f, 0.554577f, 0.916411f, 0.241236f,
                     0.443768f, 0.585215f, 0.244899f, 0.517885f, 0.981157f,
                     0.255286f, 0.53242f, 0.728973f, 0.243507f, 0.535631f,
                     0.832079f, 0.260184f,0.552204f, 0.843705f, 0.253314f,
                     0.508654f, 0.634243f, 0.236118f, 0.522571f, 0.650016f};
  
  vcl_vector<float> data(nsamp);
  for(unsigned i = 0; i<nsamp; ++i)
    data[i]=samples[i];
  driver.setup_data(data);

  driver.setup_result_data(13*30);

  bsta_gauss_f1 this_gauss(0.0f, sigma_init*sigma_init);
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type, 3> > mix_gauss_nobs;
  // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(this_gauss, 3 ,
                                                      t_match, min_sigma);
  mix_gauss_nobs mix;

  if (driver.create_kernel("test_update_mix")!=SDK_SUCCESS) {
    TEST("Create Kernel test_update_mix", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_update_mix", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
  if (results) {
    for(unsigned i =0; i<size; i+=13){
      unsigned j=i/13;
      updater(mix, samples[j], 1.0f);
      good = good && mix_eq(mix, i, results);
    }      
    TEST("test_update_mix", good, true);
    if (!good)
      for(unsigned i =0; i<size; i+=13)
        print_mix(i, results);
  }
  driver.release_kernel();
}

template <class T>
void stat_tests(stat_test_driver<T>& test_driver)
{
  boxm_stat_manager<T>* stat_mgr = boxm_stat_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  if (!stat_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/statistics_library_functions.cl"))
    return;
  if (!stat_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/stat_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_gaussian_pdf(test_driver);
  test_gaussian_3_mixture_pdf(test_driver);
  test_update_gauss(test_driver);
  test_mixture_sort(test_driver);
  test_insert(test_driver);
  test_update_mix(test_driver);

  //==============================================================
  //END STAT  TESTS
  test_driver.cleanup_stat_test();
}

static void test_statistics()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  stat_test_driver<float > test_driver;
  boxm_stat_manager<float >* stat_mgr = boxm_stat_manager<float >::instance();
  good = test_driver.init();
  if(good)stat_tests(test_driver);
  else{ TEST("stat_test_driver", true, false); }
}

TESTMAIN(test_statistics);
