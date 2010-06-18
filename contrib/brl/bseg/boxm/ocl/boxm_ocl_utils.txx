// This is brl/bseg/boxm/ocl/boxm_ocl_utils.txx
#ifndef boxm_ocl_utils_txx_
#define boxm_ocl_utils_txx_
//:
// \file
#include "boxm_ocl_utils.h"
#if defined(unix) || defined(__unix) || defined(__unix__)
#include <malloc.h>
#endif
template<class T>
void boxm_ocl_utils<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_SIMPLE_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha

  boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype appear=cell_data.appearance_;
  data[1]=appear.color();
  data[2]=appear.sigma();
  data[3]=appear.gauss_weight();
}

template<class T>
void boxm_ocl_utils<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_MOG_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha
  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype appear=cell_data.appearance_;
  //: assuming 3 components
  data[1]=appear.distribution(0).mean();
  data[2]=appear.distribution(0).var();
  data[3]=appear.weight(0);
  data[4]=appear.distribution(0).num_observations;
  data[5]=appear.distribution(1).mean();
  data[6]=appear.distribution(1).var();
  data[7]=appear.weight(1);
  data[8]=appear.distribution(1).num_observations;
  data[9]=appear.distribution(2).mean();
  data[10]=appear.distribution(2).var();
  data[11]=appear.weight(2);
  data[12]=appear.distribution(2).num_observations;
  data[13]=appear.num_observations;

}

template<class T>
void boxm_ocl_utils<T>::pack_cell_data(boct_tree_cell<short, float> * cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  float cell_data = cell_ptr->data();
  data[0]=cell_data; // alpha
}


//: Recursive algorithm to take in a tree cell and copy structure and data into two arrays
template<class T>
void boxm_ocl_utils<T>::copy_to_arrays(boct_tree_cell<short, T >* cell_ptr,
									   vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
									   vcl_vector<vnl_vector_fixed<float, 16> >& data_array,
									   int cell_input_ptr)
{
  // cell_input_ptr is the array index for the cell being constructed
  // it already exists in the cell array but only has the parent index set
  // no data or child pointers

  // convert the data to 16 vector size
  vnl_vector_fixed<float, 16> data;
  boxm_ocl_utils<T>::pack_cell_data(cell_ptr,data);

  // data pointer will be at index == size after the push_back
  cell_array[cell_input_ptr][2] = data_array.size();
  data_array.push_back(data);
  cell_array[cell_input_ptr][3] = cell_ptr->level();
  // if the cell has chidren then they must be copied
  if (!cell_ptr->is_leaf()) {
    // initialize data values to null
    data_array[cell_array[cell_input_ptr][2]].fill(0.0);
    // create the children on the cell array
    int child_ptr = -1;
    split(cell_array, cell_input_ptr, child_ptr);
    cell_array[cell_input_ptr][1]=child_ptr;
    boct_tree_cell<short,T >* children =
      cell_ptr->children();
    for (unsigned i = 0; i<8; ++i) {
      boct_tree_cell<short, T >* child_cell_ptr = children + i;
      int child_cell_input_ptr = child_ptr +i;
      copy_to_arrays(child_cell_ptr, cell_array, data_array, child_cell_input_ptr);
    }
  }

}

// allocate child cells on the array
template<class T>
void boxm_ocl_utils<T>::split(vcl_vector<vnl_vector_fixed<int, 4> >& cell_array,
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
template<class T>
void boxm_ocl_utils<T>::print_tree_array(int* tree, unsigned numcells, float* data)
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

template<class T>
void* boxm_ocl_utils<T>::alloc_aligned(unsigned n, unsigned unit_size, unsigned block_size)
{
#if defined (_WIN32)
  return _aligned_malloc(n * unit_size, block_size);
#elif defined(__APPLE__)
  return malloc(n * unit_size, block_size);
#else
  return memalign(block_size, n * unit_size);
#endif
}

template<class T>
void boxm_ocl_utils<T>::free_aligned(void* ptr)
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

#define BOXM_OCL_UTILS_INSTANTIATE(T) \
  template class boxm_ocl_utils<T >\

#endif

