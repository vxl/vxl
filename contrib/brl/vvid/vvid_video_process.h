//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_video_process_h_
#define vvid_video_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief live vvid_video_process
//   A generic video processor that is called from the live_video_manager
//   to carry out algorithms on the live video frames.
//   
// \endbrief
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 9, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vbl/vbl_ref_count.h>
#include <vtol/vtol_topology_object_sptr.h>

class vvid_video_process : public vbl_ref_count
{
public:
  enum output_type {NOTYPE=0, IMAGE, TOPOLOGY};
  vvid_video_process();
  ~vvid_video_process();
  void clear_input(){input_images_.clear();}
  void add_input_image(vil_image&  im){input_images_.push_back(im);}
  int get_N_inputs(){return input_images_.size();}
  vil_image get_input_image(int i);
  vil_image get_output_image(){return output_image_;}
  vcl_vector<vtol_topology_object_sptr> const & get_segmentation();
  virtual output_type get_output_type(){return NOTYPE;}
  virtual bool execute()=0;  
  virtual bool finish()=0;  
protected: 
  //members
  vcl_vector<vil_image> input_images_;
  vil_image output_image_;
  vcl_vector<vtol_topology_object_sptr> topo_objs_;
};
 

#endif // vvid_video_process_h_
