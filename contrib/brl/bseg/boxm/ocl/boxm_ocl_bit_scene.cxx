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

#define BLOCK_NAME "blocks.bin"
#define DATA_NAME "data.bin"


//: initializes Scene from XML file
boxm_ocl_bit_scene::boxm_ocl_bit_scene(vcl_string filename)
{
  //default values for blank scene init
  max_mb_ = 400;
  pinit_ = .01;

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
  if (loaded)
    vcl_cout<<"existing scene initialized"<<vcl_endl;
  else {
    vcl_cerr<<"!!!! Bad scene - not initialized !!!!!"<<vcl_endl;
    return false;
  }
  return true;
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
  max_mb_ = vcl_ceil( (blockBytes + buffBytes + dataBytes)/1024.0/1024.0 );
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
    (*iter)[2] = block_ptrs[index++];
    (*iter)[3] = block_ptrs[index++];
  }
}

void boxm_ocl_bit_scene::set_tree_buffers(unsigned char* tree_buffers)
{
  int index = 0;
  vbl_array_2d<uchar16>::iterator iter;
  for (iter = tree_buffers_.begin(); iter != tree_buffers_.end(); iter++) {
    for(int c=0; c<16; c++) 
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
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++)
    alphas[index++] = (*iter)[0];
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
  int sizeBlks = 2*sizeof(short)*x_num*y_num*z_num; //both on disk and gpu
  int sizeTree = 16*sizeof(char)*num*tree_len; //both disk and gpu
  int sizeDataDisk = 16*sizeof(float)*num*data_len;
  int sizeDataUpdate = (5*sizeof(float)+8*sizeof(char)+4*sizeof(short))*num*data_len;
  int sizeDataRender = (sizeof(float) + 8*sizeof(char))*num*data_len;
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

