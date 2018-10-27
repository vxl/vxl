//:
// \file
#include <algorithm>
#include <cstring>
#include <iostream>
#include <list>
#include "boct_bit_tree.h"
#include "boct_tree_cell.h"

//: copy constructor
boct_bit_tree::boct_bit_tree(const boct_bit_tree &other)
    : is_owning_(other.is_owning_), num_levels_(other.num_levels_) {
  if (this->is_owning_) {
    bits_ = new unsigned char[16];
    std::memcpy(bits_, other.get_bits(), 16);
  } else {
    bits_ = other.bits_;
  }
}

//: constructor from an array of char bits
boct_bit_tree::boct_bit_tree(const unsigned char *bits, int num_levels)
    : is_owning_(true) {
  bits_ = new unsigned char[16];

  // initialize num levels, bits
  num_levels_ = std::min(4, num_levels);

  // copy 16 bytes
  std::memcpy(bits_, bits, 16);

  ////zero out bits to start
  // for (int i=0;i<16; i++)
  // bits_[i] = bits[i];
}

// Copy assignment operator
boct_bit_tree &boct_bit_tree::operator=(boct_bit_tree that) {
  // swap members so 'that' can be safely destroyed
  std::swap(this->bits_, that.bits_);
  std::swap(this->is_owning_, that.is_owning_);
  std::swap(this->num_levels_, that.num_levels_);

  // If is_owning, replace `that`s buffer with a copy
  if (this->is_owning_) {
    auto *new_bits = new unsigned char[16];
    std::memcpy(new_bits, this->bits_, 16);
    this->bits_ = new_bits;
  }
  // Don't destroy old buffer in case of "self assignment" (i.e assignment when
  // both trees point to same buffer).
  if (this->bits_ == that.bits_) {
    that.is_owning_ = false;
  }
  return *this;
}

int boct_bit_tree::traverse(const vgl_point_3d<double> p, int deepest, bool full) {
  // deepest level to traverse is either
  deepest = std::max(deepest - 1, num_levels_ - 1);

  // force 1 register: curr = (bit, child_offset, depth, c_offset)
  int curr_bit = (int)(bits_[0]);
  int child_offset = 0;
  int depth = 0;

  // bit index to be returned
  int bit_index = 0;

  // clamp point
  double pointx = p.x(); // clamp(p.x(), 0.0001f, 0.9999f);
  double pointy = p.y(); // clamp(p.y(), 0.0001f, 0.9999f);
  double pointz = p.z(); // clamp(p.z(), 0.0001f, 0.9999f);

  // while the curr node has children
  while ((curr_bit || full) && (depth < deepest)) {
    // determine child offset and bit index for given point
    pointx += pointx; // point = point*2
    pointy += pointy;
    pointz += pointz;
    int codex = ((int)std::floor(pointx)) & 1;
    int codey = ((int)std::floor(pointy)) & 1;
    int codez = ((int)std::floor(pointz)) & 1;

    int c_index = codex + (codey << 1) + (codez << 2); // c_index = binary(zyx)
    bit_index = (8 * bit_index + 1) + c_index;         // i = 8i + 1 + c_index

    // update value of curr_bit and level
    curr_bit = (1 << c_index) &
               bits_[(depth + 1 +
                      child_offset)]; // int curr_byte = (curr.z + 1) + curr.y;
    child_offset = c_index;
    depth++;
  }
  return bit_index;
}

int boct_bit_tree::traverse_to_level(const vgl_point_3d<double> p,
                                     int deepest) {
  // deepest level to traverse is either
  deepest = std::min(deepest - 1, num_levels_ - 1);

  // force 1 register: curr = (bit, child_offset, depth, c_offset)
  int curr_bit = (int)(bits_[0]);
  int child_offset = 0;
  int depth = 0;

  // bit index to be returned
  int bit_index = 0;

  // clamp point
  double pointx = p.x(); // clamp(p.x(), 0.0001f, 0.9999f);
  double pointy = p.y(); // clamp(p.y(), 0.0001f, 0.9999f);
  double pointz = p.z(); // clamp(p.z(), 0.0001f, 0.9999f);

  // while the curr node has children
  while (curr_bit && depth < deepest) {
    // determine child offset and bit index for given point
    pointx += pointx; // point = point*2
    pointy += pointy;
    pointz += pointz;
    int codex = ((int)std::floor(pointx)) & 1;
    int codey = ((int)std::floor(pointy)) & 1;
    int codez = ((int)std::floor(pointz)) & 1;

    int c_index = codex + (codey << 1) + (codez << 2); // c_index = binary(zyx)
    bit_index = (8 * bit_index + 1) + c_index;         // i = 8i + 1 + c_index

    // update value of curr_bit and level
    curr_bit = (1 << c_index) &
               bits_[(depth + 1 +
                      child_offset)]; // int curr_byte = (curr.z + 1) + curr.y;
    child_offset = c_index;
    depth++;
  }
  return bit_index;
}
vgl_point_3d<double> boct_bit_tree::cell_center(int bit_index) {
  // Indexes into precomputed cell_center matrix
  return {
      centerX[bit_index], centerY[bit_index], centerZ[bit_index]};
}

//: Cell bounding box given bit_index, tree origin and tree len
vgl_box_3d<double>
boct_bit_tree::cell_box(int bit_index, vgl_point_3d<double> orig, double len) {
  double half_len = cell_len(bit_index) / 2.0;
  return {orig.x() + len * (centerX[bit_index] - half_len),
                            orig.y() + len * (centerY[bit_index] - half_len),
                            orig.z() + len * (centerZ[bit_index] - half_len),
                            orig.x() + len * (centerX[bit_index] + half_len),
                            orig.y() + len * (centerY[bit_index] + half_len),
                            orig.z() + len * (centerZ[bit_index] + half_len)};
}

double boct_bit_tree::cell_len(int bit_index) const {
  if (bit_index == 0)
    return 1.0;
  else if (bit_index < 9)
    return .5;
  else if (bit_index < 73)
    return .25;
  else
    return .125;
}

bool boct_bit_tree::valid_cell(int bit_index) const {
  return (bit_index == 0) || this->bit_at((bit_index - 1) >> 3);
}

bool boct_bit_tree::is_leaf(int bit_index) const {
  return this->valid_cell(bit_index) && (this->bit_at(bit_index) == 0);
}

// returns bit indices of all tree nodes under rootBit
std::vector<int> boct_bit_tree::get_cell_bits(int rootBit) const {
  // use num cells to accelerate (cut off for loop)
  std::vector<int> leafBits;

  // special root case
  if (bits_[0] == 0 && rootBit == 0) {
    leafBits.push_back(0);
    return leafBits;
  }

  // otherwise calc list of bit indices in the subtree of rootBIT, and then
  // verify leaves
  std::vector<int> subTree;
  std::list<unsigned> toVisit;
  toVisit.push_back(rootBit);
  while (!toVisit.empty()) {
    int currBitIndex = toVisit.front();
    toVisit.pop_front();

    subTree.push_back(currBitIndex);

    if (!this->is_leaf(currBitIndex)) { // add children to the visit list
      unsigned firstChild = 8 * currBitIndex + 1;
      for (int ci = 0; ci < 8; ++ci)
        toVisit.push_back(firstChild + ci);
    }
  }
  return subTree;
}

// returns bit indices of leaf nodes under rootBit
std::vector<int> boct_bit_tree::get_leaf_bits(int rootBit) const {
  // use num cells to accelerate (cut off for loop)
  std::vector<int> leafBits;

  // special root case
  if (bits_[0] == 0 && rootBit == 0) {
    leafBits.push_back(0);
    return leafBits;
  }

  // otherwise calc list of bit indices in the subtree of rootBIT, and then
  // verify leaves
  std::vector<int> subTree;
  std::list<unsigned> toVisit;
  toVisit.push_back(rootBit);
  while (!toVisit.empty()) {
    int currBitIndex = toVisit.front();
    toVisit.pop_front();
    if (this->is_leaf(currBitIndex)) {
      subTree.push_back(currBitIndex);
    } else { // add children to the visit list
      unsigned firstChild = 8 * currBitIndex + 1;
      for (int ci = 0; ci < 8; ++ci)
        toVisit.push_back(firstChild + ci);
    }
  }
  return subTree;
}
// returns bit indices of leaf nodes or nodes at the depth mentioned whichever
// comes first under rootBit
std::vector<int> boct_bit_tree::get_leaf_bits(int rootBit, int depth) const {
  // use num cells to accelerate (cut off for loop)
  std::vector<int> leafBits;
  int curr_depth = 0;
  // special root case
  if (bits_[0] == 0 && rootBit == 0) {
    leafBits.push_back(0);
    return leafBits;
  }

  // otherwise calc list of bit indices in the subtree of rootBIT, and then
  // verify leaves
  std::vector<int> subTree;
  std::list<unsigned> toVisit;
  toVisit.push_back(rootBit);
  while (!toVisit.empty()) {
    int currBitIndex = toVisit.front();
    toVisit.pop_front();
    if ((this->depth_at(currBitIndex) < depth && this->is_leaf(currBitIndex)) ||
        this->depth_at(currBitIndex) == depth) {
      subTree.push_back(currBitIndex);
    } else { // add children to the visit list
      curr_depth++;
      unsigned firstChild = 8 * currBitIndex + 1;
      for (int ci = 0; ci < 8; ++ci)
        toVisit.push_back(firstChild + ci);
    }
  }

  return subTree;
}
//: Return cell with a particular locational code
int boct_bit_tree::get_data_index(int bit_index, bool is_random) const {
  ////Unpack data offset (offset to root data)
  // tree[10] and [11] should form the short that refers to data offset
  // root and first gen are special case, return just the root offset +
  // bit_index

  int count_offset;
  if (is_random)
    count_offset = (int)bits_[10] * 256 + (int)bits_[11];
  else
    count_offset = (int)(bits_[13] << 24) | (bits_[12] << 16) |
                   (bits_[11] << 8) | (bits_[10]);

  return count_offset + this->get_relative_index(bit_index);
}

//: returns bit index assuming root data is located at 0
int boct_bit_tree::get_relative_index(int bit_index) const {
  if (bit_index < 9)
    return bit_index;

  // otherwise get parent index, parent byte index and relative bit index
  unsigned char oneuplevel = (bit_index - 1) >> 3; // bit index of parent
  unsigned char byte_index =
      ((oneuplevel - 1) >> 3) + 1; // byte where parent is found

  // count pre parent bits
  int count = 0;
  for (int i = 0; i < byte_index; ++i)
    count += bit_lookup[bits_[i]];

  // dont forget parent bits occurring the parent BYTE
  unsigned char sub_bit_index = 8 - ((oneuplevel - 1) & (8 - 1));
  unsigned char temp = bits_[byte_index] << sub_bit_index;

  count = count + bit_lookup[temp];
  unsigned char finestleveloffset = (bit_index - 1) & (8 - 1);
  count = 8 * count + 1 + finestleveloffset;

  return count;
}

//: return number of cells in this tree (size of data chunk)
int boct_bit_tree::num_cells() const {
  // count bits for each byte
  int count = 0;
  for (int i = 0; i < 10; i++) {
    unsigned char n = bits_[i];
    while (n) {
      ++count;
      n &= (n - 1);
    }
  }
  return 8 * count + 1;
}

// returns the number of leaf cells
int boct_bit_tree::num_leaves() const { return get_leaf_bits(0).size(); }

//----BIT MANIP Methods -----------------------------------------------
unsigned char boct_bit_tree::bit_at(int index) const {
  // make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72)
    return 0;

  // root is special case
  if (index == 0)
    return bits_[0];

  // second generation is sort of a special case
  if (index < 9)
    return (1 << (index - 1) & bits_[1]) ? 1 : 0;

  int i = (index - 9) / 8 + 2; // byte index i
  int bi = (index - 9) % 8;
  return (1 << bi & bits_[i]) ? 1 : 0;
}

void boct_bit_tree::set_bit_at(int index, bool val) {
  if (index > 72) {
    std::cerr << "No bit above 72, bad set call!\n";
    return;
  }

  // zero is a special case,
  if (index == 0)
    bits_[0] = (val) ? 1 : 0;

  int byte_index = (index - 1) / 8 + 1;
  int child_offset = (index - 1) % 8;
  unsigned char mask = 1 << child_offset;
  unsigned char byte = bits_[byte_index];
  bits_[byte_index] = (val) ? (byte | mask) : (byte & (mask ^ 0xFF));
}

void boct_bit_tree::set_bit_and_parents_to_true(int index) {
  while (index > 0) {
    this->set_bit_at(index, true);
    index = parent_index(index);
  }
  this->set_bit_at(0, true);
}

// A local implementation for floor(log(a)/log(8)) with integer argument a;
// this is a more straightforward (and not too inefficient) alternative for
//  std::floor(std::log(double a)/std::log(8.0)).
// Negative arguments make of course no sense; strictly speaking, also a=0
// makes no sense, but in that case a "very negative" value is returned.
inline static int int_log8(unsigned int a) {
  if (a == 0)
    return -0x7FFFFFFFL - 1L; // stands for minus infinity
  int r = 0;
  while (a >= 8)
    ++r, a >>= 3; // divide by 8
  return r;
}

// A local (and recursive) implementation for a^b with a and b both integer;
// this is a more accurate alternative for std::pow(double a,double b),
// certainly in those cases where b is relatively small.
inline static long int_pow8(unsigned int b) {
  if (b == 0)
    return 1;
  return 1L << (3 * b);
}

int boct_bit_tree::max_num_cells() {
  return int((int_pow8(num_levels_ + 1) - 1.0) / 7.0);
}

int boct_bit_tree::max_num_inner_cells() {
  return int((int_pow8(num_levels_) - 1.0) / 7.0);
}

int boct_bit_tree::depth_at(int index) const { return int_log8(7 * index + 1); }

int boct_bit_tree::depth() {

  int max_depth = 3;
  int tree_depth = 0;
  std::list<unsigned> toVisit; // maintain a queue of nodes and
  std::list<int> visit_depth;  // node depth
  toVisit.push_back(0);
  visit_depth.push_back(0);
  while (!toVisit.empty()) {
    int currBitIndex = toVisit.front();
    int currDepth = visit_depth.front();
    if (currDepth > tree_depth) { // cut search if max depth is reached.
      tree_depth = currDepth;
      if (tree_depth == max_depth)
        return tree_depth;
    }
    toVisit.pop_front();
    visit_depth.pop_front();
    if (!this->is_leaf(currBitIndex)) {
      unsigned firstChild = 8 * currBitIndex + 1;
      for (int ci = 0; ci < 8; ++ci) {
        int cind = firstChild + ci;
        toVisit.push_back(cind);
        visit_depth.push_back(depth_at(cind));
      }
    }
  }
  return tree_depth;
}
#if 0
//: gets and sets buffer pointers (located at bytes 12 and 13
int boct_bit_tree::get_buffer_ptr()
{
  unsigned char hi = this->bits_[12];
  unsigned char lo = this->bits_[13];
  unsigned short value = (unsigned short) ((hi << 8) | lo);
  return int(value);
}

int boct_bit_tree::set_buffer_ptr(int ptr)
{
  unsigned char hi = (unsigned char)(ptr >> 8);
  unsigned char lo = (unsigned char)(ptr & 255);
  this->bits_[12] = hi;
  this->bits_[13] = lo;
  return  0;
}
#endif // 0

int boct_bit_tree::get_data_ptr(bool is_random) {
  if (is_random) {
    unsigned char hi = this->bits_[10];
    unsigned char lo = this->bits_[11];
    auto value = (unsigned short)((hi << 8) | lo);
    return int(value);
  } else {
    return int((bits_[13] << 24) | (bits_[12] << 16) | (bits_[11] << 8) |
               (bits_[10]));
  }
}

void boct_bit_tree::set_data_ptr(int ptr, bool is_random) {
  if (is_random) {
    auto hi = (unsigned char)(ptr >> 8);
    auto lo = (unsigned char)(ptr & 255);
    this->bits_[10] = hi;
    this->bits_[11] = lo;
  } else {
    this->bits_[10] = (ptr)&0xff;
    this->bits_[11] = (ptr >> 8) & 0xff;
    this->bits_[12] = (ptr >> 16) & 0xff;
    this->bits_[13] = (ptr >> 24) & 0xff;
  }
}


const boct_bit_tree boct_bit_tree::wrap_const(const uchar16 &data, int num_levels) {
  // (uchar16 &bits, int num_levels = 4)
  return boct_bit_tree(const_cast<uchar16 &>(data), num_levels);
}


unsigned char boct_bit_tree::bit_lookup[] = {
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

float boct_bit_tree::centerX[] = {
    0.5,    0.25,   0.75,   0.25,   0.75,   0.25,   0.75,   0.25,   0.75,
    0.125,  0.375,  0.125,  0.375,  0.125,  0.375,  0.125,  0.375,  0.625,
    0.875,  0.625,  0.875,  0.625,  0.875,  0.625,  0.875,  0.125,  0.375,
    0.125,  0.375,  0.125,  0.375,  0.125,  0.375,  0.625,  0.875,  0.625,
    0.875,  0.625,  0.875,  0.625,  0.875,  0.125,  0.375,  0.125,  0.375,
    0.125,  0.375,  0.125,  0.375,  0.625,  0.875,  0.625,  0.875,  0.625,
    0.875,  0.625,  0.875,  0.125,  0.375,  0.125,  0.375,  0.125,  0.375,
    0.125,  0.375,  0.625,  0.875,  0.625,  0.875,  0.625,  0.875,  0.625,
    0.875,  0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625,
    0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625,
    0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375,
    0.3125, 0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625,
    0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375,
    0.3125, 0.4375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125,
    0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875,
    0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125,
    0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875,
    0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125,
    0.9375, 0.8125, 0.9375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125,
    0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625,
    0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625,
    0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375,
    0.3125, 0.4375, 0.3125, 0.4375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125,
    0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875,
    0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125,
    0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.0625, 0.1875, 0.0625, 0.1875,
    0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125,
    0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125,
    0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625,
    0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.5625, 0.6875, 0.5625,
    0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375,
    0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125,
    0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.0625, 0.1875,
    0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125,
    0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625, 0.1875,
    0.0625, 0.1875, 0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125,
    0.4375, 0.3125, 0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.0625, 0.1875, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125,
    0.4375, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875, 0.0625, 0.1875,
    0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.3125, 0.4375, 0.5625,
    0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375,
    0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875, 0.5625,
    0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375,
    0.8125, 0.9375, 0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.5625, 0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375,
    0.8125, 0.9375, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625, 0.6875, 0.5625,
    0.6875, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375, 0.8125, 0.9375};

float boct_bit_tree::centerY[] = {
    0.5,    0.25,   0.25,   0.75,   0.75,   0.25,   0.25,   0.75,   0.75,
    0.125,  0.125,  0.375,  0.375,  0.125,  0.125,  0.375,  0.375,  0.125,
    0.125,  0.375,  0.375,  0.125,  0.125,  0.375,  0.375,  0.625,  0.625,
    0.875,  0.875,  0.625,  0.625,  0.875,  0.875,  0.625,  0.625,  0.875,
    0.875,  0.625,  0.625,  0.875,  0.875,  0.125,  0.125,  0.375,  0.375,
    0.125,  0.125,  0.375,  0.375,  0.125,  0.125,  0.375,  0.375,  0.125,
    0.125,  0.375,  0.375,  0.625,  0.625,  0.875,  0.875,  0.625,  0.625,
    0.875,  0.875,  0.625,  0.625,  0.875,  0.875,  0.625,  0.625,  0.875,
    0.875,  0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125,
    0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.0625, 0.0625, 0.1875,
    0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.0625, 0.0625, 0.1875, 0.1875, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125,
    0.4375, 0.4375, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875,
    0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.0625, 0.0625,
    0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875,
    0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.3125, 0.3125, 0.4375, 0.4375,
    0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125,
    0.3125, 0.4375, 0.4375, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875,
    0.6875, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375,
    0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.5625,
    0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.8125, 0.8125, 0.9375,
    0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375,
    0.8125, 0.8125, 0.9375, 0.9375, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625,
    0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375,
    0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375,
    0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625,
    0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.8125, 0.8125,
    0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375,
    0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.0625, 0.0625, 0.1875, 0.1875,
    0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625,
    0.0625, 0.1875, 0.1875, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125,
    0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375,
    0.4375, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125,
    0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.0625, 0.0625, 0.1875,
    0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.0625, 0.0625, 0.1875, 0.1875, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125,
    0.4375, 0.4375, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875,
    0.1875, 0.0625, 0.0625, 0.1875, 0.1875, 0.0625, 0.0625, 0.1875, 0.1875,
    0.3125, 0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.3125,
    0.3125, 0.4375, 0.4375, 0.3125, 0.3125, 0.4375, 0.4375, 0.5625, 0.5625,
    0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875,
    0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.8125, 0.8125, 0.9375, 0.9375,
    0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125,
    0.8125, 0.9375, 0.9375, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875,
    0.6875, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375,
    0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.5625,
    0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.8125, 0.8125, 0.9375,
    0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375,
    0.8125, 0.8125, 0.9375, 0.9375, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625,
    0.5625, 0.6875, 0.6875, 0.5625, 0.5625, 0.6875, 0.6875, 0.5625, 0.5625,
    0.6875, 0.6875, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375,
    0.9375, 0.8125, 0.8125, 0.9375, 0.9375, 0.8125, 0.8125, 0.9375, 0.9375};

float boct_bit_tree::centerZ[] = {
    0.5,    0.25,   0.25,   0.25,   0.25,   0.75,   0.75,   0.75,   0.75,
    0.125,  0.125,  0.125,  0.125,  0.375,  0.375,  0.375,  0.375,  0.125,
    0.125,  0.125,  0.125,  0.375,  0.375,  0.375,  0.375,  0.125,  0.125,
    0.125,  0.125,  0.375,  0.375,  0.375,  0.375,  0.125,  0.125,  0.125,
    0.125,  0.375,  0.375,  0.375,  0.375,  0.625,  0.625,  0.625,  0.625,
    0.875,  0.875,  0.875,  0.875,  0.625,  0.625,  0.625,  0.625,  0.875,
    0.875,  0.875,  0.875,  0.625,  0.625,  0.625,  0.625,  0.875,  0.875,
    0.875,  0.875,  0.625,  0.625,  0.625,  0.625,  0.875,  0.875,  0.875,
    0.875,  0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875,
    0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.0625,
    0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.0625, 0.0625,
    0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.3125, 0.3125, 0.3125,
    0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125,
    0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125, 0.4375,
    0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125, 0.4375, 0.4375,
    0.4375, 0.4375, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875,
    0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875,
    0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.0625,
    0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.3125, 0.3125,
    0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125,
    0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125,
    0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125, 0.4375,
    0.4375, 0.4375, 0.4375, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875,
    0.1875, 0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875,
    0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875,
    0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875, 0.3125,
    0.3125, 0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125,
    0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125,
    0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125, 0.3125,
    0.4375, 0.4375, 0.4375, 0.4375, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875,
    0.1875, 0.1875, 0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875,
    0.1875, 0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875,
    0.1875, 0.0625, 0.0625, 0.0625, 0.0625, 0.1875, 0.1875, 0.1875, 0.1875,
    0.3125, 0.3125, 0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125,
    0.3125, 0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125,
    0.3125, 0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.3125, 0.3125, 0.3125,
    0.3125, 0.4375, 0.4375, 0.4375, 0.4375, 0.5625, 0.5625, 0.5625, 0.5625,
    0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875,
    0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875, 0.6875,
    0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875, 0.6875, 0.6875,
    0.6875, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375,
    0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.8125,
    0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.8125, 0.8125,
    0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.5625, 0.5625, 0.5625,
    0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625,
    0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875,
    0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875, 0.6875,
    0.6875, 0.6875, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375,
    0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375,
    0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.8125,
    0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.5625, 0.5625,
    0.5625, 0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625,
    0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625,
    0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625, 0.6875,
    0.6875, 0.6875, 0.6875, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375,
    0.9375, 0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375,
    0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375,
    0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375, 0.5625,
    0.5625, 0.5625, 0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625,
    0.5625, 0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625,
    0.5625, 0.6875, 0.6875, 0.6875, 0.6875, 0.5625, 0.5625, 0.5625, 0.5625,
    0.6875, 0.6875, 0.6875, 0.6875, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375,
    0.9375, 0.9375, 0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375,
    0.9375, 0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375,
    0.9375, 0.8125, 0.8125, 0.8125, 0.8125, 0.9375, 0.9375, 0.9375, 0.9375};

//------ I/O ----------------------------------------------------------
std::ostream &operator<<(std::ostream &s, const boct_bit_tree &t) {
  s << "boct_bit_tree:\n"
    << "Tree bits:\n"
    << "depth 0: " << (int)(t.bit_at(0)) << '\n';

  // one
  s << "depth 1:";
  for (int i = 1; i < 9; i++)
    s << "  " << (int)t.bit_at(i);
  s << '\n';

  // two
  s << "depth 2:";
  for (int i = 9; i < 73; i++)
    s << "  " << (int)t.bit_at(i);
  s << '\n';

  return s;
}
