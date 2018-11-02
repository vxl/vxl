// This is brl/bbas/imesh/algo/imesh_imls_surface.cxx
#include <iostream>
#include <cmath>
#include <limits>
#include "imesh_imls_surface.h"
//:
// \file

#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_intersect.h>
#include <imesh/algo/imesh_kd_tree.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>


//: Constructor
imesh_imls_surface::imesh_imls_surface(const imesh_mesh& mesh, double eps, double lambda,
                                       bool enforce_bounded,
                                       const std::set<unsigned int>& no_normal_faces)
  : verts_(mesh.num_verts()), phi_(mesh.num_verts(),0.0),
    eps2_(eps*eps), lambda_(lambda), iso_level_(0.0), bounded_(enforce_bounded)
{
  const imesh_vertex_array<3>& v = mesh.vertices<3>();

  for (unsigned int i=0; i<v.size(); ++i)
    verts_[i] = vgl_point_3d<double>(v[i]);

  if (mesh.faces().regularity() != 3)
    triangles_ = imesh_triangulate(mesh.faces());
  else
    triangles_.reset(static_cast<imesh_regular_face_array<3>*>
                     (mesh.faces().clone()));

  if (bounded_)
  {
    // build enclosure
    vgl_box_3d<double> box;
    for (const auto & vert : verts_) {
      box.add(vert);
    }
    box.expand_about_centroid(1);
    unsigned int base = verts_.size();
    verts_.emplace_back(box.min_x(),box.min_y(),box.min_z());
    verts_.emplace_back(box.min_x(),box.min_y(),box.max_z());
    verts_.emplace_back(box.min_x(),box.max_y(),box.min_z());
    verts_.emplace_back(box.min_x(),box.max_y(),box.max_z());
    verts_.emplace_back(box.max_x(),box.min_y(),box.min_z());
    verts_.emplace_back(box.max_x(),box.min_y(),box.max_z());
    verts_.emplace_back(box.max_x(),box.max_y(),box.min_z());
    verts_.emplace_back(box.max_x(),box.max_y(),box.max_z());
    triangles_->push_back(imesh_tri(base+0,base+1,base+2));
    triangles_->push_back(imesh_tri(base+1,base+3,base+2));
    triangles_->push_back(imesh_tri(base+0,base+6,base+4));
    triangles_->push_back(imesh_tri(base+0,base+2,base+6));
    triangles_->push_back(imesh_tri(base+2,base+7,base+6));
    triangles_->push_back(imesh_tri(base+2,base+3,base+7));
    triangles_->push_back(imesh_tri(base+3,base+5,base+7));
    triangles_->push_back(imesh_tri(base+3,base+1,base+5));
    triangles_->push_back(imesh_tri(base+1,base+4,base+5));
    triangles_->push_back(imesh_tri(base+1,base+0,base+4));
    triangles_->push_back(imesh_tri(base+4,base+7,base+5));
    triangles_->push_back(imesh_tri(base+4,base+6,base+7));
    phi_.resize(verts_.size(),1.0);
  }

  std::vector<vgl_box_3d<double> > boxes(triangles_->size());
  for ( unsigned int i=0; i<boxes.size(); ++i ) {
    const imesh_regular_face<3>& tri = (*triangles_)[i];
    boxes[i].add(verts_[tri[0]]);
    boxes[i].add(verts_[tri[1]]);
    boxes[i].add(verts_[tri[2]]);
  }

  kd_tree_ = imesh_build_kd_tree(boxes);
  unsigned num_tree_nodes = triangles_->size()*2-1;

  unweighted_.resize(num_tree_nodes);
  centroid_.resize(num_tree_nodes);
  normals_.resize(num_tree_nodes);
  normal_len_.resize(num_tree_nodes);
  area_.resize(num_tree_nodes);


  compute_centroids_rec(kd_tree_, no_normal_faces);
  compute_unweighed_rec(kd_tree_);

  compute_enclosing_phi();
}


//: Copy Constructor
imesh_imls_surface::imesh_imls_surface(const imesh_imls_surface& other)
  : verts_(other.verts_),
    triangles_(other.triangles_.get() ?
               new imesh_regular_face_array<3>(*other.triangles_) :
               nullptr),
    kd_tree_(other.kd_tree_.get() ?
             new imesh_kd_tree_node(*other.kd_tree_) :
             nullptr),
    phi_(other.phi_),
    area_(other.area_),
    unweighted_(other.unweighted_),
    centroid_(other.centroid_),
    normals_(other.normals_),
    normal_len_(other.normal_len_),
    eps2_(other.eps2_),
    lambda_(other.lambda_),
    iso_level_(other.iso_level_)
{
}


//: compute the iso value such that the mean value at the vertices is zero
void imesh_imls_surface::compute_iso_level()
{
  double mean = 0.0;
  for (const auto & vert : verts_)
    mean += (*this)(vert);
  iso_level_ = mean / verts_.size();
}


//: adjust the phi values until all vertices are within the iso surface
// Also computes the iso level
void imesh_imls_surface::compute_enclosing_phi()
{
  typedef std::pair<unsigned int,double> pair_id;
  std::vector<pair_id> outside;
  double mean = 0.0;
  const unsigned int num_verts = bounded_ ?
                                 verts_.size() :
                                 verts_.size() - 8;
  for (unsigned int i=0; i<num_verts; ++i) {
    double val = (*this)(verts_[i]);
    mean += val;
    if (val > 0)
      outside.emplace_back(i,val);
  }
  iso_level_ = mean / verts_.size();

  for (double s=1.0; !outside.empty(); s*=2) {
    std::cout << outside.size() << " outside" << std::endl;
    for (std::vector<pair_id>::const_iterator i=outside.begin();
         i!=outside.end(); ++i) {
      phi_[i->first] -= s*i->second;
    }
    compute_unweighed_rec(kd_tree_);
    std::vector<pair_id> next_outside;
    for (std::vector<pair_id>::const_iterator i=outside.begin();
         i!=outside.end(); ++i) {
      double val = (*this)(verts_[i->first]);
      if (val > std::abs(std::numeric_limits<double>::epsilon()*phi_[i->first])) {
        next_outside.emplace_back(i->first,val);
      }
    }
    outside.swap(next_outside);
  }
}


//: recursively compute the area weighted centroids
void imesh_imls_surface::
compute_centroids_rec(const std::unique_ptr<imesh_kd_tree_node>& node,
                      const std::set<unsigned int>& no_normal_faces)
{
  const unsigned int& i = node->index_;
  if (node->is_leaf()) {
    const imesh_regular_face<3>& tri = (*triangles_)[i];
    const vgl_point_3d<double>& v0 = verts_[tri[0]];
    const vgl_point_3d<double>& v1 = verts_[tri[1]];
    const vgl_point_3d<double>& v2 = verts_[tri[2]];
    vgl_vector_3d<double>& n = normals_[i];
    n = cross_product(v1-v0,v2-v0)/2.0;
    normal_len_[i] = n.length();
    area_[i] = normal_len_[i];
    centroid_[i] = centre(v0,v1,v2);
    if (no_normal_faces.find(i) != no_normal_faces.end()) {
      n = vgl_vector_3d<double>(0,0,0);
      normal_len_[i] = 0.0;
    }
    return;
  }

  compute_centroids_rec(node->left_,no_normal_faces);
  compute_centroids_rec(node->right_,no_normal_faces);

  const unsigned int& i_left = node->left_->index_;
  const unsigned int& i_right = node->right_->index_;

  const vgl_point_3d<double>& cl = centroid_[i_left];
  const double& al = area_[i_left];
  const vgl_point_3d<double>& cr = centroid_[i_right];
  const double& ar = area_[i_right];
  double& at = area_[i];
  at = al + ar;
  centroid_[i].set((ar*cr.x() + al*cl.x()) / at,
                   (ar*cr.y() + al*cl.y()) / at,
                   (ar*cr.z() + al*cl.z()) / at);
  //normals_[i] = normalized(normals_[i_left]*al + normals_[i_right]*ar);
  normals_[i] = normals_[i_left] + normals_[i_right];
  normal_len_[i] = normals_[i].length();
}


//: recursively compute the unweighted integrals
void imesh_imls_surface::
compute_unweighed_rec(const std::unique_ptr<imesh_kd_tree_node>& node)
{
  const unsigned int& i = node->index_;
  if (node->is_leaf()) {
    const imesh_regular_face<3>& tri = (*triangles_)[i];
    unweighted_[i] = (phi_[tri[0]]
                    + phi_[tri[1]]
                    + phi_[tri[2]])/3.0 * area_[i];
          //- dot_product(normals_[i],centroid_[i]-vgl_point_3d<double>(0,0,0));
    return;
  }

  compute_unweighed_rec(node->left_);
  compute_unweighed_rec(node->right_);

  const unsigned int& i_left = node->left_->index_;
  const unsigned int& i_right = node->right_->index_;
  unweighted_[i] = unweighted_[i_left] + unweighted_[i_right];
}


//: return a bounding box for the original input mesh
vgl_box_3d<double> imesh_imls_surface::bounding_box() const
{
  return kd_tree_->inner_box_;
}


//: change the epsilon (smoothness) of the surface
void imesh_imls_surface::set_epsilon(double eps)
{
  eps2_ = eps*eps;
  iso_level_ = 0.0;

  // reset the phi values at each vertex
  for (double & i : phi_)
    i = 0.0;

  compute_unweighed_rec(kd_tree_);

  compute_enclosing_phi();
}


namespace {
class tri_dist_func
{
 public:
  tri_dist_func(const std::vector<vgl_point_3d<double> >& v,
                const imesh_regular_face_array<3>& t)
  : verts(v), tris(t), closest_index(static_cast<unsigned int>(-1)),
    closest_dist(std::numeric_limits<double>::infinity()) {}
  const std::vector<vgl_point_3d<double> >& verts;
  const imesh_regular_face_array<3>& tris;
  unsigned int closest_index;
  double closest_dist, closest_u, closest_v;

  double operator () (const vgl_point_3d<double>& pt, unsigned int i)
  {
    const imesh_regular_face<3>& tri = tris[i];
    double dist,u,v;
    /* unsigned char s = */
    imesh_triangle_closest_point(pt,
                                 verts[tri[0]], verts[tri[1]], verts[tri[2]],
                                 dist, u, v);
    if (dist < closest_dist) {
      closest_dist = dist;
      closest_index = i;
      closest_u = u;
      closest_v = v;
    }
    return dist;
  }
};
// end of namespace
}


//: evaluate the implicit surface at a point
double imesh_imls_surface::operator() (const vgl_point_3d<double>& p) const
{
  double sum=0.0, sum_phi = 0.0;

  std::vector<imesh_kd_tree_queue_entry> remain;
  tri_dist_func dist(verts_,*triangles_);
  /* unsigned int ind = */
  imesh_closest_index<tri_dist_func&>(p,kd_tree_,dist,&remain);

  // compute the (negative) maximum error of integration
  // stored negative so that the max error is first when sorted by <
  auto itr = remain.begin();
  for (; itr != remain.end(); ++itr) {
    double min = w2(itr->val_);
    double max = w2(imesh_max_sq_dist(p,itr->node_->inner_box_));
    itr->val_ = (max - min)*area_[itr->node_->index_];
  }
  std::make_heap( remain.begin(), remain.end() );

  std::pop_heap( remain.begin(), remain.end() );
  while (!remain.empty() && -remain.back().val_ > lambda_*sum)
  {
    imesh_kd_tree_node* current = remain.back().node_;
    remain.pop_back();

    if (current->is_leaf())
    {
      unsigned int i = current->index_;
      const imesh_regular_face<3>& tri = (*triangles_)[i];
      unsigned int i1=tri[0], i2=tri[1], i3=tri[2];
      typedef vgl_vector_2d<double> T;
      typedef vgl_vector_2d<double> (*F) (const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                          const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                          double, double, double, double);
      vgl_vector_2d<double> I = triangle_quadrature<T,F>(split_triangle_quadrature,
                                                         p,verts_[i1],verts_[i2],verts_[i3],
                                                         normals_[i]*2.0,
                                                         phi_[i1],phi_[i2],phi_[i3],eps2_);
      assert(!vnl_math::isnan(I.x()) && !vnl_math::isnan(I.y()));

      sum_phi += I.x();
      if (normal_len_[i]>0.0)
        sum_phi += dot_product(normals_[i],p-verts_[i1]) * I.y() / normal_len_[i];
      sum += I.y();
    }
    else
    {
      double min = w2(imesh_min_sq_dist(p,current->left_->inner_box_));
      double max = w2(imesh_max_sq_dist(p,current->left_->inner_box_));
      remain.emplace_back((max - min)*area_[current->left_->index_],
                                                 current->left_.get());
      std::push_heap(remain.begin(), remain.end());

      min = w2(imesh_min_sq_dist(p,current->right_->inner_box_));
      max = w2(imesh_max_sq_dist(p,current->right_->inner_box_));
      remain.emplace_back((max - min)*area_[current->right_->index_],
                                                 current->right_.get());
      std::push_heap(remain.begin(), remain.end());
    }
    if (!remain.empty())
      std::pop_heap( remain.begin(), remain.end() );
  }

  // approximate the contribution from the remain nodes
  for (itr = remain.begin(); itr != remain.end(); ++itr) {
    // Use approximation assuming the weight is constant
    unsigned int i = itr->node_->index_;
    vgl_vector_3d<double> v(p-centroid_[i]);
    double w = w2(v.sqr_length());

    sum_phi += w*unweighted_[i] + dot_product(normals_[i],v)*w;
    sum += w*area_[i];
  }

  assert(sum != 0.0);
  return sum_phi / sum - iso_level_;
}


//: evaluate the function and its derivative (returned by reference)
double imesh_imls_surface::deriv(const vgl_point_3d<double>& p,
                                 vgl_vector_3d<double>& dp) const
{
  integral_data sums;

  std::vector<imesh_kd_tree_queue_entry> remain;
  tri_dist_func dist(verts_,*triangles_);
  /* unsigned int ind = */
  imesh_closest_index<tri_dist_func&>(p,kd_tree_,dist,&remain);

  // compute the (negative) maximum error of integration
  // stored negative so that the max error is first when sorted by <
  auto itr = remain.begin();
  for (; itr != remain.end(); ++itr) {
    double min = w2(itr->val_);
    double max = w2(imesh_max_sq_dist(p,itr->node_->inner_box_));
    itr->val_ = (max - min)*area_[itr->node_->index_];
  }
  std::make_heap( remain.begin(), remain.end() );

  std::pop_heap( remain.begin(), remain.end() );
  while (!remain.empty() && -remain.back().val_ > lambda_*sums.I)
  {
    imesh_kd_tree_node* current = remain.back().node_;
    remain.pop_back();

    if (current->is_leaf())
    {
      unsigned int i = current->index_;
      const imesh_regular_face<3>& tri = (*triangles_)[i];
      unsigned int i1=tri[0], i2=tri[1], i3=tri[2];
      typedef integral_data T;
      typedef integral_data (*F) (const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                  const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                  double, double, double, double);
      integral_data Id = triangle_quadrature<T,F>(split_triangle_quadrature_with_deriv,
                                                  p,verts_[i1],verts_[i2],verts_[i3],
                                                  normals_[i]*2.0,
                                                  phi_[i1],phi_[i2],phi_[i3],eps2_);
      assert(!vnl_math::isnan(Id.I) && !vnl_math::isnan(Id.I_phi));
      //assert(!vnl_math::isnan(Id.dI) && !vnl_math::isnan(Id.dI_phi));

      sums.I += Id.I;
      sums.I_phi += Id.I_phi;
      sums.dI += Id.dI;
      sums.dI_phi += Id.dI_phi;
      // terms involving the normal constraints
      if (normal_len_[i]>0.0) {
        const vgl_vector_3d<double>& n = normals_[i];
        double plane_dist = dot_product(n,p-verts_[i1])/normal_len_[i];
        sums.I_phi += plane_dist*Id.I;
        sums.dI_phi += n*(Id.I/normal_len_[i]) + plane_dist*Id.dI;
      }
    }
    else
    {
      double min = w2(imesh_min_sq_dist(p,current->left_->inner_box_));
      double max = w2(imesh_max_sq_dist(p,current->left_->inner_box_));
      remain.emplace_back((max - min)*area_[current->left_->index_],
                                                 current->left_.get());
      std::push_heap(remain.begin(), remain.end());

      min = w2(imesh_min_sq_dist(p,current->right_->inner_box_));
      max = w2(imesh_max_sq_dist(p,current->right_->inner_box_));
      remain.emplace_back((max - min)*area_[current->right_->index_],
                                                 current->right_.get());
      std::push_heap(remain.begin(), remain.end());
    }
    if (!remain.empty())
      std::pop_heap( remain.begin(), remain.end() );
  }

  // approximate the contribution from the remain nodes
  for (itr = remain.begin(); itr != remain.end(); ++itr) {
    // Use approximation assuming the weight is constant
    unsigned int i = itr->node_->index_;
    vgl_vector_3d<double> v(p-centroid_[i]);
    double w = 1.0/(v.sqr_length() + eps2_);
    vgl_vector_3d<double> dw(v);
    dw *= -4*w;
    w *= w;
    dw *= w;

    const vgl_vector_3d<double>& n = normals_[i];
    double plane_dist = dot_product(n,v);
    sums.I += w*area_[i];
    sums.I_phi += w*unweighted_[i] + plane_dist*w;
    sums.dI += dw*area_[i];
    sums.dI_phi += n*w + dw*(unweighted_[i] + plane_dist);
  }


  assert(sums.I != 0.0);
  dp = (-sums.I_phi/(sums.I*sums.I))*sums.dI + sums.dI_phi/sums.I;
  return sums.I_phi / sums.I - iso_level_;
}


//: evaluate the function and its first and second derivatives (returned by reference)
double imesh_imls_surface::deriv2(const vgl_point_3d<double>& p,
                                  vgl_vector_3d<double>& dp,
                                  vnl_double_3x3& ddp) const
{
  double eps = 1e-8;
  double val = this->deriv(p,dp);
  vgl_vector_3d<double> dpx,dpy,dpz;
  this->deriv(p+vgl_vector_3d<double>(eps,0,0),dpx);
  this->deriv(p+vgl_vector_3d<double>(0,eps,0),dpy);
  this->deriv(p+vgl_vector_3d<double>(0,0,eps),dpz);
  dpx -= dp;
  dpx /= eps;
  dpy -= dp;
  dpy /= eps;
  dpz -= dp;
  dpz /= eps;
  ddp(0,0) = dpx.x();   ddp(0,1) = dpy.x();   ddp(0,2) = dpz.x();
  ddp(1,0) = dpx.y();   ddp(1,1) = dpy.y();   ddp(1,2) = dpz.y();
  ddp(2,0) = dpx.z();   ddp(2,1) = dpy.z();   ddp(2,2) = dpz.z();
  return val;
}

//: integrals of f(x)dx and x*f(x)dx over [0,1] where f(x)= 1/((x+k1)^2 + k2)^2
//
//  These equations are wrong in the paper, they should be (for a=1):
// Beta = atan( k1/sqrt(k2) ) - atan( (k1+1)/sqrt(k2) )
// I1 = -Beta * 1/(2*k2^(3/2))  + (k2 - k1*(k1+1)) / (2*k2*(k1^2+k2)*((k1+1)^2+k2))
// Ix = Beta * k1/(2*k2^(3/2))  + (k1 + 1) / (2*k2*((k1+1)^2+k2))
void
imesh_imls_surface::line_integrals(double k1, double k2, double& I1, double& Ix)
{
  double sqrt_k2 = std::sqrt(k2);
  double k1_p1 = k1+1.0;
  I1 = Ix = (std::atan(k1_p1/sqrt_k2) - std::atan(k1/sqrt_k2) ) / (2.0*k2*sqrt_k2);
  Ix *= -k1;

  double denom = 1.0/(2.0*k2*(k1_p1*k1_p1+k2));

  Ix += k1_p1 * denom;
  I1 += (k2 - k1*k1_p1)*denom / (k1*k1+k2);
}


//: integrals of f(x)dx and x*f(x)dx over [0,1] where f(x)= 1/((x+k1)^2 + k2)^2
//  Also compute the integrals when f(x)=1/((x+k1)^2 + k2)^3 (for use in derivatives)
//
// Beta = atan( k1/sqrt(k2) ) - atan( (k1+1)/sqrt(k2) )
// I1 = -Beta * 1/(2*k2^(3/2))  + (k2 - k1*(k1+1)) / (2*k2*(k1^2+k2)*((k1+1)^2+k2))
// Ix = Beta * k1/(2*k2^(3/2))  + (k1 + 1) / (2*k2*((k1+1)^2+k2))
// dI1 = 1/8 * ( -Beta*3/k2^(5/2) +
//               (5*k2^3 - (k1*(k1+1)-3)*k2^2 - k1*(k1+1)*(9*k1*(k1+1)+5)*k2 - 3*k1^3*(k1+1)^3)
//               / (k2^2*(k1^2+k2)^2*((k1+1)^2+k2)^2) )
// dIx = 1/8 * ( Beta*3*k1/k2^(5/2) +
//               ((k1^2+k2)*((3*(k1+1)+1)*k2^2 + (k1+1)*(6*k1^2+3*k1+2)*k2 + 3*k1^2*(k1+1)^3))
//               /(k2^2*(k1^2+k2)^2*((k1+1)^2+k2)^2)  )
// dIx2 = 1/8 * ( -Beta*(3*k1^2+k2)/k2^(5/2) -
//                ((k1^2+k2)^2*(k2^2 + (k1+1)*(4*k1-1)*k2 + 3*k1*(k1+1)^3))
//                /(k2^2*(k1^2+k2)^2*((k1+1)^2+k2)^2)  )
void
imesh_imls_surface::line_integrals(double k1, double k2,
                                   double& I1, double& Ix,
                                   double& dI1, double& dIx, double& dIx2)
{
  double sqrt_k2 = std::sqrt(k2);
  double k1_p1 = k1+1.0;
  double k1_2 = k1*k1;
  double k2_2 = k2*k2;
  double k1_p1_2 = k1_p1*k1_p1;
  double t1 = k2 + k1_p1_2;
  double t2 = k2 + k1_2;
  double t3 = 3*k1_p1_2*k1_p1*k1;


  I1 = dI1 = (std::atan(k1_p1/sqrt_k2) - std::atan(k1/sqrt_k2) ) / (2.0*k2*sqrt_k2);
  dI1 *= 0.75/k2;
  Ix = -k1 * I1;
  dIx = -k1 * dI1;
  dIx2 = 0.25*I1*(3*k1_2+k2)/k2;


  double denom = 0.5/(k2*t1);
  double ddenom = 0.125/(k2_2*t2*t2*t1*t1);

  I1 += (k2 - k1*k1_p1)*denom / t2;
  Ix += k1_p1 * denom;

  dI1 += (5*k2*k2_2 - (k1*k1_p1-3)*k2_2 - k1*k1_p1*(9*k1*k1_p1+5)*k2 - k1_2*t3 )*ddenom;
  dIx += t2*((3*k1_p1+1)*k2_2 + k1_p1*(6*k1_2+3*k1+2)*k2 + k1*t3)*ddenom;
  dIx2 -= t2*t2*(k2_2 + k1_p1*(4*k1-1)*k2 + t3)*ddenom;
}


//: line integral of the squared weight function times a linear value on the line from p0 to p1
//  (value at p0 is v0 and at p1 is v1)
//  \a eps2 is epsilon^2
double
imesh_imls_surface::line_integral(const vgl_point_3d<double>& x,
                                  const vgl_point_3d<double>& p0,
                                  const vgl_point_3d<double>& p1,
                                  double v0, double v1, double eps2)
{
  vgl_vector_3d<double> ab(p1-p0);
  vgl_vector_3d<double> xa(p0-x);
  double denom = 1.0/ab.sqr_length();
  double k1 = dot_product(ab,xa)*denom;
  double k2 = (xa.sqr_length()+eps2)*denom - k1*k1;
  double I1,Ix;
  line_integrals(k1,k2,I1,Ix);
  return (v0*I1 + (v1-v0)*Ix)*std::sqrt(denom)*denom;
}


//: The derivative of the line integral with respect to x
vgl_vector_3d<double>
imesh_imls_surface::line_integral_deriv(const vgl_point_3d<double>& x,
                                        const vgl_point_3d<double>& p0,
                                        const vgl_point_3d<double>& p1,
                                        double v0, double v1, double eps2)
{
  vgl_vector_3d<double> ab(p1-p0);
  vgl_vector_3d<double> xa(p0-x);
  double denom = 1.0/ab.sqr_length();
  double k1 = dot_product(ab,xa)*denom;
  double k2 = (xa.sqr_length()+eps2)*denom - k1*k1;
  double I1,Ix,dI1,dIx,dIx2;
  line_integrals(k1,k2,I1,Ix,dI1,dIx,dIx2);
  denom = 4*std::sqrt(denom)*denom*denom;

  return (xa*(v0*dI1+(v1-v0)*dIx)
        + ab*(v0*dIx + (v1-v0)*dIx2))*denom;//4*std::sqrt(denom)*denom*denom;
}


//: area integral of the squared weight function times a linearly interpolated value
//  \a m is the point closest point on the triangle to sample point \a x
//  \a p0 and \a p1 are the other vertices
//  call triangle_quadrature to first split an arbitrary triangle
//  \a eps2 is epsilon^2
vgl_vector_2d<double>
imesh_imls_surface::split_triangle_quadrature(const vgl_point_3d<double>& x,
                                              const vgl_point_3d<double>& pm,
                                              const vgl_point_3d<double>& p1,
                                              const vgl_point_3d<double>& p2,
                                              double vm, double v1, double v2,
                                              double eps)
{
  vgl_point_3d<double> pp(p1), pn(p2);
  double vp(v1), vn(v2);
  if ((pp-x).length() > (pn-x).length()) {
    // swap so that pp is closest to x
    pp = p2; pn = p1;
    vp = v2; vn = v1;
  }

  vgl_vector_3d<double> d1(pp-pm);
  vgl_vector_3d<double> d2(pn-pm);
  vgl_vector_3d<double> d3(pm-x);

  double t1 = d1.sqr_length();
  double t2 = dot_product(d1,d2);
  double t3 = dot_product(d1,d3);
  double t4 = d2.sqr_length();
  double t5 = dot_product(d2,d3) * 2;
  double t6 = d3.sqr_length() + eps;

  // compute height (divided by 2*sqrt(t1))
  // early exit if triangle flat
  double height = t4/t1 - t2*t2/(t1*t1);
  if (!(height > 0.0))
    return {0,0};
  height = std::sqrt(height)/2.0;

  double vt1 = vn-vm;
  double vt2 = vp-vm;

  double alpha = 2.0/3.0;
  double sum1 = 0.0, sum2 = 0.0;
  double weight = (1.0/alpha-alpha);
  double I1,Ix,u_1,denom,k1,k2;
  double u = alpha;
  double last_li1 = 0.0, last_li2 = 0.0;
  // integrate using the trapezoid rule with non-uniform sampling
  for (; u>0.01; u*=alpha) {
    sum1 += last_li1;
    sum2 += last_li2;
    u_1 = 1.0-u;
    denom = 1.0/(u_1*u_1*t1);
    k1 = (t3 + u*t2)*u_1*denom;
    k2 = (t6 + u*t5 + u*u*t4)*denom - k1*k1;
    line_integrals(k1,k2,I1,Ix);
    denom *= u / u_1;
    last_li1 = ((vm+u*vt1)*I1 + u_1*vt2*Ix) * denom;
    last_li2 = I1 * denom;
  }
  sum1 *= weight;
  sum1 += last_li1/alpha;
  sum2 *= weight;
  sum2 += last_li2/alpha;

  // add the last trapezoid covering the remaining area
  denom = 1.0/t1;
  k1 = t3*denom;
  k2 = t6*denom - k1*k1;
  line_integrals(k1,k2,I1,Ix);
  denom *= u/alpha;
  sum1 += (vm*I1 + vt2*Ix) * denom;
  sum2 += I1 * denom;

  sum1 *= height;
  sum2 *= height;


  return {sum1,sum2};
}


//: area integral of the squared weight function times a linearly interpolated value
//  Also computes vector term used in the derivative
//  \a m is the point closest point on the triangle to sample point \a x
//  \a p0 and \a p1 are the other vertices
//  call triangle_quadrature to first split an arbitrary triangle
//  \a eps2 is epsilon^2
imesh_imls_surface::integral_data imesh_imls_surface::
split_triangle_quadrature_with_deriv(const vgl_point_3d<double>& x,
                                     const vgl_point_3d<double>& pm,
                                     const vgl_point_3d<double>& p1,
                                     const vgl_point_3d<double>& p2,
                                     double vm, double v1, double v2,
                                     double eps)
{
  vgl_point_3d<double> pp(p1), pn(p2);
  double vp(v1), vn(v2);
  if ((pp-x).length() > (pn-x).length()) {
    // swap so that pp is closest to x
    pp = p2; pn = p1;
    vp = v2; vn = v1;
  }

  vgl_vector_3d<double> d1(pp-pm);
  vgl_vector_3d<double> d2(pn-pm);
  vgl_vector_3d<double> d3(pm-x);

  double t1 = d1.sqr_length();
  double t2 = dot_product(d1,d2);
  double t3 = dot_product(d1,d3);
  double t4 = d2.sqr_length();
  double t5 = dot_product(d2,d3) * 2;
  double t6 = d3.sqr_length() + eps;

  // compute height (divided by 2*sqrt(t1))
  // early exit if triangle flat
  double height = t4/t1 - t2*t2/(t1*t1);
  if (!(height > 0.0))
    return {};
  height = std::sqrt(height)/2.0;

  double vt1 = vn-vm;
  double vt2 = vp-vm;

  double alpha = 2.0/3.0;
  integral_data i_data, last_i_data;
  double weight = (1.0/alpha-alpha);
  double I1,Ix,dI1,dIx,dIx2,u_1,denom,k1,k2;
  double u = alpha;
  // integrate using the trapezoid rule with non-uniform sampling
  constexpr double lower_bound = 0.01;//((t6<t4)?(t6/t4):1.0) * 0.01;
  for (; u>lower_bound; u*=alpha) {
    i_data += last_i_data;
    u_1 = 1.0-u;
    denom = 1.0/(u_1*u_1*t1);
    k1 = (t3 + u*t2)*u_1*denom;
    k2 = (t6 + u*t5 + u*u*t4)*denom - k1*k1;
    line_integrals(k1,k2,I1,Ix,dI1,dIx,dIx2);
    double phi_c = vm+u*vt1;
    double phi_x = u_1*vt2;
    last_i_data.I = I1;
    last_i_data.I_phi = (phi_c*I1 + phi_x*Ix);

    vgl_vector_3d<double> d_c(d3+u*d2), d_x(d1*u_1);
    last_i_data.dI = (d_c*dI1 + d_x*dIx)*denom;
    last_i_data.dI_phi = ( d_c*(phi_c*dI1 + phi_x*dIx)
                         + d_x*(phi_c*dIx + phi_x*dIx2) ) * denom;

    denom *= u / u_1;
    last_i_data *= denom;
  }
  i_data *= weight;
  last_i_data *= 1.0/alpha;
  i_data += last_i_data;


  // add the last trapezoid covering the remaining area
  denom = 1.0/t1;
  k1 = t3*denom;
  k2 = t6*denom - k1*k1;
  line_integrals(k1,k2,I1,Ix,dI1,dIx,dIx2);
  denom *= u/alpha;

  i_data.I += I1 * denom;
  i_data.I_phi += (vm*I1 + vt2*Ix) * denom;
  denom /= t1;
  i_data.dI += (d3*dI1 + d1*dIx)*denom;
  i_data.dI_phi += ( d3*(vm*dI1 + vt2*dIx)
                   + d1*(vm*dIx + vt2*dIx2) ) * denom;

  i_data *= height;
  i_data.dI *= 4.0;
  i_data.dI_phi *= 4.0;

  return i_data;
}


//=============================================================================
// External functions

//: find the zero crossing point by bisection between positive point \a pp and negative point \a pn
//  Stops searching when $||pp-pn|| < xeps$ or $|f(pm)| < feps$
vgl_point_3d<double> bisect(const imesh_imls_surface& f,
                            vgl_point_3d<double> pp,
                            vgl_point_3d<double> pn,
                            double feps, double xeps)
{
  assert(f(pp) > 0.0);
  assert(f(pn) < 0.0);
  vgl_point_3d<double> pm=centre(pp,pn);
  const unsigned num_itr =
      static_cast<unsigned>(std::ceil(std::log((pp-pn).length()
                                              / xeps)
                                     / 0.301029995663981)); // log_2
  vgl_vector_3d<double> dp;
  double val = f.deriv(pm,dp);
  val /= dp.length();
  for (unsigned int i=0; i<num_itr; ++i) {
    if (std::abs(val) < feps)
      return pm;
    else if (val > 0.0)
      pp = pm;
    else
      pn = pm;
    pm=centre(pp,pn);
    val = f.deriv(pm,dp);
    val /= dp.length();
  }
  return pm;
}


//: Move the point \a p along the gradient direction until reaching a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface(const imesh_imls_surface& f,
                     vgl_point_3d<double>& p,
                     double step, double eps)
{
  vgl_point_3d<double> p1(p);
  vgl_vector_3d<double> dp;
  double val1 = f.deriv(p1,dp);
  double dl = dp.length();
  val1 /= dl;
  if (std::abs(val1) < eps)
    return true;

  vgl_point_3d<double> p2 = p1 - (step*val1/dl)*dp;
  dl = dp.length();
  double val2 = f.deriv(p2,dp);
  val2 /= dl;
  unsigned int i=0;
  for (; i<100 && val1*val2 > 0.0; ++i) {
    p1 = p2;
    val1 = val2;
    p2 -= (step*val2/dl)*dp;
    val2 = f.deriv(p2,dp);
    dl = dp.length();
    val2 /= dl;
    if (std::abs(val2) < eps) {
      p = p2;
      return true;
    }
  }
  if (i >= 100)
    return false;

  if (val1 > 0.0)
    p = bisect(f,p1,p2,eps);
  else
    p = bisect(f,p2,p1,eps);


  return true;
}


namespace{
double func(const vgl_vector_3d<double>& n,
            double v,
            const vgl_vector_3d<double>& dp)
{
  v *= v;
  v /= dp.sqr_length();
  double tmp = dot_product(n,dp)/dp.length() - 1.0;
  return v + tmp*tmp;
}

vgl_vector_3d<double> dfunc(const vgl_vector_3d<double>& n,
                            double v,
                            const vgl_vector_3d<double>& dp,
                            const vnl_double_3x3& ddp)
{
  vnl_double_3 nn(n.x(),n.y(),n.z());
  vnl_double_3 ndp(dp.x(), dp.y(), dp.z());
  double sqr_len = dp.sqr_length();
  double len = std::sqrt(sqr_len);
  double n_dot_dp = dot_product(nn,ndp);
  vnl_double_3 df = (2*v/sqr_len)*ndp;
  df += ddp.transpose() * ( ((-2*v*v/(sqr_len*sqr_len))*ndp) +
                            (2/len*(n_dot_dp/len - 1)*(nn - (n_dot_dp/sqr_len)*ndp)) );
  return {df[0],df[1],df[2]};
}
// end of namespace
}

//: Move the point \a p to minimize $(f^2 + (n*f' - 1)^2)/f'*f'$ a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface_with_normal(const imesh_imls_surface& f,
                                 vgl_point_3d<double>& p,
                                 vgl_vector_3d<double> n,
                                 double step, double eps)
{
  vgl_point_3d<double> p1(p);
  normalize(n);
  vgl_vector_3d<double> dp;
  vnl_double_3x3 ddp;
  double val = f.deriv2(p1, dp, ddp);
  double f1 = func(n,val,dp);
  if (f1 < eps)
    return true;

  vgl_vector_3d<double> df = dfunc(n,val,dp,ddp);
  double dl = df.sqr_length();
  unsigned int i;
  for (i=0; i<1000; ++i) {
    vgl_point_3d<double> p2 = p1 - (step*f1/dl)*df;
    val = f.deriv2(p2,dp,ddp);
    double f2 = func(n,val,dp);
    //std::cout << i<<" f: "<<f2<<" step: "<< step<<std::endl;
    if ( f2 > f1) {
      step /= 2;
      continue;
    }
    if (f2 < eps || step < eps) {
      p = p2;
      return true;
    }
    f1 = f2;
    p1 = p2;
    df = dfunc(n,val,dp,ddp);
    dl = df.sqr_length();
    step *= 2;
  }
  if (i >= 100)
    return true;

  p = p1;
  return true;
}


//: Move the point \a p along direction \a dir until reaching a zero crossing of \a f (within \a eps).
//  Return true if successful
bool snap_to_surface(const imesh_imls_surface& f,
                     vgl_vector_3d<double> dir,
                     vgl_point_3d<double>& p,
                     double step, double eps)
{
  vgl_point_3d<double> p1(p);
  normalize(dir);
  vgl_vector_3d<double> dp;
  double val1 = f.deriv(p1,dp);
  dp = dir * dot_product(dp,dir);
  double dl = dp.length();
  val1 /= dl;
  if (std::abs(val1) < eps)
    return true;

  vgl_point_3d<double> p2 = p1 - (step*val1/dl)*dp;
  dl = dp.length();
  double val2 = f.deriv(p2,dp);
  val2 /= dl;
  unsigned int i=0;
  for (; i<100 && val1*val2 > 0.0; ++i) {
    p1 = p2;
    val1 = val2;
    p2 -= (step*val2/dl)*dp;
    val2 = f.deriv(p2,dp);
    dp = dir * dot_product(dp,dir);
    dl = dp.length();
    val2 /= dl;
    if (std::abs(val2) < eps) {
      p = p2;
      return true;
    }
  }
  if (i >= 100)
    return false;

  if (val1 > 0.0)
    p = bisect(f,p1,p2,eps);
  else
    p = bisect(f,p2,p1,eps);


  return true;
}

// Explicit instantiation needed in the implementations in this file:
#include <imesh/algo/imesh_imls_surface.hxx>
IMESH_IMLS_SURFACE_INSTANTATE(vgl_vector_2d<double>,vgl_point_3d<double>);
IMESH_IMLS_SURFACE_INSTANTATE(imesh_imls_surface::integral_data,vgl_point_3d<double>);
