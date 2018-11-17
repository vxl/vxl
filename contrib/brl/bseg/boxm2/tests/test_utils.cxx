// test utils to produce test blocks, data, scene, etc.
#include "test_utils.h"
#include <vnl/vnl_random.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
const int boxm2_test_utils::nums_[] = {64,64,64,0};
const double boxm2_test_utils::dims_[] = {0.5,0.5,0.5,0};

char* boxm2_test_utils::construct_block_test_stream(int  /*numBuffers*/,
                                                    int treeLen,
                                                    const int* nums,
                                                    double*  /*dims*/,
                                                    int  /*init_level*/,
                                                    int  /*max_level*/,
                                                    int  /*max_mb*/ )
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;

    //write size, init_level, max_level, max_mb
    int numTrees = nums[0]*nums[1]*nums[2];
    long size = numTrees*(sizeof(uchar16));

    //1. construct a dummy block byte stream manually
    char* bsize = new char[size];
    for (int i=0; i<size; i++) bsize[i] = (char) 0;
    int curr_byte = 0;

    //4. put some tree values in there
    //write in the buffer some values for the trees (each tree gets a 1 as the root)
    short buff_index=0;
    short data_index=0;
    for (int i=0; i<numTrees; i++)
    {
      bsize[curr_byte + 16*i] = (unsigned char) 0;
      bsize[curr_byte + 16*i+10]=(unsigned char)(data_index>>8);
      bsize[curr_byte + 16*i+11]=(unsigned char)(data_index&255);
      bsize[curr_byte + 16*i+12]=(unsigned char)(buff_index>>8);
      bsize[curr_byte + 16*i+13]=(unsigned char)(buff_index&255);

      data_index++;
      if (data_index>=treeLen)
      {
          data_index=0;
          buff_index++;
      }
    }
    curr_byte += sizeof(uchar16)*numTrees;

    if (curr_byte != size)
      std::cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<'\n';

    return bsize;
}

std::map<boxm2_block_id,boxm2_block_metadata> boxm2_test_utils::generate_simple_metadata(){

  vnl_random rand;
  std::map<boxm2_block_id, boxm2_block_metadata> mdata;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(i,j,k);
        vgl_point_3d<double> local_origin = vgl_point_3d<double>(0,0,0) + vgl_vector_3d<double>((double)nums_[0]*dims_[0]*(double)id.i() ,
                                                                                                (double)nums_[0]*dims_[0]*(double)id.j(),
                                                                                                (double)nums_[0]*dims_[0]*(double)id.k());
        mdata[id] = boxm2_block_metadata  (id,
                                               local_origin,
                                               vgl_vector_3d<double>(dims_[0],dims_[1],dims_[2]),
                                               vgl_vector_3d<unsigned>(nums_[0],nums_[1],nums_[2]),
                                               init_level_,
                                               max_level_,
                                               max_mb_,
                                               0.001,2);
      }
    }
  }
  return mdata;
}
void boxm2_test_utils::save_test_scene_to_disk()
{
  // ensure 8 test blocks and 8 data blocks are saved to disk
  std::map<boxm2_block_id,boxm2_block_metadata> mdata = generate_simple_metadata();
  vnl_random rand;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(i,j,k);
        if (mdata.find(id)==mdata.end())
          break;
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers_,
                                                                      treeLen_,
                                                                      const_cast<int*>(nums_),
                                                                      const_cast<double*>(dims_),
                                                                      mdata[id].init_level_,
                                                                      mdata[id].max_level_,
                                                                      mdata[id].max_mb_ );

        boxm2_block b(id,mdata[id], stream);

        std::cout<<"saving test block for "<<id<<std::endl;
        boxm2_sio_mgr::save_block("", &b);
      }
    }
  }

  // save the same random data block 8 times
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;
  const unsigned int array_size = 5*1024*1024; //roughly 20 megs for alpha
  auto * farray = new float[array_size];
  auto* carray = new uchar8[array_size];
  for (unsigned c=0; c<array_size; ++c) {
    auto rnd = (float) rand.drand32(0,100);
    farray[c] = rnd;
    carray[c] = uchar8((unsigned char) rnd);
  }
  char * buffer = reinterpret_cast<char *>(farray);
  char * cbuffer = reinterpret_cast<char *>(carray);
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float), boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_MOG3_GREY> test_mog(cbuffer, array_size*sizeof(uchar8), boxm2_block_id(0,0,0));
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(boxm2_block_id(i,j,k));
        std::cout<<"saving alpha and mog3 test data for "<<id<<std::endl;
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>("", id, &test_data);
        boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>("", id, &test_mog);
      }
    }
  }
}

void boxm2_test_utils::delete_test_scene_from_disk(const std::string& dir)
{
  //use vul_file to
  vul_file::delete_file_glob(dir+"/"+"*id_*.bin");
}


void boxm2_test_utils::test_block_equivalence(boxm2_block& a, boxm2_block& b)
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;


    if (a.tree_buff_length() != b.tree_buff_length()) {
        TEST("boxm2_block: tree_buff_length failed", true, false);
        return;
    }
    if (a.init_level() != b.init_level()) {
        TEST("boxm2_block: init_level failed", true, false);
        return;
    }
    if (a.max_level() != b.max_level()) {
        TEST("boxm2_block: max_level failed", true, false);
        return;
    }
    if (a.max_mb() != b.max_mb()) {
        TEST("boxm2_block: max_mb failed", true, false);
        return;
    }
    if (a.sub_block_dim() != b.sub_block_dim()) {
        TEST("boxm2_block: sub_block_dim failed", true, false);
        return;
    }
    if (a.sub_block_num() != b.sub_block_num()) {
        TEST("boxm2_block: sub_block_num failed", true, false);
        return;
    }
    TEST("boxm2_block: meta data from disk passed", true, true);

    const boxm2_array_3d<uchar16>&  treesa = a.trees();
    const boxm2_array_3d<uchar16>&  treesb = b.trees();
    for (unsigned int i=0; i<a.sub_block_num().x(); ++i) {
      for (unsigned int j=0; j<a.sub_block_num().y(); ++j) {
        for (unsigned int k=0; k<a.sub_block_num().z(); ++k) {
          if (treesa[i][j][k] != treesb[i][j][k]) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);
}

std::string boxm2_test_utils::save_test_empty_scene()
{
    std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
    std::string test_file = test_dir + "test.xml";

    std::map<boxm2_block_id, boxm2_block_metadata> blocks = generate_simple_metadata();
  //create scene
  boxm2_scene scene;
  scene.set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene.set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene.set_lvcs(lvcs);
  scene.set_xml_path(test_file);
  scene.set_data_path(test_dir);
  scene.set_blocks(blocks);
  scene.save_scene();

  return test_file;
}


std::string boxm2_test_utils::save_test_simple_scene(const std::string& filename )
{
    std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
    std::string test_file = test_dir + filename;

    std::map<boxm2_block_id, boxm2_block_metadata> blocks = generate_simple_metadata();

  //create scene
  boxm2_scene scene;
  scene.set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene.set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene.set_lvcs(lvcs);
  scene.set_xml_path(test_file);
  scene.set_data_path(test_dir);
  scene.set_blocks(blocks);
  scene.set_version(2);
  scene.save_scene();

  // ensure 8 test blocks and 8 data blocks are saved to disk
  vnl_random rand;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(i,j,k);
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers_,
                                                                      treeLen_,
                                                                      const_cast<int*>(nums_),
                                                                      const_cast<double*>(dims_),
                                                                      blocks[id].init_level_,
                                                                      blocks[id].max_level_,
                                                                      blocks[id].max_mb_ );
        boxm2_block b(id, stream);

        std::cout<<"saving test block for "<<id<<std::endl;
        boxm2_sio_mgr::save_block(test_dir+"/", &b);
      }
    }
  }

  // save the same random data block 8 times
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;
  constexpr unsigned int array_size = 4; //roughly 20 megs for alpha
  auto * farray = new float[array_size];
  auto* carray = new uchar8[array_size];
  for (unsigned c=0; c<array_size; ++c) {
    auto rnd = (float) rand.drand32(0,100);
    farray[c] = rnd;
    carray[c] = uchar8((unsigned char) rnd);
  }
  char * buffer = reinterpret_cast<char *>(farray);
  char * cbuffer = reinterpret_cast<char *>(carray);
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float), boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_MOG3_GREY> test_mog(cbuffer, array_size*sizeof(uchar8), boxm2_block_id(0,0,0));
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(boxm2_block_id(i,j,k));
        std::cout<<"saving alpha and mog3 test data for "<<id<<std::endl;
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>(test_dir+"/", id, &test_data);
        boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>(test_dir+"/", id, &test_mog);
      }
    }
  }

  return test_file;
}
bool boxm2_test_utils::create_test_simple_scene(boxm2_scene_sptr & scene)
{
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = generate_simple_metadata();
  scene = new boxm2_scene();
  scene->set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene->set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene->set_lvcs(lvcs);
  scene->set_xml_path("./scene.xml");
  scene->set_data_path(".");
  scene->set_blocks(blocks);
  scene->set_version(2);
  return true;
}

vpgl_camera_double_sptr boxm2_test_utils::test_camera()
{
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_point_3d<double> t(0.5,0.5,100);

  std::cout<<mk<<mr<<t;
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,t,R);
  return cam;
}
