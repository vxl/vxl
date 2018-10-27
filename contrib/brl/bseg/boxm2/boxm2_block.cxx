#include "boxm2_block.h"
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_distance.h>
//:
// \file
#include <boxm2/boxm2_util.h>

boxm2_block::boxm2_block(boxm2_block_id const& id, char* buff): version_(1)
{
  block_id_ = id;
  buffer_ = buff;
  this->b_read(buff);
  read_only_ = true;
  n_cells_ =this->recompute_num_cells();
}

boxm2_block::boxm2_block(boxm2_block_id const& id, boxm2_block_metadata const& data, char* buffer)
{
  version_ = data.version_;
  init_level_ = data.init_level_;
  max_level_  = data.max_level_;
  max_mb_     = int(data.max_mb_);
  sub_block_dim_ = data.sub_block_dim_;
  sub_block_num_ = data.sub_block_num_;
  local_origin_ = data.local_origin_;
  block_id_ = id;
  buffer_ = buffer;
  this->b_read(buffer_);
  read_only_ = true;
  n_cells_ =this->recompute_num_cells();
}

boxm2_block::boxm2_block(boxm2_block_metadata const& data)
{
  version_ = data.version_;
  block_id_ = data.id_;
  this->init_empty_block(data);
  read_only_ = false;  // make sure that it is written back to disc
  n_cells_ =this->recompute_num_cells();
}

unsigned boxm2_block::recompute_num_cells(){
  unsigned N = 0;
  for(auto it : this->trees()){
      boct_bit_tree curr_tree( (unsigned char*) it.data_block(),this->max_level_);
      N += curr_tree.num_cells();
  }
  return N;
}

bool boxm2_block::b_read(char* buff)
{
  if (version_ == 1)
  {
    long bytes_read = 0;

    //0. first 8 bytes denote size
    std::memcpy(&byte_count_, buff, sizeof(byte_count_));
    bytes_read += sizeof(byte_count_);

    //1. read init level, max level, max mb
    std::memcpy(&init_level_, buff+bytes_read, sizeof(init_level_));
    bytes_read += sizeof(init_level_);
    std::memcpy(&max_level_, buff+bytes_read, sizeof(max_level_));
    bytes_read += sizeof(max_level_);
    std::memcpy(&max_mb_, buff+bytes_read, sizeof(max_mb_));
    bytes_read += sizeof(max_mb_);

    //2. read in sub block dimension, sub block num
    double dims[4];
    std::memcpy(&dims, buff+bytes_read, sizeof(dims));
    bytes_read += sizeof(dims);
    int    nums[4];
    std::memcpy(&nums, buff+bytes_read, sizeof(nums));
    bytes_read += sizeof(nums);
    sub_block_dim_ = vgl_vector_3d<double>(dims[0], dims[1], dims[2]);
    sub_block_num_ = vgl_vector_3d<unsigned>(nums[0], nums[1], nums[2]);

    //4. setup big arrays (3d block of trees)
    auto* treesBuff = reinterpret_cast<uchar16*>(buff+bytes_read);
    trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                          sub_block_num_.y(),
                                          sub_block_num_.z(),
                                          treesBuff);
    return true;
  }
  else if (version_ == 2)
  {
    auto* treesBuff = reinterpret_cast<uchar16*>(buff);
    byte_count_ = sizeof(uchar16)* sub_block_num_.x()*sub_block_num_.y()*sub_block_num_.z();
    trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                          sub_block_num_.y(),
                                          sub_block_num_.z(),
                                          treesBuff);
    return true;
  }
  else
    return false;
}

//:
//  This type of writing is sort of counter intuitive, as the buffer
//  just needs to be returned and written to disk. The first few calls
//  ensure the meta data is lined up correctly.  To use this, just pass in
//  the boxm2_block buffer.
bool boxm2_block::b_write(char* buff)
{
  long bytes_written = 0;

  if (version_ == 1)
  {
    //0. writing total size
    std::memcpy(buff, &byte_count_, sizeof(byte_count_));
    bytes_written += sizeof(byte_count_);

    //1. write init level, max level, max mb
    std::memcpy(buff+bytes_written, &init_level_, sizeof(init_level_));
    bytes_written += sizeof(init_level_);
    std::memcpy(buff+bytes_written, &max_level_, sizeof(max_level_));
    bytes_written += sizeof(max_level_);
    std::memcpy(buff+bytes_written, &max_mb_, sizeof(max_mb_));
    bytes_written += sizeof(max_mb_);

    //2. Write sub block dimension, sub block num
    double dims[4] = {sub_block_dim_.x(), sub_block_dim_.y(), sub_block_dim_.z(), 0.0};
    std::memcpy(buff+bytes_written, dims, 4 * sizeof(double));
    bytes_written += 4 * sizeof(double);

    unsigned int nums[4] = {sub_block_num_.x(), sub_block_num_.y(), sub_block_num_.z(), 0 };
    std::memcpy(buff+bytes_written, nums, 4 * sizeof(unsigned int));
  }
  //the arrays themselves should be already in the char buffer, so no need to copy
  return true;
}


//: initializes empty scene given
// This method uses the max_mb parameter to determine how many data cells to
// allocate.  MAX_MB is assumed to include blocks, alpha, mog3, num_obs and 16 byte aux data
bool boxm2_block::init_empty_block(boxm2_block_metadata const& data)
{
#if 0 // unused constants
  //calc max number of bytes, data buffer length, and alpha init (consts)
  const int MAX_BYTES    = int(data.max_mb_)*1024*1024;
  const int BUFF_LENGTH  = 1L<<16; // 65536
#endif

  //total number of (sub) blocks in the scene
  int total_blocks =  data.sub_block_num_.x()
                    * data.sub_block_num_.y()
                    * data.sub_block_num_.z();

  //to initialize
  int num_buffers, blocks_per_buffer;

  //only 1 buffer, blocks per buffer is all blocks
  num_buffers = 1;
  blocks_per_buffer = total_blocks;
  std::cout<<"Num buffers: "<<num_buffers
          <<" .. num_trees: "<<blocks_per_buffer<<std::endl;

  //now construct a byte stream, and read in with b_read
  byte_count_ = calc_byte_count(num_buffers, blocks_per_buffer, total_blocks);
  init_level_ = data.init_level_;
  max_level_  = data.max_level_;
  max_mb_     = int(data.max_mb_);
  local_origin_ = data.local_origin_;
  buffer_ = new char[byte_count_];

  //get member variable metadata straight, then write to the buffer
  long bytes_read = 0;

//double dims[4];
  int nums[4]; nums[0]=0;//remove warning about unused variable

  if (version_==1)
  {
  bytes_read += sizeof(byte_count_);   //0. first 8 bytes denote size
  bytes_read += sizeof(init_level_);   //1. read init level, max level, max mb
  bytes_read += sizeof(max_level_);
  bytes_read += sizeof(max_mb_);
//bytes_read += sizeof(dims);          //2. read in sub block dimension, sub block num
  bytes_read += sizeof(nums);
  }
  sub_block_dim_ = data.sub_block_dim_;
  sub_block_num_ = data.sub_block_num_;

  //4. setup big arrays (3d block of trees)
  auto* treesBuff = reinterpret_cast<uchar16*>(buffer_+bytes_read);
  trees_     = boxm2_array_3d<uchar16>( sub_block_num_.x(),
                                        sub_block_num_.y(),
                                        sub_block_num_.z(),
                                        treesBuff);
  //--- Now initialize blocks and their pointers --------- ---------------------
  //6. initialize blocks in order
  int tree_index = 0;
  boxm2_array_3d<uchar16>::iterator iter;
  for (iter = trees_.begin(); iter != trees_.end(); ++iter)
  {
    //initialize empty tree
    uchar16 treeBlk( (unsigned char) 0 );

    //store root data index in bits [10, 11, 12, 13] ;
    treeBlk[10] = (tree_index) & 0xff;
    treeBlk[11] = (tree_index>>8)  & 0xff;
    treeBlk[12] = (tree_index>>16) & 0xff;
    treeBlk[13] = (tree_index>>24) & 0xff;

    //Set Init_Level, 1=just root, 2=2 generations, 3=3 generations, 4=all four
    if (init_level_== 1) {
      treeBlk[0] = 0;
      ++tree_index;
    }
    else if (init_level_ == 2){
      treeBlk[0] = 1;
      tree_index += 9;                //root + 1st
    }
    else if (init_level_ == 3) {
      treeBlk[0] = 1;
      treeBlk[1] = 0xff;
      tree_index += 1 + 8 + 64;       //root + 1st + 2nd
    }
    else if (init_level_ == 4) {
      treeBlk[0] = 1;
      for (int i=1; i<1+9; ++i)
        treeBlk[i] = 0xff;
      tree_index += 1 + 8 + 64 + 512; // root + 1st + 2nd + 3rd...
    }

    //store this tree in block bytes
    for (int i=0; i<16; i++)
      (*iter)[i] = treeBlk[i];
  }
  return true;
}


//: Given number of buffers, number of trees in each buffer, and number of total trees (x*y*z number)
// \return size of byte stream
long boxm2_block::calc_byte_count(int num_buffers, int trees_per_buffer, int num_trees)
{
  long toReturn = num_trees * sizeof(uchar16) ;
  if (version_ == 1)
  {
    toReturn += num_buffers*trees_per_buffer * sizeof(int)     //tree pointers
              + num_buffers*(sizeof(ushort) + sizeof(ushort2)) //blocks in buffers and mem ptrs
              + sizeof(long)                                   // this number
              + 3*sizeof(int)                                  // init level, max level, max_mb
              + 4*sizeof(double)                               // dims
              + 4*sizeof(int)                                  // nums
              + sizeof(int) + sizeof(int)                      // numBuffers, treeLen
    ;
  }
  return toReturn;
}
vgl_box_3d<double> boxm2_block::bounding_box_global() const{
  vgl_vector_3d<double> diag(sub_block_dim_.x()*sub_block_num_.x(),
                             sub_block_dim_.y()*sub_block_num_.y(),
                             sub_block_dim_.z()*sub_block_num_.z());
  return vgl_box_3d<double>(local_origin_,local_origin_+diag);
}

bool boxm2_block::contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<double>& local_pt) const{
  vgl_box_3d<double> bbox = this->bounding_box_global();
  if(bbox.contains(global_pt.x(), global_pt.y(), global_pt.z())) {
    double local_x=(global_pt.x()-local_origin_.x())/sub_block_dim_.x();
    double local_y=(global_pt.y()-local_origin_.y())/sub_block_dim_.y();
    double local_z=(global_pt.z()-local_origin_.z())/sub_block_dim_.z();
    local_pt.set(local_x, local_y, local_z);
    return true;
  }
  return false;
}
bool boxm2_block::contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<int>& local_pt) const{
  vgl_point_3d<double> p;
  if(!this->contains(global_pt, p))
    return false;
  int index_x=(int)std::floor(p.x());
  int index_y=(int)std::floor(p.y());
  int index_z=(int)std::floor(p.z());
  local_pt.set(index_x, index_y, index_z);
  return true;
}

bool boxm2_block::contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<double>& local_tree_coords,
                           vgl_point_3d<double>& cell_center, double& side_length) const{
  if(!this->contains(global_pt, local_tree_coords))
    return false;
  int index_x=(int)std::floor(local_tree_coords.x());
  int index_y=(int)std::floor(local_tree_coords.y());
  int index_z=(int)std::floor(local_tree_coords.z());
  if(index_x >= this->trees_.get_row1_count() || index_y >= this->trees_.get_row2_count() || index_z>= this->trees_.get_row3_count())
    return false;

  vnl_vector_fixed<unsigned char,16> treebits=trees_(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),max_level_);
  int bit_index=tree.traverse(local_tree_coords);
  unsigned depth=tree.depth_at(bit_index);
  side_length=static_cast<float>(sub_block_dim_.x()/((float)(1<<depth)));
  cell_center = tree.cell_center(bit_index);
  cell_center.set(cell_center.x()+index_x, cell_center.y()+index_y,cell_center.z()+index_z);
  return true;
}

bool boxm2_block::data_index(vgl_point_3d<double> const& global_pt, unsigned& index, unsigned& depth, double& side_length) const{
  vgl_point_3d<double> loc;
  if(!this->contains(global_pt, loc))
    return false;
  int index_x=(int)std::floor(loc.x());
  int index_y=(int)std::floor(loc.y());
  int index_z=(int)std::floor(loc.z());
  //could be on block boundary so double check array bounds
  if(index_x<0||index_x>=trees_.get_row1_count()) return false;
  if(index_x<0||index_y>=trees_.get_row2_count()) return false;
  if(index_x<0||index_z>=trees_.get_row3_count()) return false;
  vnl_vector_fixed<unsigned char,16> treebits=trees_(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),max_level_);
  int bit_index=tree.traverse(loc);
  depth=tree.depth_at(bit_index);
  index=tree.get_data_index(bit_index,false);
  side_length=static_cast<float>(sub_block_dim_.x()/((float)(1<<depth)));
  return true;
}
bool boxm2_block::data_index(vgl_point_3d<double> const& global_pt, unsigned& index) const{
  unsigned depth;
  double cell_side_length;
  return this->data_index(global_pt, index, depth, cell_side_length);
}

std::vector<cell_info> boxm2_block::cells_in_box(vgl_box_3d<double> const& global_box){
  std::vector<cell_info> temp;
  vgl_box_3d<double> bbox = this->bounding_box_global();
  vgl_box_3d<double> inter = vgl_intersection<double>(global_box, bbox);
  if(inter.is_empty())
    return temp;
  double dx = sub_block_dim_.x(), dy = sub_block_dim_.y(), dz = sub_block_dim_.z();
  // get sub_block bounds with bounds checks
  vgl_point_3d<double> min_pt = inter.min_point();
  double local_x_min =(min_pt.x()-local_origin_.x())/dx;
  double local_y_min =(min_pt.y()-local_origin_.y())/dy;
  double local_z_min =(min_pt.z()-local_origin_.z())/dz;

  int index_x_min=(int)std::floor(local_x_min);
  if(index_x_min<0) index_x_min = 0;

  int index_y_min=(int)std::floor(local_y_min);
   if(index_y_min<0) index_y_min = 0;

  int index_z_min=(int)std::floor(local_z_min);
  if(index_z_min<0) index_z_min = 0;

  vgl_point_3d<double> max_pt = inter.max_point();
  double local_x_max =(max_pt.x()-local_origin_.x())/dx;
  double local_y_max =(max_pt.y()-local_origin_.y())/dy;
  double local_z_max =(max_pt.z()-local_origin_.z())/dz;

  int index_x_max=(int)std::floor(local_x_max);
  int nx = static_cast<int>(trees_.get_row1_count());
  if(index_x_max >=nx) index_x_max = nx-1;

  int index_y_max=(int)std::floor(local_y_max);
  int ny = static_cast<int>(trees_.get_row2_count());
  if(index_y_max >=ny) index_y_max = ny-1;

  int index_z_max=(int)std::floor(local_z_max);
  int nz = static_cast<int>(trees_.get_row3_count());
  if(index_z_max >=nz) index_z_max = nz-1;

  // iterate over sub_blocks
  vgl_point_3d<double> loc;
  for(int iz = index_z_min; iz<=index_z_max; ++iz){
    for(int iy = index_y_min; iy<=index_y_max; ++iy){
      for(int ix = index_x_min; ix<=index_x_max; ++ix){
        cell_info ci;
        // get the tree for the current sub_block
        vnl_vector_fixed<unsigned char,16> treebits=trees_(ix,iy,iz);
        boct_bit_tree tree(treebits.data_block(),max_level_);
        // iterate over the leaves of the tree
        std::vector<int> leafBits = tree.get_leaf_bits(0,max_level_);
        std::vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
           int currBitIndex = (*iter);
           vgl_point_3d<double> localCenter = tree.cell_center(currBitIndex);
           vgl_point_3d<double> cellCenter(localCenter.x() + ix, localCenter.y()+ iy, localCenter.z() + iz);
           ci.cell_center_.set(cellCenter.x() * dx +local_origin_.x(),
                               cellCenter.y() * dy +local_origin_.y(),
                               cellCenter.z() * dz +local_origin_.z());
           // if tree leaf center is in the itersection box, include it in the returned centers
           if(!inter.contains(ci.cell_center_))
             continue;

           int curr_depth = tree.depth_at(currBitIndex);
           ci.depth_= curr_depth;
           int currIdx = tree.get_data_index(currBitIndex); //data index
           ci.data_index_=currIdx;
           ci.side_length_= static_cast<float>(dx)/static_cast<float>(1<<curr_depth);
           temp.push_back(ci);
        }
      }
    }
  }
  return temp;
}

std::vector<vgl_point_3d<double> > boxm2_block::sub_block_neighbors(vgl_point_3d<double> const& probe, double distance) const{
  std::vector<vgl_point_3d<double> > ret;
  double r = sub_block_dim_.x();//assume cubical tree subblock
  double dr = std::floor(distance/r) + r;// add r as margin for roundoff
  vgl_box_3d<double> bb = this->bounding_box_global();

  // find the voxel center (x0, y0, z0) containing the probe
  vgl_vector_3d<double> loc = (probe-local_origin_)/r;
  double x0 = std::floor(loc.x()), y0 = std::floor(loc.y()), z0 = std::floor(loc.z());
  x0 = x0*r + local_origin_.x();   y0 = y0*r + local_origin_.y();  z0 = z0*r + local_origin_.z();
  // scan the NxNxN neigborhood around the origin voxel
  for(double x = (x0-dr); x<=(x0+dr); x+=r)
    for(double y = (y0-dr); y<=(y0+dr); y+=r)
      for(double z = (z0-dr); z<=(z0+dr); z+=r){
        vgl_point_3d<double> p(x, y, z);//block cell of probe is also a neighbor
        if(!bb.contains(p)) // be sure neighbor is inside block
          continue;
        double d = vgl_distance(p, probe);
        if(d<=distance)
          ret.push_back(p);
      }
  return ret;
}
void boxm2_block::leaf_neighbors(vgl_point_3d<double> const& probe, double distance, std::vector<vgl_point_3d<double> >& nbrs, std::vector<double>& nbr_edge_lengths, std::vector<unsigned>& data_indices, bool relative_distance) const{
  nbrs.clear();
  nbr_edge_lengths.clear();
  data_indices.clear();
  vgl_box_3d<double> gbox = this->bounding_box_global();
  if(!gbox.contains(probe))
    return;
  // find the tree index that contains probe
  vgl_vector_3d<double> v0 = probe-local_origin_;
  double dx = v0.x()/sub_block_dim_.x(), dy = v0.y()/sub_block_dim_.y(), dz = v0.z()/sub_block_dim_.z();
  int ix = static_cast<int>(std::floor(dx)), iy = static_cast<int>(std::floor(dy)), iz = static_cast<int>(std::floor(dz));

  // find tree neighborhood radius and relative or absolute neighbor distance
  int dri = static_cast<int>(distance/sub_block_dim_.x()) + 1;// add 1 as margin for roundoff
  double cell_distance = distance;//absolute spatial distance
  if(relative_distance){
    vnl_vector_fixed<unsigned char, 16>  probe_tree = trees_(ix, iy, iz);
    boct_bit_tree probe_bit_tree((unsigned char*) probe_tree.data_block(), max_level_);
    vgl_point_3d<double> pt(dx-double(ix), dy-double(iy),dz-double(iz));
    int bit_index = probe_bit_tree.traverse(pt);
    double probe_len = probe_bit_tree.cell_len(bit_index);
    dri = static_cast<int>(distance*probe_len) + 1;// add 1 to insure 3x3x3 search in neighboring trees
    cell_distance = distance*probe_len*sub_block_dim_.x();//here the distance is scaled to size of probe cell
  }
  // find neighboring trees
  for(int z = iz-dri; z<=iz+dri; ++z)
    for(int y = iy-dri; y<=iy+dri; ++y)
      for(int x = ix-dri; x<=ix+dri; ++x){
        // check bounds
        if(x<int(0) || y<int(0) ||z<int(0) || x>=int(sub_block_num_.x()) || y>=int(sub_block_num_.y()) || z>=int(sub_block_num_.z()))
          continue;
        vnl_vector_fixed<unsigned char, 16>  tree = trees_(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), max_level_);

        //compute global origin of tree sub_block
        vgl_vector_3d<double> v(x*sub_block_dim_.x(), y*sub_block_dim_.y(), z*sub_block_dim_.z());
        vgl_point_3d<double> subblock_origin = local_origin_ + v;

        //iterate through leaves of the tree
        std::vector<int> leafBits = bit_tree.get_leaf_bits();
        for (int currBitIndex : leafBits) {
          int data_indx = bit_tree.get_data_index(currBitIndex); //data index
          vgl_point_3d<double> cell_pos = bit_tree.cell_center(currBitIndex);
          vgl_vector_3d<double> cell_offset(cell_pos.x()*sub_block_dim_.x(), cell_pos.y()*sub_block_dim_.y(), cell_pos.z()*sub_block_dim_.z());
          // global position of leaf cell center
          vgl_point_3d<double> pos = subblock_origin + cell_offset;
          double side_len = bit_tree.cell_len(currBitIndex);
          double d = (probe-pos).length();
          if(d<=cell_distance){
            nbrs.push_back(pos);
            nbr_edge_lengths.push_back(side_len*sub_block_dim_.x());
            data_indices.push_back(data_indx);
          }
        }
      }
}

std::vector<vgl_point_3d<int> >  boxm2_block::sub_blocks_intersect_box(vgl_box_3d<double> const& box) const {
  //std::cout << "Box " << box << '\n';
  std::vector<vgl_point_3d<int> > ret;
  int nx = static_cast<int>(sub_block_num_.x()), ny = static_cast<int>(sub_block_num_.y()), nz = static_cast<int>(sub_block_num_.z());
  vgl_point_3d<double> minp = box.min_point();
  vgl_vector_3d<double> vmin= (minp-local_origin_);
  double x_min = vmin.x()/sub_block_dim_.x(), y_min = vmin.y()/sub_block_dim_.y(), z_min = vmin.z()/sub_block_dim_.z();
  int ix_min = static_cast<int>(std::floor(x_min)), iy_min = static_cast<int>(std::floor(y_min)), iz_min = static_cast<int>(std::floor(z_min));
  ix_min--; iy_min--; iz_min--; //allow corners to intersect box as origins

  vgl_point_3d<double> maxp = box.max_point();
  vgl_vector_3d<double> vmax= (maxp-local_origin_);
  double x_max = vmax.x()/sub_block_dim_.x(), y_max = vmax.y()/sub_block_dim_.y(), z_max = vmax.z()/sub_block_dim_.z();
  int ix_max = static_cast<int>(std::floor(x_max)), iy_max = static_cast<int>(std::floor(y_max)), iz_max = static_cast<int>(std::floor(z_max));
  ix_max++;   iy_max++;   iz_max++; // allow corners to intersect box as origins

  // modify box to include origins below min and above max by 1 box length
  // handles round off problems
  vgl_point_3d<double> pmin(x_min-1.0, y_min-1.0, z_min-1.0);
  vgl_point_3d<double> pmax(x_max+1.0, y_max+1.0, z_max+1.0);
  vgl_box_3d<double> mbox;
  mbox.add(pmin); mbox.add(pmax);
  for(int x = ix_min; x<=ix_max; x++){
    if(x<0 || x>=nx)
      continue;
    for(int y = iy_min; y<=iy_max; y++){
      if(y<0 || y>=ny)
        continue;
      for(int z = iz_min; z<=iz_max; z++){
        if(z<0 || z>=nz)
          continue;
        vgl_point_3d<int> p(x, y, z);
        vgl_point_3d<double> pd(x, y, z);
        if(mbox.contains(pd))
          ret.push_back(p);
      }
    }
  }
  return ret;
}

//------------ I/O -------------------------------------------------------------
std::ostream& operator <<(std::ostream &s, boxm2_block& block)
{
  return
  s << "Block ID=" << block.block_id() << '\n'
    << "Byte Count=" << block.byte_count() << '\n'
    << "Init level=" << block.init_level() << '\n'
    << "Max level=" << block.max_level() << '\n'
    << "Max MB=" << block.max_mb() << '\n'
    << "Sub Block Dim=" << block.sub_block_dim() << '\n'
    << "Sub Block Num=" << block.sub_block_num() << '\n'
    << "Local Origin " << block.local_origin() << std::endl;
}

//: Binary write boxm2_block to stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_write(vsl_b_ostream&, boxm2_block_sptr const&) {}

//: Binary load boxm2_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, boxm2_block_sptr&) {}
//: Binary load boxm2_block from stream.
// DUMMY IMPLEMENTATION: does nothing!
void vsl_b_read(vsl_b_istream&, boxm2_block_sptr const&) {}
