// This is brl/bseg/brec/brec_part_hierarchy.h
#ifndef brec_part_hierarchy_h_
#define brec_part_hierarchy_h_
//:
// \file
// \brief class to represent a hierarchy of composable parts for recognition
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <iostream>
#include <bgrl2/bgrl2_graph.h>
#include "brec_part_base.h"
#include "brec_hierarchy_edge.h"
#include "brec_part_base_sptr.h"
#include "brec_hierarchy_edge_sptr.h"

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

class brec_part_hierarchy : public bgrl2_graph<brec_part_base , brec_hierarchy_edge>
{
 public:

  brec_part_base_sptr get_node(unsigned layer, unsigned type);
  brec_part_instance_sptr get_node_instance(unsigned layer, unsigned type);

  static void generate_map(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, vil_image_view<unsigned>& type_map);
  static void generate_map(std::vector<brec_part_instance_sptr>& extracted_parts, std::vector<std::vector<brec_part_instance_sptr> >& map);
  //: generate a float map with normalized strengths and receptive fields marked
  static void generate_output_map(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map);

  //: generate a float map with various posterior
  static void generate_output_map_posterior(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, unsigned type = brec_posterior_types::CLASS_FOREGROUND);
  static void generate_output_map_posterior_centers(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, unsigned type = brec_posterior_types::CLASS_FOREGROUND);

  //: stretch the values to be used for imaging
  static void generate_output_map3(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<float>& map);

  //: output_img needs to have 3 planes
  static void generate_output_img(std::vector<brec_part_instance_sptr>& extracted_parts, vil_image_view<vxl_byte>& input_img, vil_image_view<vxl_byte>& output_img, unsigned posterior_type);

  //: check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
  brec_part_instance_sptr exists(const brec_part_base_sptr& upper_p, const brec_part_instance_sptr& central_p, vil_image_view<float>& map, vil_image_view<unsigned>& type_map, std::vector<std::vector<brec_part_instance_sptr> >& part_map, float det_threshold);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  void extract_upper_layer(std::vector<brec_part_instance_sptr>& extracted_parts,
                           unsigned ni, unsigned nj, std::vector<brec_part_instance_sptr>& extracted_upper_parts);

  void add_dummy_primitive_instance(brec_part_instance_sptr p) { dummy_primitive_instances_.push_back(p); }
  std::vector<brec_part_instance_sptr>& get_dummy_primitive_instances() { return dummy_primitive_instances_; }

#if 0 // Ozge TODO: adapt to different posterior types
  //: compute the probabilistic existence score for a given part (upper_p) if this one or equivalent ones exist in this hierarchy
  //  Equivalence is determined by the types of the primitive layer parts
  //  (i.e. all layer 1 parts with alpha and alpha_prime as primitive parts are equivalent)
  bool get_score(brec_part_instance_sptr upper_p, std::vector<double>& scores);
  bool get_score_helper(brec_part_instance_sptr ins_p, brec_part_base_sptr p, double& score);
#endif

  unsigned highest_layer_id();
  //: number of vertices in the layer
  unsigned layer_cnt(unsigned layer);

  //: name will be used to create training directories
  void set_name(std::string name) { name_ = name; }
  std::string name() const { return name_; }

  void set_model_dir(std::string dir) { model_dir_ = dir; }
  std::string model_dir() const { return model_dir_; }

  void write_xml(std::ostream& os);
  bool read_xml(std::istream& is);

  //: draw a ps image with sampled parts, draw N samples from the distributions
  bool draw_to_ps(unsigned N, const std::string& output_img, float drawing_radius);

  //: draw the nodes of the given layer side by side to the output image
  bool draw_to_image(unsigned N, unsigned layer_id, float drawing_radius, const std::string& output_img);

 public:
  //: a map to store dummy instances of primitive parts, so that they could be extracted properly for a constructed hierarchy
  std::vector<brec_part_instance_sptr> dummy_primitive_instances_;

  std::string name_;

  //: after constructing the background and foreground response models for the primitive instances, the name of the directory to load these models should be saved in the hierarchy
  std::string model_dir_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & os, brec_part_hierarchy const &ph);
void vsl_b_read(vsl_b_istream & is, brec_part_hierarchy &ph);
void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy* ph);
void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy* &ph);

#endif  //brec_part_hierarchy_h_
