// This is brl/bseg/bvpl/bvpl_octree/bvpl_global_corners.h
#ifndef bvpl_global_corners_h
#define bvpl_global_corners_h
//:
// \file
// \brief A class to compute corner features across different boxm_scenes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  21-Jul-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene.h>
#include <bvpl_octree/bvpl_global_taylor.h>

class bvpl_global_corners : public vbl_ref_count
{

  typedef vbl_smart_ptr<bvpl_global_taylor<double, 10> > bvpl_global_taylor_sptr;

  public:

  //: Constructor  from xml file
  bvpl_global_corners(const std::string &path);

  //: Compute Harris' measure extension to 3-d as proposed by:
  //  I. Laptev. On space-time interest points. Int. J. Computer Vision, 64(2):107--123, 2005
  void compute_laptev_corners(const bvpl_global_taylor_sptr& global_taylor, int scene_id, int block_i, int block_j, int block_k);

  //: Compute corner measure extension to 3-d as proposed by:
  // P. Beaudet, Rotationally invariant image operators, in Proc. 4th Int. Joint Conf. Patt. Recog. 1978.
  void compute_beaudet_corners(const bvpl_global_taylor_sptr& global_taylor,int scene_id, int block_i, int block_j, int block_k);

  //: Threshold corners below a threshold
  void threshold_laptev_corners(const bvpl_global_taylor_sptr& global_taylor,int scene_id, float thresh, const std::string& output_path);

  //: Take a histogram of corners and get different values for percentage of thresholds
  void explore_corner_statistics(const bvpl_global_taylor_sptr& global_taylor,int scene_id);

  //: Write to global_corners_info.xml
  void xml_write();

  // Accessors
  std::string path_out() {return path_out_;}
  std::vector<std::string> aux_dirs() { return aux_dirs_; }
  std::string aux_dirs( unsigned i ) { return aux_dirs_[i]; }
  std::vector<double> cell_lengths() { return finest_cell_length_; }

  boxm_scene_base_sptr load_corner_scene (int scene_id);
  boxm_scene_base_sptr load_valid_scene (int scene_id);

  std::string xml_path() { return path_out_ + "/global_corners_info.xml"; }

 protected:

  //: A vector to hold paths to keep taylor coefficient scenes and valid scenes
  std::vector<std::string> aux_dirs_;
  //: A vector to hold the finest cell length of the corresponding scene scene
  std::vector<double> finest_cell_length_;
  //: Path to xml info file
  std::string path_out_;
  //: The value of the parameter k for harris corners
  float harris_k_;
};

typedef vbl_smart_ptr<bvpl_global_corners > bvpl_global_corners_sptr;


#endif // bvpl_global_corners_h
