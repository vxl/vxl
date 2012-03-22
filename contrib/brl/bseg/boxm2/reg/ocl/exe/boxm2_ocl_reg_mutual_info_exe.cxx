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
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_powell.h>

#include <vcl_iostream.h>
struct ltstr
{
  bool operator()(const vcl_pair<int,int> & s1, const vcl_pair<int,int> & s2) const
  {
	if(s1.first < s2.first) 
		return true;

	if(s1.first == s2.first)
	{
		if(s1.second < s2.second ) 
			return true;
	}
	return false;
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

  vnl_powell powell(&func);
  vnl_vector<double> x(6,0.0); 
  vul_timer t ;
  t.mark();
  powell.minimize(x);
  vcl_cout<<"Solution is "<< x<<vcl_endl;

  vcl_cout<<"Time Taken is "<<t.all()<<vcl_endl;
  vgl_rotation_3d<double> r(x[3],x[4],x[5]);

  vcl_cout<<"Translation is ("<<x[0]<<","<<x[1]<<","<<x[2]<<")"<<vcl_endl;
  vcl_cout<<"Rotation is "<<r.angle()<<" angles about "<<r.axis()<<vcl_endl;

  vcl_cout<<"Initial Mutual Info "<<func.mutual_info(vgl_rotation_3d<double>(),vgl_vector_3d<double>())<<vcl_endl;
  vcl_cout<<"Final Mutual Info "<<func.mutual_info(r,vgl_vector_3d<double>(x[0],x[1],x[2]))<<vcl_endl;

  return 0;
}
