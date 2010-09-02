//:
// \file
#include "boct_bit_tree.h"
#include "boct_tree_cell.h"
#include <vcl_iostream.h>
#include <vcl_list.h>

//: default constructor
boct_bit_tree::boct_bit_tree()
{
  bits_ = new unsigned char[16];
  data_ = new float[585*16];

  //initialize num levels, bits
  num_levels_ = 4;
  for (int i=0;i<16; i++)
    bits_[i] = 0;
}

//: constructor from an array of char bits
boct_bit_tree::boct_bit_tree(char* bits)
{
  bits_ = new unsigned char[16];
  data_ = new float[585*16];

  //initialize num levels, bits
  num_levels_ = 4;

  //zero out bits to start
  for (int i=0;i<16; i++)
    bits_[i] = 0;

  //initialize bits
  for (int i=0;i<73; i++)
    this->set_bit_at(i, (bool) bits[i]);
}

//: Constructor from boct_tree<short, float> *
boct_bit_tree::boct_bit_tree(boct_tree<short,float > * tree)
{
  bits_ = new unsigned char[16];
  data_ = new float[585*16];

  //initialize num levels, bits
  num_levels_ = 4;

  //zero out bits to start
  for (int i=0;i<16; i++)
    bits_[i] = 0;

  if (tree->number_levels() > 4) {
    vcl_cerr<<"Tree is too deep\n";
    return;
  }

  boct_tree_cell<short, float>* root = tree->root();
  this->encode(root, 0);
}

//: constructor from vcl_vector<int4>
boct_bit_tree::boct_bit_tree(vcl_vector<int4> tree, vcl_vector<float16> data)
{
  bits_ = new unsigned char[16];
  data_ = new float[585*16];

  //initialize num levels, bits
  num_levels_ = 4;

  //zero out bits to start
  for (int i=0; i<16; i++)
    bits_[i] = 0;
  for (int i=0; i<585*16; i++)
    data_[i] = 0.0;

  //encode tree
  int4 root = tree[0];
  this->encode(root, 0, tree);

  //encode data..
  this->encode_data(tree, data);

  //run a comparison test to make sure the information is identical
  if (!this->verify_tree(0, 0, tree,data)) {
    vcl_cout<<"-----------------------------------------------\n"
            <<(*this)<<vcl_endl;
    this->print_input_tree(tree, data);
    vcl_cout<<"-----------------------------------------------"<<vcl_endl;
  }
}

//Depth first search traversal, comparing children's data along the way...
bool boct_bit_tree::verify_tree(int i, int node, vcl_vector<int4> tree, vcl_vector<float16> data)
{
  //make sure tree encoding is correct
  int child = tree[node][1];
  if ( ((bit_at(i) == 1) && (child<0)) || ((bit_at(i) == 0) && (child>0)) ) {
    vcl_cout<<"Bit at "<<i<<" is one, input child is null - BAD"<<vcl_endl;
    return false;
  }

  if (bit_at(i) == 1) {
    bool good = true;
    for (int c=0; c<8; c++)
      good = good && verify_tree(8*i+1+c, child+c, tree, data);
    return good;
  }

  float16 tree_dat = data[tree[node][2]];
  int bit_dat  = get_data_index(i);
  int same = true;
  for (int j=0; j<16; j++)
    same = same && (data_[bit_dat+j] == tree_dat[j]);

  if (!same)
      vcl_cout<<"!! TREE ENCODING IS BAD AT BIT: "<<i<<" node "<<node<<vcl_endl;
  return same;
}


//debug printer - should print tree depth first search wise
void boct_bit_tree::print_input_tree(vcl_vector<int4> tree, vcl_vector<float16> data)
{
  vcl_cout<<"INPUT TREE: "<<vcl_endl;

  //print generation 1 data (root data)
  int root_data = tree[0][2];
  vcl_cout<<"root_data: "<<data[root_data]<<vcl_endl;

  //print generation 2 data
  int child = tree[0][1];
  if (child > 0) {
    for (int i=0; i<8; i++) {
      int data_ptr = tree[child][2];
      vcl_cout<<" child@"<<i+1<<": "<<data[data_ptr]<<vcl_endl;
    }
  }
}

//: helper recursive method encode
void boct_bit_tree::encode(boct_tree_cell<short, float>* node, int i)
{
  if (!node->is_leaf())
  {
    //if it's got children - set it's bit to 1
    this->set_bit_at(i, 1);

    //recur on each child
    boct_tree_cell<short, float>* children = node->children();
    for (unsigned c=0; c<8; c++) {
      this->encode(&children[c], (8*i+1)+c);
    }
  }
}

void boct_bit_tree::encode(int4 node, int i, vcl_vector<int4> tree)
{
  if (node[1] >= 0)
  {
    //set bit to 1
    this->set_bit_at(i,1);

    //recur on each child
    int child_ptr = node[1];
    for (unsigned c=0; c<8; c++) {
      this->encode(tree[child_ptr+c], (8*i+1)+c, tree);
    }
  }
}

void boct_bit_tree::encode_data(vcl_vector<int4> tree, vcl_vector<float16> data)
{
  int dataIndex = 0;

  //run a BFS to re-order data items
  vcl_list<int> open;
  open.push_back(0);
  while (!open.empty())
  {
    int curr = open.front(); open.pop_front();
    int4 currNode = tree[curr];
    float16 currData = data[currNode[2]];

    //insert current node's data into array
    for (int i=0; i<16; i++)
      data_[dataIndex+i] = currData[i];
    dataIndex += 16;

    //enqueue child nodes
    if (currNode[1] > 0) {
      int childPtr = currNode[1];
      for (int i=0; i<8; i++)
        open.push_back(childPtr+i);
    }
  }
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

int boct_bit_tree::traverse(const vgl_point_3d<double> p)
{
  vcl_cout<<"Traverse to point "<<p
          <<" through "<<num_levels_<<" levels"<<vcl_endl;

  //find location code for point
  boct_loc_code<short> target_code = boct_loc_code<short>(p, num_levels_-1);
  int target_level = target_code.level;
  if (target_level < 0)
    return -1;

  //initialize current cell (curr_cell = has_children?)
  unsigned char curr_cell = this->bit_at(0);
  int cell_index = 0;
  int curr_level = num_levels_-1;  //root level
  boct_loc_code<short> found_code = target_code;
  while (target_level<curr_level && curr_cell)
  {
    //update found loc code (loc code belonging to correct child)
    short c_index = target_code.child_index(curr_level);
    found_code    = found_code.child_loc_code(c_index, curr_level-1);

    //update cell_index = first_child_index + child_offset
    cell_index = (cell_index*8+1) + (int) c_index; //8i+1 + c_index
    curr_cell  = this->bit_at(cell_index);

    //decrement curr_level
    --curr_level;
  }
  return cell_index;
#if 0
  //get offset in max generation
  unsigned short offset = this->loc_code_to_gen_offset(loc_code, num_levels_);

  //starting at maximum level, look for parent bit to be equal to 1
  unsigned int d = num_levels_-1;

  //initialize BI to point to the first index of depth d
  int bi = (int_pow(8, d)-1) / 7;

  //offset bi to point to the 'leaf bit' pointed to by the loc_code
  bi += offset;

  //find the parent, if this parent is 0, keep going until you find pi=1
  int pi = (bi-1)/8; // automatically rounding downwards, since bi is integer
  vcl_cout<<"    start Bit Index: "<<bi<<"  parent: "<<pi<<vcl_endl;
  while (bit_at(pi) == 0 && pi > 0) {
    bi = pi;
    pi = (bi-1)/8;
  }

  //now that you have bi = valid leaf, return it's index and use it to find its data
  return bi;
#endif // 0
}

int boct_bit_tree::traverse_opt(const vgl_point_3d<double> p)
{
  // Pseudo code for new optimized traverse algo:
  // i = 0;
  // point p;
  // int code;
  // while (bit_at(i) == 1)
  //    p *= 2;
  //    code = (int) p;
  //    c_index = code & 1; (LSB)
  //    i = 2i+1 + c_index;
  // endwhile

  int bit_index = 0;
  vnl_vector_fixed<double,3> point;
  point[0] = p.x(), point[1] = p.y(), point[2] = p.z();
  while (bit_at(bit_index) == 1) {
    point += point;
    unsigned c_x = ((unsigned) point[0]) & 1;
    unsigned c_y = ((unsigned) point[1]) & 1;
    unsigned c_z = ((unsigned) point[2]) & 1;
    int c_index = c_x + (c_y<<1) + (c_z<<2);
    bit_index = (8*bit_index + 1) + c_index;
  }
  return bit_index;
}


//TODO This isn't debugged
unsigned short
boct_bit_tree::loc_code_to_gen_offset(boct_loc_code<short> loc_code, int depth)
{
  //need to map the location code to a number between 0 and 2^(num_levels-1)
  //note: i believe X needs to be the LSB, followed by Y and Z (Z,Y,X)
  unsigned short packed = 0;
  unsigned short mask = 1;
  for (int i=0; i<depth; i++) {
    unsigned short mz = (mask & loc_code.z_loc_); //>>i;
    unsigned short my = (mask & loc_code.y_loc_); //>>i;
    unsigned short mx = (mask & loc_code.x_loc_); //>>i;

    //vcl_cout<<"Packed = "<<packed<< "   mask: "<<mask<<'\n'
    //        <<mz<<' '<<my<<' '<<mx<<vcl_endl;
    //note that mz is shifted to the right i times, and then left
    //3*i times... can just shift to the left 2*i times..
    packed += (mx <<  2*i)
            + (my << (2*i + 1))
            + (mz << (2*i + 2));
    mask <<= 1;
  }
  vcl_cout<<"    Loc code "<<loc_code<<" maps to "<<packed<<vcl_endl;
  return packed;
}

int boct_bit_tree::loc_code_to_index(boct_loc_code<short> loc_code, int root_level)
{
  int level = loc_code.level;
  int depth = root_level - level;

  //index of first node at depth
  int level_index = (int) (int_pow(8, depth)-1) / 7;

  //need to map the location code to a number between 0 and 2^(num_levels-1)
  //note: i believe X needs to be the LSB, followed by Y and Z (Z,Y,X)
  int ri = root_level-1;
  unsigned short packed = 0;
  for (int i=depth; i>0; i--, ri--)  {
    unsigned short mask = 1<<ri;
    //get bit at mask
    unsigned short mz = (mask & loc_code.z_loc_)>>ri;
    unsigned short my = (mask & loc_code.y_loc_)>>ri;
    unsigned short mx = (mask & loc_code.x_loc_)>>ri;

    packed += (mz << (3*i-1))
            + (my << (3*i-2))
            + (mx << (3*i-3));
  }
  return level_index + packed;
}


//: Return cell with a particular locational code
int boct_bit_tree::get_data_index(int bit_index)
{
  //root is special case - if bit_index is root, then return 0;
  if (bit_index == 0)
    return 0;

  //data index starts at 1
  int di = 1;

  int pi = (bit_index-1)/8; // automatically rounding downwards

  //check to make sure that the parent of this index is one, otherwise return failure;
  if (bit_at(pi) != 1) {
    vcl_cout<<"This bit_index is invalid, no data cell exists for "<<bit_index<<vcl_endl;
    return -1;
  }

  //add up bits that occur before the parent index
  for (int i=0; i<pi; i++)
    di += 8*bit_at(i);

  //offset for child...
  di += (bit_index+8-1)%8;

  return di*16;   //times 16 because data is stored as 16 floats
}

//: return number of cells in this tree (size of data chunk)
int boct_bit_tree::size() const
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
boct_bit_tree::bit_at(int index)
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
boct_bit_tree::set_bit_at(int index, bool val)
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

int boct_bit_tree::depth_at(int index) const
{
  return int_log8(7*index+1);
}


//------ I/O ----------------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree &t)
{
  float* data = t.get_data();
  s << "BOCT_BIT_TREE:" << vcl_endl;
#ifdef DEBUG
  unsigned char* bits = t.get_bits();
  s << "bytes: " << vcl_endl;
  for (int i=0; i<16; i++)
    s << "byte "<<i<<": "<< (int) bits[i] <<vcl_endl;
#endif

  s << "Tree bits:\n"
    << "depth 0: "<< (int) (t.bit_at(0)) << vcl_endl;
  int dat_index = t.get_data_index(0);
  s << "   data at 0 ("<<dat_index<<"):";
  for (int i=0; i<16; i++)
    s << ' ' << data[dat_index+i];
  s << '\n';

  //one
  if (t.bit_at(0)) {
    s << "depth 1:";
    for (int i=1; i<9; i++)
      s << "  " << (int) t.bit_at(i);
    s << '\n';
    for (int i=1; i<9; i++) {
      dat_index = t.get_data_index(i);
      s << "   data at 1 ("<<dat_index<<"):";
      for (int j=0; j<16; j++)
        s << ' ' << data[dat_index+j];
      s << '\n';
    }
  }

#if 0
  //two
  s << "depth 2:";
  for (int i=9; i<73; i++)
  {
    if ((i-9)%16 == 0 && i != 9)
      s<<"\n         ";
    else if ((i-9)%8 == 0 && i != 9)
      s<<"    ";

    s << ' ' << (int) t.bit_at(i);
  }
  s << '\n';
#endif // 0
  return s;
}

#if 0 // recursive function to write out the binary representation of number
void binary(unsigned int number)
{
  if (number <= 1) {
    vcl_cout << number;
  }
  else {
    binary(number >> 1);
    vcl_cout << (number%2);
  }
}
#endif // 0
