//This is brl/bseg/bvxm/pro/processes/bvxm_report_statistics_point_cloud_process.cxx

//:
// \file
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>
#include <vgl/vgl_point_3d.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include "bvxm_report_statistics_point_cloud_process.h"

struct compare_point_3d
{
bool operator()(const vgl_point_3d<float> &a, vgl_point_3d<float>  &b) const
{
return a.z() > b.z();
}
};
bool bvxm_report_statistics_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_report_statistics_point_cloud_process_globals;

  //process takes 4inputs
  //input[0]: filename for input point cloud (x y z)
  //input[1]: filename for the voxel grid
  //input[2]: spacing in x-y
  //input[3]: spacing in z
  //input[4]: use opinion ?
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  //output has 1 output
  //output[0] : The updated probability map
  //output[1] : The mask of image pixels used in update
 return true;
}

bool bvxm_report_statistics_point_cloud_process(bprb_func_process& pro)
{
  using namespace bvxm_report_statistics_point_cloud_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  vcl_string point_filename = pro.get_input<vcl_string>(i++);
  vcl_ifstream ifile(point_filename.c_str());
  if(!ifile)
  {
    vcl_cout<<"Failed to open "<<point_filename<<vcl_endl;
    return false;
  }

  vcl_vector<vgl_point_3d<float> > point_cloud;
  float x,y,z;
  float minx=1e20;    float miny=1e20;     float minz=1e20;
  float maxx=-1e20;   float maxy=-1e20;    float maxz=-1e20;
  
  //: read the file to obtain min, max
  while(ifile)
  {
      
      ifile>>x>>y>>z;
      vgl_point_3d<float> p3d(x,y,z);
      if(x<minx)
          minx=x;
      if(x>maxx)
          maxx=x;
      if(y<miny)
          miny=y;
      if(y>maxy)
          maxy=y;
      if(z<minz)
          minz=z;
      if(z>maxz)
          maxz=z;
      point_cloud.push_back(p3d);
  }
  vcl_cout<<"Min: ("<<minx<<","<<miny<<","<<minz<<")"<<vcl_endl;
  vcl_cout<<"Max: ("<<maxx<<","<<maxy<<","<<maxz<<")"<<vcl_endl;
  vcl_cout<<"("<<maxx-minx<<","<<maxy-miny<<","<<maxz-minz<<")"<<vcl_endl;

  return true;
}
