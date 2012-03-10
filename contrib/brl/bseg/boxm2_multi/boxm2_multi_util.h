#ifndef boxm2_multi_util_h
#define boxm2_multi_util_h
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

//: boxm2_multi_util - example realization of abstract cache class
class boxm2_multi_util
{
  public:

    // Helper to pick out data type
    static bool get_scene_appearances( boxm2_scene_sptr    scene,
                                       vcl_string&         data_type,
                                       vcl_string&         options,
                                       int&                apptypesize);
};

//: Pair class, to help sorting by distance
template<class T>
class boxm2_dist_pair
{
  public:
    double dist_;
    T dat_;
    boxm2_dist_pair(double dist, T dat) : dist_(dist), dat_(dat) {}
    inline bool operator < (boxm2_dist_pair const& v) const {
      return dist_ < v.dist_;
    }
};

#endif // boxm2_multi_util_h
