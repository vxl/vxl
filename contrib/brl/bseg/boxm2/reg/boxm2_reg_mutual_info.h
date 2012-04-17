#ifndef boxm2_reg_mutual_info_h
#define boxm2_reg_mutual_info_h
//:
// \file
#include <boxm2/io/boxm2_cache.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>

//returns mutual information given this
double boxm2_reg_mutual_info(boxm2_cache_sptr& cacheA,
                             boxm2_cache_sptr& cacheB,
                             const vgl_vector_3d<int>& trans);

void align_samples(boxm2_cache_sptr& cacheA,
                   boxm2_cache_sptr& cacheB,
                   const vgl_box_3d<double>& roi,
                   vil_image_view<double>& A,
                   vil_image_view<double>& B);

float register_world(boxm2_cache_sptr& cacheA,
                     boxm2_scene_sptr& sceneB,
                     vgl_vector_3d<double> tx);

#endif // boxm2_reg_mutual_info_h
