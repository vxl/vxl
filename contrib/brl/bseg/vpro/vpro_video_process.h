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
// \date
//   October 9, 2002    Initial version.
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vtol/vtol_topology_object.h>

class vpro_video_process : public vbl_ref_count
{
 public:
  enum process_data_type {NOTYPE=0, IMAGE, SPATIAL_OBJECT, TOPOLOGY};

  vpro_video_process();
  vpro_video_process(vpro_video_process const& p)
    : vbl_ref_count(),
      frame_index_(p.frame_index_), n_frames_(p.n_frames_),
      input_images_(p.input_images_),
      input_spat_objs_(p.input_spat_objs_),
      input_topo_objs_(p.input_topo_objs_),
      output_image_(p.output_image_),
      output_topo_objs_(p.output_topo_objs_),
      output_spat_objs_(p.output_spat_objs_) {}
  virtual ~vpro_video_process() {}
  void clear_input();
  void clear_output();

  
  void set_n_frames(int n_frames) { n_frames_ = n_frames; }
  void set_frame_index(int index) { frame_index_= index; }

  void add_input_image(vil1_image const& im) { input_images_.push_back(im); }

  void add_input_spatial_object(vsol_spatial_object_2d_sptr const& so);

  void add_input_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& spat_objs);

  void add_input_topology_object(vtol_topology_object_sptr const& to);

  void add_input_topology(vcl_vector<vtol_topology_object_sptr> const& topo_objs);

  int n_frames() { return n_frames_; }
  int frame_index() { return frame_index_; }
  int get_N_input_images() { return input_images_.size(); }
  vil1_image get_input_image(unsigned int i);
  vil1_image get_output_image() { return output_image_; }

  int get_N_input_spat_objs() { return input_spat_objs_.size(); }
  vcl_vector<vsol_spatial_object_2d_sptr> const& get_input_spatial_objects()
  { return input_spat_objs_; }

  int get_N_input_topo_objs() { return input_topo_objs_.size(); }
  vcl_vector<vtol_topology_object_sptr> const& get_input_topology()
  { return input_topo_objs_; }


  //:output handling may depend on the specific process
  virtual vcl_vector<vsol_spatial_object_2d_sptr> const& get_output_spatial_objects()
  { return output_spat_objs_; }

  virtual vcl_vector<vtol_topology_object_sptr> const & get_output_topology()
  { return output_topo_objs_; }

  //graph output for data display
  void set_graph_flag(){graph_flag_ = true;}
  void clear_graph_flag(){graph_flag_ = false;}
  bool graph_flag(){return graph_flag_;}
  void  set_graph(vcl_vector<float> const& graph){graph_=graph;}
  vcl_vector<float> graph(){return graph_;}
  //start and end of processed sequence
  unsigned int start_frame(){return start_frame_;}
  unsigned int end_frame(){return end_frame_;}

  virtual process_data_type get_input_type() { return NOTYPE; }
  virtual process_data_type get_output_type() { return NOTYPE; }
  virtual bool execute()=0;
  virtual bool finish()=0;
 protected:
  //members
  int frame_index_;
  int n_frames_;
  unsigned int start_frame_;//start of the processed sequence
  unsigned int end_frame_;  //end of the processed sequence
  vcl_vector<vil1_image> input_images_;
  vcl_vector<vsol_spatial_object_2d_sptr> input_spat_objs_;
  vcl_vector<vtol_topology_object_sptr> input_topo_objs_;
  vil1_image output_image_;
  vcl_vector<vtol_topology_object_sptr> output_topo_objs_;
  vcl_vector<vsol_spatial_object_2d_sptr> output_spat_objs_;
  bool graph_flag_;
  vcl_vector<float> graph_;//for histograms and other plots
};

#endif // vpro_video_process_h_
