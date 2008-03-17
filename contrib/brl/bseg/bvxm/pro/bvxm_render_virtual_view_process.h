// This is brl/bseg/bvxm/pro/bvxm_render_virtual_view_process.h
#ifndef bvxm_render_virtual_view_process_h_
#define bvxm_render_virtual_view_process_h_
//:
// \file
// \brief A process that renders a video frame from a new viewpoint (used for 3-D registration)
//
// \author Daniel Crispell
// \date 02/10/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_render_virtual_view_process : public bprb_process
{
 public:

   bvxm_render_virtual_view_process();

  //: Copy Constructor (no local data)
  bvxm_render_virtual_view_process(const bvxm_render_virtual_view_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_render_virtual_view_process(){};

  //: Clone the process
  virtual bvxm_render_virtual_view_process* clone() const {return new bvxm_render_virtual_view_process(*this);}

  vcl_string name(){return "bvxmRenderVirtualView";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_render_virtual_view_process_h_
