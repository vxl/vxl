#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <bvgl/bvgl_articulated_poly.h>

//empty constructor
bvgl_articulated_poly::bvgl_articulated_poly(const unsigned n_joints)
{
  joint_transforms_.resize(n_joints);
  //make the transforms identity with unit length joints
  for (unsigned i = 0; i<n_joints; ++i)
  {
    joint_transforms_[i].set_identity();
    if (i>0)
      joint_transforms_[i].set_translation(1.0, 0.0);
  }
  for (unsigned i = 0; i<n_joints; ++i)
  {
    vsol_point_2d_sptr p = this->joint_position(i);
    this->add_vertex(p);
  }
}

//link lengths are specified. For n joints there are n-1 links
bvgl_articulated_poly::
bvgl_articulated_poly(const unsigned n_joints,
                      vcl_vector<double> const& link_lengths)
{
  assert(n_joints==link_lengths.size()+1);

  joint_transforms_.resize(n_joints);
  //make the transforms identity with unit length joints
  for (unsigned i = 0; i<n_joints; ++i)
  {
    joint_transforms_[i].set_identity();
    if (i>0)
      joint_transforms_[i].set_translation(link_lengths[i-1], 0.0);
  }
  for (unsigned i = 0; i<n_joints; ++i)
  {
    vsol_point_2d_sptr p = this->joint_position(i);
    this->add_vertex(p);
  }
}

bvgl_articulated_poly::bvgl_articulated_poly(const bvgl_articulated_poly& poly)
{
  unsigned n = poly.size();
  joint_transforms_.resize(n);
  for (unsigned i = 0; i<n; ++i)
  {
    this->add_vertex(poly.joint_position(i));
    joint_transforms_[i]=poly.joint_transform(i);
  }
}

//Transform the joint position to world coordinates
vsol_point_2d_sptr
bvgl_articulated_poly::joint_position(const unsigned joint) const
{
  if (joint == 0)
    return new vsol_point_2d(0.0, 0.0);

  vgl_h_matrix_2d<double> T = joint_transforms_[0];
  for (unsigned i=1; i<=joint; ++i)
  {
    vgl_h_matrix_2d<double> Tp = joint_transforms_[i];
    T = T*Tp;
  }
  //The last joint has local coordinates (0,0) at the joint
  vgl_homg_point_2d<double> zero(0.0,0.0,1.0);
  vgl_homg_point_2d<double> homg_wp = T(zero);
  vgl_point_2d<double> wp(homg_wp);
  return new vsol_point_2d(wp.x(), wp.y());
}

void bvgl_articulated_poly::update()
{
  unsigned n = this->size();
  for (unsigned i = 0; i<n; ++i)
  {
    vsol_point_2d_sptr p = this->joint_position(i);
    (*storage_)[i]->set_x(p->x());
    (*storage_)[i]->set_y(p->y());
  }
}

void bvgl_articulated_poly::
transform(vcl_vector<double > const& delta_joint_angle)
{
  unsigned n = delta_joint_angle.size();
  assert(n==joint_transforms_.size());
  for (unsigned i = 0; i<n; ++i)
  {
    vgl_h_matrix_2d<double> r;
    r.set_identity();
    r.set_rotation(delta_joint_angle[i]);
    joint_transforms_[i]=joint_transforms_[i]*r;
  }
  this->update();
}

void bvgl_articulated_poly::
sub_manifold_transform(const double t,
                       vcl_vector<double > const& basis_angles)
{
  vcl_vector<double > angles;
  for (vcl_vector<double >::const_iterator ait = basis_angles.begin();
       ait != basis_angles.end(); ++ait)
    angles.push_back(t*(*ait));
  this->transform(angles);
  this->update();
}

//only works up to equiform
static double angle_from_rotation_matrix(vgl_h_matrix_2d<double> const& r)
{
  double c = r.get(0,0);
  double s = r.get(1,0);
  double ang = vcl_atan2(s,c);
  if (ang>vnl_math::pi)
    ang = 2*vnl_math::pi - ang;
  return ang;
}

double bvgl_articulated_poly::joint_angle(unsigned joint) const
{
  return angle_from_rotation_matrix(this->joint_transform(joint));
}

double bvgl_articulated_poly::link_length(unsigned joint) const
{
  unsigned n = this->size();
  if (joint>=n-1)
    return 0;
  vgl_h_matrix_2d<double> T = this->joint_transform(joint+1);
  double tx = T.get(0,2), ty = T.get(1,2);
  return vcl_sqrt(tx*tx + ty*ty);
}

//The earlier joints in the chain are weighted more since they appear in more
//backward chain matrices.
double bvgl_articulated_poly::
lie_distance(bvgl_articulated_poly const& apa,
             bvgl_articulated_poly const& apb)
{
  unsigned na = apa.size();
  unsigned nb = apb.size();
  assert(na==nb);
  double d = 0;//distance
  //note that there is no effect of the angle of the last joint
  //The weight is N-(joint+1)
  for (unsigned i =0; i<na-1; ++i)
  {
    vgl_h_matrix_2d<double> Ta = apa.joint_transform(i);
    vgl_h_matrix_2d<double> Tb = apb.joint_transform(i);
    double ra = angle_from_rotation_matrix(Ta);
    double rb = angle_from_rotation_matrix(Tb);
    d += (na-i-1)*(ra-rb)*(ra-rb);
  }
  return vcl_sqrt(d);
}

void bvgl_articulated_poly::print()
{
  for (unsigned i = 0; i<joint_transforms_.size(); ++i)
  {
    vsol_point_2d_sptr p = this->joint_position(i);
    vcl_cout << "Joint[" << i << "](" << p->x() << ' ' << p->y()
             << ")| " << this->joint_angle(i) << "|\n";
  }
}


void bvgl_articulated_poly::print_xforms()
{
  for (unsigned i = 0; i<joint_transforms_.size(); ++i)
    vcl_cout << "T[" << i << "]=>\n" <<  joint_transforms_[i] << '\n';
}

bvgl_articulated_poly_sptr bvgl_articulated_poly::
projection(bvgl_articulated_poly_sptr const& target,
           vcl_vector<double > const& manifold_basis)
{
  //copy the target
  unsigned n = target->size();
  vcl_vector<double> links(n-1);
  for (unsigned i = 0; i<n-1; ++i)
    links[i]=target->link_length(i);
  //search for the projection.
  bvgl_articulated_poly_sptr manifold = new bvgl_articulated_poly(n, links);
  double d = vnl_numeric_traits<double>::maxval, tmin=0;
  for (double t = - 3.0; t<=3.0; t+=0.05)
  {
    manifold->sub_manifold_transform(t, manifold_basis);
    double dt = bvgl_articulated_poly::lie_distance(*manifold, *target);
    if (dt<d)
    {
      d = dt;
      tmin = t;
    }
    //undo the transform
    manifold->sub_manifold_transform(-t, manifold_basis);
  }
  manifold->sub_manifold_transform(tmin, manifold_basis);
  return manifold;
}
