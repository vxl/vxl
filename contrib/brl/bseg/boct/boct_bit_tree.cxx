//:
// \file
#include "boct_bit_tree.h"
#include "boct_tree_cell.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>

typedef vnl_vector_fixed<float,16> float16;
typedef vnl_vector_fixed<unsigned char,16> uchar16;

//: default constructor
boct_bit_tree::boct_bit_tree() {
    
  //initialize num levels, bits 
  num_levels_ = 4;
  for(int i=0;i<16; i++)
    bits_[i] = 0;
    
  //init data
  float16 init(0.0);
  for(int i=0; i<585; i++) 
    data_[i] = init;
}

//: constructor from an array of char bits
boct_bit_tree::boct_bit_tree(char* bits) {
    
  //initialize num levels, bits 
  num_levels_ = 4;
  
  //zero out bits to start
  for(int i=0;i<16; i++)
    bits_[i] = 0;
  
  //initialize bits
  for(int i=0;i<73; i++)
    this->set_bit_at(i, (bool) bits[i]); 
  
  //init data
  float16 init(0.0);
  for(int i=0; i<585; i++) 
    data_[i] = init;
}

//: Constructor from boct_tree<short, float> *
boct_bit_tree::boct_bit_tree(boct_tree<short,float > * tree) 
{
  //initialize num levels, bits 
  num_levels_ = 4;
  
  //zero out bits to start
  for(int i=0;i<16; i++)
    bits_[i] = 0;
    
  //init data
  float16 init(0.0);
  for(int i=0; i<585; i++) 
    data_[i] = init;
  
  if(tree->number_levels() > 4) {
    vcl_cout<<"Tree is to deep "<<vcl_endl;
    return;
  }
  
  boct_tree_cell<short, float>* root = tree->root();
  this->encode(root, 0);
  
}

//: helper recursive method encode
void boct_bit_tree::encode(boct_tree_cell<short, float>* node, int i)
{
  if(!node->is_leaf()) {
    
    //if it's got children - set it's bit to 1
    this->set_bit_at(i, 1);
    
    //recur on each child
    boct_tree_cell<short, float>* children = node->children();
    for (unsigned c=0; c<8; c++) {
      this->encode(&children[c], (8*i+1)+c);
    }
  }
}

int boct_bit_tree::traverse(const vgl_point_3d<double> p)
{
  vcl_cout<<"Traverse to point "<<p
          <<" through "<<num_levels_<<" levels"<<vcl_endl;

  //find location code for point 
  boct_loc_code<short> loc_code = boct_loc_code<short>(p, num_levels_-1);
  
  //get offset in max generation
  unsigned short offset = this->loc_code_to_gen_offset(loc_code, num_levels_);

  //starting at maximum level, look for parent bit to be equal to 1
  int d = num_levels_-1; 
  
  //initialize BI to point to the first index of depth d
  int bi = (int) ( (1.0/7.0) * (vcl_pow(8, d)-1) );
  
  //offset bi to point to the 'leaf bit' pointed to by the loc_code
  bi += offset; 
    
  //find the parent, if this parent is 0, keep going until you find pi=1
  int pi = vcl_floor( (bi-1)/8 );
  vcl_cout<<"    start Bit Index: "<<bi<<"  parent: "<<pi<<vcl_endl;
  while(bit_at(pi) == 0 && pi > 0) {
    bi = pi;
    pi = vcl_floor((bi-1)/8);
  }
  
  //now that you have bi = valid leaf, return it's index and use it to find its data
  return bi;  
}


unsigned short 
boct_bit_tree::loc_code_to_gen_offset(boct_loc_code<short> loc_code, int depth)
{
  //need to map the location code to a number between 0 and 2^(num_levels-1)
  //note: i believe X needs to be the LSB, followed by Y and Z (Z,Y,X)
  unsigned short packed = 0;
  unsigned short mask = 1;  
  for(int i=0; i<depth; i++) {
    unsigned short mz = (mask & loc_code.z_loc_); //>>i;
    unsigned short my = (mask & loc_code.y_loc_); //>>i;
    unsigned short mx = (mask & loc_code.x_loc_); //>>i;
    
    //vcl_cout<<"Packed = "<<packed<< "   mask: "<<mask<<vcl_endl;
    //vcl_cout<<mz<<" "<<my<<" "<<mx<<vcl_endl;
    //note that mz is shifted to the right i times, and then left
    //3*i times... can just shift to the left 2*i times..
    packed += (mx << 2*i) 
            + (my << 2*i + 1)
            + (mz << 2*i + 2);
    mask = (mask << 1);
  }
  vcl_cout<<"    Loc code "<<loc_code<<" maps to "<<packed<<vcl_endl;
  return packed; 
}

//: Return cell with a particular locational code
int boct_bit_tree::get_data_index(int bit_index)
{
  //root is special case - if bit_index is root, then return 0;
  if(bit_index == 0)
    return 0;
  
  //data index starts at 1
  int di = 1;
  
  //pi = floor((i-1)/8) (because child = 8i+1)
  int pi = vcl_floor( (bit_index-1)/8 );

  //check to make sure that the parent of this index is one, otherwise return failure;
  if(bit_at(pi) != 1) {
    vcl_cout<<"This bit_index is invalid, no data cell exists for "<<bit_index<<vcl_endl;
    return -1;
  }
  
  //add up bits that occur before the parent index
  for(int i=0; i<pi; i++) 
    di += 8*bit_at(i);
  
  //offset for child... 
  di += (bit_index-1)%8;
  
  return di;
}

//: return number of cells int his tree (size of data chunk)
int boct_bit_tree::size() 
{
  //count bits for each byte
  int count = 0 ;
  for(int i=0; i<10; i++) {
    unsigned char n = bits_[i];
    while (n)  {
      count++ ;
      n &= (n - 1) ;
    }
  }  
  return 8*count+1;
}


//----BIT MANIP Methods -----------------------------------------------
unsigned char
boct_bit_tree::bit_at(int index) 
{
  if(index > 72) {
    vcl_cout<<"No bit above 72, bad get call! "<<vcl_endl;
    return 1;
  }
  int bi = 0;
  unsigned char mask = 0;
  unsigned char byte = byte_at(index, mask, bi);
  if(mask & byte)
    return 1;
  return 0;
}

void
boct_bit_tree::set_bit_at(int index, bool val)
{
  if(index > 72) {
    vcl_cout<<"No bit above 72, bad set call! "<<vcl_endl;
    return; 
  }
  
  //get the relevant byte
  int byte_index = 0;
  unsigned char mask = 0;
  unsigned char byte = byte_at(index, mask, byte_index);
  
  //set relevant bit (to set 1 or it with the mask)
  bits_[byte_index] = (val)? (byte | mask) : (byte & (mask ^ 0xFF));
}

unsigned char 
boct_bit_tree::byte_at(int index, unsigned char &mask, int &i) 
{
  if(index > 72) {
    vcl_cout<<"No bit above 72, bad get byte call! "<<vcl_endl;
    return 1;
  }
  //get the relevant byte
  unsigned char byte;
  int depth = this->depth_at(index);
  i = 0;
  if(depth==0) {
    byte = bits_[depth];
    mask = 1;
    i=0;
  }
  else if (depth==1) {
    byte = bits_[depth];
    mask = 1<<(index-1);
    i=1;
  }
  else {
    i = vcl_floor((index-9)/8.0) + 2;  //fraction of 64 + 2...
    byte = bits_[i];
    int bi = (index-9) % 8;
    mask = 1<<bi;
  }
  return byte;
}



//------ I/O ----------------------------------------------------------
vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree t)
{
  s << "Tree bits: " << vcl_endl
    << "depth 0: "<< (int) (t.bit_at(0)) << vcl_endl;
  
  //one
  s << "depth 1: ";
  for(int i=1; i<9; i++)
    s<< (int) t.bit_at(i) << "  ";
  s << "\n";

  //two
  s << "depth 2: ";
  for(int i=9; i<73; i++) {
    
    if( (i-9)%16 == 0 && i != 9)
      s<<"\n         ";
    else if( (i-9)%8 == 0 && i != 9) 
      s<<"    ";
 
    s << (int) t.bit_at(i) << " "; 
  }
  s << "\n";
  return s;
}



//void binary(int number) {
	//int remainder;

	//if(number <= 1) {
		//cout << number;
		//return;
	//}

	//remainder = number%2;
	//binary(number >> 1);    
	//cout << remainder;
//}

