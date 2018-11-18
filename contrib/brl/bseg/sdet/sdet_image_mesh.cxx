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
#include <vsol/vsol_line_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

#include <brip/brip_vil_float_ops.h>
#include <brip/brip_line_generator.h>

#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_fit_lines.h>
#include <bbas/bil/algo/bil_cedt.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>
#include <imesh/algo/imesh_generate_mesh.h>

//note: this method is somewhat of a hack and should be replaced by
// a computed step function transition width, e.g. by a 2nd derivative
// operator
bool sdet_image_mesh:: step_boundary(vgl_line_segment_2d<double> const& parent,
                                     vgl_line_segment_2d<double>& child0,
                                     vgl_line_segment_2d<double>& child1)
{
  if (! resc_ ) return false;
  unsigned ni = resc_->ni(), nj = resc_->nj();
  vgl_vector_2d<double> n = parent.normal();
  vgl_point_2d<double> p1 = parent.point1();
  vgl_point_2d<double> p2 = parent.point2();
  vgl_point_2d<double> p10 = p1 - step_half_width_*n;
  vgl_point_2d<double> p11 = p1 + step_half_width_*n;
  vgl_point_2d<double> p20 = p2 - step_half_width_*n;
  vgl_point_2d<double> p21 = p2 + step_half_width_*n;
  if (p10.x()<0 || p10.y()<0||p20.x()<0||p20.y()<0||
      p10.x()>=ni ||p10.y()>=nj||p20.x()>=ni||p20.y()>=nj)
    return false;
  else
    child0 = vgl_line_segment_2d<double>(p10, p20);
  if (p11.x()<0 || p11.y()<0||p21.x()<0||p21.y()<0||
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

// constructor from a parameter block (the only way)
sdet_image_mesh::sdet_image_mesh(sdet_image_mesh_params& imp)
  : sdet_image_mesh_params(imp), mesh_valid_(false), resc_(nullptr)
{
}

// Destructor
sdet_image_mesh::~sdet_image_mesh()
= default;

bool sdet_image_mesh::compute_line_segments(vil_image_resource_sptr const& resc,
                                            std::vector<vgl_line_segment_2d<double> > & segs)
{
  if (!resc) return false;
  mesh_valid_ = false;
  sdet_detector_params dp;
  dp.smooth= smooth_;
  dp.noise_multiplier = thresh_;
  dp.aggressive_junction_closure=0;
  dp.junctionp=false;
  sdet_detector det(dp);
  det.SetImage(resc);
  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  if (!edges) {
    std::cout<<"sdet_image_mesh:: could not detect edges in buffer"<<std::endl;
    return false;
  }

  //fit lines on edges
  sdet_fit_lines_params flp;
  flp.min_fit_length_ = min_fit_length_;
  flp.rms_distance_ = rms_distance_;
  sdet_fit_lines fl(flp);
  fl.set_edges(*edges);
  bool fit_worked = fl.fit_lines();
  if (!fit_worked) {
    std::cout<<"sdet_image_mesh:: could not fit lines on edges"<<std::endl;
    return false;
  }

  fl.get_line_segs(segs);
  return true;
}

bool sdet_image_mesh::compute_mesh()
{
  vil_image_view<unsigned char> line_img(resc_->ni(),resc_->nj());//some resource
  line_img.fill(255);

  std::vector<vgl_line_segment_2d<double> > segs, segs_pair;
  sdet_detector_params dp;
  dp.smooth= smooth_;
  dp.noise_multiplier = thresh_;
  dp.aggressive_junction_closure=0;
  dp.junctionp=false;
  sdet_detector det(dp);
  det.SetImage(resc_);
  det.DoContour();
  std::vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
  for (auto & edge : *edges)
  {
    vsol_curve_2d_sptr c = edge->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    int nedgl = ec->size();
    for (int i=0; i<nedgl; i++)
    {
      // anchor_points_.push_back(vgl_point_2d<double>((*ec)[i].x(),(*ec)[i].y()));
      line_img((int)(*ec)[i].x(),(int) (*ec)[i].y())=0;
    }
  }

 bil_cedt dt(line_img);
 if (!dt.compute_cedt())
  std::cout<<"Error in computing DT"<<std::endl;

 vil_image_view<float> dtimg=dt.cedtimg();
 vil_image_view<unsigned char> dtimg_threshed(dtimg.ni(),dtimg.nj());dtimg_threshed.fill(0);
 for (unsigned i=0;i<dtimg.ni();i++)
   for (unsigned j=0;j<dtimg.nj();j++)
     if (dtimg(i,j)<2.0)
       dtimg_threshed(i,j)=255;
 vil_save(dtimg_threshed,"F:/visdt/dt_thresh.png");

 //segs_pair.clear();
 std::vector<vgl_line_segment_2d<double> > lines;
 this->compute_line_segments(vil_new_image_resource_of_view(dtimg_threshed),lines);
 segs_pair.insert(segs_pair.end(),lines.begin(),lines.end());
 line_img.fill(255);

 for (auto & i : segs_pair)
 {
   bool init = true;
   float xs= float(i.point1().x());
   float ys= float(i.point1().y());
   float xe= float(i.point2().x());
   float ye= float(i.point2().y());
   float x=0.0f;
   float y=0.0f;
   while (brip_line_generator::generate(init, xs, ys, xe, ye, x, y))
   {
     int xi = (int)x;
     int yi = (int)y; //convert the pixel location to integer
     line_img(xi, yi)=0;
   }
 }
 vil_save(line_img,"F:/visdt/line_img_more.png");

 bil_cedt dt1(line_img);
 if (!dt1.compute_cedt())
   std::cout<<"Error in computing DT"<<std::endl;

  //generate a 2d mesh based on the edges
  vgl_point_2d<double> ul(0.0, 0.0), ur(resc_->ni()-1,0.0);
  vgl_point_2d<double> lr(resc_->ni()-1, resc_->nj()-1), ll(0.0, resc_->nj()-1);

  std::vector<vgl_point_2d<double> > convex_hull;
  convex_hull.push_back(ul);   convex_hull.push_back(ur);
  convex_hull.push_back(lr);   convex_hull.push_back(ll);
  std::vector<vgl_point_2d<double> > cvexh = convex_hull;
  imesh_mesh mesh_one;
  imesh_generate_mesh_2d_2(convex_hull, segs_pair,anchor_points_, mesh_one);

  //lift vertices to 3-d
  const imesh_vertex_array<2>& verts = mesh_one.vertices<2>();
  auto* verts3 = new imesh_vertex_array<3>();

  // convert image to float
  vil_image_view<float> view = brip_vil_float_ops::convert_to_float(resc_);
  unsigned ni = view.ni(), nj = view.nj();
  float minv=0, maxv=0;
  vil_math_value_range(view, minv, maxv);
  unsigned nverts = mesh_one.num_verts();
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    auto i = static_cast<unsigned>(verts[iv][0]);
    auto j = static_cast<unsigned>(verts[iv][1]);
    double height =maxv;
    if (i<ni && j<nj)
      height = static_cast<double>(view(i,j));
    height = maxv-height;
    imesh_vertex<3> v3(verts[iv][0], verts[iv][1], height);
    verts3->push_back(v3);
  }
  std::unique_ptr<imesh_vertex_array_base> v3(verts3);
  mesh_one.set_vertices(std::move(v3));
  //mesh_valid_ = true;

  ///////////////////////////////////////////////////////
  //compute anchor points, and rerun generate_mesh_2d_2
  ///////////////////////////////////////////////////////
  this->set_anchor_points(mesh_one,dt1.cedtimg());

  std::cout<<"Number of anchor points: "<<anchor_points_.size()<<std::endl;
  imesh_generate_mesh_2d_2(cvexh, segs_pair, anchor_points_, mesh_);
  const imesh_vertex_array<2>& verts2 = mesh_.vertices<2>();
  auto* newVerts = new imesh_vertex_array<3>();

  // convert image to float
  ni = view.ni(), nj = view.nj();
  vil_math_value_range(view, minv, maxv);
  nverts = mesh_.num_verts();
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    auto i = static_cast<unsigned>(verts2[iv][0]);
    auto j = static_cast<unsigned>(verts2[iv][1]);
    double height =maxv;
    if (i<ni && j<nj)
      height = static_cast<double>(view(i,j));
    height = maxv-height;
    imesh_vertex<3> v3(verts2[iv][0], verts2[iv][1], height);
    newVerts->push_back(v3);
  }
  std::unique_ptr<imesh_vertex_array_base> v3_ptr(newVerts);
  mesh_.set_vertices(std::move(v3_ptr));
  mesh_valid_ = true;

  return true;
}

//takes the 3d mesh, calculates depth disparity map (from top)
void sdet_image_mesh::set_anchor_points(imesh_mesh& mesh, vil_image_view<float> dt_img)
{
  //create a tri_depth image
  int ni = resc_->ni();
  int nj = resc_->nj();
  vil_image_view<float> tri_depth(ni, nj);

  //find the range of triangles in the Z direction
  auto& faces = (imesh_regular_face_array<3>&) mesh.faces();
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  unsigned nfaces = mesh.num_faces();
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    unsigned v1 = faces[iface][0];
    unsigned v2 = faces[iface][1];
    unsigned v3 = faces[iface][2];
    double verts_x[] = { verts[v1][0], verts[v2][0], verts[v3][0] };
    double verts_y[] = { verts[v1][1], verts[v2][1], verts[v3][1] };
    double verts_z[] = { verts[v1][2], verts[v2][2], verts[v3][2] };
    bvgl_triangle_interpolation_iterator<double> tsi(verts_x, verts_y, verts_z);

    //scan the triangle, storing the depth
    for (tsi.reset(); tsi.next(); ) {
      int y = tsi.scany();
      if (y<0 || y>=nj) continue;
      int min_x = tsi.startx();
      int max_x = tsi.endx();
      if (min_x >= ni || max_x < 0)
        continue;
      if (min_x < 0) min_x = 0;
      if (max_x >= ni) max_x = ni-1;
      for (int x = min_x; x <= max_x; ++x) {
        tri_depth(x,y) = (float) tsi.value_at(x);
      }
    }
  }

  //get range of triangle depths
  float minz, maxz;
  vil_math_value_range(tri_depth, minz, maxz);

  //maximum z diff allowed for a triangle is 1/512 of the total z range
  vil_image_view<float> z_img = brip_vil_float_ops::convert_to_float(resc_);

  //for ( unsigned niter=1;niter<16;niter*=2)
  {
    double max_z_diff = (maxz-minz)/64.0;
    for (int i=0; i<ni; i+=4)
      for (int j=0; j<nj; j+=4)
        if ( std::fabs( tri_depth(i,j)- z_img(i,j) ) > max_z_diff && dt_img(i,j) >= 3.5 )
          anchor_points_.emplace_back(i,j);
  }
}

//ensure the image is a byte image (between 0 and 255)
void sdet_image_mesh::set_image(vil_image_resource_sptr const& resource)
{
  resc_ = resource;
  if (resc_->pixel_format() != VIL_PIXEL_FORMAT_BYTE )
  {
    std::cout<<"Converting image from "<<resc_->pixel_format()<<" to vxl_byte image"<<std::endl;

    //make the float image on the range of [0,1];
    vil_image_view_base_sptr stretched = vil_convert_stretch_range( float(0.0f), resc_->get_view());

    //makvil_image_view_base_sptr byte_img = vil_convert_cast<vxl_byte>(0, stretched*255.0f);
    vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(stretched->ni(), stretched->nj());
    auto* dest = (vil_image_view<float>*) dest_sptr.ptr();
    auto* stf  = (vil_image_view<float>*) stretched.ptr();
    vil_convert_stretch_range<float>( *stf, *dest, 0.0f, 255.0f);

    //now turn em into bytes
    vil_image_view_base_sptr converted = vil_convert_cast<vxl_byte>(0, dest_sptr);

    //create new resource sptr
    resc_ = vil_new_image_resource_of_view(*converted.ptr());
  }
}
