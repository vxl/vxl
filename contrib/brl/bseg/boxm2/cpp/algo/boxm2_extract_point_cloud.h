#ifndef boxm2_extract_point_cloud_h_included_
#define boxm2_extract_point_cloud_h_included_
//:
// \file
// \brief  A method for extracting a point cloud from a boxm2 scene. The points correspond to the cell centroids of leaf cells in the scene.
//         There is a probability threshold on cells to save for convenience. The points are saved in the BOXM2_POINT datatype.
//
// \author Ali Osman Ulusoy
// \author Daniel Crispell: ported heavy lifting from process to this class (25 June 2015)
// \date Mar 21, 2011


#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

class boxm2_extract_point_cloud
{
  public:
    static bool extract_point_cloud(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache,
                                    float prob_thresh, unsigned int depth);

};

#endif
