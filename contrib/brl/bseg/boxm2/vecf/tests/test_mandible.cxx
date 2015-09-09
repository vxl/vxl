//:
// \file
// \author J.L. Mundy
// \date 08/28/15


#include <testlib/testlib_test.h>
#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_mandible_params.h"
#include "../boxm2_vecf_mandible.h"
#include "../boxm2_vecf_spline_field.h"
#define BUILD_TEST_MANDIBLE
void test_mandible()
{
  vcl_string base_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/";
  //vcl_string axis_path = base_dir  + "mandible_spline_v0.txt";
  //vcl_string axis_path1 = base_dir  + "mandible_spline_v1.txt";
  //  vcl_string axis_path2 = base_dir  + "mandible_spline_v2.txt";
  vcl_string axis_path3 = base_dir  + "mandible_spline_v3.txt";
  vcl_string axis_display_path = base_dir  + "mandible_axis_vrml.wrl";
  vcl_string cross_path = base_dir + "mandible-2x-zero-samp-1.0-norm.txt";
  vcl_string cross_display_path = base_dir + "mandible_display_cross.wrl";
  vcl_string cross_normal_display_path = base_dir + "mandible_display_cross_normal.wrl";
  vcl_string cross_disks_display_path = base_dir + "mandible_display_cross_disks.wrl";
  vcl_ifstream istr(axis_path3.c_str());
   if(!istr){
     vcl_cout << "Bad axis file path " << axis_path3 << '\n';
     return;
   }
   boxm2_vecf_mandible mand;
   mand.read_axis_spline(istr);
#if 0
   // translate the mandible axis
   vgl_vector_3d<double> v(-3.46635, 0.0, 0.0);
   mand.translate(v);
   vcl_ofstream ostr(axis_path1.c_str());
   ostr << mand.axis();
   ostr.close();

   // scale the mandible along x
   double offset = 5.0;
   mand.offset_axis(offset);
   // tilt the ramus
   double dz_at_top = 5.0;
   mand.tilt_ramus(dz_at_top);

   double dy_at_chin = 5.0;
   mand.tilt_body(dy_at_chin);
#endif
  
  vcl_ifstream cistr(cross_path.c_str());
   if(!cistr){
     vcl_cout << "Bad cross file path " << cross_path << '\n';
     return;
   }
  
   mand.load_cross_section_pointsets(cistr);
   //
   // deformation goes here
    // scale the mandible along x
   
   double offset = 15.0;
   boxm2_vecf_spline_field field = mand.offset_axis(offset);

  boxm2_vecf_mandible mand_v = mand.apply_vector_field(field);

    // display the mandible axis
   vcl_ofstream vostr(axis_display_path.c_str());
   if(!vostr){
     vcl_cout << "Bad axis display file path " << axis_display_path << '\n';
     return;
   }
   mand_v.display_axis_spline(vostr);
 
   //
   // display the cross section plane normals
   vcl_ofstream cnostr(cross_normal_display_path.c_str());
   if(!cnostr){
     vcl_cout << "Bad cross section normal  display file path " << cross_normal_display_path << '\n';
     return;
   }
   mand_v.display_cross_section_planes(cnostr);

   // display the mandible cross section pointsets
   vcl_ofstream costr(cross_display_path.c_str());
   if(!costr){
     vcl_cout << "Bad cross section display file path " << cross_display_path << '\n';
     return;
   }
   mand_v.display_cross_section_pointsets(costr);

   vcl_ofstream cdostr(cross_disks_display_path.c_str());
   if(!cdostr){
     vcl_cout << "Bad cross section display file path " << cross_disks_display_path << '\n';
     return;
   }
   mand_v.display_surface_disks(cdostr);

}
TESTMAIN( test_mandible );
 
