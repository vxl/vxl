#ifndef boxm2_test_utils_h_
#define boxm2_test_utils_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>
#include <testlib/testlib_test.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <boxm2/boxm2_scene.h>

class boxm2_test_utils
{
  public:
    //: creates a valid, though predictable block byte stream
    static char* construct_block_test_stream( int numBuffers,
                                              int treeLen,
                                              const int* nums,
                                              double* dims,
                                              int init_level,
                                              int max_level,
                                              int max_mb );

    static void   save_test_scene_to_disk();
    static void   delete_test_scene_from_disk(const std::string& dir="");

    static std::string   save_test_simple_scene(const std::string& filename= "test.xml");
    static   std::map<boxm2_block_id,boxm2_block_metadata> generate_simple_metadata();
    static std::string   save_test_empty_scene();
    static bool create_test_simple_scene(boxm2_scene_sptr & scene);
    static void  test_block_equivalence(boxm2_block& a, boxm2_block& b);

    template <boxm2_data_type data_type>
    static void test_data_equivalence(boxm2_data<data_type>& a, boxm2_data<data_type>& b);
    static vpgl_camera_double_sptr test_camera();
    static const int init_level_ =1;
    static const int max_level_ =4;
    static const int treeLen_ = 64*64;
    static const int numBuffers_ = 64;
    static const int max_mb_ = 400;
    static const int nums_[];
    static const double dims_[];

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
  std::cout<<"Adat size: "<<adat.size()<<'\n'
          <<"Bdat size: "<<bdat.size()<<std::endl;
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
