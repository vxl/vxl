// This is brl/bseg/tests/sdet_test_region_proc.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_intensity_face.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <sdet/sdet_info_tracker_params.h>
#include <sdet/sdet_info_tracker.h>
#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

int main(int argc, char * argv[])
{
  int success=0, failures=0;
  int width = 200, height = 200;
  vil1_memory_image_of<unsigned char> I0, I1;
  I0.resize(width, height);
  I1.resize(width, height);
  //initialize images
  for(int x =0; x<width;x++)
    for(int y = 0; y<height; y++)
      {
        I0(x,y)=0;
        I1(x,y)=0;
      }
  //Make a centered 11x11 square
  for(int x =0; x<11;x++)
    for(int y = 0; y<11; y++)
      I0(x+45, y+45)=100;
  //Shifted 1 pixel
  for(int x =0; x<11;x++)
    for(int y = 0; y<11; y++)
      I1(x+46, y+46)=100;
  //Make a tracking box.
  int S = 100;
  int low = (width/2)-(S/2);
  int high = (width/2)+(S/2);
  vtol_vertex_sptr v0 = new vtol_vertex_2d(low, low);  
  vtol_vertex_sptr v1 = new vtol_vertex_2d(low, high);  
  vtol_vertex_sptr v2 = new vtol_vertex_2d(high, high);  
  vtol_vertex_sptr v3 = new vtol_vertex_2d(high, low); 
  vcl_vector<vtol_vertex_sptr> verts;
  verts.push_back(v0);   verts.push_back(v1);
  verts.push_back(v2);   verts.push_back(v3);
  vtol_face_2d_sptr box = new vtol_face_2d(verts);
  //set up the tracker
  sdet_info_tracker_params tp;
  sdet_info_tracker trk(tp);
  trk.set_image_0(I0);
  trk.set_initial_model(box);
  trk.init();
  trk.set_image_i(I1);
  trk.track();

  //Test construction time
  sdet_augmented_face* af = trk.af(0);
  int npix = af->face()->Npix();
  int n = 10000;
  vul_timer t;
//   for(int i = 0; i<n; i++)
//     {
//       sdet_augmented_face* naf = new sdet_augmented_face(af);
//       delete naf;
//     }
//   vcl_cout <<"constructed/destructed "<< n << " augmented faces, (area = " 
// 	       << npix << "), in " 
//            << t.real() << " msecs.\n";

//   vtol_intensity_face_sptr intf = af->face();
//   for(int i = 0; i<n; i++)
//     for(intf->reset(); intf->next();)
//       {
//         //changing from double to float didn't change the access time.
//         float x = intf->X();
//         float y = intf->Y();
//         unsigned short v = intf->I();
//       }


  double a = 3.141591, b = 2.731324;
  for(int i = 0; i<(n*n); i++)
    double y = a*b;
  vcl_cout << "double multiply " << n*n << " times, (area = " 
           << npix << "), in " 
           << t.real() << " msecs.\n";

  vcl_cout << "finished testing sdet_detector\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
