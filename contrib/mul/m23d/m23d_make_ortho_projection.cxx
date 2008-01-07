#include "m23d_make_ortho_projection.h"
//:
// \file
// \author Tim Cootes
// \brief Return 2 x 3 projection matrix based on viewing from angle (a,b)

#include <m23d/m23d_rotation_matrix.h>

//: Return 2 x 3 projection matrix based on viewing from angle (Ax,Ay,Az)
//  If Ax=Ay=Az, then returns matrix (I|0)
vnl_matrix<double> m23d_make_ortho_projection(double Ax, double Ay, double Az)
{
  vnl_matrix<double> R=m23d_rotation_matrix(Ax,Ay,Az);
  return R.extract(2,3);
}

// Return projection matrix combination for ns shapes, nm modes
vnl_matrix<double> m23d_make_ortho_projection(vnl_random& r,
                                              unsigned ns, unsigned nm,
                                              bool first_is_identity,
                                              bool basis_true)
{
  vnl_matrix<double> P(2*ns,3*(1+nm));
  for (unsigned i=0;i<ns;++i)
  {
    double a = r.drand64(-0.5,0.5);
    double b = r.drand64(-0.5,0.5);
    double c = r.drand64(-0.5,0.5);
    vnl_matrix<double> Pi=m23d_make_ortho_projection(a,b,c);
    if (i==0 && first_is_identity)
      Pi=m23d_make_ortho_projection(0,0,0);

    for (unsigned j=0;j<=nm;++j)
    {
      double w=r.drand64(-1.0,1.0);
      if (nm==0) w=1.0;  // Ensure no scaling effects for rigid case

      if (basis_true)
      {
        // Force basis constraints to be true
        if (i<=nm && j<=nm)
        {
          if (i==j) w=1.0;
          else      w=0.0;
        }
      }

      P.update(w*Pi,i*2,j*3);
    }
  }
  return P;
}


