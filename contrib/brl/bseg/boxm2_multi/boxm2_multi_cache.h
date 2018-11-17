#ifndef boxm2_multi_cache_h
#define boxm2_multi_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks

#include <vector>
#include <iosfwd>
#include <iostream>
#include <algorithm>
#include "boxm2_multi_cache_group.h"
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <vpgl/vpgl_generic_camera.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>

//vgl includes
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_3d.h>

// other includes
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class boxm2_multi_cache;
typedef vbl_smart_ptr<boxm2_multi_cache> boxm2_multi_cache_sptr;
//: example realization of abstract cache class; holds an opencl cache for each device
class boxm2_multi_cache: public vbl_ref_count
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    boxm2_multi_cache( const boxm2_scene_sptr&               scene,
                 const std::vector<bocl_device_sptr> & devices );
    ~boxm2_multi_cache() override;

    std::vector<boxm2_opencl_cache1*>& ocl_caches() { return ocl_caches_; }
    std::vector<boxm2_scene_sptr>&    sub_scenes() { return sub_scenes_; }
    boxm2_scene_sptr                 get_scene()  { return scene_; }
    std::string to_string();

    //clear all OCL caches
    void clear();

    //get group order
    std::vector<boxm2_multi_cache_group*> get_vis_groups(const vpgl_camera_double_sptr& cam);
    std::vector<boxm2_multi_cache_group*> group_order_from_pt(vgl_point_3d<double> const& pt,
                                                             vgl_box_2d<double> const& camBox);

    //: return a vector of sub scene pointers in visibility order from this camera
    std::vector<boxm2_opencl_cache1*> get_vis_order_from_pt(vgl_point_3d<double> const& pt);
    std::vector<boxm2_opencl_cache1*> get_vis_sub_scenes(vpgl_perspective_camera<double>* cam);
    std::vector<boxm2_opencl_cache1*> get_vis_sub_scenes(vpgl_generic_camera<double>* cam);
    std::vector<boxm2_opencl_cache1*> get_vis_sub_scenes(const vpgl_camera_double_sptr& cam);

  private:
    boxm2_scene_sptr                     scene_;

    //: scene this cache is operating on
    std::vector<boxm2_scene_sptr>         sub_scenes_;

    //: list of regular opencl caches
    std::vector<boxm2_opencl_cache1*>      ocl_caches_;

    //: list of block groups
    std::vector<boxm2_multi_cache_group*> groups_;
};

std::ostream& operator<<(std::ostream &s, boxm2_multi_cache& cache);


#endif // boxm2_multi_cache_h
