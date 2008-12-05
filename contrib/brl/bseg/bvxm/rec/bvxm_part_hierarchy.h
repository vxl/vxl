// This is brl/bseg/bvxm/rec/bvxm_part_hierarchy.h
#ifndef bvxm_part_hierarchy_h_
#define bvxm_part_hierarchy_h_
//:
// \file
// \brief class to represent a hiearachy of composable parts for recognition
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bgrl2/bgrl2_graph.h>
#include <rec/bvxm_part_base.h>
#include <rec/bvxm_hierarchy_edge.h>
#include <rec/bvxm_part_base_sptr.h>
#include <rec/bvxm_hierarchy_edge_sptr.h>

#include <vil/vil_image_view.h>
#include <vcl_iostream.h>

class bvxm_part_hierarchy : public bgrl2_graph<bvxm_part_base , bvxm_hierarchy_edge>
{
 public:

  bvxm_part_base_sptr get_node(unsigned layer, unsigned type);

  static void generate_map(vcl_vector<bvxm_part_instance_sptr>& extracted_parts, vil_image_view<float>& map, vil_image_view<unsigned>& type_map);
  static void generate_map(vcl_vector<bvxm_part_instance_sptr>& extracted_parts, vcl_vector<vcl_vector<bvxm_part_instance_sptr> >& map);
  //: generate a float map with normalized strengths and receptive fields marked
  static void generate_output_map(vcl_vector<bvxm_part_instance_sptr>& extracted_parts, vil_image_view<float>& map);
  //: output_img needs to have 3 planes
  static void generate_output_img(vcl_vector<bvxm_part_instance_sptr>& extracted_parts, vil_image_view<vxl_byte>& input_img, vil_image_view<vxl_byte>& output_img);

  // check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
  bvxm_part_instance_sptr exists(bvxm_part_base_sptr upper_p, bvxm_part_instance_sptr central_p, vil_image_view<float>& map, vil_image_view<unsigned>& type_map, vcl_vector<vcl_vector<bvxm_part_instance_sptr> >& part_map, float det_threshold);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  void extract_upper_layer(vcl_vector<bvxm_part_instance_sptr>& extracted_parts,
                           unsigned ni, unsigned nj, vcl_vector<bvxm_part_instance_sptr>& extracted_upper_parts);

  void add_dummy_primitive_instance(bvxm_part_instance_sptr p) { dummy_primitive_instances_.push_back(p); }
  vcl_vector<bvxm_part_instance_sptr>& get_dummy_primitive_instances() { return dummy_primitive_instances_; }

  unsigned highest_layer_id();

  void write_xml(vcl_ostream& os);
  bool read_xml(vcl_istream& is);

  //: a map to store dummy instances of primitive parts, so that they could be extracted properly for a constructed hierarchy
  vcl_vector<bvxm_part_instance_sptr> dummy_primitive_instances_;
};

#endif  //bvxm_part_hierarchy_h_
