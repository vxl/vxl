#include "vnl_affine_approx.h"
#include <assert.h>

//
template <class T>
vnl_affine_approx<T>::vnl_affine_approx(const vnl_matrix<T> &H)
  : n(H.rows()-1)
  , A(n,n)
  , p(n)
  , v(n)
{
  assert(n+1==(int)H.columns());
  for (int i=0;i<n;i++) {
    for (int j=0;j<n;j++)
      A(i,j)=H(i,j)/H(n,n);
    p(i)=H(i,n)/H(n,n);
    v(i)=H(n,i)/H(n,n);
  }
  if (H(n,n) == 0)
    cerr << __FILE__ " : WARNING. H(n,n)=0" << endl;
}

//
template <class T>
vnl_affine_approx<T>::~vnl_affine_approx() {
}

// compute affine approximation at x, in the form x -> M x + t
template <class T>
bool vnl_affine_approx<T>::compute(const vnl_vector<T> x,
			      vnl_matrix<T> & M,
			      vnl_vector<T> & t) const
{
  assert((int)x.size() == n);
  assert((int)M.rows() == n);
  assert((int)M.columns() == n);
  assert((int)t.size() == n);

  T tmp=1+dot_product(v,x);
  vnl_vector<T> Hx_tmp=(A*x+p);
  M = (tmp*A - outer_product(Hx_tmp,v))/(tmp*tmp);
  t = Hx_tmp/tmp - M*x;
  return true;
}
