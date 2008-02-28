#include "mfpf_edge_finder.h"
//:
// \file
// \brief Locates strongest edge along a profile
// \author Tim Cootes

#include <vsl/vsl_binary_loader.h>
#include <vul/vul_string.h>
#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

#include <vimt/vimt_bilin_interp.h>
#include <vimt/vimt_sample_profile_bilin.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_edge_finder::mfpf_edge_finder()
  : step_size_(1.0),search_ni_(5)
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_edge_finder::~mfpf_edge_finder()
{
}

void mfpf_edge_finder::set_step_size(double s)
{
  step_size_=s;
}


void mfpf_edge_finder::set_search_area(unsigned ni, unsigned)
{
  search_ni_=ni;
}

//: Evaluate match at p, using u to define scale and orientation
// Returns -1*edge strength at p along direction u
double mfpf_edge_finder::evaluate(const vimt_image_2d_of<float>& image,
                                  const vgl_point_2d<double>& p,
                                  const vgl_vector_2d<double>& u)
{
  double v1 = vimt_bilin_interp_safe(image,p+0.5*step_size_*u);
  double v2 = vimt_bilin_interp_safe(image,p-0.5*step_size_*u);
  return -1.0*vcl_fabs(v1-v2);
}

   //: Evaluate match at in a region around p
   // Returns a qualtity of fit at a set of positions.
   // response image (whose size and transform is set inside the
   // function), indicates the points at which the function was
   // evaluated.  response(i,j) is the fit at the point
// response.world2im().inverse()(i,j).  The world2im() transformation
// may be affine.
void mfpf_edge_finder::evaluate_region(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vimt_image_2d_of<double>& response)
{
  int n=1+2*search_ni_;
  vnl_vector<double> v(n+1);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p-(search_ni_+0.5)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n+1);
  response.image().set_size(n,1);
  double* r = response.image().top_left_ptr();
  for (int i=0;i<n;++i,++r)
  {
    *r = -1*vcl_fabs(v[i+1]-v[i]);
  }

  // Set up transformation parameters

  // Point (i,j) in dest corresponds to p1+i.u+j.v,
  // an affine transformation for image to world
  const vgl_point_2d<double> p1 = p-search_ni_*u1;

  vimt_transform_2d i2w;
  i2w.set_similarity(vgl_point_2d<double>(u1.x(),u1.y()),p1);
  response.set_world2im(i2w.inverse());
#if 0 // commented out
  // Inverse:
  double d2 = u.x()*u.x()+u.y()*u.y();
  vgl_point_2d<double> ui(u.y()/d2,-u.x()/d2);
  vgl_point_2d<double> vi(-ui.y(),ui.x()); // Rotate ui by 90 degrees
  // q = -R*p1, where R is rotatoin defined by ui
  vgl_point_2d<double> q(-ui.x()*p1.x()-vi.x()*p1.y(),
                         -ui.y()*p1.x()-vi.y()*p1.y());
  vimt_transform_2d w2i;
  w2i.set_similarity(ui,q);
  response.set_world2im(w2i);
#endif // 0
}

   //: Search given image around p, using u to define scale and orientation
   //  On exit, new_p and new_u define position, scale and orientation of
   //  the best nearby match.  Returns a qualtity of fit measure at that
   //  point (the smaller the better).
double mfpf_edge_finder::search(const vimt_image_2d_of<float>& image,
                                const vgl_point_2d<double>& p,
                                const vgl_vector_2d<double>& u,
                                vgl_point_2d<double>& new_p,
                                vgl_vector_2d<double>& new_u)
{
  int n=1+2*search_ni_;
  vnl_vector<double> v(n+1);
  vgl_vector_2d<double> u1=step_size_*u;
  const vgl_point_2d<double> p0 = p-(search_ni_+0.5)*u1;
  vimt_sample_profile_bilin(v,image,p0,u1,n+1);
  int best_i=0;
  double best_e = vcl_fabs(v[1]-v[0]);
  for (int i=1;i<n;++i)
  {
    double e = vcl_fabs(v[i+1]-v[i]);
    if (e>best_e) { best_e=e; best_i=i; }
  }
  new_p = p+(best_i-search_ni_)*u1;
  new_u = u;
  return -1.0 * best_e;
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mfpf_edge_finder::set_from_stream(vcl_istream &is)
{
  // Cycle through string and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  search_ni_=5;
  // Extract the properties
  if (props.find("step_size")!=props.end())
  {
    step_size_=vul_string_atof(props["step_size"]);
    props.erase("step_size");
  }
  if (props.find("search_ni")!=props.end())
  {
    search_ni_=vul_string_atoi(props["search_ni"]);
    props.erase("search_ni");
  }

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mfpf_edge_finder::set_from_stream", props, mbl_read_props_type());
  return true;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_edge_finder::is_a() const
{
  return vcl_string("mfpf_edge_finder");
}

//: Create a copy on the heap and return base class pointer
mfpf_point_finder* mfpf_edge_finder::clone() const
{
  return new mfpf_edge_finder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mfpf_edge_finder::print_summary(vcl_ostream& os) const
{
  os<<"{ step_size: "<<step_size_
    <<" search_ni: "<<search_ni_
    <<" }";
}

void mfpf_edge_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_);
  vsl_b_write(bfs,search_ni_);
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_edge_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,search_ni_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
