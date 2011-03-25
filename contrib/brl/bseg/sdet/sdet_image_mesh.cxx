// This is brl/bseg/sdet/sdet_image_mesh.cxx
#include "sdet_image_mesh.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_save.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>
#include <vgl/vgl_line_segment_2d.h>
#include <imesh/algo/imesh_generate_mesh.h>
//note: this method is somewhat of a hack and should be replaced by
// a computed step function transition width, e.g. by a 2nd derivative
// operator
bool sdet_image_mesh:: step_boundary(vgl_line_segment_2d<double> const& parent,
                                     vgl_line_segment_2d<double>& child0,
                                     vgl_line_segment_2d<double>& child1)
{
  if(! resc_ ) return false;
  unsigned ni = resc_->ni(), nj = resc_->nj();
  vgl_vector_2d<double> n = parent.normal();
  vgl_point_2d<double> p1 = parent.point1();
  vgl_point_2d<double> p2 = parent.point2();
  vgl_point_2d<double> p10 = p1 - step_half_width_*n;
  vgl_point_2d<double> p11 = p1 + step_half_width_*n;
  vgl_point_2d<double> p20 = p2 - step_half_width_*n;
  vgl_point_2d<double> p21 = p2 + step_half_width_*n;
  if(p10.x()<0 || p10.y()<0||p20.x()<0||p20.y()<0||
     p10.x()>=ni ||p10.y()>=nj||p20.x()>=ni||p20.y()>=nj)
    return false;
  else
    child0 = vgl_line_segment_2d<double>(p10, p20);
  if(p11.x()<0 || p11.y()<0||p21.x()<0||p21.y()<0||
     p11.x()>=ni ||p11.y()>=nj||p21.x()>=ni||p21.y()>=nj)
    return false;
  else
    child1 = vgl_line_segment_2d<double>(p11, p21);
  return true;
}

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
sdet_image_mesh::sdet_image_mesh(sdet_image_mesh_params& imp)
  : sdet_image_mesh_params(imp), mesh_valid_(false), resc_(0)
{
}

// Default Destructor
sdet_image_mesh::~sdet_image_mesh()
{
}

bool sdet_image_mesh::compute_mesh()
{
  //ensure the image resource is loaded
  if(!resc_) return false;
  mesh_valid_ = false;
  
  //detect edges in the image
  sdet_detector_params dp;
  dp.smooth= smooth_;
  dp.noise_multiplier = thresh_;
  dp.aggressive_junction_closure=0;
  dp.junctionp=false;
  sdet_detector det(dp);
  det.SetImage(resc_);
  det.DoContour();
  vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if(!edges) {
    vcl_cout<<"sdet_image_mesh:: could not detect edges in buffer"<<vcl_endl;
    return false;
  }
  
  //fit lines on edges
  sdet_fit_lines_params flp;
  flp.min_fit_length_ = min_fit_length_;
  flp.rms_distance_ = rms_distance_;
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  bool fit_worked = fl.fit_lines();
  if(!fit_worked) {
    vcl_cout<<"sdet_image_mesh:: could not fit lines on edges"<<vcl_endl;
    return false;
  }
  
  //get line segments, and do a little hack to make it fall in the middle
  vcl_vector<vgl_line_segment_2d<double> > segs, segs_pair;
  fl.get_line_segs(segs);
  for(unsigned i = 0; i<segs.size(); ++i)
  {
    vgl_line_segment_2d<double> child0, child1;
    if(this->step_boundary(segs[i],child0, child1))
      {
        segs_pair.push_back(child0);
        segs_pair.push_back(child1);
      } else {
        segs_pair.push_back(segs[i]);
      }
  }

  //generate a 2d mesh based on the edges
  vgl_point_2d<double> ul(0.0, 0.0), ur(resc_->ni()-1,0.0);
  vgl_point_2d<double> lr(resc_->ni()-1, resc_->nj()-1), ll(0.0, resc_->nj()-1);

  vcl_vector<vgl_point_2d<double> > convex_hull;
  convex_hull.push_back(ul);   convex_hull.push_back(ur);
  convex_hull.push_back(lr);   convex_hull.push_back(ll);
  unsigned nsegs = segs_pair.size();
  imesh_generate_mesh_2d(convex_hull, segs_pair, mesh_);

  //lift vertices to 3-d
  const imesh_vertex_array<2>& verts = mesh_.vertices<2>();
  imesh_vertex_array<3>* verts3 = new imesh_vertex_array<3>();
  
  // convert image to float
  vil_image_view<float> view = brip_vil_float_ops::convert_to_float(resc_);
  unsigned ni = view.ni(), nj = view.nj();
  float minv=0, maxv=0;
  vil_math_value_range(view, minv, maxv);
  unsigned nverts = mesh_.num_verts();
  for(unsigned iv = 0; iv<nverts; ++iv)
  {
    unsigned i = static_cast<unsigned>(verts[iv][0]);
    unsigned j = static_cast<unsigned>(verts[iv][1]);
    double height =maxv;
    if(i<ni && j<nj)
       height = static_cast<double>(view(i,j));
    height = maxv-height;
    imesh_vertex<3> v3(verts[iv][0], verts[iv][1], height);
    verts3->push_back(v3);
  }
  vcl_auto_ptr<imesh_vertex_array_base> v3(verts3);
  mesh_.set_vertices(v3);
  mesh_valid_ = true;
  return true;
}

//ensure the image is a byte image (between 0 and 255)
void sdet_image_mesh::set_image(vil_image_resource_sptr const& resource)
{
  resc_ = resource; 
  if(resc_->pixel_format() != VIL_PIXEL_FORMAT_BYTE )
  {
    vcl_cout<<"Converting image from "<<resc_->pixel_format()<<" to vxl_byte image"<<vcl_endl;
    
    //make the float image on the range of [0,1]; 
    vil_image_view_base_sptr stretched = vil_convert_stretch_range( float(0.0f), resc_->get_view()); 
    
    //makvil_image_view_base_sptr byte_img = vil_convert_cast<vxl_byte>(0, stretched*255.0f); 
    vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(stretched->ni(), stretched->nj()); 
    vil_image_view<float>* dest = (vil_image_view<float>*) dest_sptr.ptr(); 
    vil_image_view<float>* stf  = (vil_image_view<float>*) stretched.ptr(); 
    vil_convert_stretch_range<float>( *stf, *dest, 0.0f, 255.0f);
        
    //now turn em into bytes
    vil_image_view_base_sptr converted = vil_convert_cast<vxl_byte>(0, dest_sptr); 
    
    vil_save(*converted, "/media/VXL/mesh/downtown/topdown.png");
    
    //create new resource sptr
    resc_ = vil_new_image_resource_of_view(*converted.ptr());
  }

}

