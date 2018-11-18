//:
// \file
// \brief Warp a set of points using a Thin Plate Spline
// \author Tim Cootes

#include <iostream>
#include <fstream>
#include <msm/msm_cubic_bezier.h>

#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_cholesky.h>

void print_usage()
{
  std::cout << "Usage: msm_test_bezier"
           << std::endl;
  std::cout<<"Tests bezier"<<std::endl;

  vul_arg_display_usage_and_exit();

}

int main(int argc, char** argv)
{
  // Create a square
  std::vector<vgl_point_2d<double> > pts(4);

  pts[0].set(0,0);
  pts[1].set(0,1);
  pts[2].set(1,1);
  pts[3].set(1,0);

  msm_cubic_bezier bezier(pts,true);

  // Save resulting curve, using k points per segment
  // Test closed curve
  unsigned k=20;
  std::ofstream ofs("bez_curve.txt");
  for (unsigned i=0;i<pts.size();++i)
    for (unsigned j=0;j<k;++j)
    {
      vgl_point_2d<double> p = bezier.point(i,j/double(k));
      ofs<<p.x()<<" "<<p.y()<<std::endl;
    }
//  vgl_point_2d<double> p = bezier.point(pts.size()-1);
//  ofs<<p.x()<<" "<<p.y()<<std::endl;

  ofs.close();
  std::cout<<"Points (closed) saved to bez_curve.txt"<<std::endl;


  msm_cubic_bezier open_bezier(pts,false);

  // Save resulting curve, using k points per segment
  // Test open curve
  std::ofstream ofs3("bez_curve_open.txt");
  for (unsigned i=0;i<pts.size()-1;++i)
    for (unsigned j=0;j<k;++j)
    {
      vgl_point_2d<double> p = bezier.point(i,j/double(k));
      ofs3<<p.x()<<" "<<p.y()<<std::endl;
    }
  vgl_point_2d<double> p = bezier.point(pts.size()-1);
  ofs3<<p.x()<<" "<<p.y()<<std::endl;

  ofs3.close();
  std::cout<<"Points saved to bez_curve_open.txt"<<std::endl;

/*
  // Test matrix structure
  unsigned n=7;
  vnl_matrix<double> M(n,n,0.0);
  for (unsigned i=0;i<n;++i)
  {
    M(i,(i+n-1)%n)=1.0; M(i,i)=4.0; M(i,(i+1)%n)=1.0;
  }
  std::cout<<"M: \n"<<M<<std::endl;

  vnl_svd<double> svd(M);
  std::cout<<"Inverse: \n"<<svd.inverse()<<std::endl;

  std::cout<<"Singular values:\n"<<svd.W()<<std::endl;

  vnl_cholesky chol(M);
  std::cout<<"Inverse: \n"<<chol.inverse()<<std::endl;
*/

  // Generate equally spaced points
  std::vector<vgl_point_2d<double> > new_pts;
  bezier.equal_space(2,1, 17, 0.1, new_pts);

  for (unsigned i=1;i<new_pts.size();++i)
    std::cout<<i<<" Length: "<<(new_pts[i]-new_pts[i-1]).length()<<std::endl;

  std::ofstream ofs2("bez_equal_pts.txt");
  for (unsigned i=0;i<new_pts.size();++i)
  {
    ofs2<<new_pts[i].x()<<" "<<new_pts[i].y()<<std::endl;
  }



  ofs2.close();
  std::cout<<"Points saved to bez_equal_pts.txt"<<std::endl;

  return 0;
}
