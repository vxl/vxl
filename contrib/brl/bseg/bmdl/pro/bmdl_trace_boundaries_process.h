// This is brl/bseg/bmdl/pro/bmdl_trace_boundaries_process.h
#ifndef bmdl_trace_boundaries_process_h_
#define bmdl_trace_boundaries_process_h_
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - label image "vil_image_view_base_sptr"
//             - File path for polygons (binary format)
//
//        -  Output:
//
//        -  Params:
//
// \author  Gamze D. Tunali
// \date    10/14/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>

class bmdl_trace_boundaries_process : public bprb_process
{
 public:

   bmdl_trace_boundaries_process();

   //: Copy Constructor (no local data)
   bmdl_trace_boundaries_process(const bmdl_trace_boundaries_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bmdl_trace_boundaries_process(){}

  //: Clone the process
  virtual bmdl_trace_boundaries_process* clone() const
    { return new bmdl_trace_boundaries_process(*this); }

  vcl_string name(){return "bmdlTraceBoundariesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
   
  bool trace_boundaries(vil_image_view_base_sptr label_img, vcl_string fpath);
};

#endif // bmdl_trace_boundaries_process_h_
