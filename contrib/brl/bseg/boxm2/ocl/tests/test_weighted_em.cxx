//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <iostream>
#include <ios>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/algo/bsta_sigma_normalizer.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/ocl/boxm2_ocl_util.h>


void mog_samples(float* samps)
{
  //mu = .25, sigma = .06
  float mode1[] = {0.30379690014757355, 0.28398201060058043, 0.2943911897717762, 0.29554249051465387, 0.25036897662946456,
                   0.25549424398511705, 0.29891513517069201, 0.14853695403446637, 0.24113954487952871, 0.26341571068280933,
                   0.30260051941567706, 0.26134741888779478, 0.11880256142618917, 0.25101962163019631, 0.23151293297545156,
                   0.26720328010011379, 0.24732171438831776, 0.39769973987319168, 0.29549215792551908, 0.2358631231366328,
                   0.2139691167996427, 0.2823641459086581, 0.28999592189937706, 0.26198238775894345, 0.33805484983932677};

  //mu = .75, sigma = .1
  float mode2[] = {0.87325394637314291, 0.69515977265953222, 0.79767398608666218, 0.80051357134604817, 0.81414255593445839,
                   0.66897597424702882, 0.7406910756126307, 0.65699199445733247, 0.80719869327245219, 0.67310697233514216,
                   0.87185180539848361, 0.75278383121102233, 0.75820857425782995, 0.96191233327723635, 0.69855343912868184 };

  //mu = .5, sigma = .15
  float mode3[] = {0.67146615989813541, 0.60330803063835836, 0.46633873293634936, 0.42366551340449088, 0.4707333498526044,
                   0.74645874517355715, 0.47955437786233279, 0.37463417810951577, 0.38937639603429275, 0.48731303837214823};

  int c=0;
  for (int i=0; i<25; ++i,++c) samps[c] = mode1[i];
  for (int i=0; i<15; ++i,++c) samps[c] = mode2[i];
  for (int i=0; i<10; ++i,++c) samps[c] = mode3[i];
}

void test_weighted_em()
{
  //create the observations and their weights
  auto* obs = new float[50];
  mog_samples(obs);
  auto* vis = new float[50];
  for (int i=0; i<50; ++i)
    vis[i] = (float) 1.0f;

  //----- C++ Weighted EM -------------
  std::vector<float> visV, obsV;
  for (int i=0; i<50; ++ i) {
    visV.push_back(vis[i]);
    obsV.push_back(obs[i]);
  }
  bsta_sigma_normalizer_sptr n_table = new bsta_sigma_normalizer(0.2f, 40);
  vnl_vector_fixed<vxl_byte, 8> mog;
  boxm2_processor_type<BOXM2_MOG3_GREY>::type::compute_app_model(mog,obsV,visV,n_table,0.03f);

  std::cout<<"CPP MOG-------------:\n"
          <<"  mu0,sigma0,w0: "<<(float) mog[0]/255.0f<<", "<<(float) mog[1]/255.0f<<", "<<(float)mog[2]/255.0f<<std::endl
          <<"  mu1,sigma1,w1: "<<(float) mog[3]/255.0f<<", "<<(float) mog[4]/255.0f<<", "<<(float)mog[5]/255.0f<<std::endl
          <<"  mu2,sigma2,w2: "<<(float) mog[6]/255.0f<<", "<<(float) mog[7]/255.0f<<", "<<(float)(255 - mog[2] - mog[5])/255.0f<<std::endl;

  //----- OpenCL Weighted EM -------------
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)
    return;
  bocl_device_sptr device = mgr.gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "stat/weighted_em.cl");
  std::string opts = " -D SAMPLE_GLOBAL_FLOAT -D EM_WEIGHT_GLOBAL_FLOAT ";
  bocl_kernel em_test;
  em_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_weighted_em", opts, "test weighted em kernel");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  bocl_mem_sptr obsBuff = new bocl_mem( device->context(), obs, sizeof(float)*50, "input obs");
  obsBuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr visBuff = new bocl_mem( device->context(), vis, sizeof(float)*50, "vis buff");
  visBuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  cl_uint numSamps = 50;
  bocl_mem_sptr numSampsMem = new bocl_mem( device->context(), &numSamps, sizeof(cl_uint), "num samps buff");
  numSampsMem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vxl_byte mogBuff[8];
  bocl_mem_sptr mogMem = new bocl_mem( device->context(), mogBuff, sizeof(mogBuff), "mog3 buff");
  mogMem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float minSigma = .3f;
  bocl_mem_sptr minSigmaMem = new bocl_mem( device->context(), &minSigma, sizeof(float), "min sigma buff");
  minSigmaMem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set first kernel args
  em_test.set_arg( obsBuff.ptr() );
  em_test.set_arg( visBuff.ptr() );
  em_test.set_arg( numSampsMem.ptr() );
  em_test.set_arg( mogMem.ptr() );
  em_test.set_arg( minSigmaMem.ptr() );

  //set workspace
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {8, 8};

  ////execute kernel
  em_test.execute( queue, 2, lThreads, gThreads);
  clFinish( queue );
  mogMem->read_to_buffer(queue);

  //report
  std::cout<<"REAL MOG------------:\n"
          <<"  mu0,sigma0,w0: (.25, .06, .5)\n"
          <<"  mu0,sigma0,w0: (.75, .1,  .3)\n"
          <<"  mu0,sigma0,w0: (.5 , .15, .2)\n"
          <<"CPP MOG-------------:\n"
          <<"  mu0,sigma0,w0: "<<(float) mog[0]/255.0f<<", "<<(float) mog[1]/255.0f<<", "<<(float)mog[2]/255.0f<<std::endl
          <<"  mu1,sigma1,w1: "<<(float) mog[3]/255.0f<<", "<<(float) mog[4]/255.0f<<", "<<(float)mog[5]/255.0f<<std::endl
          <<"  mu2,sigma2,w2: "<<(float) mog[6]/255.0f<<", "<<(float) mog[7]/255.0f<<", "<<(float)(255 - mog[2] - mog[5])/255.0f<<std::endl
          <<"OPENCL MOG-------------:\n"
          <<"  mu0,sigma0,w0: "<<(float) mogBuff[0]/255.0f<<", "<<(float) mogBuff[1]/255.0f<<", "<<(float)mogBuff[2]/255.0f<<std::endl
          <<"  mu1,sigma1,w1: "<<(float) mogBuff[3]/255.0f<<", "<<(float) mogBuff[4]/255.0f<<", "<<(float)mogBuff[5]/255.0f<<std::endl
          <<"  mu2,sigma2,w2: "<<(float) mogBuff[6]/255.0f<<", "<<(float) mogBuff[7]/255.0f<<", "<<(float)(255 - mogBuff[2] - mogBuff[5])/255.0f<<std::endl;
}


TESTMAIN( test_weighted_em );
