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

class vvid_video_process : public vbl_ref_count
{
public:
  vvid_video_process();
  ~vvid_video_process();
  void clear_input(){_input_images.clear();}
  void add_input_image(vil_image&  im){_input_images.push_back(im);}
  int get_N_inputs(){return _input_images.size();}
  vil_image get_input_image(int i);
  vil_image get_output_image(){return _output_image;}
  virtual bool execute()=0;  
protected: 
  //members
  vcl_vector<vil_image> _input_images;
  vil_image _output_image;
};
 

#endif // vvid_video_process_h_
