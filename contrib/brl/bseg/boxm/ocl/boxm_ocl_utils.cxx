// allocate child cells on the array
#include <boxm/ocl/boxm_ocl_utils.h>


 
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

 
void* boxm_ocl_utils ::alloc_aligned(unsigned n, unsigned unit_size, unsigned block_size)
{
#if defined (_WIN32)
  return _aligned_malloc(n * unit_size, block_size);
#elif defined(__APPLE__)
  return malloc(n * unit_size, block_size);
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
    if (child_ptr != curr_index+1) {
      vcl_cout<<"Children of "<<currNode<<" not in the right place"<<vcl_endl;
      vcl_cout<<"should be at "<<curr_index+1<<", actually at "<<child_ptr<<vcl_endl;
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


 
bool boxm_ocl_utils ::writetree(vcl_string tree_file,cl_int* cell_array, int tree_size )
{

    vcl_ofstream ofile(tree_file.c_str());
    if(!ofile)
        return false;
    ofile.write(reinterpret_cast<const char*>(&tree_size),sizeof(int));
    ofile.write(reinterpret_cast<const char*>(cell_array),sizeof(cl_int4)*tree_size);
    ofile.close();

    return true;

}
 
cl_int* boxm_ocl_utils ::readtree(vcl_string tree_file, int & tree_size )
{

    vcl_ifstream ifile(tree_file.c_str());
    if(!ifile)
        return 0;
    ifile.read(reinterpret_cast<char*>(&tree_size),sizeof(int));
    if(tree_size<=0)
        return 0;
    cl_int * cell_array=new cl_int[sizeof(cl_int4)*tree_size];
    ifile.read(reinterpret_cast<char*>(cell_array),sizeof(cl_int4)*tree_size);
    ifile.close();
    return cell_array;

}
 
bool boxm_ocl_utils ::writetreedata(vcl_string tree_data_file,cl_float* data_array, int  tree_data_size )
{

    vcl_ofstream ofile(tree_data_file.c_str());
    if(!ofile)
        return false;
    ofile.write(reinterpret_cast<const char*>(&tree_data_size),sizeof(int));
    ofile.write(reinterpret_cast<const char*>(data_array),sizeof(cl_float16)*tree_data_size);
    ofile.close();

    return true;

}
 
cl_float * boxm_ocl_utils ::readtreedata(vcl_string tree_data_file, int &tree_data_size)
{

    vcl_ifstream ifile(tree_data_file.c_str());
    if(!ifile)
        return 0;
    ifile.read(reinterpret_cast<char*>(&tree_data_size),sizeof(int));
    if(tree_data_size<=0)
        return 0;
    cl_float * data_array=new cl_float[sizeof(cl_float16)*tree_data_size];
    ifile.read(reinterpret_cast<char*>(data_array),sizeof(cl_float16)*tree_data_size);
    return data_array;

}