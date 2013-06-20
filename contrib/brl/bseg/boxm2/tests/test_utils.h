#ifndef boxm2_test_utils_h_
#define boxm2_test_utils_h_
//:
// \file

#include <vcl_iostream.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <testlib/testlib_test.h>
#include <vpgl/vpgl_camera_double_sptr.h>

class boxm2_test_utils
{
  public:
    //: creates a valid, though predictable block byte stream
    static char* construct_block_test_stream( int numBuffers,
                                              int treeLen,
                                              int* nums,
                                              double* dims,
                                              int init_level,
                                              int max_level,
                                              int max_mb );

    static void   save_test_scene_to_disk();
    static void   delete_test_scene_from_disk(vcl_string dir="");

    static vcl_string   save_test_simple_scene(vcl_string filename= "test.xml");
    static vcl_string   save_test_empty_scene();

    static void  test_block_equivalence(boxm2_block& a, boxm2_block& b);

    template <boxm2_data_type data_type>
    static void test_data_equivalence(boxm2_data<data_type>& a, boxm2_data<data_type>& b);
    static vpgl_camera_double_sptr test_camera();
};


template <boxm2_data_type data_type>
void boxm2_test_utils::test_data_equivalence(boxm2_data<data_type>& a, boxm2_data<data_type>& b)
{
  // make sure data type matches
  TEST("Data length size matches", a.buffer_length(), b.buffer_length());

  // buffer size matches
  typedef typename boxm2_data<data_type>::datatype dtype;
  boxm2_array_1d<dtype> adat = a.data();
  boxm2_array_1d<dtype> bdat = b.data();
  vcl_cout<<"Adat size: "<<adat.size()<<'\n'
          <<"Bdat size: "<<bdat.size()<<vcl_endl;
  TEST("Data array size matches", adat.size(), bdat.size());

  // make sure buffers match
  for (unsigned int i=0; i<adat.size(); ++i) {
    if (adat[i] != bdat[i]) {
      TEST("Data array does not match", true, false);
      return;
    }
  }
  TEST("Data array matches !", true, true);
}

#endif
