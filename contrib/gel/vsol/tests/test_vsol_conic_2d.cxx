//*****************************************************************************
// File name: test_vsol_conic_2d.cxx
// Description: Test the vsol_conic_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/16| François BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h> // vcl_abs(double)
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

void f(const vsol_conic_2d::vsol_conic_type &t)
{
  switch(t)
    {
    case vsol_conic_2d::invalid:
      vcl_cout<<"invalid\n";
      break;
    case vsol_conic_2d::real_ellipse:
      vcl_cout<<"real_ellipse\n";
      break;
    case vsol_conic_2d::real_circle:
      vcl_cout<<"real_circle\n";
      break;
    case vsol_conic_2d::complex_ellipse:
      vcl_cout<<"complex_ellipse\n";
      break;
    case vsol_conic_2d::complex_circle:
      vcl_cout<<"complex_circle\n";
      break;
    case vsol_conic_2d::hyperbola:
      vcl_cout<<"hyperbola\n";
      break;
    case vsol_conic_2d::parabola:
      vcl_cout<<"parabola\n";
      break;
    case vsol_conic_2d::real_intersecting_lines:
      vcl_cout<<"real_intersecting_lines\n";
      break;
    case vsol_conic_2d::complex_intersecting_lines:
      vcl_cout<<"complex_intersecting_lines\n";
      break;
    case vsol_conic_2d::real_parallel_lines:
      vcl_cout<<"real_parallel_lines\n";
      break;
    case vsol_conic_2d::complex_parallel_lines:
      vcl_cout<<"complex_parallel_lines\n";
      break;
    case vsol_conic_2d::coincident_lines:
      vcl_cout<<"coincident_lines\n";
      break;
    default:
      vcl_cout<<"Impossible\n";
      break;
    }
}

//-----------------------------------------------------------------------------
// Entry point of the test program
//-----------------------------------------------------------------------------
void test_vsol_conic_2d()
{
  vsol_conic_2d_sptr p, q;
  double cx, cy, phi, cosphi, sinphi, width, height;

  p=q=new vsol_conic_2d(1,2,3,4,5,6);

  TEST("vsol_conic_2d::a()", p->a(), 1);
  TEST("vsol_conic_2d::b()", p->b(), 2);
  TEST("vsol_conic_2d::c()", p->c(), 3);
  TEST("vsol_conic_2d::d()", p->d(), 4);
  TEST("vsol_conic_2d::e()", p->e(), 5);
  TEST("vsol_conic_2d::f()", p->f(), 6);

  vnl_double_3x3 m = p->matrix();
  vcl_cout << "Matrix of p:\n" << m;

  p=new vsol_conic_2d(1,0,1,0,0,-1); // circle center=(0,0) r=1
  f(p->real_type());
  TEST("vsol_conic_2d::is_real_circle()", p->is_real_circle(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(0,0));
  p->ellipse_parameters(cx,cy,phi,width,height);
  TEST("vsol_conic_2d::ellipse_parameters(): cx", cx, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): cy", cy, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): phi", phi, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): width", width, 1);
  TEST("vsol_conic_2d::ellipse_parameters(): height", height, 1);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(0.25,0,1,0,0,-1); // ellipse center=(0,0) rx=2 ry=1
  f(p->real_type());
  TEST("vsol_conic_2d::is_real_ellipse()", p->is_real_ellipse(), true);
  TEST("!vsol_conic_2d::is_real_circle()", p->is_real_circle(), false);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(0,0));
  TEST("vsol_conic_2d::axis()",
       *(p->axis()),
       vsol_line_2d(vgl_point_2d<double>(-0.5, 0),vgl_point_2d<double>(0.5, 0))) ;
  p->ellipse_parameters(cx,cy,phi,width,height);
  TEST("vsol_conic_2d::ellipse_parameters(): cx", cx, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): cy", cy, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): phi", phi, 0);
  TEST("vsol_conic_2d::ellipse_parameters(): width", width, 2);
  TEST("vsol_conic_2d::ellipse_parameters(): height", height, 1);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(5,6,5,-16,-16,8); // ellipse: centre 1,1; axes 2,1; dir -45 degrees
  f(p->real_type());
  TEST("vsol_conic_2d::is_real_ellipse()", p->is_real_ellipse(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(1,1));
  p->ellipse_parameters(cx,cy,phi,width,height);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  // Ignore rounding errors:
  if (vcl_abs(q->a()-2.5) < 1e-12 && vcl_abs(q->b()-3) < 1e-12 && vcl_abs(q->d()+8) < 1e-12 &&
      vcl_abs(q->c()-2.5) < 1e-12 && vcl_abs(q->f()-4) < 1e-12 && vcl_abs(q->e()+8) < 1e-12)
    q->set(2.5, 3, 2.5, -8, -8, 4); 
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(52,-72,73,0,0,-100); // ellipse: centre 0,0; axes 2,1; dir atan(3/4)
  f(p->real_type());
  TEST("vsol_conic_2d::is_real_ellipse()", p->is_real_ellipse(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(0,0));
  p->ellipse_parameters(cx,cy,phi,width,height);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  // Ignore rounding errors:
  if (vcl_abs(q->a()-2.08) < 1e-12 && vcl_abs(q->b()+2.88) < 1e-12 &&
      vcl_abs(q->c()-2.92) < 1e-12 && vcl_abs(q->f()+4.0) < 1e-12)
    q->set(52, -72, 73, 25*q->d(), 25*q->e(), -100); 
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(1,0,0,0,-1,0); // parabola: y=x^2, x^2-y=0
  f(p->real_type());
  TEST("vsol_conic_2d::is_parabola()", p->is_parabola(), true);
  p->parabola_parameters(cx,cy,cosphi,sinphi);
  phi = -0.5;
  q->set_parabola_parameters(vgl_vector_2d<double>(cosphi,sinphi), vsol_point_2d(cx,cy), phi);
  TEST("vsol_conic_2d::set_parabola_parameters()", *q, *p);

  p=new vsol_conic_2d(0,0,1,-1,0,0); // parabola: x=y^2, y^2-x=0
  f(p->real_type());
  TEST("vsol_conic_2d::is_parabola()", p->is_parabola(), true);

  p=new vsol_conic_2d(0,1,0,0,0,-1); // hyperbola: y=1/x, xy-1=0
  f(p->real_type());
  TEST("vsol_conic_2d::is_hyperbola()", p->is_hyperbola(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(0,0));
  p->hyperbola_parameters(cx,cy,phi,width,height);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  // Ignore rounding errors:
  if (vcl_abs(q->a()) < 1e-12 && vcl_abs(q->b()+4) < 1e-12 &&
      vcl_abs(q->c()) < 1e-12 && vcl_abs(q->f()-4) < 1e-12)
    q->set(0.0, -4.0, 0.0, q->d(), q->e(), 4.0); 
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(11,-24,4,0,0,20); // hyperbola: centre 0,0; axes 2,-1; dir atan(4/3)
  f(p->real_type());
  TEST("vsol_conic_2d::is_hyperbola()", p->is_hyperbola(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(0,0));
  p->hyperbola_parameters(cx,cy,phi,width,height);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  // Ignore rounding errors:
  if (vcl_abs(q->a()-2.2) < 1e-12 && vcl_abs(q->b()+4.8) < 1e-12 &&
      vcl_abs(q->c()-0.8) < 1e-12 && vcl_abs(q->f()-4.0) < 1e-12)
    q->set(11, -24, 4, 5*q->d(), 5*q->e(), 20); 
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);

  p=new vsol_conic_2d(4,-24,11,16,2,-29); // hyperbola: centre 1,1; axes 1,-2; dir atan(-4/3)
  f(p->real_type());
  TEST("vsol_conic_2d::is_hyperbola()", p->is_hyperbola(), true);
  TEST("vsol_conic_2d::midpoint()", *(p->midpoint()), vsol_point_2d(1,1));
  p->hyperbola_parameters(cx,cy,phi,width,height);
  q->set_central_parameters(vsol_point_2d(cx,cy), width, height, phi);
  // Ignore rounding errors:
  if (vcl_abs(q->a()+0.8) < 1e-12 && vcl_abs(q->b()-4.8) < 1e-12 && vcl_abs(q->d()+3.2) < 1e-12 &&
      vcl_abs(q->c()+2.2) < 1e-12 && vcl_abs(q->f()-5.8) < 1e-12 && vcl_abs(q->e()+0.4) < 1e-12)
    q->set(-4, 24, -11, -16, -2, 29); 
  TEST("vsol_conic_2d::set_central_parameters()", *q, *p);
}

TESTMAIN(test_vsol_conic_2d);
