#ifndef boxm2_ocl_test_utils_h
#define boxm2_ocl_test_utils_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  4/16/12
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm2/boxm2_scene.h>

class boxm2_ocl_test_utils
{
  
public:
  static void create_edge_scene(boxm2_scene* scene, float val1, float val2);
  static void print_alpha_scene(boxm2_scene* scene);
  static void clean_up();
  
protected:
  
};

#endif
