//:
// \file
#include <algorithm>
#include <cstring>
#include <iostream>
#include <list>
#include "bstm_time_tree.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: default constructor
bstm_time_tree::bstm_time_tree() : is_owning_(true) {
  bits_ = new unsigned char[TT_NUM_BYTES];
  std::memset(bits_, 0, TT_NUM_BYTES);
}

//: copy constructor
bstm_time_tree::bstm_time_tree(const bstm_time_tree &other) : is_owning_(true) {
  bits_ = new unsigned char[TT_NUM_BYTES];
  num_levels_ = other.number_levels();
  std::memcpy(bits_, other.get_bits(), TT_NUM_BYTES);
}

//: constructor from an array of char bits
bstm_time_tree::bstm_time_tree(const unsigned char *bits, int num_levels)
    : is_owning_(true) {
  bits_ = new unsigned char[TT_NUM_BYTES];

  // initialize num levels, bits
  num_levels_ = std::min(TT_NUM_LVLS, num_levels);
  // copy bytes
  std::memcpy(bits_, bits, TT_NUM_BYTES);
}

int bstm_time_tree::max_num_cells() const {
  return int(std::pow(2.0f, (float)num_levels_) - 1.0);
}

int bstm_time_tree::max_num_inner_cells() const {
  return int(std::pow(2.0f, (float)num_levels_ - 1) - 1.0);
}

int bstm_time_tree::depth_at(const int index) const {
  return (int)(std::log((double)index + 1) / std::log(2.0));
}

unsigned char bstm_time_tree::bit_at(int index) const {
  // make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 30)
    return 0;

  int byte_index =
      index /
      8; // is already effectively a "floor()"; no need for std::floor() here
  int bit_index = index % 8;
#ifdef DEBUG
  std::cout << "query: " << index << " byte index: " << byte_index
            << " bit index: " << bit_index << std::endl;
#endif
  return (1 << (7 - bit_index) & bits_[byte_index]) ? 1 : 0;
}

void bstm_time_tree::set_bit_at(int index, bool val) {
  if (index > 30) {
    std::cerr << "No bit above 30, bad set call!\n";
    return;
  }

  int byte_index =
      index /
      8; // is already effectively a "floor()"; no need for std::floor() here
  int bit_index = index % 8;
  unsigned char mask = 1 << (7 - bit_index);
  unsigned char byte = bits_[byte_index];
  bits_[byte_index] = (val) ? (byte | mask) : (byte & (mask ^ 0xFF));
#ifdef DEBUG
  std::cout << "byte: " << (int)byte << " mask: " << (int)mask << " after "
            << (int)bits_[byte_index] << std::endl;
#endif
}

void bstm_time_tree::set_bit_and_parents_to_true(int index) {
  while (index > 0) {
    this->set_bit_at(index, true);
    index = parent_index(index);
  }
  this->set_bit_at(0, true);
}

//: erase tree to only the root, retain the data ptr.
void bstm_time_tree::erase_cells() {
  unsigned char zeros[TT_NUM_BYTES];
  std::fill_n(zeros, TT_NUM_BYTES, (unsigned char)0);

  int data_ptr = this->get_data_ptr();
  std::memcpy(bits_, zeros, TT_NUM_BYTES);
  this->set_data_ptr(data_ptr);
}

int bstm_time_tree::get_data_ptr() const {
  return int((bits_[7] << 24) | (bits_[6] << 16) | (bits_[5] << 8) |
             (bits_[4]));
}

void bstm_time_tree::set_data_ptr(int ptr) {
  this->bits_[4] = (ptr)&0xff;
  this->bits_[5] = (ptr >> 8) & 0xff;
  this->bits_[6] = (ptr >> 16) & 0xff;
  this->bits_[7] = (ptr >> 24) & 0xff;
}

//: Return cell with a particular locational code, the bit index must be a leaf
int bstm_time_tree::get_data_index(int bit_index) const {
  int root_data_ptr =
      (int)(bits_[7] << 24) | (bits_[6] << 16) | (bits_[5] << 8) | (bits_[4]);
  int relative_index = this->get_relative_index(bit_index);
  if (relative_index == -1)
    return -1;
  else
    return root_data_ptr + relative_index;
}

#if 0 // OLD DEPRECATED CODE

//: returns bit index assuming root data is located at 0
int  bstm_time_tree::get_relative_index(int bit_index) const
{
  if (bit_index < 3)
    return bit_index;

  //otherwise get parent index, parent byte index and relative bit index
  const unsigned char oneuplevel = parent_index(bit_index);   //bit index of parent
  unsigned char byte_index = oneuplevel/8;                    //byte where parent is found

  //count pre parent bits
  int count=0;
  for (int i=0; i<byte_index; ++i)
    count += bit_lookup[bits_[i]];

  //dont forget parent bits occurring the parent BYTE
  unsigned char sub_bit_index = 8- oneuplevel%8;
  unsigned char temp = bits_[byte_index]>>sub_bit_index;

#ifdef DEBUG
  std::cout << "parent bit: " << (int)oneuplevel << '\n'
           << "count up tp parents bit: " << count << '\n'
           << "sub_bit_index: " << (int)sub_bit_index << '\n'
           << "bits_[byte_index]: " << (int)bits_[byte_index] << '\n'
           << "bits_[byte_index] (after shift): " << (int)temp << '\n'
           << "bit_lookup[temp]: " << (int)bit_lookup[temp] << std::endl;
#endif

  count = count + bit_lookup[temp];

  unsigned char finestleveloffset=(bit_index-1)&(2-1);
  count = 2*count+1 +finestleveloffset;

  return count;
}

#endif // OLD DEPRECATED CODE

//: returns bit index assuming root data is located at 0
//  \p bit_index must be a leaf, otherwise returns -1
int bstm_time_tree::get_relative_index(int bit_index) const {
  if (!is_leaf(bit_index))
    return -1;

  // if looking for root
  if (bit_index == 0)
    return 0;

  // initialize stack with the root node
  unsigned char stack[TT_NUM_LVLS];
  stack[0] = 0;
  int ptr = 0;

  unsigned char curr_bit;
  int count = 0;

  while (ptr >= 0) {
    curr_bit = stack[ptr--]; // pop
    if (curr_bit == bit_index)
      break;

    if (!is_leaf(curr_bit)) // push right child and then left child
    {
      stack[++ptr] = (2 * curr_bit + 2);
      stack[++ptr] = (2 * curr_bit + 1);
    } else // reached leaf, increment count
      count++;
  }

  return count;
}

int bstm_time_tree::traverse(const double t, int deepest) const {
  // deepest level to traverse is either
  deepest = std::max(deepest - 1, num_levels_ - 1);

  int depth = 0;

  // bit index to be returned
  int bit_index = 0;

  // clamp point
  double range_min = 0;
  double range_max = cell_len(0);

  // if outside of range
  if (t < range_min || t >= range_max)
    return -1;

  // while the curr node has children
  while (bit_at(bit_index) && depth < deepest) {
    if (t < range_min + (range_max - range_min) / 2)
      bit_index = (2 * bit_index + 1); // left child
    else
      bit_index = (2 * bit_index + 2); // right child

    depth++;

    if (t < range_min + (range_max - range_min) / 2)
      range_max -= cell_len(bit_index);
    else
      range_min += cell_len(bit_index);
  }
  return bit_index;
}

bool bstm_time_tree::valid_cell(int bit_index) const {
  return (bit_index == 0) || this->bit_at(parent_index(bit_index));
}

bool bstm_time_tree::is_leaf(int bit_index) const {
  return this->valid_cell(bit_index) && (this->bit_at(bit_index) == 0);
}

//: Return number of cells in this tree
int bstm_time_tree::num_cells() const {
  // count bits for each byte
  int count = 0;
  for (int i = 0; i < 4; i++) {
    unsigned char n = bits_[i];
    while (n) {
      ++count;
      n &= (n - 1);
    }
  }
  return 2 * count + 1;
}

// returns the number of leaf cells
int bstm_time_tree::num_leaves() const { return get_leaf_bits(0).size(); }

// returns bit indices of leaf nodes under rootBit, using pre-order traversal
std::vector<int> bstm_time_tree::get_leaf_bits(int rootBit) const {
  std::vector<int> leafBits;

  // special root case
  if (bits_[0] == 0 && rootBit == 0) {
    leafBits.push_back(0);
    return leafBits;
  }

  // initialize stack with the root node
  unsigned stack[TT_NUM_LVLS];
  stack[0] = rootBit;
  int ptr = 0;

  int curr_bit;
  while (ptr != -1) {
    curr_bit = stack[ptr--]; // pop

    if (!is_leaf(curr_bit)) // push right child and then left child
    {
      stack[++ptr] = (2 * curr_bit + 2);
      stack[++ptr] = (2 * curr_bit + 1);
    } else // reached leaf, increment count
      leafBits.push_back(curr_bit);
  }
  return leafBits;
}

// returns the maximum depth of its elements
int bstm_time_tree::max_depth(int rootBit) const {
  int max_index = 0;
  std::vector<int> leaves = get_leaf_bits(rootBit);
  for (int leave : leaves)
    max_index = std::max(max_index, leave);
  return depth_at(max_index);
}

// returns the leaves residing at the maximum depth
std::vector<int> bstm_time_tree::max_depth_leaves() const {
  int max_depth = this->max_depth(0);
  std::vector<int> leaves = get_leaf_bits(0);
  std::vector<int> selected_leaves;
  for (int leave : leaves)
    if (depth_at(leave) == max_depth)
      selected_leaves.push_back(leave);
  return selected_leaves;
}

// OLD DEPRECATED CODE
#if 0
//returns bit indices of leaf nodes under rootBit
std::vector<int> bstm_time_tree::get_leaf_bits(int rootBit) const
{
  //use num cells to accelerate (cut off for loop)
  std::vector<int> leafBits;

  //special root case
  if ( bits_[0] == 0 && rootBit == 0 ) {
    leafBits.push_back(0);
    return leafBits;
  }

  //otherwise calc list of bit indices in the subtree of rootBIT, and then verify leaves
  std::vector<int> subTree;
  std::list<unsigned> toVisit;
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
#endif

float bstm_time_tree::cell_center(int bit_index) const {
  // Indexes into precomputed cell_center matrix
  return cell_centers[bit_index];
}

void bstm_time_tree::cell_range(int bit_index, float &min, float &max) const {
  float half_len = cell_len(bit_index) / 2.0f;
  min = cell_center(bit_index) - half_len;
  max = cell_center(bit_index) + half_len;
}

float bstm_time_tree::cell_len(int bit_index) const {
  if (bit_index == 0)
    return 1;
  else if (bit_index < 3)
    return 0.5;
  else if (bit_index < 7)
    return 0.25;
  else if (bit_index < 15)
    return 0.125;
  else if (bit_index < 31)
    return 0.0625;
  else
    return 0.03125;
}

void bstm_time_tree::fill_cells(bool (&frames)[32]) {
  for (int i = 0; i < 32; i += 2) {
    int idx = 15 + (i / 2);
    if (frames[i] || frames[i + 1]) {
      this->set_bit_at(idx, true);
    }
  }
  // Heirarchically merge similar regions of time
  for (int level = 3; level >= 0; --level) {
    for (int idx = (1 << level) - 1; idx < (2 << level) - 1; ++idx) {
      int child1_idx = this->child_index(idx);
      int child2_idx = child1_idx + 1;
      bool c1 = this->bit_at(child1_idx), c2 = this->bit_at(child2_idx);
      if (c1 && c2) {
        this->set_bit_at(idx, true);
      } else if (c1 && !c2) {
        // second region is same as first
        // only "merge" regions if first region has no children
        bool c1_has_children;
        if (level < 3) {
          int c1_child_idx = this->child_index(child1_idx);
          c1_has_children =
              this->bit_at(c1_child_idx) || this->bit_at(c1_child_idx + 1);
        } else {
          // in level 3, we need to check for the individual frames covered two
          // levels down.
          // If the odd-numbered frame is different, then child 1 can't be
          // merged.
          int frame_idx = (child1_idx - 15) * 2;
          c1_has_children = frames[frame_idx + 1];
        }
        this->set_bit_at(idx, true);
        this->set_bit_at(child1_idx, c1_has_children);
      } else if (!c1 && c2) {
        this->set_bit_at(idx, true);
      }
    }
  }

  // prune leaf nodes - in bstm_time_trees, nodes with no children should be 0's
  // -- 1's indicate a fully refine node with two children.
  for (int idx = 0; idx < 15; idx++) {
    if (this->bit_at(this->child_index(idx)) ||
        this->bit_at(this->child_index(idx) + 1)) {
      continue;
    }
    this->set_bit_at(idx, false);
  }
}

const bstm_time_tree bstm_time_tree::wrap_const(const uchar8 &data) {
  return bstm_time_tree(const_cast<uchar8 &>(data));
}

unsigned char bstm_time_tree::bit_lookup[] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

float bstm_time_tree::cell_centers[] = {
    0.5,        0.25,       0.75,       0.12500,    0.37500,   0.62500,
    0.875000,   0.06250,    0.1875000,  0.31250,    0.437500,  0.56250000,
    0.687500,   0.812500,   0.9375,

    0.0312500,  0.0937500,  0.1562500,  0.2187500,  0.2812500, 0.3437500,
    0.40625000, 0.4687500,  0.5312500,  0.5937500,  0.6562500, 0.7187500,
    0.7812500,  0.84375000, 0.9062500,  0.968750,

    0.0156250,  0.0468750,  0.0781250,  0.1093750,  0.1406250, 0.1718750,
    0.20312500, 0.2343750,  0.2656250,  0.2968750,  0.3281250, 0.3593750,
    0.3906250,  0.42187500, 0.4531250,  0.4843750,  0.5156250, 0.5468750,
    0.5781250,  0.6093750,  0.64062500, 0.6718750,  0.7031250, 0.7343750,
    0.7656250,  0.7968750,  0.8281250,  0.85937500, 0.8906250, 0.9218750,
    0.9531250,  0.9843750};

//------ I/O ----------------------------------------------------------
std::ostream &operator<<(std::ostream &s, bstm_time_tree &t) {
  for (int i = 0; i < 5; i++) {
    s << "level " << i << ": ";
    for (int j = (1 << i) - 1; j < (2 << i) - 1; ++j) {
      s << (int)(t.bit_at(j));
      for (int l = 0; l < (2 << (4 - i)) - 1; ++l) {
        s << " ";
      }
    }
    s << std::endl;
  }

  return s;
}
