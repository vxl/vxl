// This is brl/bseg/bvxm/pro/bvxm_generate_edge_map_process.h
#ifndef bvxm_generate_edge_map_process_h_
#define bvxm_generate_edge_map_process_h_
//:
// \file
// \brief A process that takes a grayscale image and returns the corresponding edge map
//
// \author Ibrahim Eden
// \date 03/05/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_generate_edge_map_process : public bprb_process
{
 public:
   bvxm_generate_edge_map_process();

  //: Copy Constructor (no local data)
  bvxm_generate_edge_map_process(const bvxm_generate_edge_map_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_generate_edge_map_process(){};

  //: Clone the process
  virtual bvxm_generate_edge_map_process* clone() const
  { return new bvxm_generate_edge_map_process(*this); }

  vcl_string name(){return "bvxmGenerateEdgeMapProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_generate_edge_map_process_h_
