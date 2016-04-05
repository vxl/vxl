// This is brl/bseg/bvxm/pro/processes/bvxm_report_statistics_point_cloud_process.cxx
#include "bvxm_report_statistics_point_cloud_process.h"
//:
// \file
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vgl/vgl_point_3d.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

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

  // process takes 1 input, no outputs
  // input[0]: filename for input point cloud (x y z)
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  return pro.set_input_types(input_types_);
}

bool bvxm_report_statistics_point_cloud_process(bprb_func_process& pro)
{
  using namespace bvxm_report_statistics_point_cloud_process_globals;

  // check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< " but is " << pro.n_inputs() << std::endl;
    return false;
  }

  // get inputs
  unsigned i = 0;
  std::string point_filename = pro.get_input<std::string>(i++);
  std::ifstream ifile(point_filename.c_str());
  if (!ifile)
  {
    std::cout<<"Failed to open "<<point_filename<<std::endl;
    return false;
  }

  std::vector<vgl_point_3d<float> > point_cloud;
  float minx=1e20f,  miny=1e20f,  minz=1e20f,
        maxx=-1e20f, maxy=-1e20f, maxz=-1e20f;

  //: read the file to obtain min, max
  while (ifile)
  {
    float x,y,z;
    ifile>>x>>y>>z;
    vgl_point_3d<float> p3d(x,y,z);
    if (x<minx) minx=x;
    if (x>maxx) maxx=x;
    if (y<miny) miny=y;
    if (y>maxy) maxy=y;
    if (z<minz) minz=z;
    if (z>maxz) maxz=z;
    point_cloud.push_back(p3d);
  }
  std::cout<<"Min: ("<<minx<<','<<miny<<','<<minz<<")\n"
          <<"Max: ("<<maxx<<','<<maxy<<','<<maxz<<")\n"
          <<'('<<maxx-minx<<','<<maxy-miny<<','<<maxz-minz<<')'<<std::endl;

  return true;
}
