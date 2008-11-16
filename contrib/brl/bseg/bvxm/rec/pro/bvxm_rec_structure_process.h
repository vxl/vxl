// This is contrib/bvxm/rec/pro/bvxm_rec_structure_process.h
#ifndef bvxm_rec_structure_process_h_
#define bvxm_rec_structure_process_h_
//:
// \file
// \brief A class to find instances of various structures, objects (e.g. vehicles) according to a part hierarchy constructed a priori
//
// \author Ozge Can Ozcanli
// \date 10/28/2008
//
// \verbatim
// Modifications 
//
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class bvxm_rec_structure_process : public bprb_process
{
 public:

  bvxm_rec_structure_process();

  //: Copy Constructor (no local data)
  bvxm_rec_structure_process(const bvxm_rec_structure_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_rec_structure_process(){};

  //: Clone the process
  virtual bvxm_rec_structure_process* clone() const {return new bvxm_rec_structure_process(*this);}

  vcl_string name() { return "bvxmRecStructureProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_rec_structure_process_h_
