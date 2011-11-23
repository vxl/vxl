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
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

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

#endif //multi_cache.h
