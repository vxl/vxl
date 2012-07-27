#ifdef QUADRATIC
void f(__local float * x,
       __local float * y,
       int m,
       int n,
       AuxArgs args)
{
  y[0] = (x[0]-2)*(x[0]-2) + (x[1]-1)*(x[1]-1)*(x[1]-1) ;
  y[1] = (x[0]-2)*(x[0]-2) + (x[2]-5)*(x[2]-5) ;
}

void Jacobian(__local float * x,
              __local float * J,
              int m,
              int n,
              AuxArgs args)
{
  J[0] = 2*(x[0]-2);
  J[1] = 3*(x[1]-1)*(x[1]-1);
  J[2] = 0;

  J[3] = 2*(x[0]-2);
  J[4] = 0;
  J[5] = 2*(x[2]-5);
}
#endif
