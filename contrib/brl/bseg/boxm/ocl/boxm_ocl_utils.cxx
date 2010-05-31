
#include "boxm_ocl_utils.h"


void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
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


void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data)
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

void pack_cell_data(boct_tree_cell<short, float> * cell_ptr, vnl_vector_fixed<float, 16> &data)
{
  data.fill(0.0f);
  float cell_data = cell_ptr->data();
  data[0]=cell_data; // alpha
}
