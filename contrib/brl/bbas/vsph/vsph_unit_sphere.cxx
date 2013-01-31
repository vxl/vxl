
#include "vsph_unit_sphere.h"
#include "vsph_sph_point_3d.h"
#include "vsph_utils.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vcl_algorithm.h>
bool operator < (vsph_edge const& a, vsph_edge const& b){ 
  if(a.vs_!=b.vs_)
    return a.vs_ < b.vs_;
  return a.ve_ < b.ve_;
}

vgl_vector_3d<double> vsph_unit_sphere::cart_coord(vsph_sph_point_2d const& sp) const
{
  double x = vcl_sin(sp.theta_)*vcl_cos(sp.phi_);
  double y = vcl_sin(sp.theta_)*vcl_sin(sp.phi_);
  double z = vcl_cos(sp.theta_);
  return vgl_vector_3d<double>(x, y, z);
}

vsph_sph_point_2d vsph_unit_sphere::spher_coord(vgl_vector_3d<double> const& cp) const
{
  double x = cp.x(), y = cp.y(), z = cp.z();
  double phi = vcl_atan2(y,x);
  double theta = vcl_acos(z);
  return vsph_sph_point_2d(theta, phi);
}

void vsph_unit_sphere::add_uniform_views(double cap_angle, double point_angle)
{
  double pt_angle_rad = point_angle/vnl_math::deg_per_rad;
  double cap_angle_rad = cap_angle/vnl_math::deg_per_rad;

  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  vcl_vector<vgl_vector_3d<double> > verts;
  vgl_vector_3d<double> v1(0.0,0.0, 1.0); verts.push_back(v1);
  vgl_vector_3d<double> v2(0.0,0.0,-1.0); verts.push_back(v2);
  vgl_vector_3d<double> v3( 1.0,0.0,0.0); verts.push_back(v3);
  vgl_vector_3d<double> v4(-1.0,0.0,0.0); verts.push_back(v4);
  vgl_vector_3d<double> v5(0.0, 1.0,0.0); verts.push_back(v5);
  vgl_vector_3d<double> v6(0.0,-1.0,0.0); verts.push_back(v6);

  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  vcl_vector<vcl_vector<int> > triangles;

  vcl_vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);

  vcl_vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);

  vcl_vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);

  vcl_vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);

  vcl_vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);

  vcl_vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);

  vcl_vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);

  vcl_vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);

  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spherical coordinate system
  // vgl_vector_3d<double> vector1=verts[triangles[0][0]]-center;
  // vgl_vector_3d<double> vector2=verts[triangles[0][1]]-center;

  bool done=false;
  while (!done) {
    vcl_vector<vcl_vector<int> >  new_triangles;
    int ntri=triangles.size();
    for (int i=0; i<ntri; i++) {
      vcl_vector<int> points;
      for (int j=0; j<3; j++) {
        // find the mid points of edges
        int next=j+1; if (next == 3) next=0;
	vgl_vector_3d<double> v0 = verts[triangles[i][j]];
	vgl_vector_3d<double> vn = verts[triangles[i][next]];
	vgl_point_3d<double> p0(v0.x(), v0.y(), v0.z());
	vgl_point_3d<double> pn(vn.x(), vn.y(), vn.z());
        vgl_line_segment_3d<double> edge1(p0,pn);
        vgl_point_3d<double> midp=edge1.point_t(0.5);
	// convert to a unit vector on the sphere surface
	vgl_vector_3d<double> mid(midp.x(), midp.y(), midp.z());
	mid  = normalized(mid);
        // add a new vertex for mid points of the edges of the triangle
        int idx = verts.size();
        verts.push_back(mid);

        points.push_back(triangles[i][j]);  // existing vertex of the bigger triangle
        points.push_back(idx);              // new mid-point vertex
      }

      // add new samller 4 triangles instead of the old big one
      /******************************
                   /\
                  /  \
                 /    \
                /      \
               /--------\
              / \      / \
             /   \    /   \
            /     \  /     \
           /       \/       \
           -------------------
      *******************************/
      done=true;
      vcl_vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      // check for point_angles
      vcl_vector<vgl_vector_3d<double> > triangle;
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back(verts[list[0]]); triangle.push_back(verts[list[1]]); triangle.push_back(verts[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;
    }
    // check the angle again to see if the threashold is met
    //vgl_vector_3d<double> vector1=verts[new_triangles[0][0]]-center;
    //vgl_vector_3d<double> vector2=verts[new_triangles[0][1]]-center;
    triangles.clear();
    triangles=new_triangles;
  }
  // refine the vertices to points, eliminate duplicate ones and also eliminate the ones below given elevation
  // note that the relationship between vertex id and the id of the
  // cart and sphere containers is changed by this filter
  int ntri = triangles.size();
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      int vidx = triangles[i][j];
      vgl_vector_3d<double>& cv = verts[vidx];
      vsph_sph_point_2d sv = spher_coord(cv);
      //is cv already in cart_pts?
      int id = -1;
      bool equal = this->find_near_equal(cv, id);
      if(equal)
	equivalent_ids_[vidx]=id;// keep track of map between old and new ids
      // if not add it
      if(!equal&&(sv.theta_ <= cap_angle_rad)) {
	int np = cart_pts_.size();
	cart_pts_.push_back(cv);
	sph_pts_.push_back(sv);
      }
    }
  }
  // step through the triangles and construct unique edges
  // two edges are equal if their end points are equal regardless
  // of order.
  vcl_vector<vsph_edge>::iterator eit;
  for (int i=0; i<ntri; i++) {
    int v[3];// triangle vertices
    v[0] = equivalent_ids_[triangles[i][0]];//construct edges with current
    v[1] = equivalent_ids_[triangles[i][1]];//cart and sphere vertex ids
    v[2] = equivalent_ids_[triangles[i][2]];//updated from initial "verts" id
    //traverse the edges of the triangle
    for(int j = 0; j<3; ++j){
      vsph_edge e(v[j],v[(j+1)%3]);//wrap around to 0
      eit = vcl_find(edges_.begin(), edges_.end(), e);
      if(eit == edges_.end())
	edges_.push_back(e);
    }
  }
}

void vsph_unit_sphere::remove_top_and_bottom(double top_angle, double bottom_angle)
{
  equivalent_ids_.clear();
  double top_angle_rad = top_angle/vnl_math::deg_per_rad;
  double bottom_angle_rad = bottom_angle/vnl_math::deg_per_rad;
  vcl_vector<vgl_vector_3d<double> > cart_pts_new;
  vcl_vector<vsph_sph_point_2d> sph_pts_new;
  vcl_vector<vsph_sph_point_2d>::iterator pit = sph_pts_.begin();
  int indx = 0;
  for (; pit!=sph_pts_.end();++pit, ++indx) {
    vsph_sph_point_2d& sp = (*pit);
    if (sp.theta_ > top_angle_rad && 
	sp.theta_ < (vnl_math::pi - bottom_angle_rad)) {
      int ns = cart_pts_new.size();
      equivalent_ids_[indx] = ns;
      sph_pts_new.push_back(sp);
      cart_pts_new.push_back(cart_pts_[indx]);
    }else{equivalent_ids_[indx] = -1;}
  }
  
  sph_pts_.clear();
  sph_pts_ = sph_pts_new;
  cart_pts_.clear();
  cart_pts_ = cart_pts_new;
  vcl_vector<vsph_edge> new_edges;
  for(vcl_vector<vsph_edge>::iterator eit = edges_.begin();
      eit != edges_.end(); ++eit){
    int is = equivalent_ids_[(*eit).vs_],
      ie = equivalent_ids_[(*eit).ve_];
    if(is == -1 || ie == -1)
      continue;
    new_edges.push_back(vsph_edge(is, ie));
  }
  edges_.clear();
  edges_ = new_edges;
}

bool vsph_unit_sphere::
find_near_equal(vgl_vector_3d<double>const& p, int& id, double tol){
  vcl_vector<vgl_vector_3d<double> >::iterator it = cart_pts_.begin();
  id = 0;
  for(;it != cart_pts_.end(); it++, ++id) {
    vgl_vector_3d<double>& cp = *it;
    double dist = 1.0 - dot_product(p, cp);
    if(dist<tol)
      return true;
  }
  id = -1;
  return false;
}


void vsph_unit_sphere::print(vcl_ostream& os) const
{
  os << "vsph_unit_sphere: " << size() << vcl_endl;
  unsigned idx = 0;
  for(vcl_vector<vsph_sph_point_2d>::const_iterator sit = sph_pts_.begin();
      sit != sph_pts_.end(); ++sit, ++idx)
    os << '(' << idx << ") " << *sit << vcl_endl;
  os << vcl_endl;
}


// point_angle must be in radians
bool vsph_unit_sphere::min_angle(vcl_vector<vgl_vector_3d<double> > list,
				 double point_angle)
{
  if (list.size() < 2)
    return false;

  for (unsigned i=0; i<list.size(); i++) {
    unsigned next = i+1;
    if (next == list.size()) next = 0;
    vgl_vector_3d<double>& vector1=list[i];
    vgl_vector_3d<double>& vector2=list[next];
    if (angle(vector1, vector2) > point_angle)
      return false;
  }
  return true;
}



void vsph_unit_sphere::display_vertices(vcl_string const & path) const{
  vcl_ofstream os(path.c_str());
  if(!os.is_open())
    return;
  os << "VRML V2.0 utf8\n";
  os << "Shape { \n";
  os << "   appearance Appearance {\n";
  os << "      material Material {\n";
  os << "         emissiveColor 1.0 1.0 1.0\n";
  os <<        "}\n";
  os <<    "}\n";
  os << "  geometry PointSet {\n";
  os << "     coord Coordinate {\n";
  os << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for(vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
      cit != cart_pts_.end(); ++cit, ++cnt){
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if(cnt != np) os << ",\n";
    else os << "\n";
  }
  os <<"    ]\n";
  os <<"   }\n";
  os << " }\n";
  os <<"}\n";
}

void vsph_unit_sphere::display_edges(vcl_string const & path) const{
 
  vcl_ofstream os(path.c_str());
  if(!os.is_open())
    return;
  os << "VRML V2.0 utf8\n";
  os << "Shape { \n";
  os << "   appearance Appearance {\n";
  os << "      material Material {\n";
  os << "         emissiveColor 1.0 1.0 1.0\n";
  os <<        "}\n";
  os <<    "}\n";
  os << "  geometry IndexedLineSet {\n";
  os << "     coord Coordinate {\n";
  os << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for(vcl_vector<vgl_vector_3d<double> >::const_iterator cit = cart_pts_.begin();
      cit != cart_pts_.end(); ++cit, ++cnt){
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if(cnt != np) os << ",\n";
    else os << "\n";
  }
  os <<             "]\n";
  os <<      "}\n";
  os << "     coordIndex [\n";

  cnt = 0;
  int ne = edges_.size()-1;
  for(vcl_vector<vsph_edge>::const_iterator eit = edges_.begin();
      eit != edges_.end(); ++eit, ++cnt){
    const vsph_edge& e = *eit;
    os << e.vs_ << ',' << e.ve_;
    if(cnt != ne)
      os << ",-1,\n";
    else os << '\n';
  }
  os <<        "]\n";
  os << "    }\n";
  os << "  }\n";
  os.close();
}
void vsph_unit_sphere::b_read(vsl_b_istream& is)
{
#if 0
  short version;
  vsl_b_read(is, version);
  switch (version) {
  case 1:
    {
      if (!coord_sys_) coord_sys_ = new vsph_spherical_coord();
      coord_sys_->b_read(is);
      unsigned size, uid;
      T view;
      vsl_b_read(is, size);
      for (unsigned i=0; i<size; i++) {
	vsl_b_read(is, uid);
	view.b_read(is);
	views_[uid] = view;
      }
      vsl_b_read(is, uid_);
      break;
    }
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_unit_sphere&)\n"
	     << "           Unknown version number "<< version << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
#endif
}


void vsph_unit_sphere::b_write(vsl_b_ostream& os) const
{
#if 0
  vsl_b_write(os, version());
  coord_sys_->b_write(os);

  typename vcl_map<int, T>::const_iterator it = views_.begin();

  // write each view point
  vsl_b_write(os, size());
  while (it != views_.end()) {
    int uid=it->first;
    T vp=it->second;
    vsl_b_write(os, uid);
    vp.b_write(os);
    ++it;
  }
  vsl_b_write(os, uid_);
#endif
}

template <class T>
void vsl_b_read(vsl_b_istream& is, vsph_unit_sphere& vs)
{
  vs.b_read(is);
}

template <class T>
void vsl_b_write(vsl_b_ostream& os, vsph_unit_sphere const& vs)
{
  vs.b_write(os);
}

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vsph_unit_sphere const& vs)
{
  vs.print(os);
  return os;
}

