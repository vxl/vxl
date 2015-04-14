//:
// \file
// \brief Warp a set of points using a Thin Plate Spline
// \author Tim Cootes

#include <msm/msm_cubic_bezier.h>

#include <vul/vul_arg.h>
#include <vcl_fstream.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_cholesky.h>

void print_usage()
{
  vcl_cout << "Usage: msm_test_bezier"
           << vcl_endl;
  vcl_cout<<"Tests bezier"<<vcl_endl;

  vul_arg_display_usage_and_exit();

}

int main(int argc, char** argv)
{
  // Create a square
  vcl_vector<vgl_point_2d<double> > pts(4);
  
  pts[0].set(0,0);
  pts[1].set(0,1);
  pts[2].set(1,1);
  pts[3].set(1,0);
  
  msm_cubic_bezier bezier(pts,true);
  
  // Save resulting curve, using k points per segment
  // Test closed curve
  unsigned k=20;
  vcl_ofstream ofs("bez_curve.txt");
  for (unsigned i=0;i<pts.size();++i)
    for (unsigned j=0;j<k;++j)
    {
      vgl_point_2d<double> p = bezier.point(i,j/double(k));
      ofs<<p.x()<<" "<<p.y()<<vcl_endl;
    }
//  vgl_point_2d<double> p = bezier.point(pts.size()-1);
//  ofs<<p.x()<<" "<<p.y()<<vcl_endl;

  ofs.close();
  vcl_cout<<"Points (closed) saved to bez_curve.txt"<<vcl_endl;
  
  
  msm_cubic_bezier open_bezier(pts,false);
  
  // Save resulting curve, using k points per segment
  // Test open curve
  vcl_ofstream ofs3("bez_curve_open.txt");
  for (unsigned i=0;i<pts.size()-1;++i)
    for (unsigned j=0;j<k;++j)
    {
      vgl_point_2d<double> p = bezier.point(i,j/double(k));
      ofs3<<p.x()<<" "<<p.y()<<vcl_endl;
    }
  vgl_point_2d<double> p = bezier.point(pts.size()-1);
  ofs3<<p.x()<<" "<<p.y()<<vcl_endl;

  ofs3.close();
  vcl_cout<<"Points saved to bez_curve_open.txt"<<vcl_endl;

/*
  // Test matrix structure
  unsigned n=7;
  vnl_matrix<double> M(n,n,0.0);
  for (unsigned i=0;i<n;++i)
  {
    M(i,(i+n-1)%n)=1.0; M(i,i)=4.0; M(i,(i+1)%n)=1.0;
  }
  vcl_cout<<"M: \n"<<M<<vcl_endl;
  
  vnl_svd<double> svd(M);
  vcl_cout<<"Inverse: \n"<<svd.inverse()<<vcl_endl;
  
  vcl_cout<<"Singular values:\n"<<svd.W()<<vcl_endl;
  
  vnl_cholesky chol(M);
  vcl_cout<<"Inverse: \n"<<chol.inverse()<<vcl_endl;
*/

  // Generate equally spaced points
  vcl_vector<vgl_point_2d<double> > new_pts;
  bezier.equal_space(2,1, 17, 0.1, new_pts);
  
  for (unsigned i=1;i<new_pts.size();++i)
    vcl_cout<<i<<" Length: "<<(new_pts[i]-new_pts[i-1]).length()<<vcl_endl;

  vcl_ofstream ofs2("bez_equal_pts.txt");
  for (unsigned i=0;i<new_pts.size();++i)
  {
    ofs2<<new_pts[i].x()<<" "<<new_pts[i].y()<<vcl_endl;
  }
  


  ofs2.close();
  vcl_cout<<"Points saved to bez_equal_pts.txt"<<vcl_endl;

  return 0;
}

