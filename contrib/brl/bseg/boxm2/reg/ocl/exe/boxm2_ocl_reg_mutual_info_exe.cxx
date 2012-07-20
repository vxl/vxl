//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>
#include <boxm2/reg/ocl/boxm2_ocl_reg_minfo_func.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>

#include <vgl/vgl_vector_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

struct ltstr
{
  bool operator()(const vcl_pair<int,int> & s1, const vcl_pair<int,int> & s2) const
  {
    return  s1.first < s2.first
        || (s1.first == s2.first && s1.second < s2.second);
  }
};

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> sceneA_file("-sceneA", "sceneA filename", "");
  vul_arg<vcl_string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr sceneA = new boxm2_scene(sceneA_file());
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file());
  boxm2_lru_cache::create(sceneA);
  boxm2_cache_sptr cacheA =boxm2_cache::instance();
  vcl_vector<vcl_string> data_types;
  vcl_vector<vcl_string> identifiers;
  data_types.push_back("alpha");
  identifiers.push_back("");

  boxm2_stream_scene_cache cacheB( sceneB, data_types,identifiers);

  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  if (mgr->gpus_.size()==0)
    return false;

  bocl_device_sptr  device = mgr->gpus_[0];

  vcl_cout<<"# of "<<device->get_references()<<vcl_endl;
  boxm2_opencl_cache_sptr opencl_cacheA = new boxm2_opencl_cache(sceneA,device);

  boxm2_ocl_reg_minfo_func func(opencl_cacheA,cacheB,device,20);

#if 1
  vnl_powell powell(&func);
  vnl_vector<double> x(6,0.0);
  powell.set_x_tolerance(1e-1);
  powell.set_max_function_evals(10);
  vul_timer t ;
  t.mark();
  powell.minimize(x);

  vcl_cout<<"Time Taken is "<<t.all()<<vcl_endl;
  vgl_rotation_3d<double> r(x[3],x[4],x[5]);
  vcl_cout<<" B to A\n"
          <<"Translation is ("<<x[0]<<','<<x[1]<<','<<x[2]<<")\n"
          <<"Rotation is "<<r.as_matrix()<<'\n'

          <<"Initial Mutual Info "<<func.mutual_info(vgl_rotation_3d<double>(),vgl_vector_3d<double>())<<'\n'
          <<"Final Mutual Info "<<func.mutual_info(r,vgl_vector_3d<double>(x[0],x[1],x[2]))<<vcl_endl;
#endif

#if 0
  int numsamples  = 20;
  float var1_range  = 0.5;
  float var2_range  = 0.5;

  vil_image_view<float> mi(numsamples,numsamples);
  mi.fill(0.0);
  vnl_vector<double> x(6,0.0);

  float var1_inc = 2* var1_range / ( (float) numsamples );
  float var2_inc = 2* var2_range / ( (float) numsamples );
  for (unsigned i = 0 ; i <numsamples; i++)
  {
    vcl_cout<<'.';
    for (unsigned j = 0 ; j <numsamples; j++)
    {
      x[3] = -var1_range + (float)i * var1_inc;
      x[4] = -var2_range + (float)j * var2_inc;
      vgl_rotation_3d<double> r(x[3],x[4],x[5]);
      mi(i,j) = func.mutual_info(r,vgl_vector_3d<double>(x[0],x[1],x[2]));
    }
  }
  vil_save(mi,"e:/data/LIDAR/theta1theta2.tiff");
#endif
  return 0;
}
