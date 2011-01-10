//:
// \file
#include "boct_bit_tree2.h"
#include "boct_tree_cell.h"
#include <vcl_iostream.h>
#include <vcl_list.h>
#include <vcl_algorithm.h>

static unsigned char bit_lookup[] =
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

//: default constructor
boct_bit_tree2::boct_bit_tree2()
{
  bits_ = new unsigned char[16];
}

//: constructor from an array of char bits
boct_bit_tree2::boct_bit_tree2(unsigned char* bits, int num_levels)
{
    bits_ = new unsigned char[16];

    //initialize num levels, bits
    num_levels_ = vcl_min(4,num_levels);
    //zero out bits to start
    for (int i=0;i<16; i++)
        bits_[i] = bits[i];
}


// A local (and recursive) implementation for a^b with a and b both integer;
// this is a more accurate alternative for std::pow(double a,double b),
// certainly in those cases where b is relatively small.
inline static int int_pow(int a, unsigned int b)
{
  if (b==0) return 1;
  else if (b==1) return a;
  else return int_pow(a*a,b/2) * int_pow(a, b%2);
}


int boct_bit_tree2::traverse(const vgl_point_3d<double> p)
{
  //force 1 register: curr = (bit, child_offset, depth, c_offset)
  int curr_bit = (int)(bits_[0]);
  int child_offset = 0;
  int depth = 0;

  //bit index to be returned
  int bit_index = 0;

  //clamp point
  double pointx = p.x();//clamp(p.x(), 0.0001f, 0.9999f);
  double pointy = p.y();//clamp(p.y(), 0.0001f, 0.9999f);
  double pointz = p.z();//clamp(p.z(), 0.0001f, 0.9999f);

  // while the curr node has children
  while (curr_bit && depth < num_levels_-1) {
    //determine child offset and bit index for given point
    pointx += pointx;                                             //point = point*2
    pointy += pointy;
    pointz += pointz;
    int codex=((int)vcl_floor(pointx)) & 1;
    int codey=((int)vcl_floor(pointy)) & 1;
    int codez=((int)vcl_floor(pointz)) & 1;

    int c_index = codex + (codey<<1) + (codez<<2);             //c_index = binary(zyx)
    bit_index = (8*bit_index + 1) + c_index;                      //i = 8i + 1 + c_index

    //update value of curr_bit and level
    curr_bit = (1<<c_index) & bits_[(depth+1 + child_offset)];      //int curr_byte = (curr.z + 1) + curr.y;
    child_offset = c_index;
    depth++;
  }
  return bit_index;
}


//: Return cell with a particular locational code
int boct_bit_tree2::get_data_index(int bit_index)
{
  ////Unpack data offset (offset to root data)
  //tree[10] and [11] should form the short that refers to data offset
  //root and first gen are special case, return just the root offset + bit_index
  int count_offset=(int)bits_[10]*256+(int)bits_[11];
  if (bit_index < 9)
    return (count_offset+bit_index) - (((count_offset+bit_index)>>16)<<16);

  //otherwise get parent index, parent byte index and relative bit index
  unsigned char oneuplevel=(bit_index-1)>>3;
  unsigned char byte_index= ((oneuplevel-1)>>3) +1;

  unsigned char sub_bit_index=8-((oneuplevel-1)&(8-1));
  int count=0;
  for (int i=0;i<byte_index;i++)
    count += bit_lookup[bits_[i]];

  unsigned char temp=bits_[byte_index]<<sub_bit_index;
  count=count+bit_lookup[temp];
  unsigned char finestleveloffset=(bit_index-1)&(8-1);
  count = 8*count+1 +finestleveloffset;

  return (count_offset+count) - (((count_offset+count)>>16)<<16);
}

//: return number of cells in this tree (size of data chunk)
int boct_bit_tree2::num_cells() const
{
  //count bits for each byte
  int count = 0 ;
  for (int i=0; i<10; i++) {
    unsigned char n = bits_[i];
    while (n)  {
      ++count;
      n &= (n - 1) ;
    }
  }
  return 8*count+1;
}


//----BIT MANIP Methods -----------------------------------------------
unsigned char
boct_bit_tree2::bit_at(int index)
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 72)
    return 0;

  //root is special case
  if (index == 0)
    return bits_[0];

  //second generation is sort of a special case
  if (index < 9)
    return (1<<(index-1) & bits_[1]) ? 1 : 0;

  int i  = (index-9)/8 + 2; //byte index i
  int bi = (index-9)%8;
  return (1<<bi & bits_[i]) ? 1 : 0;
}


void
boct_bit_tree2::set_bit_at(int index, bool val)
{
  if (index > 72) {
    vcl_cerr<<"No bit above 72, bad set call!\n";
    return;
  }

  //zero is a special case,
  if (index == 0)
    bits_[0] = (val) ? 1 : 0;

  int byte_index =   (index-1)/8+1;
  int child_offset = (index-1)%8;
  unsigned char mask = 1<<child_offset;
  unsigned char byte = bits_[byte_index];
  bits_[byte_index] = (val)? (byte | mask) : (byte & (mask ^ 0xFF));
}

// A local implementation for floor(log(a)/log(8)) with integer argument a;
// this is a more straightforward (and not too inefficient) alternative for
//  std::floor(std::log(double a)/std::log(8.0)).
// Negative arguments make of course no sense; strictly speaking, also a=0
// makes no sense, but in that case a "very negative" value is returned.
inline static int int_log8(unsigned int a)
{
  if (a==0) return -0x80000000L; // stands for minus infinity
  int r = 0;
  while (a >= 8) ++r, a>>=3; // divide by 8
  return r;
}


int boct_bit_tree2::depth_at(int index) const
{
  return int_log8(7*index+1);
}


//------ I/O ----------------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree2 &t)
{
  s << "boct_bit_tree2:\n"
    << "Tree bits:\n"
    << "depth 0: "<< (int) (t.bit_at(0))
    << "\n\n";

  //one
  if (t.bit_at(0)) {
    s << "depth 1:";
    for (int i=1; i<9; i++)
      s << "  " << (int) t.bit_at(i);
    s << '\n';
  }

  return s;
}

