
#ifndef open_cl_test_data_h_
#define open_cl_test_data_h_

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <boct/boct_tree.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>

class open_cl_test_data 
{
 public:
  static boct_tree<short, vnl_vector_fixed<float, 2> >* tree();

  static void save_tree(vcl_string const& tree_path);

  static void test_rays(vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                        vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir);
  
  static  void 
tree_and_rays_from_image(vcl_string const& image_path,
                         unsigned group_size,
                         boct_tree<short, vnl_vector_fixed<float, 2> >*& tree,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir);

  static void save_expected_image(vcl_string const& image_path,
                                  unsigned cols, unsigned rows,
                                  float* expected_img);

};
#endif // open_cl_test_data_h_
