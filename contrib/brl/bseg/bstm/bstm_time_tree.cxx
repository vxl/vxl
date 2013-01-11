//:
// \file
#include "bstm_time_tree.h"
#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vcl_cstring.h> // for std::memcpy()

//: default constructor
bstm_time_tree::bstm_time_tree()
{
  bits_ = new unsigned char[TT_NUM_BYTES];
  vcl_memset(bits_, 0, TT_NUM_BYTES);
}

//: copy constructor
bstm_time_tree::bstm_time_tree(const bstm_time_tree& other)
{
  bits_ = new unsigned char[TT_NUM_BYTES];
  num_levels_ = other.number_levels();
  vcl_memcpy(bits_, other.get_bits(), TT_NUM_BYTES);
}

//: constructor from an array of char bits
bstm_time_tree::bstm_time_tree(const unsigned char* bits, int num_levels)
{
    bits_ = new unsigned char[TT_NUM_BYTES];

    //initialize num levels, bits
    num_levels_ = vcl_min(TT_NUM_LVLS,num_levels);
    //copy bytes
    vcl_memcpy(bits_, bits, TT_NUM_BYTES);
}

int  bstm_time_tree::max_num_cells() const
{
  return int(vcl_pow(2.0f,(float)num_levels_) - 1.0);
}

int bstm_time_tree::max_num_inner_cells() const
{
  return int(vcl_pow(2.0f,(float)num_levels_-1) - 1.0);
}

int bstm_time_tree::depth_at(const int index) const
{
  return (int)(vcl_log(index+1)/vcl_log(2));
}

unsigned char bstm_time_tree::bit_at(int index) const
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 30)
    return 0;

  int byte_index = vcl_floor(index/8);
  int bit_index = index%8;
#ifdef DEBUG
  vcl_cout << "query: " << index << " byte index: " << byte_index << " bit index: " << bit_index << vcl_endl;
#endif
  return (1<<(7-bit_index) & bits_[byte_index]) ? 1 : 0;
}

void bstm_time_tree::set_bit_at(int index, bool val)
{
  if (index > 30) {
    vcl_cerr<<"No bit above 30, bad set call!\n";
    return;
  }

  int byte_index = vcl_floor(index/8);
  int bit_index = index%8;
  unsigned char mask = 1<<(7-bit_index);
  unsigned char byte = bits_[byte_index];
  bits_[byte_index] = (val) ? (byte | mask) : (byte & (mask ^ 0xFF));
#ifdef DEBUG
  vcl_cout << "byte: " << (int)byte << " mask: " << (int)mask << " after " << (int)bits_[byte_index] << vcl_endl;
#endif
}

int bstm_time_tree::get_data_ptr()
{
  return int((bits_[7]<<24) | (bits_[6]<<16) | (bits_[5]<<8) | (bits_[4]));
}

void bstm_time_tree::set_data_ptr(int ptr)
{
  this->bits_[4] = (ptr) & 0xff;
  this->bits_[5] = (ptr>>8)  & 0xff;
  this->bits_[6] = (ptr>>16) & 0xff;
  this->bits_[7] = (ptr>>24) & 0xff;
}


//: Return cell with a particular locational code
int bstm_time_tree::get_data_index(int bit_index) const
{
  int root_data_ptr = (int) (bits_[7]<<24) | (bits_[6]<<16) | (bits_[5]<<8) | (bits_[4]);
  return root_data_ptr + this->get_relative_index(bit_index);
}

//: returns bit index assuming root data is located at 0
int  bstm_time_tree::get_relative_index(int bit_index) const
{
  if (bit_index < 3)
    return bit_index;

  //otherwise get parent index, parent byte index and relative bit index
  const unsigned char oneuplevel = parent_index(bit_index);   //bit index of parent
  unsigned char byte_index = vcl_floor(oneuplevel/8);         //byte where parent is found

  //count pre parent bits
  int count=0;
  for (int i=0; i<byte_index; ++i)
    count += bit_lookup[bits_[i]];

  //dont forget parent bits occurring the parent BYTE
  unsigned char sub_bit_index = 8- oneuplevel%8;
  unsigned char temp = bits_[byte_index]>>sub_bit_index;

#ifdef DEBUG
  vcl_cout << "parent bit: " << (int)oneuplevel << '\n'
           << "count up tp parents bit: " << count << '\n'
           << "sub_bit_index: " << (int)sub_bit_index << '\n'
           << "bits_[byte_index]: " << (int)bits_[byte_index] << '\n'
           << "bits_[byte_index] (after shift): " << (int)temp << '\n'
           << "bit_lookup[temp]: " << (int)bit_lookup[temp] << vcl_endl;
#endif

  count = count + bit_lookup[temp];

  unsigned char finestleveloffset=(bit_index-1)&(2-1);
  count = 2*count+1 +finestleveloffset;

  return count;
}

int bstm_time_tree::traverse(const double t, int deepest) const
{
  //deepest level to traverse is either
  deepest = vcl_max(deepest-1, num_levels_-1);

  int depth = 0;

  //bit index to be returned
  int bit_index = 0;

  //clamp point
  double range_min = 0;
  double range_max = cell_len(0);

  //if outside of range
  if (t < range_min || t >= range_max)
    return -1;

  // while the curr node has children
  while (bit_at(bit_index) && depth < deepest ) {
    if (t < range_min + (range_max-range_min)/2 )
      bit_index = (2*bit_index + 1);                      //left child
    else
      bit_index = (2*bit_index + 2);                      //right child

    depth++;

    if (t < range_min + (range_max-range_min)/2 )
      range_max -= cell_len(bit_index);
    else
      range_min += cell_len(bit_index);
  }
  return bit_index;
}

bool bstm_time_tree::valid_cell(int bit_index)
{
  return (bit_index==0) || this->bit_at(parent_index(bit_index));
}

bool bstm_time_tree::is_leaf(int bit_index)
{
  return this->valid_cell(bit_index) && (this->bit_at(bit_index)==0);
}

//: Return number of cells in this tree
int bstm_time_tree::num_cells() const
{
  //count bits for each byte
  int count = 0 ;
  for (int i=0; i<4; i++) {
    unsigned char n = bits_[i];
    while (n)  {
      ++count;
      n &= (n - 1) ;
    }
  }
  return 2*count+1;
}


//returns bit indices of leaf nodes under rootBit
vcl_vector<int> bstm_time_tree::get_leaf_bits(int rootBit)
{
  //use num cells to accelerate (cut off for loop)
  vcl_vector<int> leafBits;

  //special root case
  if ( bits_[0] == 0 && rootBit == 0 ) {
    leafBits.push_back(0);
    return leafBits;
  }

  //otherwise calc list of bit indices in the subtree of rootBIT, and then verify leaves
  vcl_vector<int> subTree;
  vcl_list<unsigned> toVisit;
  toVisit.push_back(rootBit);
  while (!toVisit.empty()) {
    int currBitIndex = toVisit.front();
    toVisit.pop_front();
    if ( this->is_leaf(currBitIndex) ) {
      subTree.push_back(currBitIndex);
    }
    else { //add children to the visit list
      unsigned firstChild = 2 * currBitIndex + 1;
      for (int ci = 0; ci < 2; ++ci)
        toVisit.push_back( firstChild + ci );
    }
  }
  return subTree;
}

float bstm_time_tree::cell_center(int bit_index) const
{
  //Indexes into precomputed cell_center matrix
  return cell_centers[bit_index];
}

void bstm_time_tree::cell_range(int bit_index, float& min, float& max) const
{
  double half_len = cell_len(bit_index) / 2.0;
  min = cell_center(bit_index)-half_len;
  max = cell_center(bit_index)+half_len;
}

float bstm_time_tree::cell_len(int bit_index) const
{
  if (bit_index==0)
    return 1;
  else if (bit_index<3)
    return 0.5;
  else if (bit_index<7)
    return 0.25;
  else if (bit_index<15)
    return 0.125;
  else if (bit_index<31)
    return 0.0625;
  else
    return 0.03125;
}

unsigned char bstm_time_tree::bit_lookup[] =
{ 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
  1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
  1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
  1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
  2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
  3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
  3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
  4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8
};

float bstm_time_tree::cell_centers[]=
{
 0.5,
 0.25, 0.75, 0.125, 0.375,
 0.625, 0.875, 0.0625, 0.1875, 0.3125, 0.4375, 0.5625, 0.6875,
 0.8125, 0.9375, 0.03125, 0.09375, 0.15625, 0.21875, 0.28125, 0.34375, 0.40625, 0.46875, 0.53125, 0.59375, 0.65625, 0.71875, 0.78125, 0.84375,
 0.90625, 0.96875, 0.015625, 0.046875, 0.078125, 0.10938, 0.14062, 0.17188, 0.20312, 0.23438, 0.26562, 0.29688, 0.32812, 0.35938, 0.39062,
 0.42188, 0.45312, 0.48438, 0.51562, 0.54688, 0.57812, 0.60938, 0.64062, 0.67188, 0.70312, 0.73438, 0.76562, 0.79688, 0.82812, 0.85938, 0.89062,
 0.92188, 0.95312, 0.98438
};

#if 0
//------ I/O ----------------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, bstm_time_tree &t)
{
  s << "bstm_time_tree:\n"
    << "Tree bits:\n"
    << "depth 0: "<< (int) (t.bit_at(0))
    << '\n';

  //one
  s << "depth 1:";
  for (int i=1; i<9; i++)
    s << "  " << (int) t.bit_at(i);
  s << '\n';

  //two
  s << "depth 2:";
  for (int i=9; i<73; i++)
    s << "  " << (int) t.bit_at(i);
  s << '\n';

  return s;
}
#endif // 0

