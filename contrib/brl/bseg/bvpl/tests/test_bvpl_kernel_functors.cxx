#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <testlib/testlib_test.h>

#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_xx_kernel_factory.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_opinion_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/kernels/bvpl_create_directions.h>

#include <bvxm/grid/bvxm_opinion.h>
#include <bsta/bsta_gauss_sf1.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vbl/vbl_array_3d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class data_type>
void fill_in_data(vbl_array_3d<data_type> & data,data_type min_p, data_type max_p, vnl_float_3 axis)
{
  unsigned ni=data.get_row1_count();
  unsigned nj=data.get_row1_count();
  unsigned nk=data.get_row1_count();

  float ci=(float)ni*0.5f;
  float cj=(float)nj*0.5f;
  float ck=(float)nk*0.5f;
  for (unsigned i=0;i<ni;i++)
  {
    for (unsigned j=0;j<nj;j++)
    {
      for (unsigned k=0;k<nk;k++)
      {
        if (((float)i-ci)*axis[0]+((float)j-cj)*axis[1]+((float)k-ck)*axis[2]>=0.f)
          data(i,j,k)=max_p;
        else
          data(i,j,k)=min_p;
      }
    }
  }
}

// Specialization for gaussian second derivative. Here we want a wall or blob rather than an edge
template<>
void fill_in_data(vbl_array_3d<bsta_gauss_sf1> & data,bsta_gauss_sf1 min_p, bsta_gauss_sf1 max_p, vnl_float_3 axis)
{
  unsigned ni=data.get_row1_count();
  unsigned nj=data.get_row2_count();
  unsigned nk=data.get_row3_count();

  float ci=float(ni)*0.5f;
  float cj=float(nj)*0.5f;
  float ck=float(nk)*0.5f;
  for (unsigned i=0;i<ni;i++)
  {
    for (unsigned j=0;j<nj;j++)
    {
      for (unsigned k=0;k<nk;k++)
      {
        if (((float)i-ci)*axis[0]+((float)j-cj)*axis[1]+((float)k-ck)*axis[2]>=-1.f &&
            ((float)i-ci)*axis[0]+((float)j-cj)*axis[1]+((float)k-ck)*axis[2]<=1.f)
          data(i,j,k)=max_p;
        else
          data(i,j,k)=min_p;
      }
    }
  }
}


template <class F, class data_type>
data_type run_kernel_at_the_center(vbl_array_3d<data_type> & data, const bvpl_kernel_sptr& kernel, F func)
{
  unsigned ni=data.get_row1_count();
  unsigned nj=data.get_row2_count();
  unsigned nk=data.get_row3_count();

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
    //std::cout<< val << "at " << idx <<std::endl;
    bvpl_kernel_dispatch d = *kernel_iter;
    func.apply(val, d);
    ++kernel_iter;
  }
  return func.result();
}


template <class F,class data_type >
bool is_correct_solution(const bvpl_kernel_vector_sptr& kernel_vec,
                         vbl_array_3d<data_type> & data, F func,
                         data_type min_p, data_type max_p, float sigma_noise, data_type maxval)
{
  vnl_float_3 axis;
  bool flag=true;
#ifdef DEBUG
  float entropy_sum=0.0f;
#endif

  vnl_random rand;
  for (unsigned j=0;j<kernel_vec->kernels_.size();j++)
  {
    axis=kernel_vec->kernels_[j]->axis();
    axis[0]+=float(rand.normal()*sigma_noise);
    axis[1]+=float(rand.normal()*sigma_noise);
    axis[2]+=float(rand.normal()*sigma_noise);

    data.fill(min_p);
    fill_in_data<data_type> (data,min_p,max_p,axis);

    data_type max_val=maxval;
    unsigned axis_result=0;
    for (unsigned i=0;i<kernel_vec->kernels_.size();i++)
    {
      data_type val=run_kernel_at_the_center<F>(data,kernel_vec->kernels_[i],func);
#ifdef DEBUG
      if (val>0)
        entropy_sum-=val*std::log(val);
#endif
      if (val>max_val)
      {
        max_val=val;
        axis_result=i;
      }
    }
    if (axis_result!=j)
    {
      std::cout<<"Val: "<<max_val<<" Result axis: "<<kernel_vec->kernels_[axis_result]->axis()
              <<"Orig axis: "<<axis<<std::endl;
      flag=false;
    }
#ifdef DEBUG
    std::cout<<"Entropy Sum "<<entropy_sum<<std::endl;
#endif
  }
  return flag;
}

template <class F>//,class bsta_gauss_sf1 >
bool is_correct_solution(const bvpl_kernel_vector_sptr& kernel_vec,
                         vbl_array_3d<bsta_gauss_sf1> & data, F func,
                         bsta_gauss_sf1 min_p, bsta_gauss_sf1 max_p, float sigma_noise, bsta_gauss_sf1 maxval,
                         vnl_vector<float> &response)
{
  vnl_float_3 axis;
  bool flag=true;
#ifdef DEBUG
  float entropy_sum=0.0f;
#endif

  vnl_random rand;
  for (unsigned j=0;j<kernel_vec->kernels_.size();j++)
  {
    axis=kernel_vec->kernels_[j]->axis();
    axis[0]+=float(rand.normal()*sigma_noise);
    axis[1]+=float(rand.normal()*sigma_noise);
    axis[2]+=float(rand.normal()*sigma_noise);

    data.fill(min_p);
    fill_in_data<bsta_gauss_sf1> (data,min_p,max_p,axis);

    bsta_gauss_sf1 max_val=maxval;
    unsigned axis_result=0;
    for (unsigned i=0;i<kernel_vec->kernels_.size();i++)
    {
      bsta_gauss_sf1 val=run_kernel_at_the_center<F>(data,kernel_vec->kernels_[i],func);
#ifdef DEBUG
      if (val>0)
        entropy_sum-=val*std::log(val);
#endif
      if (std::abs(val.mean())>std::abs(max_val.mean()))
      {
        max_val=val;
        axis_result=i;
      }
    }
    response[j] = max_val.mean();
    if (axis_result!=j)
    {
      std::cout<<"Val: "<<max_val.mean() <<" Result axis: "<<kernel_vec->kernels_[axis_result]->axis()
              <<"Orig axis: "<<axis<<std::endl;
      flag=false;
    }
#ifdef DEBUG
    std::cout<<"Entropy Sum "<<entropy_sum<<std::endl;
#endif
  }
  return flag;
}

void test_edge_functors()
{
  vbl_array_3d<float> data(20,20,20);
  data.fill(0.0f);

  bool result=false;
  float min_p=0.1f;
  float max_p=0.9f;

  float max_val=0.0f;

  bvpl_edge3d_kernel_factory kernels_3d(-2,2,-2,2,-2,2);
  // get vector of kernel
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);
  bvpl_edge_algebraic_mean_functor<float> mean_functor;
  bvpl_edge_geometric_mean_functor<float> geom_functor;

  float sigma_noise=0.0f;
  result=is_correct_solution< bvpl_edge_algebraic_mean_functor<float>, float >(kernel_vec,data,mean_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Algebraic mean functor with no noise ", true,result);
  result=is_correct_solution< bvpl_edge_geometric_mean_functor<float>, float  >(kernel_vec,data,geom_functor,min_p,max_p,sigma_noise,max_val);
  TEST("Test Geometric Mean functor with no noise ", true,result);

  // add noise
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
}

void test_gaussian_kernels()
{
  bool result=false;
  bsta_gauss_sf1 edge(200,1);
  bsta_gauss_sf1 empty(10,1);

  vbl_array_3d<bsta_gauss_sf1> data(100,100,100);
  data.fill(empty);

  float var[3] ={1.0, 1.5, 2};

  for (float i : var)
  {
    bvpl_gauss3d_xx_kernel_factory factory(i, 1.5);
    // get vector of kernel
    bvpl_create_directions_b dir;
    bvpl_kernel_vector_sptr kernel_vec = factory.create_kernel_vector(dir);
    bvpl_gauss_convolution_functor functor;

    float sigma_noise=0.0f;
    vnl_vector<float> response(13);

    result=is_correct_solution< bvpl_gauss_convolution_functor>(kernel_vec,data,functor,empty,edge,sigma_noise,empty,response);
    TEST("Test gaussian functor with no noise ", true,result);

#if 0
    std::stringstream file;
    file << "response" << i << ".txt";
    std::string filename = file.str();
    std::fstream ofs(filename.c_str(), std::ios::out);
    if (!ofs.is_open()) {
      std::cerr << "error opening file for write!\n";
      return;
    }
    ofs << response;
    ofs.close();
#endif
  }
}

void test_gauss_convolve()
{
  //create some gaussians
  bsta_gauss_sf1 f1(0,1);
  bsta_gauss_sf1 f2(4,1);
  bsta_gauss_sf1 f3(12,3);

  bvpl_gauss_convolution_functor functor;
  bvpl_kernel_dispatch d(-6.0f);
  functor.apply(f1, d);
  d=3.0f;
  functor.apply(f2, d);
  d=-1.0f;
  functor.apply(f3, d);

  bsta_gauss_sf1 result = functor.result();

  TEST_NEAR("Mean test", result.mean(), 0.0f, 0.001f);
  TEST_NEAR("Var test", result.var(), 48.0f, 0.001f);
}

MAIN(test_bvpl_kernel_functors)
{
  //test algebraic, geometric and opinion functors
  std::cout << "-------------------------\n"
           << "Testing edged functors\n";
  test_edge_functors();

  //test gaussian convolution functor
  std::cout << "-------------------------\n"
           << "Testing gaussian functors\n";
  test_gauss_convolve();

  //test that kernels detect max response at appropriate plane direction
  test_gaussian_kernels();
  return 0;
}

// Template instantiation, only used in this test file:
#include <vbl/vbl_array_3d.hxx>
VBL_ARRAY_3D_INSTANTIATE(bvxm_opinion);
