#include <iostream>
#include <cmath>
#include <vcl_where_root_dir.h>
#include <testlib/testlib_test.h>
#include <vnl/algo/vnl_cholesky.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/tests/boxm2_ocl_test_utils.h>

#include <brad/brad_phongs_model_est.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_identity_3x3.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vul/vul_timer.h>

static void create_test_data(vnl_vector<double> & samples,
                             vnl_vector<double> & samples_weights,
                             vnl_vector<double> & camera_elev,
                             vnl_vector<double> & camera_azim,
                             double sun_elev,
                             double sun_phi,
                             unsigned int num_samples=30)
{
  // first define model parameters
  double kd=0.5;
  double ks=0.5;
  double gamma=6;
  vnl_double_3 normal(std::sin(0.75)*std::cos(0.0),
                      std::sin(0.75)*std::sin(0.0),
                      std::cos(0.75));

  vnl_double_3 lv(std::sin(sun_elev)*std::cos(sun_phi),
                  std::sin(sun_elev)*std::sin(sun_phi),
                  std::cos(sun_elev));
  vnl_identity_3x3 I;
  vnl_double_3 rlv=(I-outer_product<double>(normal,normal)-outer_product<double>(normal,normal))*lv;

  // vary the viewpoint direction to obtain the samples

  vnl_random rand(9667566);
  for (unsigned i=0; i < num_samples; ++i)
  {
    double elev = rand.drand32(vnl_math::pi/6,vnl_math::pi/3);
    double azim = rand.drand32(vnl_math::twopi);
    vnl_double_3 vv(std::sin(elev)*std::cos(azim),
                    std::sin(elev)*std::sin(azim),
                    std::cos(elev));
    double obs = kd * dot_product(lv,normal) + ks * std::pow(dot_product<double>(vv,rlv),gamma);
    samples[i]=obs;
    camera_elev[i]=elev;
    camera_azim[i]=azim;

    samples_weights[i]=1.0;
    samples[i]=obs;
  }
}

// Test the sun_pos class
static void lm_phongs_model(vnl_vector<double> & x,
                            double sun_elev,
                            double sun_phi,
                            vnl_vector<double> & samples,
                            vnl_vector<double> & samples_weights,
                            vnl_vector<double> & camera_elev,
                            vnl_vector<double> & camera_azim
                           )
{
#if 0
  //add noise
  for (unsigned i=0; i < num_samples; ++i)
    samples[i]+=rand.drand32(-0.1,0.1);
#endif
  // estimate the model from the samples.
  brad_phongs_model_est f(sun_elev,
                          sun_phi,
                          camera_elev,
                          camera_azim,
                          samples,
                          samples_weights,
                          true);

  vnl_levenberg_marquardt lm(f);

  lm.set_verbose(true);
  lm.minimize(x);

#if 0
  std::cout<<"\nSolution: "
          <<x[0]<<','<<x[1]<<','
          <<x[2]<<','<<x[3]<<','<<x[4] <<'\n';
#endif
}

static void ocl_phongs_model(vnl_vector<float> & x,
                             double sun_elev,
                             double sun_phi,
                             vnl_vector<double> & samples,
                             vnl_vector<double> & samples_weights,
                             vnl_vector<double> & camera_elev,
                             vnl_vector<double> & camera_azim
                            )
{
  int n = x.size();
  int m = samples.size();
  vnl_vector<float> obs(m);
  vnl_vector<float> weights(m);
  vnl_vector<float> viewdirs(m*3);
  vnl_vector<float> sun_angles(2);
  sun_angles[0] = sun_elev;
  sun_angles[1] = sun_phi;
  for (int i=0; i<m; ++i)
  {
    obs[i] = samples[i];
    weights[i] = samples_weights[i];
    viewdirs[i*3+0] = std::sin(camera_elev[i])*std::cos(camera_azim[i]);
    viewdirs[i*3+1] = std::sin(camera_elev[i])*std::sin(camera_azim[i]);
    viewdirs[i*3+2] = std::cos(camera_elev[i]);
  }

  vnl_vector<float> y(m);
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)   return;
  bocl_device_sptr device = mgr.gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "onl/cholesky_decomposition.cl");
  src_paths.push_back(source_dir + "onl/phongs_model.cl");
  src_paths.push_back(source_dir + "onl/levenberg_marquardt.cl");
  src_paths.push_back(source_dir + "onl/test_levenberg_marquardt.cl");

  bocl_kernel lm_test;
  lm_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_ocl_phongs_model_est", "-D PHONGS", "test levenberg marquardt");
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  bocl_mem_sptr xbuff = new bocl_mem( device->context(), x.data_block(), x.size()*sizeof(float), "x vector");
  xbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr ybuff = new bocl_mem( device->context(), y.data_block(), y.size()*sizeof(float), "y vector");
  ybuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr mbuff = new bocl_mem( device->context(), &m, sizeof(int), "dimension of x");
  mbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr nbuff = new bocl_mem( device->context(), &n, sizeof(int), "dimension of y");
  nbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int maxiter = 100;
  bocl_mem_sptr maxiterbuff = new bocl_mem( device->context(), &maxiter, sizeof(int), "Maximum # of iterations");
  maxiterbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float output[1000];
  bocl_mem_sptr outputbuff = new bocl_mem( device->context(), output, 1000* sizeof(float), "Vector for Output");
  outputbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr obsbuff = new bocl_mem( device->context(), obs.data_block(), obs.size()* sizeof(float), "Vector for Obs");
  obsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr weightsbuff = new bocl_mem( device->context(), weights.data_block(), weights.size()* sizeof(float), "Vector for Weights");
  weightsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr sun_anglesbuff = new bocl_mem( device->context(), sun_angles.data_block(), 2* sizeof(float), "Vector for Weights");
  sun_anglesbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr viewdirsbuff = new bocl_mem( device->context(), viewdirs.data_block(), viewdirs.size()* sizeof(float), "Vector for Weights");
  viewdirsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set workspace
  std::size_t lThreads[] = {64};
  std::size_t gThreads[] = {64*1000};
  //set first kernel args
  lm_test.set_arg( maxiterbuff.ptr() );
  lm_test.set_arg( mbuff.ptr() );
  lm_test.set_arg( xbuff.ptr() );
  lm_test.set_arg( outputbuff.ptr() );
  lm_test.set_arg( obsbuff.ptr() );
  lm_test.set_arg( weightsbuff.ptr() );
  lm_test.set_arg( viewdirsbuff.ptr() );
  lm_test.set_arg( sun_anglesbuff.ptr() );
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( x.size()*y.size()*sizeof(float) );   //local tree,
  lm_test.set_local_arg( x.size()*x.size()*sizeof(float) );   //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );            //local tree,
  //execute kernel

  //for (unsigned int k=0; k<100; ++k)
  float time = lm_test.execute( queue, 1, lThreads, gThreads);
  clFinish( queue );
  //xbuff->read_to_buffer(queue);
  std::cout<<"Solution (OCL): "<<time <<' ';
  for (int i=0; i<n; ++i)
    std::cout<<x[i]<<' ';
  std::cout<<std::endl;
}

static void test_ocl_lm_phongs_model()
{
  int m = 30;
  double sun_elev = 0.325398;
  double sun_phi = -3.69;
  vnl_vector<float> xinit(5);
  xinit[0]=0.25;
  xinit[1]=0.5;
  xinit[2]=6.0;
  xinit[3]=0.75;
  xinit[4]=0.0;
  vnl_vector<double> xinit_d(5);
  for (unsigned k = 0; k < xinit.size(); ++k)
    xinit_d[k] = xinit[k];

  vnl_vector<double>  samples_weights(m);
  vnl_vector<double>  samples(m);
  vnl_vector<double>  camera_elev(m);
  vnl_vector<double>  camera_azim(m);

  create_test_data(samples,
                   samples_weights,
                   camera_elev,
                   camera_azim,
                   sun_elev,
                   sun_phi,
                   m);
  vul_timer t;
  t.mark();

  ocl_phongs_model(xinit, sun_elev,sun_phi,
                   samples,samples_weights,
                   camera_elev, camera_azim);

  t.all();

  std::cout<<"Time taken ofr OCL "<<t.all()<<std::endl;
  t.mark();
  for (int k = 0; k < 1000; ++k)
  {
    xinit_d[0]=0.125;
    xinit_d[1]=0.5;
    xinit_d[2]=6.0;
    xinit_d[3]=0.75;
    xinit_d[4]=0.0;
    lm_phongs_model(xinit_d, sun_elev,sun_phi,
                    samples,samples_weights,
                    camera_elev, camera_azim);
  }
  t.all();
  std::cout<<"Time taken ofr VNL "<<t.all()<<std::endl;
}

TESTMAIN( test_ocl_lm_phongs_model );
