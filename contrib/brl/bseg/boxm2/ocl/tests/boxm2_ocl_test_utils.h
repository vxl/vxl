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
#include <vnl/vnl_random.h>
class boxm2_ocl_test_utils
{

public:
    static char* construct_block_test_stream( int numBuffers,
                                              int treeLen,
                                              const int* nums,
                                              double* dims,
                                              int init_level,
                                              int max_level,
                                              int max_mb );
  static std::string save_test_simple_scene(const std::string& filename= "test.xml");
  static bool create_test_simple_scene(boxm2_scene_sptr & scene);
  static void create_edge_scene(boxm2_scene_sptr scene, float val1, float val2);
  static void print_alpha_scene(boxm2_scene_sptr scene);
  template<class T> static void  fill_random(T *b, T *e, vnl_random &rng) ;
  static void clean_up();

protected:

};
template< class T>
void boxm2_ocl_test_utils::fill_random(T *b, T *e, vnl_random &rng)
{
  for (T *p=b; p<e; ++p)
    *p = (T)rng.drand64(-1.0, +1.0);
}
#endif
