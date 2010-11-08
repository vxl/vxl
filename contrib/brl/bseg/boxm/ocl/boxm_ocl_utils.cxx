// allocate child cells on the array
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdlib.h> // for std::malloc()
#if !defined (_WIN32) && !defined(__APPLE__)
#include <malloc.h> // for memalign()
#endif

#include <vcl_iostream.h>
void boxm_ocl_utils ::split(vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
                            int parent_ptr,
                            int& child_ptr)
{
  child_ptr = cell_array.size();
  for (unsigned i=0; i<8; i++) {
    vnl_vector_fixed<int, 4> cell(0);
    cell[0]= parent_ptr;
    cell[1]=-1;
    cell[2]=-1;
    cell_array.push_back(cell);
  }
}


//Print tree array
void boxm_ocl_utils ::print_tree_array(int* tree, unsigned numcells, float* data)
{
  unsigned cell_size = 4;
  for (unsigned i = 0, j = 0; i<numcells*cell_size; i+=cell_size, j++) {
    int parent = tree[i];
    int child = tree[i+1];

    //find alpha value - remember data size is 16
    int dataIndex = tree[i+2];
    float alpha = data[dataIndex*16];
    vcl_cout<<"[index: "<<j<<"] [parent: "<<parent<<"] [child: "<<child<<"] [alpha: "<<alpha<<"] [dataIndex: "<<dataIndex<<']'
            <<vcl_endl;
  }
}

//print multiblock tree
//blocks = multi block tree array (1 cell = 4 ints)
//block_ptrs = stores where blocks begin and how big they are (1 block_ptr = 2 ints)
void boxm_ocl_utils::print_multi_block_tree(int* blocks, int* block_ptrs, int numBlocks, float* data)
{
  //list all of the blocks
  vcl_cout<<"Blocks at: ";
  for (int i=0; i<numBlocks; i++)
    vcl_cout<<block_ptrs[2*i]<<" (size "<<block_ptrs[2*i+1]<<"), ";
  vcl_cout<<vcl_endl;

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
}


void* boxm_ocl_utils ::alloc_aligned(unsigned n, unsigned unit_size, unsigned block_size)
{
#if defined (_WIN32)
  return _aligned_malloc(n * unit_size, block_size);
#elif defined(__APPLE__)
  return vcl_malloc(n * unit_size * block_size);
#else
  return memalign(block_size, n * unit_size);
#endif
}


void boxm_ocl_utils ::free_aligned(void* ptr)
{
  if (ptr) {
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
  }
  ptr = NULL;
}


bool boxm_ocl_utils ::verify_format(vcl_vector<vnl_vector_fixed<int, 4> > cell_array)
{
  unsigned curr_index = 0;
  vcl_stack<int> open;
  open.push(0);
  while (!open.empty()) {
    int currNode = open.top();
    open.pop();
    int child_ptr = cell_array[currNode][1];

    // if the current node has no children, nothing to verify
    if (child_ptr < 0) {
      continue;
    }
    // if child pointer isn't to the right place..
    if ((unsigned int)child_ptr != curr_index+1) {
      vcl_cout<<"Children of "<<currNode<<" not in the right place"<<vcl_endl
              <<"should be at "<<curr_index+1<<", actually at "<<child_ptr<<vcl_endl;
      return false;
    }

    // push children on stack in reverse order
    for (int i=7; i>=0; i--) {
      open.push(child_ptr+i);
    }
    curr_index += 8;
  }
  return true;
}


bool boxm_ocl_utils ::writetree(vcl_string tree_file,cl_int* cell_array, unsigned int tree_size )
{
  vcl_ofstream ofile(tree_file.c_str());
  if (!ofile)
    return false;
  ofile<<tree_size;
  for (unsigned i=0;i<tree_size*4;i++)
    ofile<<' '<<cell_array[i];
  ofile.close();

  return true;
}

cl_int* boxm_ocl_utils ::readtree(vcl_string tree_file, unsigned int & tree_size )
{
  vcl_ifstream ifile(tree_file.c_str());
  if (!ifile)
    return 0;
  ifile>>tree_size;
  if (tree_size<=0)
    return 0;

  cl_int * cell_array=(cl_int*)boxm_ocl_utils::alloc_aligned(tree_size,sizeof(cl_int4),16);
  for (unsigned i=0;i<tree_size*4;i++)
    ifile>>cell_array[i];
  ifile.close();
  return cell_array;
}

bool boxm_ocl_utils ::writetreedata(vcl_string tree_data_file,cl_float* data_array, unsigned int  tree_data_size )
{
  vcl_ofstream ofile(tree_data_file.c_str());
  if (!ofile)
    return false;
  ofile<<tree_data_size;
  for (unsigned i=0;i<tree_data_size*16;i++)
    ofile<<' '<<data_array[i];

  //ofile.write(reinterpret_cast<const char*>(&tree_data_size),sizeof(int));
  //ofile.write(reinterpret_cast<const char*>(data_array),sizeof(cl_float16)*tree_data_size);
  ofile.close();

  return true;
}

cl_float * boxm_ocl_utils ::readtreedata(vcl_string tree_data_file, unsigned int &tree_data_size)
{
  vcl_ifstream ifile(tree_data_file.c_str());
  if (!ifile)
    return 0;
  ifile>>tree_data_size;
  if (tree_data_size<=0)
    return 0;
  cl_float * data_array=(cl_float*)boxm_ocl_utils::alloc_aligned(tree_data_size,sizeof(cl_float16),16);
  for (unsigned i=0;i<tree_data_size*16;i++)
    ifile>>data_array[i];
  ifile.close();
  return data_array;
}

int boxm_ocl_utils::getBufferIndex(bool rand,
                                   vbl_array_1d<ushort2> mem_ptrs,
                                   vbl_array_1d<unsigned short> blocksInBuffer,
                                   int BUFF_LENGTH,
                                   int blocks_per_buffer,
                                   int tree_size,
                                   vnl_random& random)
{
  int num_buffers = mem_ptrs.size()-1;
  if (rand) {
    //choose random buff index to start out with
    int buffIndex = random.lrand32(0, num_buffers-1);
#ifdef DEBUG
    vcl_cout<<"Random buff chosen..."<<buffIndex<<vcl_endl;
#endif
    unsigned short start = mem_ptrs[buffIndex][0];
    unsigned short end   = mem_ptrs[buffIndex][1];
    int freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start); //free cells in data buffer

    //if there isn't enough space in this buffer, find another one
    if (freeSpace < tree_size || blocksInBuffer[buffIndex] >= blocks_per_buffer) {
#ifdef DEBUG
      vcl_cout<<"Not going with first buffer "<<vcl_endl;
#endif
      int bCount = 0;
      bool buffFound = false;
      while (!buffFound && bCount < num_buffers*3) {
        bCount++;
        buffIndex = random.lrand32(0, num_buffers-1);
        start=mem_ptrs[buffIndex][0];
        end = mem_ptrs[buffIndex][1];
        freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
        if (freeSpace >= tree_size && blocksInBuffer[buffIndex] < blocks_per_buffer)
          buffFound = true;
      }
      if (!buffFound) {
#ifdef DEBUG
        vcl_cout<<"second attempt(s) didn't work, deterministically seeking buffer"<<vcl_endl;
#endif
        //resort to a deterministic search
        for (int bInt=0; bInt<num_buffers+1; bInt++) {
          unsigned short start = mem_ptrs[bInt][0];
          unsigned short end   = mem_ptrs[bInt][1];
          unsigned short num   = blocksInBuffer[bInt];
          int freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
          if (freeSpace >= tree_size && num < blocks_per_buffer) {
            buffIndex = bInt;
            buffFound = true;
          }
        }
      }
      if (!buffFound) {
        vcl_cout<<"OUT OF SPACE!!!! (RANDOM)"<<vcl_endl;
        for (unsigned int i=0; i<mem_ptrs.size(); ++i) {
          start = mem_ptrs[i][0];
          end   = mem_ptrs[i][1];
          freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
          vcl_cout<<" buff "<<i<<": "<<freeSpace<<" ["<<start<<','<<end<<']'
                  <<" #blocks: "<<blocksInBuffer[i]<<vcl_endl;
        }
        return -1;
      }
    }
    return buffIndex;
  }

  //NON RANDOM algo: find buffer that is the least full, put tree there.
  //revision: depending on size of tree, find different type of buffer

  //non random algo... filling buffers from top down

  //if it's a big tree, just find hte emptiest buffer
  int buffIndex = 0;
  bool validFound = false;
  if (tree_size >= 73) {
    int mostSpace = 0;
    for (int i=0; i<num_buffers+1; i++) {
      unsigned short start = mem_ptrs[i][0];
      unsigned short end   = mem_ptrs[i][1];
      unsigned short num   = blocksInBuffer[i];
      int freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
      if (freeSpace > mostSpace && num < blocks_per_buffer && freeSpace >= tree_size) {
        buffIndex = i;
        mostSpace = freeSpace;
        validFound = true;
      }
    }
  }

  //otherwise find the emptiest tree buffer
  else {
    int fewestBlocks = blocks_per_buffer+1;
    for (int i=0; i<num_buffers+1; i++) {
      unsigned short start = mem_ptrs[i][0];
      unsigned short end   = mem_ptrs[i][1];
      unsigned short num   = blocksInBuffer[i];
      int freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
      if (num < fewestBlocks &&  freeSpace >= tree_size && num < blocks_per_buffer) {
        buffIndex = i;
        fewestBlocks = num;
        validFound = true;
      }
    }
  }

  if (!validFound)  {
    vcl_cout<<"OUT OF SPACE!!!! (DETERMINISTIC)"<<vcl_endl;
    for (unsigned int i=0; i<mem_ptrs.size(); ++i) {
      unsigned short start = mem_ptrs[i][0];
      unsigned short end   = mem_ptrs[i][1];
      unsigned short freeSpace = (start >= end)? start-end : BUFF_LENGTH - (end-start);
      vcl_cout<<" buff "<<i<<": "<<freeSpace<<" ["<<start<<','<<end<<']'
              <<" #blocks: "<<blocksInBuffer[i]<<vcl_endl;
    }
    return -1;
  }
  return buffIndex;
}


void boxm_ocl_utils::compare_bit_scenes(boxm_ocl_bit_scene* one, boxm_ocl_bit_scene* two)
{
  vcl_cout<<"Comparing bit scenes"<<vcl_endl;

  //1. make sure block sizes/num are the same
  unsigned int numX = one->blocks_.get_row1_count();
  unsigned int numY = one->blocks_.get_row2_count();
  unsigned int numZ = one->blocks_.get_row3_count();
  if (numX != two->blocks_.get_row1_count() ||
      numY != two->blocks_.get_row2_count() ||
      numZ != two->blocks_.get_row3_count()) {
    vcl_cout<<"Scene Sizes are different "<<vcl_endl;
    return;
  }

  //2. compare alpha value for each block (by layer
  double error = 0.0;
  int cmpCount = 0;
  int diffCount = 0;
  int initCount = 0;
  int numConvBigger = 0;
  for (unsigned int i=0; i<numX; i++) {
    for (unsigned int j=0; j<numY; j++) {
      for (unsigned int k=0; k<numZ; k++) {
        cmpCount++;
        //get alpha data for block one
        unsigned short buffIndex = one->blocks_(i,j,k)[0];
        unsigned short buffOffset = one->blocks_(i,j,k)[1];
        float alpha_one = one->data_buffers_(buffIndex,buffOffset)[0];
        //get alpha for block 2
        buffIndex = two->blocks_(i,j,k)[0];
        buffOffset = two->blocks_(i,j,k)[1];
        float alpha_two = two->data_buffers_(buffIndex, buffOffset)[0];

        if (alpha_one >= 1.0f && alpha_two >= 1.0f) {
          if ( vcl_fabs(alpha_one-alpha_two) > 1e-5f ) {
            vcl_cout<<"DIFFERENCE at block "<<i<<','<<j<<','<<k
                    <<";  alphas: "<<alpha_one<<','<<alpha_two<<vcl_endl;
            diffCount++;

            error += (double) (alpha_one-alpha_two)* (double) (alpha_one-alpha_two);
            if ( alpha_two > alpha_one )
              numConvBigger ++ ;
          }
          else {
            initCount++;
          }
        }
        else {
          initCount++;
        }
      }
    }
  }

  vcl_cout<<"Blocks compared:   "<<cmpCount<<vcl_endl
          <<"differences found: "<<diffCount<<vcl_endl
          <<"converted bigger : "<<numConvBigger<<vcl_endl
          <<"blocks unchanged:  "<<initCount<<vcl_endl
          <<"Squared ERROR:     "<<error<<vcl_endl
          <<"mean sse     :     "<<error/cmpCount<<vcl_endl;

#if 0
  vcl_cout<<"===SCENE STATS=================================="<<vcl_endl;
  //simple count of cells
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  int totalCells=0;
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); iter++)
  {
    //in each block get the size of the tree - spit it out:
    ushort2 offsets = (*iter);
    unsigned short buffIndex = offsets[0];
    unsigned short buffOffset = offsets[1];
    uchar16 tree = tree_buffers_[buffIndex][buffOffset];
    int numParents = 0;
    for (int i=0; i<10; i++) {
      numParents += bits[tree[i]];
    }
    int treeCells = numParents*8 + 1;
    totalCells += treeCells;
  }
  vcl_cout<<"TOTAL NUMBER OF CELLS IN SCENE = "<<totalCells<<vcl_endl;

  //theoretical voxel count
  int numVoxels = blocks_.get_row1_count()*blocks_.get_row2_count()*blocks_.get_row3_count() * 8*8*8;
  vcl_cout<<"Theoretical Number of Voxels: "<<numVoxels<<vcl_endl
          <<"==============================================\n"<<vcl_endl;

  //probe zero alpha values
  int zeroCells=0;
  vbl_array_3d<ushort2>::iterator iter;
  for (iter = blocks_.begin(); iter != blocks_.end(); iter++)
  {
    //in each block get the size of the tree - spit it out:
    ushort2 offsets = (*iter);
    unsigned short buffIndex = offsets[0];
    unsigned short buffOffset = offsets[1];
    uchar16 tree = tree_buffers_[buffIndex][buffOffset];

    unsigned short hi  = (unsigned short) tree[10];
    unsigned short lo  = (unsigned short) tree[11];
    unsigned short dat = (hi<<8) | lo;

    if (dat != buffOffset)
      vcl_cout<<"Dat: "<<dat<<" != "
  }
#endif // 0
}


void boxm_ocl_utils::bit_lookup_table(unsigned char* bits)
{
  unsigned char b[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                       1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                       1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                       1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                       2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                       3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                       3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                       4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  for (int i=0; i<256; i++) bits[i] = b[i];
}
