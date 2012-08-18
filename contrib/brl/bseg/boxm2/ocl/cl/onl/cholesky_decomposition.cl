//:
// \file

//: Cholesky decomposition
// The $LL^\top = A$ and the L matrix is overwritten on A as lower triangular matrix.

void cholesky_decomposition(__local float* A, int n)
{
  unsigned int p = get_local_size(0);
  unsigned int u = get_local_id(0);

  for (unsigned int k=0; k<n; k++) {
    float s = sqrt(A[k*n + k]);
    for (unsigned int i=k+u; i<n; i+=p) {
      A[i*n + k] /= s;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    for (unsigned int j=k+1+u; j<n; j+=p) {
      for (unsigned int i=j; i<n; i++) {
        A[i*n + j] -= A[j*n+k]*A[i*n+k];
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }
}

//: Using cholesky decomposition for solving Ax=b
// L is a lower trianguler matrix overwritten on A
// \p L is a lower triangular matrix

void cholesky_solve( __local float* L, int n, __local float * b , __local float * x )
{
  cholesky_decomposition(L,n);
  if ( get_local_id(0) == 0)
  {
    // forward substitution
    for (unsigned i = 0; i < n; i++)
    {
      float sum  = 0.0f;
      for (unsigned j = 0; j < i; j++)
        sum += L[i*n+j]*x[j] ;
      x[i] = (b[i]-sum)/L[i*n+i];
    }
    for (unsigned i = 0; i < n; i++)
      b[i] = x[i];
    // backward substitution
    for (int i = n-1; i >=0 ; i--)
    {
      float sum  = 0.0f;
      for (unsigned j = n-1; j > i; j--)
        sum += L[j*n+i]*x[j] ;  // L[j*n+i] is the transpose of L[i*n+j]
      x[i] = (b[i]-sum)/L[i*n+i];
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);
}
