#ifndef boxm2_multi_cache_h
#define boxm2_multi_cache_h
//:
// \file
// \brief boxm2_opencl_scene_streamer assists the processor in streaming blocks
#include <boxm2/boxm2_scene.h>
#include <vcl_vector.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

//open cl includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_cache: public vbl_ref_count
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  public:
    boxm2_multi_cache( boxm2_scene_sptr              scene,
                       vcl_vector<bocl_device*> &    devices );
    ~boxm2_multi_cache();     
    
    vcl_vector<boxm2_opencl_cache*>& ocl_caches() { return ocl_caches_; }
    vcl_vector<boxm2_scene_sptr>&    sub_scenes() { return sub_scenes_; }
    boxm2_scene_sptr                 get_scene()  { return scene_; }
    vcl_string to_string();  
    
    //: return a vector of sub scene pointers in visibility order from this camera
    vcl_vector<boxm2_opencl_cache*> get_vis_order_from_pt(vgl_point_3d<double> const& pt);
    vcl_vector<boxm2_opencl_cache*> get_vis_sub_scenes(vpgl_perspective_camera<double>* cam);

  private:

    boxm2_scene_sptr                    scene_; 
  
    //: scene this cache is operating on
    vcl_vector<boxm2_scene_sptr>        sub_scenes_;

    //: list of regular opencl caches
    vcl_vector<boxm2_opencl_cache*>     ocl_caches_; 
};

//: utility class for sorting id's by their distance
class boxm2_dist_cache_pair
{
  public:
    boxm2_dist_cache_pair(double dist, boxm2_opencl_cache* c) : dist_(dist), cache_(c) {}
    double              dist_;
    boxm2_opencl_cache* cache_;
    inline bool operator < (boxm2_dist_cache_pair const& v) const {
      return dist_ < v.dist_;
    }
};

vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache& cache); 

#endif //multi_cache.h
