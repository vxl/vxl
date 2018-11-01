// This is gel/vdgl/vdgl_fit_line.cxx

#include "vdgl_fit_line.h"
#include <vnl/vnl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>


// This function fits a line to the points in the edgel_chain  in the least-squares sense.

vsol_line_2d_sptr vdgl_fit_line(vdgl_edgel_chain &chain)
{
   int SIZE = chain.size();
   vdgl_edgel ed;
   vdgl_edgel ed1;
   vdgl_edgel ed2;
   vnl_vector<double>  x(SIZE);
   vnl_vector<double>  y(SIZE);

   for (int i=0;i<SIZE;i++)
   {
      ed = chain.edgel(i);
          x(i) = ed.get_x();
          y(i) = ed.get_y();
   }

   // Denoting S_xx = sum( x(0)*x(0) + x(1) * x(1) + ......)
   //          S_xy = sum( x(0)*y(0) + x(1) * y(1) + ......) and so on

   double S_x,S_y,S_xx,S_yy,S_xy;

   S_x=S_y=S_xx=S_yy=S_xy=0;

   for (int i=0;i<SIZE;i++)
   {
     S_x   = S_x + x(i) ;
     S_y   = S_y + y(i) ;
     S_xy  = S_xy + x(i) * y(i);
     S_xx  = S_xx + x(i) * x(i);
     S_yy  = S_yy + y(i) * y(i);
   }

   // Solving the for the coefficients m,c

   double m = (S_x * S_y - SIZE * S_xy) / (S_x * S_x - S_xx) ;
   double c = ( S_xx * S_y - S_xy * S_x) / ( SIZE * S_xx - S_x * S_x) ;

   ed1 = chain.edgel(0);
   ed2 = chain.edgel(SIZE-1);

   vsol_point_2d  *x1;
   vsol_point_2d  *x2;

   x1 = new vsol_point_2d(ed1.get_x(), m * ed1.get_x() + c);
   x2 = new vsol_point_2d(ed2.get_x(), m * ed2.get_x() + c);

   vsol_line_2d   *line;

  line = new vsol_line_2d(x1,x2);

  vsol_line_2d_sptr myline(line);
  return myline;
}
