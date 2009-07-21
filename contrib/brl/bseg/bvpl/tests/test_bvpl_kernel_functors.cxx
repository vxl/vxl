//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <bvpl/bvpl_edge3d_kernel_factory.h>
#include <bvpl/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_random.h>

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

#include <vbl/vbl_array_3d.h>
#include <bvxm/grid/bvxm_opinion.h>

template<class data_type>
void fill_in_data(vbl_array_3d<data_type> & data,data_type min_p, data_type max_p, vnl_vector_fixed<float,3> axis)
{
  unsigned ni=data.get_row1_count();
  unsigned nj=data.get_row1_count();
  unsigned nk=data.get_row1_count();

  float ci=ni*0.5f;
  float cj=nj*0.5f;
  float ck=nk*0.5f;
  for (unsigned i=0;i<ni;i++)
  {
    for (unsigned j=0;j<nj;j++)
    {
      for (unsigned k=0;k<nk;k++)
      {
        if ((i-ci)*axis[0]+(j-cj)*axis[1]+(k-ck)*axis[2]>=0)
          data(i,j,k)=max_p;
        else
          data(i,j,k)=min_p;
      }
    }
  }
}


template <class F, class data_type>
data_type run_kernel_at_the_center(vbl_array_3d<data_type> & data, bvpl_kernel_sptr kernel, F func)
{
  unsigned ni=data.get_row1_count();
  unsigned nj=data.get_row1_count();
  unsigned nk=data.get_row1_count();

  unsigned ci=ni/2;
  unsigned cj=nj/2;
  unsigned ck=nk/2;

  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  //reset the iterator
  kernel_iter.begin();
  while (!kernel_iter.isDone()) {
    vgl_point_3d<int> idx = kernel_iter.index();
    data_type val;
    val=data(ci+idx.x(),cj+idx.y(),ck+idx.z());
    //vcl_cout<< val << "at " << idx <<vcl_endl;
    bvpl_kernel_dispatch d = *kernel_iter;
    func.apply(val, d);
    ++kernel_iter;
  }
  return func.result();
}

template <class F,class data_type >
bool is_correct_solution(bvpl_kernel_vector_sptr kernel_vec,
                         vbl_array_3d<data_type> & data, F func,
                         data_type min_p, data_type max_p, float sigma_noise, data_type maxval)
{
  vnl_vector_fixed<float,3> axis;
  bool flag=true;
#ifdef DEBUG
  float entropy_sum=0.0f;
#endif

  vnl_random rand;
  for (unsigned j=0;j<kernel_vec->kernels_.size();j++)
  {
    axis=kernel_vec->kernels_[j].first;
    axis[0]+=float(rand.normal()*sigma_noise);
    axis[1]+=float(rand.normal()*sigma_noise);
    axis[2]+=float(rand.normal()*sigma_noise);

    data.fill(min_p);
    fill_in_data<data_type> (data,min_p,max_p,axis);

    data_type max_val=maxval;
    unsigned axis_result=0;
    for (unsigned i=0;i<kernel_vec->kernels_.size();i++)
    {
      data_type val=run_kernel_at_the_center<F>(data,kernel_vec->kernels_[i].second,func);
#ifdef DEBUG
      if (val>0)
        entropy_sum-=val*vcl_log(val);
#endif
      if (val>max_val)
      {
        max_val=val;
        axis_result=i;
      }
    }
    if (axis_result!=j)
    {
      vcl_cout<<"Val: "<<max_val<<" Result axis: "<<kernel_vec->kernels_[axis_result].first
              <<"Orig axis: "<<axis<<vcl_endl;
      flag=false;
    }
#ifdef DEBUG
    vcl_cout<<"Entropy Sum "<<entropy_sum<<vcl_endl;
#endif
  }
  return flag;
}

MAIN(test_bvpl_kernel_functors)
{
  vbl_array_3d<float> data(100,100,100);
  data.fill(0.0f);

  bool result=false;
  float min_p=0.1f;
  float max_p=0.9f;

  float max_val=0.0f;

  bvpl_edge3d_kernel_factory kernels_3d(5,5,5);
  //: get vector of kernel
  bvpl_kernel_vector_sptr kernel_vec=kernels_3d.create_kernel_vector();
  bvpl_edge_algebraic_mean_functor<float> mean_functor;
  bvpl_edge_geometric_mean_functor<float> geom_functor;

  float sigma_noise=0.0f;
  result=is_correct_solution< bvpl_edge_algebraic_mean_functor<float>, float >(kernel_vec,data,mean_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Algebraic mean functor with no noise ", true,result);
  result=is_correct_solution< bvpl_edge_geometric_mean_functor<float>, float  >(kernel_vec,data,geom_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Geometric Mean functor with no noise ", true,result);

  //: add noise
  sigma_noise=0.1f;
  result=is_correct_solution< bvpl_edge_algebraic_mean_functor<float>, float  >(kernel_vec,data,mean_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Algebraic mean functor with  noise ", true,result);
  result=is_correct_solution< bvpl_edge_geometric_mean_functor<float>, float  >(kernel_vec,data,geom_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Geometric Mean functor with  noise ", true,result);


  vbl_array_3d<bvxm_opinion> opinion_data(100,100,100);
  bvxm_opinion minp(1.0,0.0);
  bvxm_opinion maxp(0.0,1.0);
  bvxm_opinion max_val_opinion(1.0,0.0);

  opinion_data.fill(minp);
  sigma_noise=0.0f;

  bvpl_opinion_functor opinion_functor;
  result=is_correct_solution< bvpl_opinion_functor, bvxm_opinion >(kernel_vec,opinion_data,opinion_functor,minp,maxp,sigma_noise,max_val_opinion);
  TEST("Test Opinion functor with no noise ", true,result);

  return 0;
}

// Template instantiation, only used in this test file:
#include <vbl/vbl_array_3d.txx>
VBL_ARRAY_3D_INSTANTIATE(bvxm_opinion);
