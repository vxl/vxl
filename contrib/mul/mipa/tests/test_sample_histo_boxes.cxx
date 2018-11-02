// This is mul/mipa/tests/test_sample_histo_boxes.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
#include <mipa/mipa_sample_histo_boxes.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// \file
// \author Tim Cootes
// \brief test mipa_sample_histo_boxes

float sum_over_box(const vil_image_view<float>& im,
                   unsigned i0, unsigned j0, unsigned k,
                   unsigned ni, unsigned nj)
{
  float sum=0.0f;
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      sum+=im(i+i0,j+j0,k);
  return sum;
}

void mipa_print_h_vec(const vnl_vector<double>& v,
                      unsigned ni, unsigned nj, unsigned nb)
{
  unsigned c=0;
  for (unsigned j=0;j<2*nj;++j)
  {
    for (unsigned i=0;i<2*ni;++i)
    {
      for (unsigned k=0;k<nb;++k,++c)  std::cout<<v[c]<<' ';
      std::cout<<"  ";
    }
    std::cout<<std::endl;
  }
  for (unsigned j=0;j<nj;++j)
  {
    for (unsigned i=0;i<ni;++i)
    {
      for (unsigned k=0;k<nb;++k,++c)  std::cout<<v[c]<<' ';
      std::cout<<"  ";
    }
    std::cout<<std::endl;
  }
  for (unsigned k=0;k<nb;++k,++c)  std::cout<<v[c]<<' ';
  std::cout<<std::endl;
}

void test_sample_histo_boxes()
{
  std::cout << "*********************************\n"
           << " Testing mipa_sample_histo_boxes\n"
           << "*********************************\n";

  // Create histogram image
  unsigned nb=2;
  unsigned ni=9, nj=10;
  vil_image_view<float> histo(ni,nj,1,nb);

  std::cout<<"Histo image: "<<histo<<std::endl;

  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      for (unsigned k=0;k<nb;++k)
        histo(i,j,k)=10.0f*i+100.0f*j+k;

  std::cout<<"Test at (0,0):"<<std::endl;

  unsigned nbi=2,nbj=3;

  vnl_vector<double> v0;
  mipa_sample_histo_boxes_3L(histo,0,0,v0,nbi,nbj);

  mipa_print_h_vec(v0,nbi,nbj,nb);

  TEST("Size of v0",v0.size(),nb*(5*nbi*nbj+1));
  TEST_NEAR("First element",v0[0],0.0,1e-6);
  TEST_NEAR("2nd element",v0[1],1.0,1e-6);
  TEST_NEAR("3rd element",v0[2],10.0,1e-6);

  TEST_NEAR("a",v0[2*nb*nbi],histo(0,1,0),1e-6);
  TEST_NEAR("b",v0[2*nb*nbi+1],histo(0,1,1),1e-6);

  for (unsigned k=0;k<nb;++k)
  {
    float sum=sum_over_box(histo,0,0,k,2,2);
    TEST_NEAR("1st sum block",v0[4*nbi*nbj*nb+k],sum,1e-6);
  }

  for (unsigned k=0;k<nb;++k)
  {
    float sum=sum_over_box(histo,0,0,k,2*nbi,2*nbj);
    TEST_NEAR("Total sum",v0[5*nbi*nbj*nb+k],sum,1e-6);
  }

  std::cout<<"Test at (1,1):"<<std::endl;

  mipa_sample_histo_boxes_3L(histo,1,1,v0,nbi,nbj);
  mipa_print_h_vec(v0,nbi,nbj,nb);

  TEST("Size of v0",v0.size(),nb*(5*nbi*nbj+1));
  TEST_NEAR("First element",v0[0],110.0,1e-6);
  TEST_NEAR("2nd element",v0[1],111.0,1e-6);
  TEST_NEAR("3rd element",v0[2],120.0,1e-6);

  for (unsigned k=0;k<nb;++k)
  {
    float sum=sum_over_box(histo,1,1,k,2,2);
    TEST_NEAR("1st sum block",v0[4*nbi*nbj*nb+k],sum,1e-6);
  }

  for (unsigned k=0;k<nb;++k)
  {
    float sum=sum_over_box(histo,1,1,k,2*nbi,2*nbj);
    TEST_NEAR("Total sum",v0[5*nbi*nbj*nb+k],sum,1e-6);
  }
}

TESTMAIN(test_sample_histo_boxes);
