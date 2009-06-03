#ifndef bvpl_neighb_operator_h_
#define bvpl_neighb_operator_h_

template <class T, class F>
class bvpl_neighb_operator
{
public:
  bvpl_neighb_operator(const F& functor):func_(functor) {}
  ~bvpl_neighb_operator() {}
  void convolve(bvpl_subgrid_iterator<T>& subgrid, bvpl_kernel_iterator<T>& kernel, bvpl_subgrid_iterator<T>& output);
private:
  F func_;
};

#endif