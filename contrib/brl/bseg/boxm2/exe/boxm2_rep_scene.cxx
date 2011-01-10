//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <boxm2/io/boxm2_dumb_cache.h>

//vnl and vbl array stuff
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

//brdb stuff
#include <brdb/brdb_value.h>

//  Converts a bit_scene to a boxm2 scene.
//  NOTE: currently will only create one block with id 0,0,0

int main(int argc,  char** argv)
{
  vcl_cout<<"Repeating boxm2 Scene (for test rendering)"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "boxm2 scene filename", "");
  vul_arg<vcl_string> new_dir("-out", "output directory", "");

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //set up some types
  typedef unsigned short ushort;
  typedef unsigned char  uchar;
  typedef vnl_vector_fixed<ushort,2>  ushort2;
  typedef vnl_vector_fixed<uchar, 8>  uchar8;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<float, 16> float16;

  //1. Create boxm2_scene from XML file
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  if (scene->num_blocks() > 1)
    vcl_cout<<"Scene has more than one block, just repeating first one"<<vcl_endl;

  //2. create a block meta data table
  boxm2_block_metadata data = scene->blocks()[boxm2_block_id(0,0,0)];
  vcl_cout<<"Original Origin: "<<data.local_origin_<<vcl_endl;

  //3.  rep the initial block, changing ID and origin
  vgl_vector_3d<double> block_side( data.sub_block_dim_.x() * data.sub_block_num_.x(),
                                    data.sub_block_dim_.y() * data.sub_block_num_.y(),
                                    data.sub_block_dim_.z() * data.sub_block_num_.z());
  vcl_cout<<"Block Side: "<<block_side<<vcl_endl;
  vcl_vector<boxm2_block_metadata> blocks;
  for (int i=0; i<2; ++i) {
    for (int j=0; j<2; ++j) {
      boxm2_block_metadata data_copy(data);
      boxm2_block_id id(i,j,0);
      data_copy.id_ = id;
      data_copy.local_origin_ = vgl_point_3d<double>(data_copy.local_origin_.x() + i*block_side.x(),
                                                     data_copy.local_origin_.y() + j*block_side.y(),
                                                     data_copy.local_origin_.z());
      vcl_cout<<"id: "<<id<<"  origin: "<<data_copy.local_origin_<<vcl_endl;
      blocks.push_back(data_copy);
    }
  }

  //2. create empty boxm2_scene
  boxm2_scene new_scene;

  //3. set scene metadata
  for (unsigned int i=0; i<blocks.size(); ++i)
  new_scene.add_block_metadata(blocks[i]);
  new_scene.set_local_origin(scene->local_origin());
  new_scene.set_rpc_origin(scene->local_origin());
  new_scene.set_lvcs(scene->lvcs());
  new_scene.set_xml_path(new_dir() + "/scene.xml");
  new_scene.set_data_path(new_dir());
  new_scene.save_scene();

  //4. GET SINGLE BLOCK
  boxm2_block_id id(0,0,0);
  boxm2_dumb_cache dcache(scene.ptr());
  boxm2_block_sptr blk      = dcache.get_block(id);
  boxm2_data_base_sptr alph = dcache.get_data<BOXM2_ALPHA>(id);
  boxm2_data_base_sptr mog  = dcache.get_data<BOXM2_MOG3_GREY>(id);

  //5. copy and save it
  for (int i=0; i<2; ++i) {
    for (int j=0; j<2; ++j) {
      char* blkBuffer = new char[blk->byte_count()];
      vcl_memcpy(blkBuffer, blk->buffer(), blk->byte_count());
      boxm2_block_id id(i,j,0);

      //save voxel block
      boxm2_block block(id, blkBuffer);
      boxm2_sio_mgr::save_block(new_scene.data_path(), &block);

      //save alpha buffer
      char* alphaBuffer = new char[alph->buffer_length()];
      vcl_memcpy(alphaBuffer, alph->data_buffer(), alph->buffer_length());
      boxm2_data<BOXM2_ALPHA> alpha_data(alphaBuffer, alph->buffer_length(), id);
      boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>(new_scene.data_path(), id, &alpha_data);

      //save mog buffer
      char* mogBuffer = new char[mog->buffer_length()];
      vcl_memcpy(mogBuffer, mog->data_buffer(), mog->buffer_length());
      boxm2_data<BOXM2_MOG3_GREY> mog_data(mogBuffer, mog->buffer_length(), id);
      boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>(new_scene.data_path(), id, &mog_data);
    }
  }

  return 0;
}
