// This is brl/bbas/volm/pro/processes/volm_generate_height_map_from_ply_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//         Read a set of ply files and generate a height map
//
//
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <volm/volm_category_io.h>
#include <vul/vul_file_iterator.h>
#include <rply.h>   //.ply parser

#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <vgl/vgl_polygon_scan_iterator.h>

//:
//  Take a colored segmentation output and map it to volm labels
bool volm_generate_height_map_from_ply_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");  // path to ply files
  input_types.push_back("unsigned");  // ni
  input_types.push_back("unsigned");  // nj
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // output height map
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_generate_height_map_from_ply_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    vcl_cout << "volm_map_osm_process: The number of inputs should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string path = pro.get_input<vcl_string>(0);
  unsigned ni = pro.get_input<unsigned>(1);
  unsigned nj = pro.get_input<unsigned>(2);

  vil_image_view<float> out_map(ni, nj);
  out_map.fill(0.0f);
  
  vcl_string ply_glob=path+"/*.ply";
  vul_file_iterator ply_it(ply_glob.c_str());
  while (ply_it) {
    vcl_string name(ply_it());
    vcl_cout << " name: " << name << vcl_endl;
    
    bmsh3d_mesh_mc *  bmesh = new bmsh3d_mesh_mc();
    bmsh3d_load_ply(bmesh,name.c_str());
    vcl_map <int, bmsh3d_face*>::iterator it = bmesh->facemap().begin();
    for (; it != bmesh->facemap().end(); it++) {
      int id = (*it).first;
      bmsh3d_face* tmpF = (*it).second;
      vcl_vector<bmsh3d_vertex*> vertices;
      tmpF->get_ordered_Vs(vertices);
      
      vgl_polygon<double> poly(1);
      double height = 0.0f;
      for (unsigned j = 0; j < vertices.size(); j++) {
        vcl_cout << "vertex[" << j << "]: x:" << vertices[j]->get_pt().x() << " y: " << vertices[j]->get_pt().y() << " z: " << vertices[j]->get_pt().z() << vcl_endl;
        poly.push_back(vertices[j]->get_pt().x(), nj-vertices[j]->get_pt().y());
        height += vertices[j]->get_pt().z();
      }
      height /= vertices.size();
      
      vgl_polygon_scan_iterator<double> psi(poly, false);
      for (psi.reset(); psi.next();) {
        int y = psi.scany();
        for (int x = psi.startx(); x<=psi.endx(); ++x)
        {
          int u = (int)vcl_floor(x+0.5);
          int v = (int)vcl_floor(y+0.5);
          if (u >= ni || v >= nj || u < 0 || v < 0)
            continue;
          out_map(u,v) = height;
        }
      }
    }
    ++ply_it;
  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<float>(out_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return true;
}

