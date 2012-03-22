#ifndef boxm2_reg_mutual_info_h
#define boxm2_reg_mutual_info_h
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>

#include <bocl/bocl_device.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vgl/algo/vgl_rotation_3d.h>
bool boxm2_ocl_register_world(boxm2_opencl_cache_sptr& cacheA, 
							  boxm2_stream_scene_cache& cacheB,
							   vgl_rotation_3d<double>  rot,
							  vgl_vector_3d<double> tx,
							  int nbins,
							  bocl_device_sptr device,
							  float & mi);
#endif // boxm2_reg_mutual_info_h
	