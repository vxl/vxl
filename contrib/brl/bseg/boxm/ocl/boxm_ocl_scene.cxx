#include "boxm_ocl_scene.h"
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

boxm_ocl_scene::boxm_ocl_scene(vbl_array_3d<int4> blocks,
                               vbl_array_2d<int4> tree_buffers,
                               vbl_array_2d<float16> data_buffers,
                               vbl_array_1d<int2> mem_ptrs,
                               bgeo_lvcs lvcs,
                               vgl_point_3d<double> origin,
                               vgl_vector_3d<double> block_dim)
{
  this->init_scene(blocks, tree_buffers, data_buffers, mem_ptrs, lvcs, origin, block_dim);
}

// initializes Scene from XML file
boxm_ocl_scene::boxm_ocl_scene(vcl_string filename)
{
  //default values for blank scene init
  max_mb_ = 400;
  pinit_ = .01;

  //load the scene xml file
  this->load_scene(filename);
}

//copy constructor
boxm_ocl_scene::boxm_ocl_scene(boxm_ocl_scene *that)
{
  this->init_scene(that->blocks(), that->tree_buffers(), that->data_buffers(),
                   that->mem_ptrs(), that->lvcs(), that->origin(), that->block_dim());
  this->set_max_level(that->max_level());
  this->set_init_level(that->init_level());
}

//init function for variables
void boxm_ocl_scene::init_scene(vbl_array_3d<int4> blocks,
                                vbl_array_2d<int4> tree_buffers,
                                vbl_array_2d<float16> data_buffers,
                                vbl_array_1d<int2> mem_ptrs,
                                bgeo_lvcs lvcs,
                                vgl_point_3d<double> origin,
                                vgl_vector_3d<double> block_dim)
{
  //copy all blocks
  blocks_ = vbl_array_3d<int4>(blocks);
  tree_buffers_ = vbl_array_2d<int4>(tree_buffers);
  data_buffers_ = vbl_array_2d<float16>(data_buffers);
  mem_ptrs_ = vbl_array_1d<int2>(mem_ptrs);

  num_tree_buffers_ = (int) tree_buffers.rows();
  tree_buff_length_ = (int) tree_buffers.cols();
  lvcs_ = lvcs;
  origin_ = origin;
  block_dim_ = block_dim;

  //initialize level information
  max_level_ = 4;
  init_level_ = 1;
}


//: Saves XML scene file, block binary and data binary files to 'dir'
bool boxm_ocl_scene::save_scene(vcl_string dir)
{
  vcl_cout<<"boxm_ocl_scene::save_scene to "<<dir<<vcl_endl;
  //get the paths straight...
  vcl_string block_path = dir + "blocks.bin";
  vcl_string data_path = dir + "data.bin";
  vcl_string xml_path = dir + "scene.xml";

  //write out to XML file
  vcl_ofstream xmlstrm(xml_path.c_str());
  boxm_ocl_scene scene = boxm_ocl_scene(this);
  scene.set_path(dir);
  x_write(xmlstrm, scene, "scene");

  //write to block binary file
  vsl_b_ofstream bin_os(block_path);
  if (!bin_os) {
    vcl_cout<<"cannot open "<<block_path<<" for writing";
    return false;
  }
  vsl_b_write(bin_os, num_tree_buffers_);
  vsl_b_write(bin_os, tree_buff_length_);
  vsl_b_write(bin_os, tree_buffers_);
  vsl_b_write(bin_os, blocks_);
  vsl_b_write(bin_os, mem_ptrs_);
  bin_os.close();

  //write to data binary file
  vsl_b_ofstream dat_os(data_path);
  if (!dat_os) {
    vcl_cout<<"cannot open "<<data_path<<" for writing";
    return false;
  }
  vsl_b_write(dat_os, data_buffers_);
  dat_os.close();

  return true;
}

bool boxm_ocl_scene::save()
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
    vcl_cout<<"cannot open "<<block_path<<" for writing";
    return false;
  }
  vsl_b_write(bin_os, num_tree_buffers_);
  vsl_b_write(bin_os, tree_buff_length_);
  vsl_b_write(bin_os, tree_buffers_);
  vsl_b_write(bin_os, blocks_);
  vsl_b_write(bin_os, mem_ptrs_);
  bin_os.close();

  //write to data binary file
  vsl_b_ofstream dat_os(data_path);
  if (!dat_os) {
    vcl_cout<<"cannot open "<<data_path<<" for writing";
    return false;
  }
  vsl_b_write(dat_os, data_buffers_);
  dat_os.close();

  return true;
}


//:Loads small block scene from XML file (filename is xml file path)
// First reads XML file and stores model information
// then creates blocks and reads
// This needs to initialize: blocks_, tree_buffers_, data_buffers_ and mem_ptrs_
bool boxm_ocl_scene::load_scene(vcl_string filename)
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
  bool tree_loaded = this->init_existing_scene();
  if (tree_loaded) {
    vcl_cout<<"existing scene initialized"<<vcl_endl;
    bool data_loaded = this->init_existing_data();
    if (!data_loaded) {
      //init empty data to fit the tree structure
      vcl_cout<<"data.bin not found, initializing empty data"<<vcl_endl;
      this->init_empty_data();
    }
  }
  else {
    vcl_cout<<"tree_not loaded, initializing empty scene"<<vcl_endl;
    parser_.tree_buffer_shape(num_tree_buffers_, tree_buff_length_);
    max_mb_ = parser_.max_mb();
    pinit_  = parser_.p_init();
    this->init_empty_scene();
  }

  return true;
}

//: initializes existing scene given the parser's paths
bool boxm_ocl_scene::init_existing_scene()
{
  vcl_string dir, pref;
  parser_.paths(dir, pref);
  vcl_string block_path = dir + "/blocks.bin";
  vsl_b_ifstream is(block_path, vcl_ios_binary);
  if (!is) {
    vcl_cout<<"init_existing_scene: file "<<block_path<<" doesn't exist"<<vcl_endl;
    return false;
  }
  vsl_b_read(is, num_tree_buffers_);
  vsl_b_read(is, tree_buff_length_);
  vsl_b_read(is, tree_buffers_);
  vsl_b_read(is, blocks_);
  vsl_b_read(is, mem_ptrs_);
  is.close();
  
  //initialize max_mb based blockSize+treeSize+dataSize
  int numBlocks = blocks_.get_row1_count() * blocks_.get_row2_count() * blocks_.get_row3_count();
  int blockBytes = numBlocks * sizeof(int) * 4;
  int buffBytes = tree_buff_length_ * num_tree_buffers_ * sizeof(int) * 4;
  int dataBytes = tree_buff_length_ * num_tree_buffers_ * sizeof(float) * 16;
  max_mb_ = vcl_ceil( (blockBytes + buffBytes + dataBytes)/1024.0/1024.0 ); 
  return true;
}

//: initializes existing data given the parser's paths
bool boxm_ocl_scene::init_existing_data()
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


//: initializes empty scene with specifications from xml file
bool boxm_ocl_scene::init_empty_scene()
{
  vcl_cout<<"initializing empty scene ..."<<vcl_endl;
  //initialize 3d block structure - init to -1
  int4 init_blk(-1);
  vgl_vector_3d<unsigned> blk_nums = parser_.block_nums();
  blocks_ =  vbl_array_3d<int4>(blk_nums.x(), blk_nums.y(), blk_nums.z(), init_blk);

  /* compute total number of cells that can be allocated given max_mb */
  int MAX_BYTES = max_mb_ * 1024 * 1024;
  int total_blocks = blk_nums.x() * blk_nums.y() * blk_nums.z();
  int freeBytes = MAX_BYTES-total_blocks*4*sizeof(int); 
  int sizeofCell = 4*sizeof(int) + 20*sizeof(float); 
  int num_cells = (int) (freeBytes/sizeofCell);
  vcl_cout<<"Max Bytes "<<MAX_BYTES<<vcl_endl;
  vcl_cout<<"Num cells "<<num_cells<<vcl_endl;
  tree_buff_length_ = (int) (num_cells / num_tree_buffers_);
  vcl_cout<<"Tree shape: "<<num_tree_buffers_<<" by "<<tree_buff_length_<<" cells"<<vcl_endl;
  
  //tree buffers
  int4 init_cell(-1);
  tree_buffers_ = vbl_array_2d<int4>(num_tree_buffers_, tree_buff_length_, init_cell);

  //initialize book keeping mem ptrs
  int2 mem_init; mem_init[0] = 0; mem_init[1] = 1;
  mem_ptrs_ = vbl_array_1d<int2>(num_tree_buffers_, mem_init);

  //initialize data buffers
  float16 init_dat(0.0f);
  data_buffers_ =  vbl_array_2d<float16>(num_tree_buffers_, tree_buff_length_, init_dat);

  //now assign block ptrs to things
  //for each block[i,j,k], throw it's root tree randomly into one of the tree buffers
  vnl_random random(9667566);
  for (unsigned int i=0; i<blk_nums.x(); ++i) {
    for (unsigned int j=0; j<blk_nums.y(); ++j) {
      for (unsigned int k=0; k<blk_nums.z(); ++k) {
       
        //randomly choose a buffer, and get the first free spot in memory (update blocks)
        int buffIndex = random.lrand32(0, num_tree_buffers_-1);
        int buffOffset = mem_ptrs_[buffIndex][1]-1; //minus one cause mem_end points to one past the last one

        int4 blk(0);
        blk[0] = buffIndex;  //buffer index
        blk[1] = buffOffset; //buffer offset to root
        blk[2] = 1;          //tree has size of 1 now
        blk[3] = 0;          //nothign for now
        blocks_[i][j][k] = blk;

        //put root in that memory
        int4 root;
        root[0] = -1;         //no parent for root
        root[1] = -1;         // no children yet
        root[2] = buffOffset; // points to datum
        root[3] = 0;          // not used yet
        tree_buffers_[buffIndex][buffOffset] = root;

        //put data in memory
        float16 datum(0.0f);
        float bboxLen = (float) block_dim_.x();
        float alpha_init = (-1.0/bboxLen) * vcl_log(1.0-pinit_);
        datum[0] = alpha_init;
        data_buffers_[buffIndex][buffOffset] = datum;

        //make sure mem spot is now taken
        mem_ptrs_[buffIndex][1]++;
      }
    }
  }

  /* make a pass to ensure that all tree roots point back to their cell index */
  int blk_index = 0;
  vbl_array_3d<int4>::iterator blk_iter;
  for (blk_iter = blocks_.begin(); blk_iter != blocks_.end(); blk_iter++) {
    int4 blk = (*blk_iter);
    int buffIndex = blk[0];
    int buffOffset = blk[1];
    int4 treeRoot = tree_buffers_(buffIndex, buffOffset);
    treeRoot[3] = blk_index;
    tree_buffers_(buffIndex, buffOffset) = treeRoot;
    blk_index++;
  }

  return true;
}
//: initializes scene data assuming that the tree structure has already been initialized
bool boxm_ocl_scene::init_empty_data()
{
  //initialize data buffers
  float16 init_dat(0.0f);
  data_buffers_ = vbl_array_2d<float16>(num_tree_buffers_, tree_buff_length_, init_dat);
  return true;
}


//SETTERS from int and float arrays
void boxm_ocl_scene::set_blocks(int* block_ptrs)
{
  int index = 0;
  vbl_array_3d<int4>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); iter++) {
    (*iter)[0] = block_ptrs[index++];
    (*iter)[1] = block_ptrs[index++];
    (*iter)[2] = block_ptrs[index++];
    (*iter)[3] = block_ptrs[index++];
  }
}

void boxm_ocl_scene::set_tree_buffers(int* tree_buffers)
{
  int index = 0;
  vbl_array_2d<int4>::iterator iter;
  for (iter = tree_buffers_.begin(); iter != tree_buffers_.end(); iter++) {
    (*iter)[0] = tree_buffers[index++];
    (*iter)[1] = tree_buffers[index++];
    (*iter)[2] = tree_buffers[index++];
    (*iter)[3] = tree_buffers[index++];
  }
}

void boxm_ocl_scene::set_mem_ptrs(int* mem_ptrs)
{
  int index = 0;
  vbl_array_2d<int2>::iterator iter;
  for (iter = mem_ptrs_.begin(); iter != mem_ptrs_.end(); iter++) {
    (*iter)[0] = mem_ptrs[index++];
    (*iter)[1] = mem_ptrs[index++];
  }
}

void boxm_ocl_scene::set_alpha_values(float* alpha_buffer)
{
  int index = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    (*iter)[0] = alpha_buffer[index++];
  }
}

void boxm_ocl_scene::set_data_values(float* data_buffer)
{
  int datIndex = 0;
  vbl_array_2d<float16>::iterator iter;
  for (iter = data_buffers_.begin(); iter != data_buffers_.end(); iter++) {
    for (int j=0; j<16; j++)
      (*iter)[j] = data_buffer[datIndex++];
  }
}

/******************************** XML WRITE **************************/
//MAKE scene aware of its own appearance model
void x_write(vcl_ostream &os, boxm_ocl_scene& scene, vcl_string name)
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
  int num, len;
  scene.tree_buffer_shape(num,len);
  vsl_basic_xml_element buffers(TREE_INIT_TAG);
  buffers.add_attribute("num_buffers", num);
  buffers.add_attribute("buff_size", len);
  buffers.x_write(os);

  scene_elm.x_write_close(os);
}

/********************* stream I/O *****************************/
vcl_ostream& operator <<(vcl_ostream &s, boxm_ocl_scene& scene)
{
  //get shape of tree buffers
  int num, len;
  scene.tree_buffer_shape(num, len);

  //get block numbers in each dimension
  int x_num, y_num, z_num;
  scene.block_num(x_num, y_num, z_num);

  //get dimension of each block
  double x_dim, y_dim, z_dim;
  scene.block_dim(x_dim, y_dim, z_dim);

  //estimate size of scene
  //int sizeofCell = 4*sizeof(int) + 20*sizeof(float); // unused variable
  int sizeBlks = 4*sizeof(int)*x_num*y_num*z_num;
  int sizeTree = 4*sizeof(int)*num*len;
  int sizeData = 16*sizeof(float)*num*len;
  double scene_size = (sizeBlks + sizeTree + sizeData)/1024.0/1024.0;
  s <<"---OCL_SCENE--------------------------------\n"
    <<"path: "<<scene.path()<<vcl_endl
    <<"blocks:  [block_nums "<<x_num<<','<<y_num<<','<<z_num<<"] "
    <<"[blk_dim "<<x_dim<<','<<y_dim<<','<<z_dim<<"]\n"
    <<"blk levels: [init level "<<scene.init_level()<<"] "
    <<"[max level "<<scene.max_level()<<"]\n"
    <<"buffers: [num_buffs " << num << "] "
    <<"[buff_length " << len << "] "
    <<" [total cells "<< num*len <<"]\n"
    <<"size: "<< scene_size << " MB"<<vcl_endl;

  //print out buffer free space
  typedef vnl_vector_fixed<int,2> int2;
  typedef vnl_vector_fixed<int,4> int4;
  typedef vnl_vector_fixed<float,16> float16;
  vbl_array_2d<float16> data_buffers = scene.data_buffers();
  vbl_array_2d<int4> tree_buffers = scene.tree_buffers();
  vbl_array_1d<int2> mem_ptrs = scene.mem_ptrs();
  s << "free space: "<<vcl_endl;
  for (unsigned int i=0; i<mem_ptrs.size(); ++i) {
    int start=mem_ptrs[i][0];
    int end = mem_ptrs[i][1];
    int freeSpace = (start >= end)? start-end : tree_buffers.cols() - (end-start);
    s <<"     buff["<<i<<"]="<<freeSpace<<" free cells, "
      <<"ptrs @ ("<<mem_ptrs[i][0]<<", "<<mem_ptrs[i][1]<<')'<<vcl_endl;
  }
  s << '\n'
    <<"--------------------------------------------" << vcl_endl;


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

#endif

  return s;

#if 0  //print 3 corner blocks for now
   //list all of the blocks


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
#endif // 0
}

