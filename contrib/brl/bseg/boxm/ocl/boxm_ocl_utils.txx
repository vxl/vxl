// This is brl/bseg/boxm/ocl/boxm_stack_ray_trace_manager.txx
#ifndef boxm_ocl_utils_txx_
#define boxm_ocl_utils_txx_

#include "boxm_ocl_utils.h"

template<class T>
void boxm_ocl_utils<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_SIMPLE_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha

  boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype appear=cell_data.appearance_;

  data[1]=1; // num of components
  data[2]=3; // size of component
  data[3]=appear.color();
  data[4]=appear.sigma();
  data[5]=appear.gauss_weight();
}

template<class T>
void boxm_ocl_utils<T>::pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  boxm_sample<BOXM_APM_MOG_GREY> cell_data = cell_ptr->data();
  data[0]=cell_data.alpha; // alpha
  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype appear=cell_data.appearance_;

  data[1]=(float)appear.num_components(); // num of components
  data[2]=3; // size of component
  unsigned j=3;
  for (unsigned i=0;i<data[1];i++)
  {
    data[j]=appear.distribution(i).mean();++j;
    data[j]=appear.distribution(i).var();++j;
    data[j]=appear.weight(i);++j;
  }
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
#if 0
  else{
    // Debug
    boct_loc_code<short> cd = cell_ptr->get_code();
    vgl_box_3d<double> lbb = cell_ptr->local_bounding_box(2);
    vcl_cout<< cell_ptr->level() << ' ' << cd << ' ' << lbb << '\n';
  }
#endif
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

#define BOXM_OCL_UTILS_INSTANTIATE(T) \
  template class boxm_ocl_utils<T >\

#endif

