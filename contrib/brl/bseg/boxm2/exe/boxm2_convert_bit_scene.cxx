//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_sio_mgr.h>

//boxm bit scene stuff
#include <boxm/ocl/boxm_ocl_bit_scene.h>

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
  vcl_cout<<"Converting Bit scene to Boxm2 Scene"<<vcl_endl;
  vul_arg<vcl_string> bit_file("-scene", "scene filename", "");
  vul_arg<vcl_string> new_dir("-out", "output directory", "");
  vul_arg_parse(argc, argv);

  //set up some types
  typedef unsigned short ushort;
  typedef unsigned char  uchar;
  typedef vnl_vector_fixed<ushort,2>  ushort2;
  typedef vnl_vector_fixed<uchar, 8>  uchar8;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<float, 16> float16;

  //1.  create bit_scene from xml file
  boxm_ocl_bit_scene bit_scene(bit_file());
  vcl_cout<<"Scene Initialized...\n" <<bit_scene<<vcl_endl;

  //get some block metadata
  int init_level  = bit_scene.init_level();
  int max_level   = bit_scene.max_level();
  int max_mb      = bit_scene.max_mb();
  int num_buffers, tree_len, data_len;
  bit_scene.tree_buffer_shape(num_buffers, tree_len);
  bit_scene.data_buffer_shape(num_buffers, data_len);

  //2. create empty boxm2_scene
  boxm2_scene new_scene;

  //2.5 create new block metadata
  boxm2_block_metadata data;
  data.id_ = boxm2_block_id(0,0,0);
  data.local_origin_  = bit_scene.origin();
  data.sub_block_dim_ = bit_scene.block_dim();
  int nx,ny,nz; bit_scene.block_num(nx, ny, nz);
  data.sub_block_num_ = vgl_vector_3d<unsigned>(nx,ny,nz);
  data.init_level_    = init_level;
  data.max_level_     = max_level;
  data.max_mb_        = (max_mb > 0)? max_mb : 400.0;
  data.p_init_        = .001;

  //3. set scene metadata
  new_scene.add_block_metadata(data);
  new_scene.set_local_origin(bit_scene.origin());
  new_scene.set_rpc_origin(bit_scene.origin());
  new_scene.set_lvcs(bit_scene.lvcs());
  new_scene.set_xml_path(new_dir() + "/scene.xml");
  new_scene.set_data_path(new_dir());
  new_scene.save_scene();

  //2.  Get the relevant arrays
  vbl_array_1d<ushort2> mem_ptrs = bit_scene.mem_ptrs();
  vbl_array_1d<ushort>  blocks_in_buffers = bit_scene.blocks_in_buffers();
  vbl_array_3d<ushort2> blocks = bit_scene.blocks();
  vbl_array_2d<uchar16> tree_buffers = bit_scene.tree_buffers();
  vbl_array_2d<float16> data_buffers = bit_scene.data_buffers();

  //----------------------------------------------------------------------------
  //3. construct a fat bit stream for the block
  //----------------------------------------------------------------------------
  int numTrees = blocks.get_row1_count() *
                 blocks.get_row2_count() *
                 blocks.get_row3_count();
  long size = numTrees * sizeof(uchar16) + //3d block pointers
              num_buffers*tree_len * sizeof(int) + //tree pointers
              num_buffers*(sizeof(ushort) + sizeof(ushort2)) + //blocks in buffers and mem ptrs
              sizeof(long) +                      //this number
              3*sizeof(int) +                     //init level, max level, max_mb
              4*sizeof(double) +                  //dims
              6*sizeof(int);                      //nums + numBuffers, treeLen

  //3.a construct a block byte stream manually
  uchar* bsize = new uchar[size];
  for (int i=0; i<size; i++) bsize[i] = (char) 0;
  int curr_byte = 0;

  //3.a write size, init_level, max_level, max_mb
  vcl_memcpy(bsize,   &size, sizeof(long));
  curr_byte += sizeof(long);
  vcl_memcpy(bsize+curr_byte, &init_level, sizeof(int));
  curr_byte += sizeof(int);
  vcl_memcpy(bsize+curr_byte, &max_level, sizeof(int));
  curr_byte += sizeof(int);
  vcl_memcpy(bsize+curr_byte, &max_mb, sizeof(int));
  curr_byte += sizeof(int);

  //3.b write dimension and buffer shape
  double dims[] = { bit_scene.block_dim().x(),
                  bit_scene.block_dim().y(),
                  bit_scene.block_dim().z(), 0.0 };
  int nums[] = {nx, ny, nz, 0}; //from above
  vcl_memcpy(bsize+curr_byte, dims, 4 * sizeof(double));
  curr_byte += 4 * sizeof(double);
  vcl_memcpy(bsize+curr_byte, nums, 4 * sizeof(int));
  curr_byte += 4 * sizeof(int);

  //3.c write number of buffers
  vcl_memcpy(bsize+curr_byte, &num_buffers, sizeof(num_buffers));
  curr_byte += sizeof(num_buffers);

  //3.d write length of tree buffers
  vcl_memcpy(bsize+curr_byte, &tree_len, sizeof(tree_len));
  curr_byte += sizeof(tree_len);

  //3.e copy in trees to 3d buffer
  int blkIndex = 0;
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); iter++) {
    ushort buff   = (*iter)[0];
    ushort offset = (*iter)[1];
    uchar16 tree = tree_buffers[buff][offset];

    //store data buffer and offset in tree
    uchar data_hi  = tree[10];
    uchar data_lo  = tree[11];
    uchar buff_hi   = (uchar)(buff >> 8);
    uchar buff_lo   = (uchar)(buff & 255);
    tree[10] = data_hi;
    tree[11] = data_lo;
    tree[12] = buff_hi;
    tree[13] = buff_lo;

    //copy into stream
    vcl_memcpy(bsize+curr_byte, &tree, sizeof(tree));
    curr_byte += sizeof(uchar16);
    blkIndex++;
  }

  //3.f 2d array of tree pointers TODO CORRECT MY CREATION HERE
  int* treePtrsBuff = (int*) (bsize+curr_byte);
  for (int i = 0; i < num_buffers*tree_len; i++)
    treePtrsBuff[i] = i;
  curr_byte += num_buffers*tree_len * sizeof(int);

  //6. fill in some blocks in buffers numbers
  ushort* treeCountBuff = (ushort*) (bsize + curr_byte);
  for (int i=0; i<num_buffers; i++)
    treeCountBuff[i] = blocks_in_buffers[i];
  curr_byte += sizeof(ushort) * num_buffers;

  //7. 1d array of mem pointers
  ushort2* memPtrsBuff = (ushort2*) (bsize + curr_byte);
  for (int i=0; i<num_buffers; i++) {
    memPtrsBuff[i][0] = mem_ptrs[i][0];
    memPtrsBuff[i][1] = mem_ptrs[i][1];
  }
  curr_byte += sizeof(ushort2) * num_buffers;

  if (curr_byte != size)
    vcl_cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<'\n';
  //----------------------------------------------------------------------------
  //End of block bit strem
  //----------------------------------------------------------------------------
  boxm2_block b2_block(boxm2_block_id(0,0,0), reinterpret_cast<char *>(bsize));
  boxm2_sio_mgr::save_block(new_scene.data_path(), &b2_block);

  //----------------------------------------------------------------------------
  //Construct data blocks (MOG and ALPHA)
  //----------------------------------------------------------------------------
  vcl_cout<<"DATA LENGTH IS: "<<data_len<<vcl_endl;
  int     numData = num_buffers * data_len;
  float * alphas  = new float[numData];
  uchar8* mogs    = new uchar8[numData];
  bit_scene.get_alphas(alphas);
  bit_scene.get_mixture( (uchar*) mogs);

  ////create new block from farray, save it and delete it
  boxm2_block_id id(0,0,0);
  char * alpha_stream = reinterpret_cast<char *>(alphas);
  boxm2_data<BOXM2_ALPHA> alpha_data(alpha_stream, numData*sizeof(float), id);
  boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>(new_scene.data_path(), id, &alpha_data);

  char* mog_stream = reinterpret_cast<char *>(mogs);
  boxm2_data<BOXM2_MOG3_GREY> mog_data(mog_stream, numData*sizeof(uchar8), id);
  boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>(new_scene.data_path(), id, &mog_data);

  return 0;
}
