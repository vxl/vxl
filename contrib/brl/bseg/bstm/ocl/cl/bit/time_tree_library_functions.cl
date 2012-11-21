

float cell_len_tt(int bit_index) 
{
  if (bit_index==0)
    return 1;
  else if (bit_index<3)
    return 0.5;
  else if (bit_index<7)
    return 0.25;
  else if(bit_index<15)
    return 0.125;
  else if(bit_index<31)
    return 0.0625;
  else
    return 0.03125;
}

uchar bit_at_tt(__local uchar* tree, int index) 
{
  //make sure it's in bounds - all higher cells are leaves and thus 0
  if (index > 30)
    return 0;

  int byte_index = floor( (float)index/8);
  int bit_index = index%8;

  return (1<<(7-bit_index) & tree[byte_index]) ? 1 : 0;
}


int data_index_root_tt(__local uchar* tree)
{
  return as_int((uchar4) (tree[4], tree[5], tree[6], tree[7]));
}

// returns data index assuming root data is located at 0
int get_relative_index_tt(__local uchar* time_tree, int bit_index, __constant uchar* bit_lookup) 
{
  if (bit_index < 3)
    return bit_index;

  //otherwise get parent index, parent byte index and relative bit index
  uchar oneuplevel = (bit_index-1)>>1 ;   //bit index of parent
  uchar byte_index = floor( (float)oneuplevel/8);         //byte where parent is found

  //count pre parent bits
  int count=0;
  for (int i=0; i<byte_index; ++i)
    count += bit_lookup[time_tree[i]];

  //dont forget parent bits occurring the parent BYTE
  uchar sub_bit_index = 8- oneuplevel%8;
  uchar temp = time_tree[byte_index]>>sub_bit_index;

  count = count + bit_lookup[temp];

  uchar finestleveloffset=(bit_index-1)&(2-1);
  count = 2*count+1 +finestleveloffset;

  return count;
}



int traverse_tt(__local uchar* time_tree, float t)
{
  ushort deepest = 6; //HARDCODED FOR NOW
  
  ushort depth = 0;

  //bit index to be returned
  int bit_index = 0;

  //clamp point
  float range_min = 0;
  float range_max = cell_len_tt(0);

  //if outside of range
  if(t < range_min || t >= range_max)
    return -1;

  // while the curr node has children
  while (bit_at_tt(time_tree, bit_index) && depth < deepest ) {
    if(t < range_min + (range_max-range_min)/2 )
      bit_index = (2*bit_index + 1);                      //left child
    else
      bit_index = (2*bit_index + 2);                      //right child

    depth++;

    if(t < range_min + (range_max-range_min)/2 )
      range_max -= cell_len_tt(bit_index);
    else
      range_min += cell_len_tt(bit_index);

  }
  return bit_index;
  }
