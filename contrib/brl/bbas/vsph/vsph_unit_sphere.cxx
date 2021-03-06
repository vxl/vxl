#include <iostream>
#include <algorithm>
#include <limits>
#include <tuple>
#include "vsph_unit_sphere.h"
//
#include "vsph_sph_point_2d.h"
#include "vsl/vsl_vector_io.h"
#include "vsph_utils.h"
#include "vnl/vnl_math.h"
#include "vgl/vgl_line_segment_3d.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include <bvrml/bvrml_write.h>
#include <cassert>



bool operator < (vsph_edge const& a, vsph_edge const& b)
{
  if (a.vs_!=b.vs_)
    return a.vs_ < b.vs_;
  return a.ve_ < b.ve_;
}

// an edge exists if either vertex is a neighbor of the other
bool vsph_unit_sphere::find_edge(vsph_edge const&  e)
{
  std::set<int>& ss = neighbors_[e.vs_];
  std::set<int>::iterator sit;
  sit = ss.find(e.ve_);
  return sit != ss.end();
}

// insert an edge into the neighbor index and the list of edges
void vsph_unit_sphere::insert_edge(vsph_edge const&  e)
{
  neighbors_[e.vs_].insert(e.ve_);
  neighbors_[e.ve_].insert(e.vs_);
  edges_.push_back(e);
}

vsph_unit_sphere::vsph_unit_sphere(double point_angle,
                                   double min_theta, double max_theta)
: neighbors_valid_(false), point_angle_(point_angle),
  min_theta_(min_theta), max_theta_(max_theta), verbose_(false)
{
  if(verbose_)std::cout << "Start construction" << std::endl;
  add_uniform_views();
  remove_top_and_bottom();
  if(verbose_) std::cout << "Unit sphere complete with " << sph_pts_.size() << " verts and "
                        <<  edges_.size() << " edges\n"<< std::endl;
  this->set_cart_points();
}

vgl_vector_3d<double> vsph_unit_sphere::cart_coord(vsph_sph_point_2d const& sp)
{
  double th = sp.theta_, ph = sp.phi_;
  if (!sp.in_radians_) {
    th /= vnl_math::deg_per_rad;
    ph /= vnl_math::deg_per_rad;
  }
  double x = std::sin(th)*std::cos(ph);
  double y = std::sin(th)*std::sin(ph);
  double z = std::cos(sp.theta_);
  return {x, y, z};
}


vsph_sph_point_2d vsph_unit_sphere::spher_coord(vgl_vector_3d<double> const& cp,
                                                bool in_radians)
{
  double x = cp.x(), y = cp.y(), z = cp.z();
  double phi = std::atan2(y,x);
  double theta = std::acos(z);
  if (!in_radians) {
    theta *= vnl_math::deg_per_rad;
    phi *= vnl_math::deg_per_rad;
  }
  return vsph_sph_point_2d(theta, phi, in_radians);
}

void vsph_unit_sphere::add_uniform_views()
{
  double pt_angle_rad = point_angle_/vnl_math::deg_per_rad;
  double cap_angle_rad = vnl_math::pi; //historical reasons (fix me)

  // create a octahedron on the sphere, define 6 points for the vertices of the triangles
  auto* verts = new std::vector<vgl_vector_3d<double> >();
  vgl_vector_3d<double> v1(0.0,0.0, 1.0); (*verts).push_back(v1);
  vgl_vector_3d<double> v2(0.0,0.0,-1.0); (*verts).push_back(v2);
  vgl_vector_3d<double> v3( 1.0,0.0,0.0); (*verts).push_back(v3);
  vgl_vector_3d<double> v4(-1.0,0.0,0.0); (*verts).push_back(v4);
  vgl_vector_3d<double> v5(0.0, 1.0,0.0); (*verts).push_back(v5);
  vgl_vector_3d<double> v6(0.0,-1.0,0.0); (*verts).push_back(v6);

  // vector of triangles (vector of 3 points, only indices of the vertices kept)
  std::vector<std::vector<int> > triangles;

  std::vector<int> tri1;
  tri1.push_back(0); tri1.push_back(2); tri1.push_back(4); triangles.push_back(tri1);

  std::vector<int> tri2;
  tri2.push_back(0); tri2.push_back(4); tri2.push_back(3); triangles.push_back(tri2);

  std::vector<int> tri3;
  tri3.push_back(0); tri3.push_back(3); tri3.push_back(5); triangles.push_back(tri3);

  std::vector<int> tri4;
  tri4.push_back(0); tri4.push_back(5); tri4.push_back(2); triangles.push_back(tri4);

  std::vector<int> tri5;
  tri5.push_back(1); tri5.push_back(2); tri5.push_back(4); triangles.push_back(tri5);

  std::vector<int> tri6;
  tri6.push_back(1); tri6.push_back(3); tri6.push_back(4); triangles.push_back(tri6);

  std::vector<int> tri7;
  tri7.push_back(1); tri7.push_back(5); tri7.push_back(3); triangles.push_back(tri7);

  std::vector<int> tri8;
  tri8.push_back(1); tri8.push_back(2); tri8.push_back(5); triangles.push_back(tri8);
  if(verbose_) std::cout << "formed octahedron--" << std::endl;
  // iteratively refine the triangles
  // check the angle between two vertices (of the same triangle),
  // use the center of the spherical coordinate system
  // vgl_vector_3d<double> vector1=(*verts)[triangles[0][0]]-center;
  // vgl_vector_3d<double> vector2=(*verts)[triangles[0][1]]-center;

  bool done=false;
  while (!done) {
    std::vector<std::vector<int> >  new_triangles;
    int ntri=triangles.size();
    if(verbose_) std::cout << "subdivide " << ntri << " triangles" << std::endl;
    for (int i=0; i<ntri; i++) {
      std::vector<int> points;
      for (int j=0; j<3; j++) {
        // find the mid points of edges
        int next=j+1; if (next == 3) next=0;
        //bounds check
        int i0 = triangles[i][j], inx = triangles[i][next];
        int nv = (*verts).size();
        if (i0<0||i0>=nv||inx<0||inx>=nv) {
          std::cout << "address error (" << i0 << ' ' << inx << ")["
                   << nv << "]\n";
            assert(false);
        }
        vgl_vector_3d<double> v0 = (*verts)[i0];
        vgl_vector_3d<double> vn = (*verts)[inx];
        vgl_point_3d<double> p0(v0.x(), v0.y(), v0.z());
        vgl_point_3d<double> pn(vn.x(), vn.y(), vn.z());
        vgl_line_segment_3d<double> edge1(p0,pn);
        vgl_point_3d<double> midp=edge1.point_t(0.5);
        // convert to a unit vector on the sphere surface
        vgl_vector_3d<double> mid(midp.x(), midp.y(), midp.z());
        mid  = normalized(mid);
        // add a new vertex for mid points of the edges of the triangle
        int idx = (*verts).size();
        (*verts).push_back(mid);

        points.push_back(triangles[i][j]);  // existing vertex of the bigger triangle
        points.push_back(idx);              // new mid-point vertex
      }

      // add new samller 4 triangles instead of the old big one
      /******************************
                   /\                .
                  /  \               .
                 /    \              .
                /      \             .
               /--------\            .
              / \      / \           .
             /   \    /   \          .
            /     \  /     \         .
           /       \/       \        .
           -------------------
      *******************************/
      done=true;
      std::vector<int> list(3); list[0]=points[0]; list[1]=points[5]; list[2]=points[1];
      new_triangles.push_back(list);
      // check for point_angles
      std::vector<vgl_vector_3d<double> > triangle;
      triangle.push_back((*verts)[list[0]]); triangle.push_back((*verts)[list[1]]); triangle.push_back((*verts)[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[3]; list[2]=points[2];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back((*verts)[list[0]]); triangle.push_back((*verts)[list[1]]); triangle.push_back((*verts)[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[3]; list[1]=points[5]; list[2]=points[4];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back((*verts)[list[0]]); triangle.push_back((*verts)[list[1]]); triangle.push_back((*verts)[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;

      list[0]=points[1]; list[1]=points[5]; list[2]=points[3];
      new_triangles.push_back(list);
      triangle.clear();
      triangle.push_back((*verts)[list[0]]); triangle.push_back((*verts)[list[1]]); triangle.push_back((*verts)[list[2]]);
      if (!min_angle(triangle, pt_angle_rad)) done=false;
    }
    // check the angle again to see if the threashold is met
    //vgl_vector_3d<double> vector1=(*verts)[new_triangles[0][0]]-center;
    //vgl_vector_3d<double> vector2=(*verts)[new_triangles[0][1]]-center;
    triangles.clear();
    triangles=new_triangles;
    if(verbose_)
      std::cout << "found " << triangles.size() << " trianges\n"
                << "found " << (*verts).size() << " vertices" << std::endl;
  }
  // refine the vertices to points, eliminate duplicate ones and also eliminate the ones below given elevation
  // note that the relationship between vertex id and the id of the
  // cart and sphere containers is changed by this filter
  int nv = (*verts).size();
  if(verbose_) std::cout << "Start finding equivalent vertices n = " << nv << '\n' << std::flush;
  equivalent_ids_.resize(nv, -1);
  int ntri = triangles.size();
  sph_pts_.reserve(nv);
  for (int i=0; i<ntri; i++) {
    for (int j=0; j<3; j++) {
      int vidx = triangles[i][j];
      vgl_vector_3d<double>& cv = (*verts)[vidx];
      vsph_sph_point_2d sv = spher_coord(cv);
      int id = -1;
      //      bool equal = this->find_near_equal(cv, id);
      unsigned th_idx=0, ph_idx=0;//new
      bool equal = index_.find(sv, th_idx, ph_idx, id);//new
      if (equal) {
        if (id<0||id>=nv) {
          std::cout << "address error (" << id << ")["
                   << nv << "]\n";
            assert(false);
        }
        equivalent_ids_[vidx]=id;// keep track of map between old and new ids
      }
      // if not add
      if (!equal&&(sv.theta_ <= cap_angle_rad)) {
        index_.insert(sv, sph_pts_.size());//new
        //        cart_pts_.push_back(cv);
        sph_pts_.push_back(sv);
      }
    }

    if (verbose_&&i%1000 ==0)
      std::cout << '.' << std::flush;

    int idx0 = triangles[i][0];
    int idx1 = triangles[i][1];
    int idx2 = triangles[i][2];
    vgl_vector_3d<double>& cv0 = (*verts)[idx0]; vgl_point_3d<double> cp0(cv0.x(), cv0.y(), cv0.z());
    vgl_vector_3d<double>& cv1 = (*verts)[idx1]; vgl_point_3d<double> cp1(cv1.x(), cv1.y(), cv1.z());
    vgl_vector_3d<double>& cv2 = (*verts)[idx2]; vgl_point_3d<double> cp2(cv2.x(), cv2.y(), cv2.z());
    vsph_sph_point_3d sv0, sv1, sv2;
    coord_sys_.spherical_coord(cp0, sv0);
    coord_sys_.spherical_coord(cp1, sv1);
    coord_sys_.spherical_coord(cp2, sv2);
    //adjust order of vertices so that face normal points outward
    vgl_vector_3d<double> crp = cross_product(cv1-cv0,cv2-cv0);
    crp /= crp.length();
    vgl_vector_3d<double> rdir = coord_sys_.radial_vector(sv0);
    double dp = dot_product(crp, rdir);
    if(dp>=0.0)
      triangles_.emplace_back(sv0, sv1, sv2, coord_sys_);
    else
      triangles_.emplace_back(sv2, sv1, sv0, coord_sys_);
  }
  std::cout << '\n' << std::flush;
  if(verbose_)
    std::cout << "finished refine\n"
              << "start constructing edges from " << ntri << " triangles"
              << std::endl;
  delete verts;
  neighbors_.clear();
  neighbors_.resize(this->size());
  int neq = equivalent_ids_.size();
  // step through the triangles and construct unique edges
  // two edges are equal if their end points are equal regardless
  // of order.
  std::vector<vsph_edge>::iterator eit;
  for (int i=0; i<ntri; i++) {
    int v[3];// triangle vertices
    int ti0 = triangles[i][0], ti1 = triangles[i][1], ti2 = triangles[i][2];
    if (!(ti0>=0 && ti0<neq &&ti1>=0 && ti1<neq &&ti2>=0 && ti2<neq)) {
      std::cout << "Bad Tri[" << ti0 << ',' << ti1 << ',' << ti2 << "]\n";
      continue;
    }
    v[0] = equivalent_ids_[ti0];//construct edges with current
    v[1] = equivalent_ids_[ti1];//cart and sphere vertex ids
    v[2] = equivalent_ids_[ti2];//updated from initial "verts" id
    if (v[0]<0||v[1]<0||v[2]<0) {
      std::cout << "Bad Equivalent Tri[" << v[0]<< ','
               << v[1] << ',' << v[2] << "]\n";
      continue;
    }
    //traverse the edges of the triangle
    for (int j = 0; j<3; ++j) {
      vsph_edge e(v[j],v[(j+1)%3]);//wrap around to 0
#if 0 // original n^2 find
      eit = std::find(edges_.begin(), edges_.end(), e);
      if (eit == edges_.end())
        edges_.push_back(e);
#endif
      if (!find_edge(e))
        this->insert_edge(e);
    }
    if (i%1000 ==0)
      std::cout << '+' << std::flush;
  }
  if(verbose_)
    std::cout << "\nfinished find edges "<< edges_.size()
              << " edges found\n" << std::flush;
  neighbors_valid_ = true;
}

void vsph_unit_sphere::remove_top_and_bottom()
{
  int nsph = sph_pts_.size();
  if(verbose_)
    std::cout << "entering top and bottom with " << nsph << " rays\n" << std::flush;

  equivalent_ids_.clear();
  equivalent_ids_.resize(nsph, -1);
  neighbors_valid_ = false;
  double min_theta_rad = min_theta_/vnl_math::deg_per_rad;
  double max_theta_rad = max_theta_/vnl_math::deg_per_rad;
  std::vector<vgl_vector_3d<double> > cart_pts_new;
  std::vector<vsph_sph_point_2d> sph_pts_new;
  auto pit = sph_pts_.begin();
  int indx = 0;
  for (; pit!=sph_pts_.end();++pit, ++indx) {
    vsph_sph_point_2d& sp = (*pit);
    if (sp.theta_ > (min_theta_rad-MARGIN_RAD) &&
        sp.theta_ < (max_theta_rad+MARGIN_RAD)) {
      int ns = sph_pts_new.size();
      equivalent_ids_[indx] = ns;
      sph_pts_new.push_back(sp);
    }
    else { equivalent_ids_[indx] = -1; }
  }

  sph_pts_.clear();
  sph_pts_ = sph_pts_new;
  if(verbose_)
    std::cout << "starting to remap " << edges_.size() <<  "edges\n" << std::flush;

  std::vector<vsph_edge> new_edges;
  for (auto & edge : edges_) {
    int is = equivalent_ids_[edge.vs_],
        ie = equivalent_ids_[edge.ve_];
    if (is == -1 || ie == -1)
      continue;
    new_edges.emplace_back(is, ie);
  }
  if(verbose_)
    std::cout << "finished remap edges in remove top and bottom\n" << std::flush;

  edges_.clear();
  edges_ = new_edges;
  neighbors_valid_ = false;
}

void vsph_unit_sphere::find_neighbors()
{
  int nv = this->size();
  neighbors_.clear();
  neighbors_.resize(nv);
  int ne = edges_.size();
  for (int ie =0; ie<ne; ++ie) {
    vsph_edge& e = edges_[ie];
    neighbors_[e.vs_].insert(e.ve_);
    neighbors_[e.ve_].insert(e.vs_);
    }
  neighbors_valid_ = true;
}

bool vsph_unit_sphere::
find_near_equal(vgl_vector_3d<double>const& p, int& id, double tol)
{
  auto it = cart_pts_.begin();
  id = 0;
  for (;it != cart_pts_.end(); it++, ++id) {
    vgl_vector_3d<double>& cp = *it;
    //    double dist = 1.0 - dot_product(p, cp);
    vgl_vector_3d<double> dif = cp-p;
    double dist = length(dif);
    if (dist<=tol)
      return true;
  }
  id = -1;
  return false;
}

// construct Cartesian vectors from spherical points
void vsph_unit_sphere::set_cart_points()
{
  for (auto & sph_pt : sph_pts_)
    cart_pts_.push_back(cart_coord(sph_pt));
}


void vsph_unit_sphere::print(std::ostream& os) const
{
  os << "vsph_unit_sphere: " << size() << std::endl;
  unsigned idx = 0;
  for (auto sit = sph_pts_.begin();
       sit != sph_pts_.end(); ++sit, ++idx)
    os << '(' << idx << ") " << *sit << std::endl;
  os << std::endl;
}


// point_angle must be in radians
bool vsph_unit_sphere::min_angle(std::vector<vgl_vector_3d<double> > list,
                                 double angle_rad)
{
  if (list.size() < 2)
    return false;

  for (unsigned i=0; i<list.size(); i++) {
    unsigned next = i+1;
    if (next == list.size()) next = 0;
    vgl_vector_3d<double>& vector1=list[i];
    vgl_vector_3d<double>& vector2=list[next];
    if (angle(vector1, vector2) > angle_rad)
      return false;
  }
  return true;
}

vgl_plane_3d<double> vsph_unit_sphere::
tangent_plane(vsph_sph_point_2d const& sp)
{
  vgl_vector_3d<double> cv = cart_coord(sp);
  vgl_point_3d<double> p(cv.x(), cv.y(), cv.z());
  return vgl_plane_3d<double>(cv, p);
}

bool vsph_unit_sphere::display_vertices(std::string const & path) const
{
  std::ofstream os(path.c_str());
  if (!os.is_open())
    return false;
  os << "VRML V2.0 utf8\n"
     << "Shape {\n"
     << "   appearance Appearance {\n"
     << "      material Material {\n"
     << "         emissiveColor 1.0 1.0 1.0\n"
     <<        "}\n"
     <<    "}\n"
     << "  geometry PointSet {\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (auto cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<"    ]\n"
     <<"   }\n"
     << " }\n"
     <<"}\n";
  return true;
}

bool vsph_unit_sphere::display_edges(std::string const & path) const
{
  std::ofstream os(path.c_str());
  if (!os.is_open())
    return false;
  os << "#VRML V2.0 utf8\n"
     << "Shape {\n"
     << "   appearance Appearance {\n"
     << "      material Material {\n"
     << "         emissiveColor 1.0 1.0 1.0\n"
     <<        "}\n"
     <<    "}\n"
     << "  geometry IndexedLineSet {\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (auto cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    double x = fabs(cp.x())<0.001 ? 0.0 : cp.x();
    double y = fabs(cp.y())<0.001 ? 0.0 : cp.y();
    double z = fabs(cp.z())<0.001 ? 0.0 : cp.z();
    os << x << ' ' << y << ' ' << z;
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<             "]\n"
     <<      "}\n"
     << "     coordIndex [\n";

  cnt = 0;
  int ne = edges_.size()-1;
  for (auto eit = edges_.begin();
       eit != edges_.end(); ++eit, ++cnt) {
    const vsph_edge& e = *eit;
    os << e.vs_ << ',' << e.ve_;
    if (cnt != ne)
      os << ",-1,\n";
    else os << '\n';
  }
  os <<        "]\n"
     << "    }\n"
     << "  }\n";
  os.close();
  return true;
}
void vsph_unit_sphere::display_unit_sphere(std::ofstream& os, bool open_close){
  if(open_close)
    os << "#VRML V2.0 utf8\n";
        std::vector<vgl_point_3d<double> > spts;
      std::vector<std::tuple<size_t, size_t, size_t > > mapped_in_stris;
      size_t pt_indx = 0;
      size_t nt = triangles_.size();
      double s = 0.99;
      for(size_t t = 0; t<nt; ++t){
        const vsph_spherical_triangle& tri = triangles_[t];
        spts.emplace_back(s*tri.v0_cart().x(), s*tri.v0_cart().y(), s*tri.v0_cart().z());
        spts.emplace_back(s*tri.v1_cart().x(), s*tri.v1_cart().y(), s*tri.v1_cart().z());
        spts.emplace_back(s*tri.v2_cart().x(), s*tri.v2_cart().y(), s*tri.v2_cart().z());
        mapped_in_stris.emplace_back(pt_indx, pt_indx+1, pt_indx+2);
        pt_indx +=3;
      }
    os << "Shape {\n"
     << "  geometry IndexedFaceSet {\n"
     << "   colorPerVertex FALSE\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
     int np = spts.size()-1;
     for (size_t j = 0; j <= np; ++j){
       const vgl_point_3d<double>& cp = spts[j];
       double x = fabs(cp.x())<0.001 ? 0.0 : cp.x();
       double y = fabs(cp.y())<0.001 ? 0.0 : cp.y();
       double z = fabs(cp.z())<0.001 ? 0.0 : cp.z();
       os << x << ' ' << y << ' ' << z;
       if (j != np) os << ',';
        os << '\n';
     }
      os <<             "]\n"
         <<      "}\n"
         << "    color Color {\n"
         << "      color [\n";
        os << 0.5 << ' ' <<  0.5 << ' ' << 0.5 << "\n";
        os << "    ]\n"
         << "}\n"
         << "     coordIndex [\n";

     int ne = mapped_in_stris.size()-1;
     for (size_t j = 0; j <= ne; ++j) {
       std::tuple<size_t, size_t, size_t>& tri = mapped_in_stris[j];
       if (j != ne)
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ',' << std::get<2>(tri) << ",-1,\n";
       else
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ','  << std::get<2>(tri) << ",-1\n";
     }
    os <<        "]\n";
    os << "colorIndex[\n";
    for (size_t j = 0; j<=np; ++j)
      if(j != np)
        os << 0 << ", ";
      else
        os << 0 <<"\n";
    os <<        "]\n"
     << "    }\n"
     << "  }\n";
    if(open_close)
      os.close();
}



void vsph_unit_sphere::display_data(std::string const & path,
                                    std::vector<double> const& data,
                                    vsph_sph_box_2d const& mask ) const
{
  std::ofstream os(path.c_str());
  if (!os.is_open())
    return;
  // find range of data
  double minv = std::numeric_limits<double>::max();
  double maxv = -std::numeric_limits<double>::max();
  int nd = data.size();
  for (int i=0; i<nd; ++i) {
    double d = data[i];
    if (d < minv) minv = d;
    if (d > maxv) maxv = d;
  }
  double dif = maxv-minv;
  if (dif == 0.0)
    dif = 1.0;
  bvrml_write::write_vrml_header(os);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(os, sp, 1.0f, 0.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*20, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*20, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*20, 0.0f, 1.0f, 1.0f);
  vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y(), (float)cent.z()+20, (float)rad);
  bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

  // write the voxel structure
  auto disc_radius = static_cast<float>(point_angle_/vnl_math::deg_per_rad );
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_pts_.size(); i++) {
    vsph_sph_point_2d sp = sph_pts_[i];
    if (!mask.is_empty()&&mask.defined()&&!mask.contains(sp)) continue;
    vgl_vector_3d<double> ray = cart_pts_[i];
    auto val = static_cast<float>((data[i]-minv)/dif);
    bvrml_write::write_vrml_disk(os, orig+10*ray, ray, disc_radius,
                                 val, val, 0.0f);
  }
  os.close();

  int cnt = 0;
  int np = cart_pts_.size()-1;
  for (auto cit = cart_pts_.begin();
       cit != cart_pts_.end(); ++cit, ++cnt) {
    const vgl_vector_3d<double>& cp = *cit;
    os << cp.x() << ' ' << cp.y() << ' ' << cp.z();
    if (cnt != np) os << ',';
    os << '\n';
  }
  os <<"    ]\n"
     <<"   }\n"
     << " }\n"
     <<"}\n";
}


void vsph_unit_sphere::display_color(std::string const & path,
                                     std::vector<std::vector<float> > const& cdata,
                                     std::vector<float> const& skip_color,
                                     vsph_sph_box_2d const& mask) const
{
  std::ofstream os(path.c_str());
  if (!os.is_open())
    return;
  bvrml_write::write_vrml_header(os);
  // write a world center and world axis
  double rad = 1.0;
  vgl_point_3d<float> cent(0.0,0.0,0.0);
  vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
  vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
  vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
  vgl_sphere_3d<float> sp((float)cent.x(), (float)cent.y(), (float)cent.z(), (float)rad);
  bvrml_write::write_vrml_sphere(os, sp, 0.5f, 0.5f, 0.5f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*2, 1.0f, 0.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*2, 0.0f, 1.0f, 0.0f);
  bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*2, 0.0f, 1.0f, 1.0f);
  vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y()+2, (float)cent.z(), (float)rad/10);
  bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

  // write the voxel structure
  auto disc_radius = static_cast<float>(point_angle_/vnl_math::deg_per_rad/6);
  vgl_point_3d<double> orig(0.0,0.0,0.0);
  for (unsigned i = 0; i < cart_pts_.size(); i++) {
    vgl_vector_3d<double> ray = cart_pts_[i];
    vsph_sph_point_2d sp = sph_pts_[i];
    if (!mask.is_empty()&&mask.defined()&&!mask.contains(sp)) continue;
    const std::vector<float>& cl = cdata[i];
    if (cl[0]==skip_color[0]&&cl[1]==skip_color[1]&&cl[2]==skip_color[2])
      continue;
    bvrml_write::write_vrml_disk(os, orig+ray, ray, disc_radius,
                                 cl[0], cl[1], cl[2]);
  }
  os.close();
}
bool area_less(std::pair<size_t, double> const& pa, std::pair<size_t, double> const& pb) {
  return pa.second < pb.second;
}

bool vsph_unit_sphere::display_boxes(std::string const& path,
                                     std::vector<vsph_sph_box_2d> const& boxes,
                                     bool disp_unit_sphere)
{
  std::vector<std::pair<size_t, double> > box_areas;
  size_t nb = boxes.size();
  for (size_t i = 0; i < nb; ++i)
    box_areas.emplace_back(i, boxes[i].area());
  std::sort(box_areas.begin(), box_areas.end(),area_less);
  std::vector<std::vector<float> > colors(8, std::vector<float>(3, 0.0f));
  colors[0][0] = 1.0f;
  colors[1][1] = 1.0f;
  colors[2][0] = 1.0f; colors[2][1] = 1.0f;
  colors[3][0] = 1.0f; colors[3][2] = 1.0f;
  colors[4][1] = 1.0f; colors[4][2] = 1.0f;
  colors[5][0] = 0.5f; colors[5][1] = 1.0f;
  colors[6][0] = 1.0f; colors[6][1] = 0.5f;
  colors[7][1] = 1.0f; colors[7][2] = 0.5f;

  std::vector<std::map<size_t, vsph_spherical_triangle> > in_tris(nb);
  for(size_t i = 0; i<nb; ++i){
    const vsph_sph_box_2d& box = boxes[i];
    size_t nt = triangles_.size();
    for(size_t t = 0; t<nt; ++t){
      size_t cidx = triangles_[t].cell_index();
      const vsph_sph_point_3d& c = triangles_[t].centroid(coord_sys_);
      if(box.contains(c.theta_, c.phi_))
        in_tris[i][cidx] = triangles_[t];
    }
  }

  // extract points and associate with triangles
  std::vector<std::vector<vgl_point_3d<double> > > pts(nb);
  std::vector<std::vector<std::tuple<size_t, size_t, size_t > > > mapped_in_tris(nb);
  for(size_t ii = 0; ii<nb; ++ii){
    size_t i = box_areas[ii].first;
    size_t pt_index = 0;
    double del = 0.05/(1.0+i);
    for(std::map<size_t, vsph_spherical_triangle> ::iterator trit = in_tris[i].begin();
        trit != in_tris[i].end(); ++trit){
      vsph_spherical_triangle& tri = trit->second;
      pts[i].push_back(tri.v0_cart()+ del*coord_sys_.radial_vector(tri.v0()));
      pts[i].push_back(tri.v1_cart()+ del*coord_sys_.radial_vector(tri.v1()));
      pts[i].push_back(tri.v2_cart()+ del*coord_sys_.radial_vector(tri.v2()));
      mapped_in_tris[i].emplace_back(pt_index, pt_index+1, pt_index+2);
      pt_index+=3;
    }
  }
  std::ofstream os(path.c_str());
  if(!os){
    std::cout << "can't open " << path << "to display boxes" << std::endl;
    return false;
  }
  os << "#VRML V2.0 utf8\n";
    for(size_t i = 0; i<nb; ++i){
      size_t color_idx = i%8;
     os << "Shape {\n"
     << "  geometry IndexedFaceSet {\n"
     << "   colorPerVertex FALSE\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
     int np = pts[i].size()-1;
     for (size_t j = 0; j <= np; ++j){
       const vgl_point_3d<double>& cp = pts[i][j];
       double x = fabs(cp.x())<0.001 ? 0.0 : cp.x();
       double y = fabs(cp.y())<0.001 ? 0.0 : cp.y();
       double z = fabs(cp.z())<0.001 ? 0.0 : cp.z();
       os << x << ' ' << y << ' ' << z;
       if (j != np) os << ',';
        os << '\n';
     }
      os <<             "]\n"
         <<      "}\n"
         << "    color Color {\n"
         << "      color [\n";
      for(size_t c = 0; c<7; ++c)
        os << colors[c][0] << ' ' << colors[c][1] << ' ' << colors[c][2] << ",\n";
      os << colors[7][0] << ' ' << colors[7][1] << ' ' << colors[7][2] << "\n";
      os << "    ]\n"
         << "}\n"
         << "     coordIndex [\n";

     int ne = mapped_in_tris[i].size()-1;
     for (size_t j = 0; j <= ne; ++j) {
       std::tuple<size_t, size_t, size_t>& tri = mapped_in_tris[i][j];
       if (j != ne)
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ',' << std::get<2>(tri) << ",-1,\n";
       else
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ','  << std::get<2>(tri) << ",-1\n";
     }
    os <<        "]\n";
    os << "colorIndex[\n";
    for (size_t j = 0; j<=np; ++j)
      if(j != np)
        os << color_idx << ", ";
      else
        os << color_idx <<"\n";
    os <<        "]\n"
     << "    }\n"
     << "  }\n";
    }
    if(disp_unit_sphere)
      display_unit_sphere(os);
    os.close();
    return true;
}
bool vsph_unit_sphere::display_cells(std::string const & path, std::vector<vsph_spherical_triangle> const& triangles,
                                     bool disp_unit_sphere){
  std::ofstream os(path.c_str());
  if(!os){
    std::cout << "can't open " << path << "to display boxes" << std::endl;
    return false;
  }
  size_t nt = triangles.size();
  // extract points and associate with triangles
  std::vector<vgl_point_3d<double> >  pts;
  std::vector<std::tuple<size_t, size_t, size_t > >  mapped_in_tris;
  size_t pt_index = 0;
  for(size_t t = 0; t<nt; ++t){
    const vsph_spherical_triangle& tri = triangles[t];
    //map to the unit sphere
    vsph_sph_point_3d sp0(1.0, tri.v0().theta_, tri.v0().phi_);
    vsph_sph_point_3d sp1(1.0, tri.v1().theta_, tri.v1().phi_);
    vsph_sph_point_3d sp2(1.0, tri.v2().theta_, tri.v2().phi_);

    // cache Cartesian triangle vertices
    pts.push_back(coord_sys_.cart_coord(sp0));
    pts.push_back(coord_sys_.cart_coord(sp1));
    pts.push_back(coord_sys_.cart_coord(sp2));

    // the triangle vertex indices
    mapped_in_tris.emplace_back(pt_index, pt_index+1, pt_index+2);
    pt_index+=3;
  }

  os << "#VRML V2.0 utf8\n";
     os << "Shape {\n"
     << "  geometry IndexedFaceSet {\n"
     << "   colorPerVertex FALSE\n"
     << "     coord Coordinate {\n"
     << "      point [\n";
     int np = pts.size()-1;
     for (size_t j = 0; j <= np; ++j){
       const vgl_point_3d<double>& cp = pts[j];
       double x = fabs(cp.x())<0.001 ? 0.0 : cp.x();
       double y = fabs(cp.y())<0.001 ? 0.0 : cp.y();
       double z = fabs(cp.z())<0.001 ? 0.0 : cp.z();
       os << x << ' ' << y << ' ' << z;
       if (j != np) os << ',';
        os << '\n';
     }
      os <<             "]\n"
         <<      "}\n"
         << "    color Color {\n"
         << "      color [\n";
        os << 0.0 << ' ' << 1.0 << ' ' << 0.0 << ",\n";
        os << "    ]\n"
         << "}\n"
         << "     coordIndex [\n";

     int ne = mapped_in_tris.size()-1;
     for (size_t j = 0; j <= ne; ++j) {
       std::tuple<size_t, size_t, size_t>& tri = mapped_in_tris[j];
       if (j != ne)
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ',' << std::get<2>(tri) << ",-1,\n";
       else
         os << std::get<0>(tri) << ',' << std::get<1>(tri) << ','  << std::get<2>(tri) << ",-1\n";
     }
    os <<        "]\n";
    os << "colorIndex[\n";
    for (size_t j = 0; j<=np; ++j)
      if(j != np)
        os << 0 << ", ";
      else
        os << 0 <<"\n";
    os <<        "]\n"
     << "    }\n"
     << "  }\n";

    if(disp_unit_sphere)
      display_unit_sphere(os);
    os.close();
    return true;
}

bool vsph_unit_sphere::operator==(const vsph_unit_sphere &other) const
{
  if (point_angle_ != other.point_angle() ||
      min_theta_   != other.min_theta() ||
      max_theta_   != other.max_theta() ||
      this->size() != other.size())
    return false;
  const std::vector<vsph_sph_point_2d>& osph_pts = other.sph_points();
  for (unsigned i = 0; i < sph_pts_.size(); ++i) {
    if (!(sph_pts_[i]==osph_pts[i]))
      return false;
  }
  return true;
}

// =================   binary I/O ==========================
void vsph_unit_sphere::b_read(vsl_b_istream& is)
{
  neighbors_valid_ = false;
  short version;
  vsl_b_read(is, version);
  switch (version) {
   case 1:
    vsl_b_read(is, point_angle_);
    vsl_b_read(is, min_theta_);
    vsl_b_read(is, max_theta_);
    vsl_b_read(is, sph_pts_);
    vsl_b_read(is, edges_);
    this->set_cart_points();
    this->find_neighbors();
    break;
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_unit_sphere&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

void vsph_unit_sphere::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, point_angle_);
  vsl_b_write(os, min_theta_);
  vsl_b_write(os, max_theta_);
  vsl_b_write(os, sph_pts_);
  vsl_b_write(os, edges_);
}

void vsl_b_read(vsl_b_istream& is, vsph_unit_sphere& usph)
{
  usph.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_unit_sphere const& usph)
{
  usph.b_write(os);
}

void vsph_edge::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
   case 1:
    vsl_b_read(is, vs_);
    vsl_b_read(is, ve_);
    break;
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vsph_edge&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

void vsph_edge::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, (short)version());
  vsl_b_write(os, vs_);
  vsl_b_write(os, ve_);
}

void vsl_print_summary(std::ostream& os, vsph_edge const& e)
{
  e.print(os);
}

void vsl_b_read(vsl_b_istream& is, vsph_edge& e)
{
  e.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_edge const& e)
{
  e.b_write(os);
}

std::ostream& operator<<(std::ostream& os, vsph_unit_sphere const& vs)
{
  vs.print(os);
  return os;
}

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere const* usph_ptr)
{
  if (usph_ptr==nullptr)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    vsl_b_write(os, *usph_ptr);
  }
}

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere*& usph_ptr)
{
  delete usph_ptr; usph_ptr = nullptr;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    usph_ptr = new vsph_unit_sphere();
    usph_ptr->b_read(is);
  }
}

void vsl_b_write(vsl_b_ostream &os, vsph_unit_sphere_sptr const&  usph_sptr)
{
  vsl_b_write(os, usph_sptr.ptr());
}

void vsl_b_read(vsl_b_istream &is, vsph_unit_sphere_sptr& usph_sptr)
{
  vsph_unit_sphere* usph_ptr = nullptr;
  vsl_b_read(is, usph_ptr);
  usph_sptr = usph_ptr;
}
