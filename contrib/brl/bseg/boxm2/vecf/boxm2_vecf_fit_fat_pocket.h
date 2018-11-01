// This is core/vgl/algo/boxm2_vecf_fit_fat_pocket.h
#ifndef boxm2_vecf_fit_fat_pocket_h_
#define boxm2_vecf_fit_fat_pocket_h_
//:
// \file
// \brief Estimates the fat pocket parameters to match a point cloud of the skin
// \author Joseph L. Mundy
// \date February 25, 2016
//
#include <vector>
#include <string>
#include <iosfwd>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include "boxm2_vecf_middle_fat_pocket_params.h"
#include "boxm2_vecf_middle_fat_pocket.h"
#include <bvgl/bvgl_knn_index_3d.h>
#include <vnl/vnl_least_squares_function.h>

class boxm2_vecf_fit_fat_pocket
{
  // Data Members--------------------------------------------------------------
 public:
  vgl_vector_3d<double> trans_;
  boxm2_vecf_middle_fat_pocket_params initial_pr_;
  boxm2_vecf_middle_fat_pocket_params fpr_;
  boxm2_vecf_middle_fat_pocket_params defpr_;
  bool is_right_;
  bvgl_knn_index_3d<double> neutral_face_;
  bvgl_knn_index_3d<double> deformed_face_;
  bvgl_knn_index_3d<double> skin_layer_;
  boxm2_vecf_middle_fat_pocket pocket_;

  // Constructors/Initializers/Destructors-------------------------------------

 boxm2_vecf_fit_fat_pocket():is_right_(false){}

  boxm2_vecf_fit_fat_pocket(std::string const& neutral_face_ptset_path, std::string const& skin_layer_path, std::string const& fat_pocket_geo_path);
  boxm2_vecf_fit_fat_pocket(std::string const& neutral_face_ptset_path, std::string const& deformed_face_ptset_path, std::string const& skin_ptset_path,  std::string const& fat_pocket_geo_path);
  ~boxm2_vecf_fit_fat_pocket() = default;

  // Operations---------------------------------------------------------------

  //: clear internal data
  void clear();

  //: find the parameters that adjust the shape of the fat pocket model to match the neutral face
  // returns the average distance from the data points to the fat_pocket model
  // error conditions are reported on outstream
  double fit_neutral(std::ostream* outstream=nullptr, bool verbose=false);
  double fit_deformed(std::ostream* outstream=nullptr, bool verbose=false);

// Data Access---------------------------------------------------------------

  boxm2_vecf_middle_fat_pocket_params neutral_middle_fat_pocket_parameters() const{return fpr_;}
  boxm2_vecf_middle_fat_pocket_params deformed_middle_fat_pocket_parameters() const{return defpr_;}

  // data ploting for debug purposes
  bool plot_middle_fat_pocket_fit(std::ostream& ostr);
};
#endif // boxm2_vecf_fit_fat_pocket_h_
