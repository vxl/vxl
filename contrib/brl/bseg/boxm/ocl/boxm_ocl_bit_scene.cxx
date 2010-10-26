#include "boxm_ocl_bit_scene.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <boxm/boxm_apm_traits.h>
#include <vcl_cstdlib.h> // for rand()
/* xml includes */
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

/* io includes */
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vbl/io/vbl_io_array_1d.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vbl/io/vbl_io_array_3d.h>

/* utils */
#include <boxm/ocl/boxm_ocl_utils.h>

#define BLOCK_NAME "blocks.bin"
#define DATA_NAME "data.bin"


//: initializes Scene from XML file
boxm_ocl_bit_scene::boxm_ocl_bit_scene(vcl_string filename)
{
  //default values for blank scene init
  max_mb_ = 400;
  pinit_ = .01f;

  //load the scene xml file
  this->load_scene(filename);
}

// "kind of" copy constructor
boxm_ocl_bit_scene::boxm_ocl_bit_scene(boxm_ocl_bit_scene *that)
{
  this->init_scene(that->blocks(), that->tree_buffers(), that->data_buffers(),
                   that->mem_ptrs(), that->blocks_in_buffers(),
                   that->lvcs(), that->origin(), that->block_dim());
  this->set_max_level(that->max_level());
  this->set_init_level(that->init_level());
}

//init function for variables
void boxm_ocl_bit_scene::init_scene(vbl_array_3d<ushort2> blocks,
                                    vbl_array_2d<uchar16> tree_buffers,
                                    vbl_array_2d<float16> data_buffers,
                                    vbl_array_1d<ushort2> mem_ptrs,
                                    vbl_array_1d<unsigned short> blocks_in_buffers,
                                    bgeo_lvcs lvcs,
                                    vgl_point_3d<double> origin,
                                    vgl_vector_3d<double> block_dim)
{
  //copy all blocks
  blocks_ = vbl_array_3d<ushort2>(blocks);
  tree_buffers_ = vbl_array_2d<uchar16>(tree_buffers);
  data_buffers_ = vbl_array_2d<float16>(data_buffers);
  mem_ptrs_ = vbl_array_1d<ushort2>(mem_ptrs);
  blocks_in_buffers_ = vbl_array_1d<unsigned short>(blocks_in_buffers);

  num_buffers_ = (int) tree_buffers_.rows();
  tree_buff_length_ = (int) tree_buffers_.cols();
  data_buff_length_ = (int) data_buffers_.cols();
  lvcs_ = lvcs;
  origin_ = origin;
  block_dim_ = block_dim;

  //initialize level information
  max_level_ = 4;
  init_level_ = 1;
}


void boxm_ocl_bit_scene::validate_data()
{
  vcl_cout<<"Validating scene data:\n"
          <<"   num buffers: "<<num_buffers_<<vcl_endl;

  bool buffGood = true;
  bool sizeGood = true;
  bool sizeMatch = true;
  for (int buff=0; buff<num_buffers_; buff++)
  {
    if (blocks_in_buffers_[buff] < 2)
      continue;

    //zip through every tree in the first buffer, verify it's data pointer
    ushort2 memPtrs = mem_ptrs_[buff];
    int startPtr = (int) memPtrs[0];
  //int endPtr = (int) memPtrs[1];
    for (unsigned sb=0; sb+1<blocks_in_buffers_[buff]; ++sb)
    {
      //grab tree
      uchar16 currTree = tree_buffers_[buff][sb];
      int hi = currTree[10]; int lo = currTree[11];
      int currDataPtr = (hi<<8) | (lo);

      //grab next data pointer
      uchar16 nextTree = tree_buffers_[buff][sb+1];
      hi = nextTree[10]; lo = nextTree[11];
      int nextDataPtr = (hi<<8) | (lo);

      //verify that diff in data pointers matches tree size
      int count = 0 ;
      for (int i=0; i<10; i++) {
        unsigned char n = currTree[i];
        while (n)  {
          ++count;
          n &= (n - 1) ;
        }
      }
      count = 8*count+1;
      int currSize = (nextDataPtr-currDataPtr + data_buff_length_) % data_buff_length_;
      if (count != currSize) {
        sizeMatch = false;
        vcl_cout<<"Buffer "<<buff<<", tree"<<sb<<": NextDataPtr - CurrDataPtr doesn't match tree bit size\n"
                <<"    bitSize: "<<count<<", ptrSize: "<<currSize<<vcl_endl;
      }
      if (startPtr != currDataPtr) {
        vcl_cout<<"Buffer "<<buff<<", tree "<<sb<<" doesn't match data pointer. "<<vcl_endl;
        buffGood = false;
      }

      startPtr = (startPtr + count) % data_buff_length_;
    }
    if (!buffGood)
      vcl_cout<<"Buffer "<<buff<<" has invalid pointers... "<<vcl_endl;
    if (!sizeGood)
      vcl_cout<<"Buffer "<<buff<<" has invalid tree size(not 1 or 9) ... "<<vcl_endl;
    if (!sizeMatch)
      vcl_cout<<"Buffer "<<buff<<" bit size doesn't match pointer difference ... "<<vcl_endl;
  }
}

// ===== Save to disk functions =====

//--------------------------------------------------------------------
//: Saves XML scene file, block binary and data binary files to 'dir'
//--------------------------------------------------------------------
bool boxm_ocl_bit_scene::save_scene(vcl_string dir)
{
  vcl_cout<<"boxm_ocl_scene::save_scene to "<<dir<<vcl_endl;
  //get the paths straight...
  vcl_string block_path = dir + "blocks.bin";
  vcl_string data_path = dir + "data.bin";
  vcl_string xml_path = dir + "scene.xml";

  //write out to XML file
  vcl_ofstream xmlstrm(xml_path.c_str());
  boxm_ocl_bit_scene scene = boxm_ocl_bit_scene(this);
  scene.set_path(dir);
  x_write(xmlstrm, scene, "scene");

  //write to block binary file
  vsl_b_ofstream bin_os(block_path);
  if (!bin_os) {
    vcl_cout<<"cannot open "<<block_path<<" for writing";
    return false;
  }
  vsl_b_write(bin_os, num_buffers_);
  vsl_b_write(bin_os, tree_buff_length_);
  vsl_b_write(bin_os, data_buff_length_);
  vsl_b_write(bin_os, tree_buffers_);
  vsl_b_write(bin_os, blocks_);
  vsl_b_write(bin_os, mem_ptrs_);
  vsl_b_write(bin_os, blocks_in_buffers_);
  bin_os.close();

  //write to data binary file
  vsl_b_ofstream dat_os(data_path);
  if (!dat_os) {
    vcl_cout<<"cannot open "<<data_path<<" for writing\n";
    return false;
  }
  vsl_b_write(dat_os, data_buffers_);
  dat_os.close();

  return true;
}

bool boxm_ocl_bit_scene::save()
{
  vcl_string dir,pref;
  parser_.paths(dir, pref);

  vcl_cout<<"boxm_ocl_scene::save_scene to "<<dir<<vcl_endl;
  //get the paths straight...
  vcl_string block_path = dir + "blocks.bin";
  vcl_string data_path = dir + "data.bin";

  //write out to XML file
  //write to block binary file
  vsl_b_ofstream bin_os(block_path);
  if (!bin_os) {
    vcl_cout<<"cannot open "<<block_path<<" for writing\n";
    return false;
  }
  vsl_b_write(bin_os, num_buffers_);
  vsl_b_write(bin_os, tree_buff_length_);
  vsl_b_write(bin_os, data_buff_length_);
  vsl_b_write(bin_os, tree_buffers_);
  vsl_b_write(bin_os, blocks_);
  vsl_b_write(bin_os, mem_ptrs_);
  vsl_b_write(bin_os, blocks_in_buffers_);
  bin_os.close();

  //write to data binary file
  vsl_b_ofstream dat_os(data_path);
  if (!dat_os) {
    vcl_cout<<"cannot open "<<data_path<<" for writing\n";
    return false;
  }
  vsl_b_write(dat_os, data_buffers_);
  dat_os.close();

  return true;
}

// ===== LOAD FROM DISK FUNCTIONS =====

//--------------------------------------------------------------------
//:Loads small block scene from XML file (filename is xml file path)
// First reads XML file and stores model information
// then creates blocks and reads
// This needs to initialize: blocks_, tree_buffers_, data_buffers_ and mem_ptrs_
//--------------------------------------------------------------------
bool boxm_ocl_bit_scene::load_scene(vcl_string filename)
{
  // parser_ stores all the xml information
  xml_path_ = filename;
  if (filename.size() > 0) {
    vcl_FILE* xmlFile = vcl_fopen(filename.c_str(), "r");
    if (!xmlFile){
    vcl_cerr << filename.c_str() << " error on opening\n";
    return false;
    }
    if (!parser_.parseFile(xmlFile)) {
      vcl_cerr << XML_ErrorString(parser_.XML_GetErrorCode()) << " at line "
               << parser_.XML_GetCurrentLineNumber() << '\n';
      return false;
    }
  }

  /* store scene and world meta data */
  //path (directory where you can find the scene.xml file)
  vcl_string dir, pref;
  parser_.paths(dir, pref);
  path_ = dir;

  //lvcs, origin, block dimension
  parser_.lvcs(lvcs_);
  origin_ = parser_.origin();
  rpc_origin_ = parser_.origin();
  block_dim_ = parser_.block_dim();

  //init levels
  unsigned max, init;
  parser_.levels(max, init);
  max_level_ = (int) max;
  init_level_ = (int) init;

  // load tree binary information
  bool loaded = this->init_existing_scene() &&
                this->init_existing_data();
  if (loaded) {
    vcl_cout<<"existing scene initialized"<<vcl_endl;
    return true;
  }

  //no scene files found at this point - initialize empty scene from xml file
  vcl_cout<<"block and data files not found, initializing empty scene...\n";
  bool initScene = this->init_empty_scene();

  return initScene;
}

//: initializes existing scene given the parser's paths
bool boxm_ocl_bit_scene::init_existing_scene()
{
  vcl_string dir, pref;
  parser_.paths(dir, pref);
  vcl_string block_path = dir + "/blocks.bin";
  vsl_b_ifstream is(block_path, vcl_ios_binary);
  if (!is) {
    vcl_cout<<"init_existing_scene: file "<<block_path<<" doesn't exist"<<vcl_endl;
    return false;
  }
  vsl_b_read(is, num_buffers_);
  vsl_b_read(is, tree_buff_length_);
  vsl_b_read(is, data_buff_length_);
  vsl_b_read(is, tree_buffers_);
  vsl_b_read(is, blocks_);
  vsl_b_read(is, mem_ptrs_);
  vsl_b_read(is, blocks_in_buffers_);
  is.close();

  //initialize max_mb based blockSize+treeSize+dataSize
  int numBlocks = blocks_.get_row1_count() * blocks_.get_row2_count() * blocks_.get_row3_count();
  int blockBytes = numBlocks * sizeof(short) * 2;
  int buffBytes = tree_buff_length_ * num_buffers_ * sizeof(char) * 16;
  int dataBytes = data_buff_length_ * num_buffers_ * sizeof(float) * 16;
  max_mb_ = (int)vcl_ceil( (blockBytes + buffBytes + dataBytes)/1024.0/1024.0 );
  return true;
}

//: initializes existing data given the parser's paths
bool boxm_ocl_bit_scene::init_existing_data()
{
  vcl_string dir, pref;
  parser_.paths(dir, pref);
  vcl_string data_path = dir + "/data.bin";
  vsl_b_ifstream is(data_path, vcl_ios_binary);
  if (!is) {
    vcl_cout<<"init_existing_data: file "<<data_path<<" doesn't exist"<<vcl_endl;
    return false;
  }
  vsl_b_read(is, data_buffers_);
  is.close();
  return true;
}

//: initializes empty scene given
bool boxm_ocl_bit_scene::init_empty_scene()
{
  vcl_cout<<"Parser says max mb = "<<parser_.max_mb()<<vcl_endl
          <<"parser says block nums = "<<parser_.block_nums()<<vcl_endl;
  const int MAX_BYTES = parser_.max_mb()*1024*1024;
  const int BUFF_LENGTH = vcl_pow((float)2,(float)16); //65536

  //total number of (sub) blocks in the scene
  int total_blocks =  parser_.block_nums().x()
                    * parser_.block_nums().y()
                    * parser_.block_nums().z();

  int blockBytes = total_blocks*(2*sizeof(short) + 16*sizeof(char)); //16 byte tree, 4 byte blk_ptr
  if (MAX_BYTES < blockBytes) {
    vcl_cerr<<"**************************************************\n"
            <<"*** boxm_ocl_bit_scene::init_empty_scene: ERROR!!!!\n"
            <<"*** Max scene size not large enough to accommodate scene dimensions\n"
            <<"*** max bytes specified:  "<<MAX_BYTES<<'\n'
            <<"*** bytes needed:         "<<blockBytes<<'\n'
            <<"**************************************************"<<vcl_endl;
    return false;
  }
  int freeBytes = MAX_BYTES - blockBytes;
  int dataSize  = 8*sizeof(char) +    //MOG
                  4*sizeof(short) +   //numObs
                  sizeof(float) +     //alpha
                  2*sizeof(float) +   //aux data (cum_seg_len/beta)
                  4*sizeof(char);     //aux data (mean_obs/cum_vis)
  int num_cells = (int) (freeBytes/dataSize);                         //number of cells given maxmb
  int num_buffers = (int) vcl_ceil( ((float)num_cells/(float)BUFF_LENGTH) );
  int blocks_per_buffer = (int) vcl_ceil((float)total_blocks/(float)num_buffers);
  if (num_buffers * BUFF_LENGTH <= total_blocks) {
    vcl_cerr<<"**************************************************\n"
            <<"*** boxm_ocl_bit_scene::init_empty_scene: ERROR!!!!\n"
            <<"*** Max scene size not large enough to accommodate scene dimensions\n"
            <<"*** cells allocated:  "<<num_buffers * BUFF_LENGTH<<'\n'
            <<"*** total subblocks:  "<<total_blocks<<'\n'
            <<"**************************************************"<<vcl_endl;
    return false;
  }

  vcl_cout<<"OCL Scene buffer shape: ["
          <<num_buffers<<" buffers by "
          <<BUFF_LENGTH<<" cells ("
          <<blocks_per_buffer<<" trees per buffer)]. "
          <<"[total tree:"<<num_buffers*BUFF_LENGTH<<']'<<vcl_endl;

  // 1. Set up 3D array of blocks (small blocks), assuming all blocks are similarly sized
  ushort2 blk_init(-1);
  vbl_array_3d<ushort2> blocks(parser_.block_nums().x(), parser_.block_nums().y(), parser_.block_nums().z(), blk_init);

  // 2. set up 2d array of tree_buffers (add one buffer for emergencies)
  uchar16 tree_init((unsigned char) 0);
  vbl_array_2d<uchar16> tree_buffers(num_buffers+1, blocks_per_buffer, tree_init);

  // 3. set up 2d array of data buffers (add one buffer for emergencies)
  float16 dat_init(0.0f); dat_init[1] = .1f;
  vbl_array_2d<float16> data_buffers(num_buffers+1, BUFF_LENGTH, dat_init);

  // 4. set up 1d array of mem ptrs
  ushort2 mem_init; mem_init[0] = 0; mem_init[1] = 1;
  vbl_array_1d<ushort2> mem_ptrs(num_buffers+1, mem_init);

  // 5. Go through each block and convert it to smaller blocks
  vbl_array_1d<unsigned short> blocksInBuffer(num_buffers+1, (unsigned short) 0);//# of blocks in each buffer

  //iterate through each block, randomly place it somewhere
  int index=0;
  vnl_random random(9667566);
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); iter++)
  {
    //status for scene initialization
    int chunk = (int) (total_blocks/10) +1;
    if (index%chunk==0) vcl_cout<<'['<<index/chunk<<']'<<vcl_flush;

    // randomly place block into a buffer
    bool rand = (parser_.max_mb()>0);
    int buffIndex = boxm_ocl_utils::getBufferIndex(rand, mem_ptrs,
                                                   blocksInBuffer,
                                                   BUFF_LENGTH,
                                                   blocks_per_buffer,
                                                   1,
                                                   random);
    if (buffIndex < 0) {
      vcl_cout<<"boxm_ocl_bit_scene::InitEmptyScene: ERROR\n"
              <<" block @ absolute index: "<<index<<'\n'
              <<" FILLED BUFFER. FAILED TO CREATE NEW SCENE.\n";
      return false;
    }

    //point block to chosen buffer
    unsigned short dataOffset = mem_ptrs[buffIndex][1]-1; //minus one, because mem_end points to one past the last one
    unsigned short treeOffset = blocksInBuffer[buffIndex];
    ushort2 blk;
    blk[0] = buffIndex;            //buffer index
    blk[1] = treeOffset;           //tree offset to root
    (*iter) = blk;

    //put initial alpha value, update end of tree data in mem_ptrs;
    data_buffers(buffIndex, dataOffset)[0] = 0.1f;
    mem_ptrs[buffIndex][1]++;

    //copy tree into tree buffer (pack buffOffset into chars 10-11
    uchar16 treeBlk( (unsigned char) 0);
    unsigned char hi = (unsigned char)(dataOffset >> 8);
    unsigned char lo = (unsigned char)(dataOffset & 255);
    treeBlk[10] = hi; treeBlk[11] = lo;
    tree_buffers(buffIndex, treeOffset) = treeBlk;
    blocksInBuffer[buffIndex]++;

    //count for print out
    index++;
  }
  vcl_cout<<vcl_endl;

  //use the already existing init_scene method
  bgeo_lvcs lvcs;  parser_.lvcs(lvcs);
  this->init_scene(blocks, tree_buffers, data_buffers,
                   mem_ptrs, blocksInBuffer, lvcs, parser_.origin(), parser_.block_dim());
  unsigned max, init;
  parser_.levels(max, init);
  this->set_max_level(max);

  return true;
}

//--------------------------------------------------------------------
// Setters: flat arrays to scene values
//--------------------------------------------------------------------
void boxm_ocl_bit_scene::set_blocks(unsigned short* block_ptrs)
{
  int index = 0;
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); iter++) {
    (*iter)[0] = block_ptrs[index++];
    (*iter)[1] = block_ptrs[index++];
  }
}

void boxm_ocl_bit_scene::set_tree_buffers(unsigned char* tree_buffers)
{
  int index = 0;
  vbl_array_2d<uchar16>::iterator iter;
  for (iter = tree_buffers_.begin(); iter != tree_buffers_.end(); iter++) {
    for (int c=0; c<16; c++)
      (*iter)[c] = tree_buffers[index++];
  }
}

void boxm_ocl_bit_scene::set_mem_ptrs(unsigned short* mem_ptrs)
{
  int index = 0;
  vbl_array_2d<ushort2>::iterator iter;
  for (iter = mem_ptrs_.begin(); iter != mem_ptrs_.end(); iter++) {
    (*iter)[0] = mem_ptrs[index++];
    (*iter)[1] = mem_ptrs[index++];
  }
}

void boxm_ocl_bit_scene::set_data_values(float* data_buffer)
{
  int datIndex = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    for (int j=0; j<16; j++)
      (*iter)[j] = data_buffer[datIndex++];
  }
}

void boxm_ocl_bit_scene::set_alpha_values(float* alpha_buffer)
{
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    (*iter)[0] = alpha_buffer[index++];
  }
}

void boxm_ocl_bit_scene::set_mixture_values(unsigned char* mixtures)
{
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    (*iter)[1] = (float) (mixtures[index++]/255.0);
    (*iter)[2] = (float) (mixtures[index++]/255.0);
    (*iter)[3] = (float) (mixtures[index++]/255.0);
    (*iter)[5] = (float) (mixtures[index++]/255.0);
    (*iter)[6] = (float) (mixtures[index++]/255.0);
    (*iter)[7] = (float) (mixtures[index++]/255.0);
    (*iter)[9] = (float) (mixtures[index++]/255.0);
    (*iter)[10]= (float) (mixtures[index++]/255.0);
  }
}

void boxm_ocl_bit_scene::set_num_obs_values(unsigned short* num_obs)
{
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    (*iter)[4]  = (float) num_obs[index++];
    (*iter)[8]  = (float) num_obs[index++];
    (*iter)[11] = (float) num_obs[index++];
    (*iter)[12] = (float) num_obs[index++];
  }
}

//--------------------------------------------------------------------
// Getters: information to flat arrays (compresses some data)
//--------------------------------------------------------------------
void boxm_ocl_bit_scene::get_block_ptrs(unsigned short* blocks)
{
  int index=0;
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); iter++)
  {
    blocks[index++] = (*iter)[0];
    blocks[index++] = (*iter)[1];
  }
}

void boxm_ocl_bit_scene::get_tree_cells(unsigned char* trees)
{
  //init tree structure
  int index=0;
  vbl_array_2d<uchar16>::iterator iter;
  for (iter = tree_buffers_.begin(); iter != tree_buffers_.end(); iter++)
  {
    for (int c=0; c<16; c++)
      trees[index++] = (*iter)[c];
  }
}

void boxm_ocl_bit_scene::get_alphas(float* alphas)
{
  //init data arrays
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
#if 0
    if ((*iter)[0] == 0.0f) {
      vcl_cout<<"alpha value at "<<index<<" is equal to zero"<<vcl_endl;
      alphas[index++] = 0.1f;
      continue;
    }
    if ((*iter)[0] != (*iter)[0])
      vcl_cout<<"alpha value at "<<index<<" is NAN"<<vcl_endl;
#endif
    alphas[index++] = (*iter)[0];
  }
}

void boxm_ocl_bit_scene::get_mixture(unsigned char* mixture)
{
  int indexmix = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++)
  {
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[1]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[2]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[3]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[5]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[6]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[7]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[9]);
    mixture[indexmix++] = (unsigned char) (255.0 * (*iter)[10]);
  }
}

void boxm_ocl_bit_scene::get_num_obs(unsigned short* num_obs)
{
  //init data arrays
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++)
  {
    num_obs[index++] = (unsigned short) (*iter)[4];
    num_obs[index++] = (unsigned short) (*iter)[8];
    num_obs[index++] = (unsigned short) (*iter)[11];
    num_obs[index++] = (unsigned short) (*iter)[12];
  }
}

void boxm_ocl_bit_scene::get_mem_ptrs(unsigned short* mem_ptrs)
{
  int index=0;
  vbl_array_1d<ushort2>::iterator iter;
  for (iter = mem_ptrs_.begin(); iter != mem_ptrs_.end(); iter++)
  {
    mem_ptrs[index++] = (*iter)[0];
    mem_ptrs[index++] = (*iter)[1];
  }
}

void boxm_ocl_bit_scene::get_blocks_in_buffers(unsigned short* blks_in_buffers)
{
  int index=0;
  vbl_array_1d<unsigned short>::iterator iter;
  for (iter = blocks_in_buffers_.begin(); iter != blocks_in_buffers_.end(); iter++)
  {
    blks_in_buffers[index++] = (*iter);
  }
}

//---------------------------------------------------------------------
// NON CLASS FUNCTIONS
//---------------------------------------------------------------------

//------------XML WRITE------------------------------------------------
void x_write(vcl_ostream &os, boxm_ocl_bit_scene& scene, vcl_string name)
{
  //open root tag
  vsl_basic_xml_element scene_elm(name);
  scene_elm.x_write_open(os);

  //write lvcs information
  bgeo_lvcs lvcs = scene.lvcs();
  lvcs.x_write(os, LVCS_TAG);
  x_write(os, scene.origin(), LOCAL_ORIGIN_TAG);

  //write block numbers for x,y,z
  vsl_basic_xml_element blocks(BLOCK_NUM_TAG);
  int x_num, y_num, z_num;
  scene.block_num(x_num, y_num, z_num);
  blocks.add_attribute("x_dimension", x_num);
  blocks.add_attribute("y_dimension", y_num);
  blocks.add_attribute("z_dimension", z_num);
  blocks.x_write(os);

  //write block dimensions for each
  vsl_basic_xml_element bnum(BLOCK_DIMENSIONS_TAG);
  double x_dim, y_dim, z_dim;
  scene.block_dim(x_dim, y_dim, z_dim);
  bnum.add_attribute("x", x_dim);
  bnum.add_attribute("y", y_dim);
  bnum.add_attribute("z", z_dim);
  bnum.x_write(os);

  //write scene path for (needs to know where blocks are)
  vcl_string path = scene.path();
  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", path);
  paths.x_write(os);

  //write octree levels tag
  vsl_basic_xml_element tree(OCTREE_LEVELS_TAG);
  tree.add_attribute("max", (int) scene.max_level());
  tree.add_attribute("init", (int) scene.init_level());
  tree.x_write(os);

  //write max MB for scene
  vsl_basic_xml_element max_mb(MAX_MB_TAG);
  max_mb.add_attribute("mb", (int) scene.max_mb());
  max_mb.x_write(os);

  //write p_init for scene
  vsl_basic_xml_element pinit(P_INIT_TAG);
  pinit.add_attribute("val", (float) scene.pinit());
  pinit.x_write(os);

  //write number of buffers
  int num, tree_len, data_len;
  scene.tree_buffer_shape(num, tree_len);
  scene.data_buffer_shape(num, data_len);
  vsl_basic_xml_element buffers(TREE_INIT_TAG);
  buffers.add_attribute("num_buffers", num);
  buffers.add_attribute("tree_buff_size", tree_len);
  buffers.add_attribute("data_buff_size", data_len);
  buffers.x_write(os);

  scene_elm.x_write_close(os);
}

//------------IO Stream------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boxm_ocl_bit_scene& scene)
{
  //get shape of tree buffers
  int num, tree_len, data_len;
  scene.tree_buffer_shape(num, tree_len);
  scene.data_buffer_shape(num, data_len);

  //get block numbers in each dimension
  int x_num, y_num, z_num;
  scene.block_num(x_num, y_num, z_num);

  //get dimension of each block
  double x_dim, y_dim, z_dim;
  scene.block_dim(x_dim, y_dim, z_dim);

  //Calculate size of scene (as it sits on disk, and as it sits on gpu)
  long sizeBlks = 2*sizeof(short)*x_num*y_num*z_num; //both on disk and gpu
  long sizeTree = 16*sizeof(char)*num*tree_len; //both disk and gpu
  long sizeDataDisk = 16*sizeof(float)*num*data_len;
  long sizeDataUpdate = (5*sizeof(float)+8*sizeof(char)+4*sizeof(short))*num*data_len;
  long sizeDataRender = (sizeof(float) + 8*sizeof(char))*num*data_len;
  double disk_size = (sizeBlks + sizeTree + sizeDataDisk)/1024.0/1024.0;
  double update_size = (sizeBlks + sizeTree + sizeDataUpdate)/1024.0/1024.0;
  double render_size = (sizeBlks + sizeTree + sizeDataRender)/1024.0/1024.0;
  s <<"---OCL_SCENE--------------------------------\n"
    <<"path: "<<scene.path()<<'\n'
    <<"blocks:  [block_nums "<<x_num<<','<<y_num<<','<<z_num<<"] "
    <<"[blk_dim "<<x_dim<<','<<y_dim<<','<<z_dim<<"]\n"
    <<"blk levels: [init level "<<scene.init_level()<<"] "
    <<"[max level "<<scene.max_level()<<"]\n"
    <<"tree_buffers: [num "<<num<<" by "<<tree_len<<']'<<'\n'
    <<"data_buffers: [num "<<num<<" by "<<data_len<<']'<<'\n'
    <<"total cells: "<<num*tree_len<<"(tree) and "<<num*data_len<<"(data)\n"
    <<"size (on disk)      : "<< disk_size << " MB\n"
    <<"size (on gpu update): "<< update_size << " MB\n"
    <<"size (on gpu render): "<< render_size << " MB"<<vcl_endl;

  //print out buffer free space
  typedef vnl_vector_fixed<unsigned short,2> ushort2;
  typedef vnl_vector_fixed<unsigned char,16> uchar16;
  typedef vnl_vector_fixed<float,16> float16;
  vbl_array_2d<float16> data_buffers = scene.data_buffers();
  vbl_array_2d<uchar16> tree_buffers = scene.tree_buffers();
  vbl_array_1d<ushort2> mem_ptrs = scene.mem_ptrs();
  vbl_array_1d<unsigned short> numPer = scene.blocks_in_buffers();
  s << "free space:"<<vcl_endl;
  int totalFreeCells = 0;
  for (unsigned int i=0; i<mem_ptrs.size(); ++i) {
    int start=mem_ptrs[i][0];
    int end = mem_ptrs[i][1];
    int freeSpace = (start >= end)? start-end : data_buffers.cols() - (end-start);
    s <<"     buff["<<i<<"]="<<freeSpace<<" free cells, "
      <<"ptrs @ ("<<mem_ptrs[i][0]<<", "<<mem_ptrs[i][1]<<")  "
      <<"blks in buff: "<<numPer[i]<<vcl_endl;
    totalFreeCells+=freeSpace;
  }
  s << "total free cells: " << totalFreeCells;
  s << '\n'
    <<"--------------------------------------------" << vcl_endl;

  return s;

#if 0
  //verbose scene printing
  s << "Blocks:"<<vcl_endl;
  vbl_array_3d<int4> blocks = scene.blocks();
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      for (int k=0; k<2; k++) {
        int buffIndex = blocks[i][j][k][0];
        int buffOffset = blocks[i][j][k][1];
        int blkSize = blocks[i][j][k][2];

        s <<"---- block ("<<i<<','<<j<<','<<k<<") at tree["<<buffIndex<<"]["<<buffOffset<<"] size: "<<blkSize<<vcl_endl;

        //now print tree...
        for (int l=0; l<blkSize; l++) {
          //print tree cell
          vcl_cout<<"     cell @ "<<l<<" (buffer location: "<<l+buffOffset<<") : "
                  <<tree_buffers[buffIndex][buffOffset+l]<<' ';

          //print data if it exists
          int data_ptr = tree_buffers[buffIndex][buffOffset+l][2];
          if (data_ptr >= 0) {
            vcl_cout<<"     data @ "<<data_ptr<<" : "
                    <<data_buffers[buffIndex][data_ptr]<<' ';
          }
          else {
            vcl_cout<<"     data for this cell not stored ";
          }
          vcl_cout<<vcl_endl;
        }
      }
    }
  }
  //print out each tree
  for (int i=0; i<numBlocks; i++){
    int blkRoot = block_ptrs[2*i];
    int blkSize = block_ptrs[2*i+1];
    vcl_cout<<"---- block "<<i<<" at "<<blkRoot<<"-----"<<vcl_endl;
    for (int j=0; j<blkSize; j++) {
      //print tree cell
      vcl_cout<<"cell @ "<<j<<" (absolute: "<<j+blkRoot<<" : ";
      for (int k=0; k<4; k++)
        vcl_cout<<blocks[4*blkRoot+4*j+k]<<' ';

      //print data if it exists
      int data_ptr = blocks[4*blkRoot+4*j+2];
      if (data_ptr >= 0) {
        vcl_cout<<"  data @ "<<data_ptr<<" : ";
        for (int k=0; k<16; k++)
          vcl_cout<<data[16*data_ptr+k]<<' ';
      }
      else {
        vcl_cout<<"  data for this cell not stored ";
      }
      vcl_cout<<vcl_endl;
    }
  }
#endif
}


//: Binary write bit scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_ocl_bit_scene const& /*bit_scene*/)
{}

//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm_ocl_bit_scene& /*bit_scene*/)
{}

//: Binary write boxm scene pointer to stream
void vsl_b_read(vsl_b_istream& /*is*/, boxm_ocl_bit_scene* /*ph*/)
{}

//: Binary write boxm scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_ocl_bit_scene* const& /*ph*/){}
