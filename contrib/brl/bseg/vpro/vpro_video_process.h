// This is brl/bseg/vpro/vpro_video_process.h
#ifndef vpro_video_process_h_
#define vpro_video_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vpro_video_process
//
//  A generic video processor that is called from the live_video_manager
//  to carry out algorithms on the live video frames.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 9, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>

class vpro_video_process : public vbl_ref_count
{
 public:
  enum process_data_type {NOTYPE=0, IMAGE, SPATIAL_OBJECT, TOPOLOGY};

  vpro_video_process();
 ~vpro_video_process();
  void clear_input();
  void clear_output();
  void set_n_frames(int n_frames){n_frames_ = n_frames;}
  void set_frame_index(int index){frame_index_= index;}
  
  void add_input_image(vil1_image const& im) { input_images_.push_back(im); }

  void add_input_spatial_object(vsol_spatial_object_2d_sptr const& so);

  void add_input_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& spat_objs);

  void add_input_topology_object(vtol_topology_object_sptr const& to);

  void add_input_topology(vcl_vector<vtol_topology_object_sptr> const& topo_objes);
  int n_frames(){return n_frames_;}
  int frame_index(){return frame_index_;}
  int get_N_input_images() { return input_images_.size(); }
  vil1_image get_input_image(int i);
  vil1_image get_output_image() { return output_image_; }


  int get_N_input_spat_objs() { return input_spat_objs_.size(); }
  vcl_vector<vsol_spatial_object_2d_sptr> const&
      get_input_spatial_objects() { return input_spat_objs_; }
  
  int get_N_input_topo_objs() { return input_topo_objs_.size(); }
  vcl_vector<vtol_topology_object_sptr> const &
      get_input_topology() { return input_topo_objs_; }

  //:output handling may depend on the specific process
  virtual vcl_vector<vsol_spatial_object_2d_sptr> const& get_output_spatial_objects();
  virtual vcl_vector<vtol_topology_object_sptr> const & get_output_topology();

  virtual process_data_type get_input_type(){return NOTYPE;}
  virtual process_data_type get_output_type(){return NOTYPE;}
  virtual bool execute()=0;
  virtual bool finish()=0;
 protected:
  //members
  int frame_index_;
  int n_frames_;
  vcl_vector<vil1_image> input_images_;
  vcl_vector<vsol_spatial_object_2d_sptr> input_spat_objs_;
  vcl_vector<vtol_topology_object_sptr> input_topo_objs_;
  vil1_image output_image_;
  vcl_vector<vtol_topology_object_sptr> output_topo_objs_;
  vcl_vector<vsol_spatial_object_2d_sptr> output_spat_objs_;
};

#endif // vpro_video_process_h_
