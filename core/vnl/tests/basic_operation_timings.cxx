//:
// \file
// \brief Tool to test performance of varous vnl operations.
// \author Ian Scott

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_sample.h>
#include <vcl_ctime.h>
#include <vcl_algorithm.h>
const unsigned NI=280;
const unsigned NJ=256;


const unsigned nstests = 10;

template <class T>
void distance_squared(const vcl_vector<vnl_vector<T> > &s1, const vcl_vector<vnl_vector<T> > &s2, 
                   vcl_vector<T> & d, int n_loops)
{
  vnl_vector<double> stats(nstests);
  for (int st=0;st<nstests;++st)
  {
    vcl_clock_t t0=vcl_clock();
    for (int l=0;l<n_loops;++l)
    {
      for (unsigned i=1;i<d.size();++i)
        d[i] = vnl_vector_ssd(s1[i], s2[i]);
    }
    vcl_clock_t t1=vcl_clock();
    stats[st] = (1e9*((double(t1)-double(t0)))/((double)n_loops*(double)CLOCKS_PER_SEC));
  }
  vcl_sort(stats.begin(), stats.end());
  vcl_cout<<"  Mean: "<<stats.mean()
        <<"ns  +/-"<<stats((unsigned)(nstests*0.75))-stats((unsigned)(nstests*0.25))<<"ns\n"<<vcl_endl;
}

template <class T>
void dot_product(const vcl_vector<vnl_vector<T> > &s1, const vcl_vector<vnl_vector<T> > &s2, 
                   vcl_vector<T> & d, int n_loops)
{
  vnl_vector<double> stats(nstests);
  for (int st=0;st<nstests;++st)
  {
    vcl_clock_t t0=vcl_clock();
    for (int l=0;l<n_loops;++l)
    {
      for (unsigned i=1;i<d.size();++i)
        d[i] = dot_product(s1[i], s2[i]);
    }
    vcl_clock_t t1=vcl_clock();
    stats[st] = (1e9*((double(t1)-double(t0)))/((double)n_loops*(double)CLOCKS_PER_SEC));
  }
  vcl_sort(stats.begin(), stats.end());
  vcl_cout<<"  Mean: "<<stats.mean()
        <<"ns  +/-"<<stats((unsigned)(nstests*0.75))-stats((unsigned)(nstests*0.25))<<"ns\n"<<vcl_endl;
}
template <class T>
void mat_x_vec(const vnl_matrix<T> &s1, const vcl_vector<vnl_vector<T> > &s2, 
                   vcl_vector<vnl_vector<T> > & d, int n_loops)
{
  vnl_vector<double> stats(nstests);
  for (int st=0;st<nstests;++st)
  {
    vcl_clock_t t0=vcl_clock();
    for (int l=0;l<n_loops;++l)
    {
      for (unsigned i=1;i<d.size();++i)
        d[i] = s1 * s2[i];
    }
    vcl_clock_t t1=vcl_clock();
    stats[st] = (1e6*((double(t1)-double(t0)))/((double)n_loops*(double)CLOCKS_PER_SEC));
  }
  vcl_sort(stats.begin(), stats.end());
  vcl_cout<<"  Mean: "<<stats.mean()
        <<"us  +/-"<<stats((unsigned)(nstests*0.75))-stats((unsigned)(nstests*0.25))<<"us\n"<<vcl_endl;
}


template <class T>
void run_for_size(unsigned nv, unsigned nm, T dummy, char * type)
{
  const unsigned n_data = 10;
  vcl_vector<vnl_vector<T> > va(n_data), vb(n_data), vc(n_data);
  vcl_vector<T> na(n_data);
  vnl_matrix<T> ma(nm,nv);

  for (unsigned k=0;k<n_data;++k)
  {
    va[k].resize(nv);
    vb[k].resize(nv);
    vc[k].resize(nv);
    vnl_sample_uniform(va[k].begin(), va[k].end(), -10000,10000);
    vnl_sample_uniform(vb[k].begin(), vb[k].end(), -10000,10000);
  }
  vnl_sample_uniform(ma.begin(), ma.end(), -10000,10000);

  int n_loops = 1000000/nv;
  vcl_cout<<"\nTimes to operator on "<<type<<" "<<nv<<"-d vectors and "<<nm<<" x "<<nv<<" matrices"
          <<"  [Range= 75%tile-25%tile)]"<<vcl_endl;
  vcl_cout<<"Sum of square differences       ";
  distance_squared(va,vb,na,n_loops);
  vcl_cout<<"Vector dot product              ";
  dot_product(va,vb,na,n_loops);
  vcl_cout<<"Matrix x Vector multiplication  ";
  mat_x_vec(ma,vb,vc,n_loops/nm+1);
}

int main(int argc, char *argv[])
{
  vnl_sample_reseed(12354);
  run_for_size(20, 20, double(), "double");
  run_for_size(20, 20, float(), "float");
  run_for_size(300, 300, double(), "double");
  run_for_size(300, 300, float(), "float");
  run_for_size(100, 30000, double(), "double");
  run_for_size(100, 30000, float(), "float");
  return 0;
}
