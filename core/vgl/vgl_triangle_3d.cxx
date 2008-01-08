#include "vgl_triangle_3d.h"

#include <vcl_limits.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_closest_point.h>


//! \file
//  \brief Some helpful functions when working with triangles
//  \author Kieran O'Mahony
//  \date 21/6/2007

// Define a file-scope vgl_nan constant
static const double nan = vcl_sqrt(-1.0);
static const double sqrteps = vcl_sqrt(vcl_numeric_limits<double>::epsilon());
static const double pi = 3.14159265358979323846;

//=======================================================================
//! Check for coincident edges of triangles a and b
//  \return a vector of the coincident edges
vcl_vector<vcl_pair<unsigned,unsigned> > vgl_triangle_3d_coincident_edges(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2, 
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2, 
  const vgl_point_3d<double>& b_p3)
{
  vcl_vector<vcl_pair<unsigned,unsigned> > coinc_edges;

  //create some convenient arrays for looping 
  vgl_point_3d<double> a[3] = {a_p1, a_p2, a_p3};
  vgl_point_3d<double> b[3] = {b_p1, b_p2, b_p3};
  vcl_pair<unsigned,unsigned> e[3] = { vcl_make_pair(0,1), 
    vcl_make_pair(1,2), 
    vcl_make_pair(2,0) };

  // Test each edge j of triangle a against each edge i of triangle b.
  for(unsigned j = 0; j < 3; ++j)
  {
    for(unsigned i = 0; i < 3; ++i)
    {
      //check if one edge is entirely contained within the other and vice versa

      double e1_len = length(a[e[j].first] - a[e[j].second]);
      double b1_dist = length(a[e[j].first] - b[e[i].first]) +
        length(a[e[j].second] - b[e[i].first]);
      double b2_dist = length(a[e[j].first] - b[e[i].second]) +
        length(a[e[j].second] - b[e[i].second]);

      double e2_len = length(b[e[i].first] - b[e[i].second]);
      double a1_dist = length(b[e[i].first] - a[e[j].first]) +
        length(b[e[i].second] - a[e[j].first]);
      double a2_dist = length(b[e[i].first] - a[e[j].second]) +
        length(b[e[i].second] - a[e[j].second]);

      if((vcl_fabs(e1_len - b1_dist) < sqrteps &&
        vcl_fabs(e1_len - b2_dist) < sqrteps) ||
        (vcl_fabs(e2_len - a1_dist) < sqrteps &&
        vcl_fabs(e2_len - a2_dist) < sqrteps)) 
      {
        coinc_edges.push_back(vcl_make_pair(j,i));
        break;
      }
    }
  }

  return coinc_edges;
}

//=======================================================================
//! Check if the given point is inside the triangle 
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
bool vgl_triangle_3d_test_inside(const vgl_point_3d<double>& i_pnt, 
                                 const vgl_point_3d<double>& p1,
                                 const vgl_point_3d<double>& p2, 
                                 const vgl_point_3d<double>& p3 ) 
{
  //firstly perform some degeracy checks 
  if(collinear(p1,p2,p3))
  { //the triangle is degenerate - its vertices are collinear
    
    if(p1==p2&&p2==p3&&p1==p3) 
    { //all its vertices are the same
      return i_pnt == p1;
    }
    
    vgl_line_segment_3d<double> test_line(p1,p2);
    if(test_line.contains(i_pnt))
      return true;
    
    test_line.set(p2,p3);
    if(test_line.contains(i_pnt))
      return true;
    
    test_line.set(p1,p3);
    if(test_line.contains(i_pnt))
      return true;
    
    return false;
  }

  // use badouel's algorithm ( a barycentric method)
  // based on the code & paper found at http://jgt.akpeters.com/papers/MollerTrumbore97/

  // Project to 2d plane, to avoid a degenerate result get 
  // the plane normal and identify the largest (abs) x,y or z component
  vgl_plane_3d<double> plane(p1,p2,p3);
  
  //the point needs to be in the triangles plane
  if(vcl_fabs(vgl_distance(plane,i_pnt)) > sqrteps)
    return false;
  
  vgl_vector_3d<double> norm = plane.normal();
  norm.set(vcl_fabs(norm.x()),vcl_fabs(norm.y()),vcl_fabs(norm.z()));

  unsigned i1 = 0; // Default is z.
  unsigned i2 = 1;
  if (norm.y()>=norm.x() && norm.y()>=norm.z()) 
  {
    i2 = 2; // Max component is y
  }
  else if (norm.x()>=norm.y() && norm.x()>=norm.z()) 
  {
    i1 = 2; // Max component is x
  }

  double point[3] = {i_pnt.x(), i_pnt.y(), i_pnt.z()};
  double vert0[3] = {p1.x(), p1.y(), p1.z()};
  double vert1[3] = {p2.x(), p2.y(), p2.z()};
  double vert2[3] = {p3.x(), p3.y(), p3.z()};

  double beta = 0.0;
  double alpha = 0.0;

  //compute the barycentric vectors....& ignore numerical roundoff errors
  double u0 = (fabs(point[i1]) < sqrteps ? 0 : point[i1]) - (fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double v0 = (fabs(point[i2]) < sqrteps ? 0 : point[i2]) - (fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);

  double u1 = (fabs(vert1[i1]) < sqrteps ? 0 : vert1[i1]) - (fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double u2 = (fabs(vert2[i1]) < sqrteps ? 0 : vert2[i1]) - (fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double v1 = (fabs(vert1[i2]) < sqrteps ? 0 : vert1[i2]) - (fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);
  double v2 = (fabs(vert2[i2]) < sqrteps ? 0 : vert2[i2]) - (fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);

  // calculate and compare barycentric coordinates 
  if (u1 == 0) 
  {    // uncommon case 
    beta = u0 / u2;
    if (beta < -sqrteps/*0*/ || beta > 1+sqrteps)
      return false;
    alpha = (v0 - beta * v2) / v1;
  }
  else 
  {      // common case 
    beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
    if (beta < -sqrteps/*0*/ || beta > 1+sqrteps)
      return false;
    alpha = (u0 - beta * u2) / u1;
  }

  if (alpha < -sqrteps/*0*/ || (alpha + beta) > 1.0+sqrteps)
    return false;

  return true;
}

//=======================================================================
//! Check if point \a i_pnt is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
//
//  \note this method uses the less efficient 'angles' method which requires 3 calls to acos()
bool vgl_triangle_3d_test_inside_simple(const vgl_point_3d<double>& i_pnt, 
                                        const vgl_point_3d<double>& p1,
                                        const vgl_point_3d<double>& p2, 
                                        const vgl_point_3d<double>& p3 ) 
{

  vgl_vector_3d<double> vec1 = normalized(i_pnt - p1);
  vgl_vector_3d<double> vec2 = normalized(i_pnt - p2);
  vgl_vector_3d<double> vec3 = normalized(i_pnt - p3);

  double int_ang = vcl_acos(dot_product(vec1,vec2)) + vcl_acos(dot_product(vec2,vec3)) + vcl_acos(dot_product(vec3,vec1));
  double test_val = vcl_fabs(int_ang-(2*pi));

  return test_val < sqrteps;
}

//=======================================================================
//! Compute the intersection point between the line segment and triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return intersection type
vgl_triangle_3d_intersection_t vgl_triangle_3d_line_intersection(
  const vgl_line_segment_3d<double>& line, 
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2, 
  const vgl_point_3d<double>& p3,
  vgl_point_3d<double>& i_pnt,
  bool ignore_coplanar /*= false*/) 
{
  vgl_point_3d<double> line_p1 = line.point1();
  vgl_point_3d<double> line_p2 = line.point2();
  
  // perform some degeneracy checks on the line and triangle
  if(line_p1 == line_p2) 
  { //the line is degnerate - it has zero length
    if(!ignore_coplanar && vgl_triangle_3d_test_inside(line_p1,p1,p2,p3))
      return Coplanar;
    
    return None;
  }

  // Check that the line isn't actually an edge of the triangle
  if ((line_p1==p1 && line_p2==p2) || (line_p1==p2 && line_p2==p1) ||
      (line_p1==p2 && line_p2==p3) || (line_p1==p3 && line_p2==p2) ||
      (line_p1==p3 && line_p2==p1) || (line_p1==p1 && line_p2==p3))
  {
    return None;
  }

  if(collinear(p1,p2,p3))
  { //the triangle is degenerate - it's points are collinear 
    if(p1==p2&&p2==p3&&p1==p3) 
    { //all its vertices are the same
      return !ignore_coplanar && line.contains(p1) ? Coplanar : None;
    }
    
    vgl_line_3d_2_points<double> i_line(line_p1,line_p2);
    if( !ignore_coplanar && (
        ( p1!=p2 && concurrent(vgl_line_3d_2_points<double>(p1,p2), i_line) && vgl_intersection(line,vgl_line_segment_3d<double>(p1,p2),i_pnt) ) || 
        ( p2!=p3 && concurrent(vgl_line_3d_2_points<double>(p2,p3), i_line) && vgl_intersection(line,vgl_line_segment_3d<double>(p2,p3),i_pnt) ) || 
        ( p1!=p3 && concurrent(vgl_line_3d_2_points<double>(p1,p3), i_line) && vgl_intersection(line,vgl_line_segment_3d<double>(p1,p3),i_pnt) ) ) )
    {
      return Coplanar;
    }
    
    return None;
  }

  //get triangle plane
  //vgl_plane_3d<double> plane(normalized(cross_product(p2-p1,p3-p1)),p1);
  vgl_plane_3d<double> plane(p1,p2,p3);
  
  //check for parallel/coplanarity
  vgl_vector_3d<double> dir = line_p2-line_p1;
  double denom = plane.a()*(dir.x()) +
                 plane.b()*(dir.y()) +
                 plane.c()*(dir.z());
  if(vcl_fabs(denom) < sqrteps) 
  {
    if(!ignore_coplanar && vgl_distance(line_p1, plane) < sqrteps ) 
    { //coplanar line - uncommon case
      
      //check each triangle edge.
      //behaviour is to return the first found intersetion point
      vgl_line_3d_2_points<double> i_line(line_p1,line_p2);
      vgl_line_segment_3d<double> edge1(p1,p2);
      
      vgl_point_3d<double> test_pt;
      if(concurrent(vgl_line_3d_2_points<double>(p1,p2),i_line) &&
         vgl_intersection(edge1,line,test_pt))
      {
        i_pnt = test_pt;
        return Coplanar;
      }
      vgl_line_segment_3d<double> edge2(p1,p3);
      if(concurrent(vgl_line_3d_2_points<double>(p1,p3),i_line) && 
         vgl_intersection(edge2,line,test_pt))
      {
        i_pnt = test_pt;
        return Coplanar;
      }
      vgl_line_segment_3d<double> edge3(p2,p3);
      if(concurrent(vgl_line_3d_2_points<double>(p2,p3),i_line) &&
         vgl_intersection(edge3,line,test_pt))
      {
        i_pnt = test_pt;
        return Coplanar;
      }
            
      //special case of line completely contained within the triangle
      if(vgl_triangle_3d_test_inside(line_p1, p1, p2, p3))
      {
        i_pnt.set(nan, nan, nan);
        return Coplanar;
      }
      
      return None;
    }

    //coplanar ignored OR
    //parallel line - no intersection
    return None;
  }
  
  //find point of intersection of line with triangle plane
  double numer = -(plane.a()*line_p1.x() +
                   plane.b()*line_p1.y() +
                   plane.c()*line_p1.z() +
                   plane.d());
  dir *= numer/denom;
  i_pnt = line_p1 + dir;

  //check if intersection point is actually on the line segment
  //and point inside triangle
  if(line.contains(i_pnt) && vgl_triangle_3d_test_inside(i_pnt, p1, p2, p3))
    return Skew;

  return None;
}

//=======================================================================
//! compute the intersection line of the given triangles 
//  \see vgl_triangle_3d_triangle_intersection()
//  \note an intesection line is not computed for a coplanar intersection
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2, 
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2, 
  const vgl_point_3d<double>& b_p3,
  vgl_line_segment_3d<double>& i_line) 
{  
  // triangle intersection algorithm based on code & paper
  // found at http://jgt.akpeters.com/papers/Moller97/

  //sanity check for degenerate triangles
  if(collinear(a_p1,a_p2,a_p3)) 
  {
    if(a_p1 == a_p2 && a_p2==a_p3 && a_p1 == a_p3)
    {
      if(vgl_triangle_3d_test_inside(a_p1,b_p1,b_p2,b_p3))
      {
        i_line.set(a_p1,a_p1);
        return Coplanar;
      }
      return None;
    }
    
    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if( ( a_p1 != a_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
        ( a_p2 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
        ( a_p1 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) )
    {
      i_line.set(i_pnt,i_pnt);
      return ret;      
    }
    
    return None;
  }
  if(collinear(b_p1,b_p2,b_p3)) 
  {
    if(b_p1 == b_p2 && b_p2==b_p3 && b_p1 == b_p3)
    {
      if(vgl_triangle_3d_test_inside(b_p1,a_p1,a_p2,a_p3))
      {
        i_line.set(b_p1,b_p1);
        return Coplanar;
      }
      return None;
    }
    
    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if( ( b_p1 != b_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p2), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
        ( b_p2 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p2,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
        ( b_p1 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) )
    {
      i_line.set(i_pnt,i_pnt);
      return ret;      
    }
    
    return None;
  }
  //computing intersection of triangles a and b

  vgl_vector_3d<double> edge1,edge2;
  vgl_vector_3d<double> a_norm, b_norm, int_line;
  //vgl_vector_3d<double> int_line;
  double a_d, b_d; 
  double d_b[3], d_a[3];
  double isect1[2], isect2[2];
  double d_b1d_b2, d_b1d_b3, d_a1d_a2, d_a1d_a3;

  double p_a[3];
  double p_b[3];
  bool coplanar = false;
  
  double TRI_TRI_EPS = 1000000*vcl_numeric_limits<double>::epsilon();

  //Firstly check if each triangle intersects
  // the plane of the other

  //construct plane equation of triangle a
  edge1 = a_p2 - a_p1;
  edge2 = a_p3 - a_p1;

  a_norm = normalized(cross_product(edge1, edge2));  
  a_d = -( a_norm.x()*a_p1.x() + a_norm.y()*a_p1.y() +a_norm.z()*a_p1.z() );
  //vgl_plane_3d<double> a_plane(a_p1,a_p2,a_p3);

  // get signed distance of triangle b to triangle a's plane
  d_b[0] = ( a_norm.x()*b_p1.x() + a_norm.y()*b_p1.y() + a_norm.z()*b_p1.z() ) + a_d;
  d_b[1] = ( a_norm.x()*b_p2.x() + a_norm.y()*b_p2.y() + a_norm.z()*b_p2.z() ) + a_d;
  d_b[2] = ( a_norm.x()*b_p3.x() + a_norm.y()*b_p3.y() + a_norm.z()*b_p3.z() ) + a_d;
  /*d_b[0] = (a_plane.nx()*b_p1.x() + a_plane.ny()*b_p1.y() + a_plane.nz()*b_p1.z() ) + a_plane.d();
  d_b[1] = (a_plane.nx()*b_p2.x() + a_plane.ny()*b_p2.y() + a_plane.nz()*b_p2.z() ) + a_plane.d();
  d_b[2] = (a_plane.nx()*b_p3.x() + a_plane.ny()*b_p3.y() + a_plane.nz()*b_p3.z() ) + a_plane.d();*/

  // coplanarity robustness check 
  if(vcl_fabs(d_b[0]) < TRI_TRI_EPS) d_b[0] = 0.0;
  if(vcl_fabs(d_b[1]) < TRI_TRI_EPS) d_b[1] = 0.0;
  if(vcl_fabs(d_b[2]) < TRI_TRI_EPS) d_b[2] = 0.0;

  d_b1d_b2 = d_b[0]*d_b[1];
  d_b1d_b3 = d_b[0]*d_b[2];

  // all distances same sign => no intersection
  if(d_b1d_b2 > 0 && d_b1d_b3 > 0) 
  {
    return None;                   
  }

  //construct plane equation of triangle b
  edge1 = b_p2 - b_p1;
  edge2 = b_p3 - b_p1;

  b_norm = normalized(cross_product(edge1,edge2));  
  b_d = -( b_norm.x()*b_p1.x() + b_norm.y()*b_p1.y() + b_norm.z()*b_p1.z() );
  //vgl_plane_3d<double> b_plane(b_p1,b_p2,b_p3);

  // get signed distance of triangle a to triangle b's plane
  d_a[0] = ( b_norm.x()*a_p1.x() + b_norm.y()*a_p1.y() + b_norm.z()*a_p1.z() ) + b_d;
  d_a[1] = ( b_norm.x()*a_p2.x() + b_norm.y()*a_p2.y() + b_norm.z()*a_p2.z() ) + b_d;
  d_a[2] = ( b_norm.x()*a_p3.x() + b_norm.y()*a_p3.y() + b_norm.z()*a_p3.z() ) + b_d;
  /*d_a[0] = (b_plane.nx()*a_p1.x() + b_plane.ny()*a_p1.y() + b_plane.nz()*a_p1.z() ) + b_plane.d();
  d_a[1] = (b_plane.nx()*a_p2.x() + b_plane.ny()*a_p2.y() + b_plane.nz()*a_p2.z() ) + b_plane.d();
  d_a[2] = (b_plane.nx()*a_p3.x() + b_plane.ny()*a_p3.y() + b_plane.nz()*a_p3.z() ) + b_plane.d();*/

  // coplanarity robustness check 
  if(vcl_fabs(d_a[0]) < TRI_TRI_EPS) d_a[0] = 0.0;
  if(vcl_fabs(d_a[1]) < TRI_TRI_EPS) d_a[1] = 0.0;
  if(vcl_fabs(d_a[2]) < TRI_TRI_EPS) d_a[2] = 0.0;

  d_a1d_a2 = d_a[0]*d_a[1];
  d_a1d_a3 = d_a[0]*d_a[2];

  // all distances same sign => no intersection
  if(d_a1d_a2 > 0 && d_a1d_a3 > 0) 
  {
    return None;                   
  }

  // Now know triangles contain
  // the line of their planes intersection.
  // So...compute each triangles interval of the intersection
  // line and determine if they overlap i.e. if the triangles intersect

  // compute direction of intersection line 
  int_line = cross_product(a_norm,b_norm);
  //int_line = cross_product(a_plane.normal(),b_plane.normal());

  // largest component of int_line?
  // to get a simplified 2D projection 
  int_line.set(vcl_fabs(int_line.x()),vcl_fabs(int_line.y()),vcl_fabs(int_line.z()));

  if (int_line.y()>=int_line.x() && int_line.y()>=int_line.z()) 
  { // Max component is y
    p_a[0] = a_p1.y();
    p_a[1] = a_p2.y();
    p_a[2] = a_p3.y();

    p_b[0] = b_p1.y();
    p_b[1] = b_p2.y();
    p_b[2] = b_p3.y();
  }
  else if (int_line.x()>=int_line.y() && int_line.x()>=int_line.z()) 
  { // Max component is x
    p_a[0] = a_p1.x();
    p_a[1] = a_p2.x();
    p_a[2] = a_p3.x();

    p_b[0] = b_p1.x();
    p_b[1] = b_p2.x();
    p_b[2] = b_p3.x();
  }
  else { // Max component is z
    p_a[0] = a_p1.z();
    p_a[1] = a_p2.z();
    p_a[2] = a_p3.z();

    p_b[0] = b_p1.z();
    p_b[1] = b_p2.z();
    p_b[2] = b_p3.z();
  }

  int a_ival[3] = {0,1,2};
  // compute interval for triangle a
  if(d_a1d_a2 > 0) //a1, a2 on same side of b's plane, a3 on the other side
  { 
    a_ival[0] = 2;
    a_ival[1] = 0;
    a_ival[2] = 1;
  } 
  else if(d_a1d_a3 > 0) //a1, a3 on same side of b's plane, a2 on the other side
  { 
    a_ival[0] = 1;
    a_ival[1] = 0;
    a_ival[2] = 2;
  } 
  else if(d_a[1]*d_a[2] > 0 || d_a[0] != 0) 
  { 
    a_ival[0] = 0;
    a_ival[1] = 1;
    a_ival[2] = 2;
  } 
  else if(d_a[1] != 0) 
  { 
    a_ival[0] = 1;
    a_ival[1] = 0;
    a_ival[2] = 2;
  } 
  else if(d_a[2] != 0) 
  { 
    a_ival[0] = 2;
    a_ival[1] = 0;
    a_ival[2] = 1;
  } 
  else 
  { 
    // triangles are coplanar 
    coplanar = true;
  } 

  int b_ival[3] = {0,1,2};
  if(!coplanar) 
  {

    // compute interval for triangle b
    if(d_b1d_b2 > 0) //b1, b2 on same side of a's plane, b3 on the other side
    { 
      b_ival[0] = 2;
      b_ival[1] = 0;
      b_ival[2] = 1;
    } 
    else if(d_b1d_b3 > 0) //b1, b3 on same side of a's plane, b2 on the other side
    { 
      b_ival[0] = 1;
      b_ival[1] = 0;
      b_ival[2] = 2;
    } 
    else if(d_b[1]*d_b[2] > 0 || d_b[0] != 0) 
    { 
      b_ival[0] = 0;
      b_ival[1] = 1;
      b_ival[2] = 2;
    } 
    else if(d_b[1] != 0) 
    { 
      b_ival[0] = 1;
      b_ival[1] = 0;
      b_ival[2] = 2;
    } 
    else if(d_b[2] != 0) 
    { 
      b_ival[0] = 2;
      b_ival[1] = 0;
      b_ival[2] = 1;
    } 
    else 
    {
      coplanar = true;
    }
  }

  //special case when triangles are coplanar
  if(coplanar) 
  {    
    //check if they intersect in their common plane
    vgl_point_3d<double> i_pnt1, i_pnt2, i_pnt3;
    bool isect1 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p1,a_p2), b_p1, b_p2, b_p3, i_pnt1) != None;
    bool isect2 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p2,a_p3), b_p1, b_p2, b_p3, i_pnt2) != None;
    bool isect3 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p3,a_p1), b_p1, b_p2, b_p3, i_pnt3) != None;

    if( isect1 || isect2 || isect3 ) 
    {
      i_line.set( isect1 ? i_pnt1 : isect2 ? i_pnt2 : i_pnt3, 
                  isect1 && isect2 ? i_pnt2 : (isect1 || isect2) && isect3 ? i_pnt3 : isect1 ? i_pnt1 : isect2 ? i_pnt2 : i_pnt3 );
      return Coplanar;
    }

    // finally, test if triangle a is totally contained in triangle b or vice versa 
    if(vgl_triangle_3d_test_inside(a_p1, b_p1, b_p2, b_p3) ||
      vgl_triangle_3d_test_inside(b_p1, a_p1, a_p2, a_p3)) 
    {
      i_line.set(vgl_point_3d<double> (nan, nan, nan),vgl_point_3d<double> (nan,nan,nan));
      return Coplanar;
    }

    return None;
  } 

  vgl_point_3d<double> i_pnts[4];
  //intersection line interval for triangle a
  double tmp = d_a[a_ival[0]]/(d_a[a_ival[0]]-d_a[a_ival[1]]);          
  isect1[0] = p_a[a_ival[0]] + (p_a[a_ival[1]] - p_a[a_ival[0]])*tmp;         
  vgl_point_3d<double> a_vs[] = {a_p1,a_p2,a_p3};
  vgl_vector_3d<double> diff = a_vs[a_ival[1]] - a_vs[a_ival[0]];
  diff *= tmp;
  i_pnts[0] = a_vs[a_ival[0]] + diff  ;
  
  tmp = d_a[a_ival[0]]/(d_a[a_ival[0]]-d_a[a_ival[2]]);  
  isect1[1] = p_a[a_ival[0]] + (p_a[a_ival[2]] - p_a[a_ival[0]])*tmp;   
  diff = a_vs[a_ival[2]] - a_vs[a_ival[0]];   
  diff *= tmp;                  
  i_pnts[1] = a_vs[a_ival[0]] + diff;

  //intersection line interval for triangle b
  tmp = d_b[b_ival[0]]/(d_b[b_ival[0]] - d_b[b_ival[1]]);          
  isect2[0] = p_b[b_ival[0]] + (p_b[b_ival[1]] - p_b[b_ival[0]])*tmp;         
  vgl_point_3d<double> b_vs[] = {b_p1,b_p2,b_p3};
  diff = b_vs[b_ival[1]] - b_vs[b_ival[0]];
  diff *= tmp;
  i_pnts[2] = b_vs[b_ival[0]] + diff ;
  
  tmp = d_b[b_ival[0]]/(d_b[b_ival[0]]-d_b[b_ival[2]]);  
  isect2[1] = p_b[b_ival[0]] + (p_b[b_ival[2]] - p_b[b_ival[0]])*tmp;   
  diff = b_vs[b_ival[2]] - b_vs[b_ival[0]];   
  diff *= tmp;                  
  i_pnts[3] = b_vs[b_ival[0]] + diff;

  unsigned smallest1 = 0;
  if(isect1[0] > isect1[1])       
  {               
    tmp = isect1[0];        
    isect1[0]= isect1[1];
    isect1[1] = tmp;        
    smallest1 = 1; 
  }
  unsigned smallest2 = 0;
  if(isect2[0] > isect2[1])       
  {               
    tmp = isect2[0];        
    isect2[0]= isect2[1];
    isect2[1] = tmp;        
    smallest2 = 1; 
  }

  if(isect1[1] < isect2[0] || isect2[1] < isect1[0]) 
  {
    return None; //no intersection
  }  


  unsigned i_pt1,i_pt2;
  //find the correct intersection line
  if(isect2[0]<isect1[0])
  {
    if(smallest1==0) 
    { 
      i_pt1 = 0;
    }
    else 
    { 
      i_pt1 = 1;
    }

    if(isect2[1]<isect1[1])
    {
      if(smallest2==0) 
      { 
        i_pt2 = 3;
      }
      else 
      {
        i_pt2 = 2;
      }
    }
    else
    {
      if(smallest1==0) 
      { 
        i_pt2 = 1;
      }
      else 
      { 
        i_pt2 = 0;
      }
    }
  }
  else
  {
    if(smallest2==0) 
    { 
      i_pt1 = 2;
    }
    else 
    { 
      i_pt1 = 3;
    }

    if(isect2[1]>isect1[1])
    {
      if(smallest1==0) 
      { 
        i_pt2 = 1;
      }
      else 
      { 
        i_pt2 = 0;
      }      
    }
    else
    {
      if(smallest2==0) 
      { 
        i_pt2 = 3;
      }
      else 
      { 
        i_pt2 = 2;
      } 
    }
  }

  i_line.set(i_pnts[i_pt1],i_pnts[i_pt2]);
  return Skew;

}

//=======================================================================
//! Compute if the given triangles a and b intersect
//  The triangle are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
//  \return intersection type
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2, 
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2, 
  const vgl_point_3d<double>& b_p3) 
{
  // triangle intersection algorithm based on code & paper
  // found at http://jgt.akpeters.com/papers/Moller97/
  
  //sanity check for degenerate triangles
  if(collinear(a_p1,a_p2,a_p3)) 
  {
    if(a_p1 == a_p2 && a_p2==a_p3 && a_p1 == a_p3)
    {
      if(vgl_triangle_3d_test_inside(a_p1,b_p1,b_p2,b_p3))
        return Coplanar;
      return None;
    }
    
    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if( ( a_p1 != a_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
        ( a_p2 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
        ( a_p1 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) )
      return ret;
    
    return None;
  }
  if(collinear(b_p1,b_p2,b_p3)) 
  {
    if(b_p1 == b_p2 && b_p2==b_p3 && b_p1 == b_p3)
    {
      if(vgl_triangle_3d_test_inside(b_p1,a_p1,a_p2,a_p3))
        return Coplanar;
      return None;
    }
    
    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if( ( b_p1 != b_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p2), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
        ( b_p2 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p2,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
        ( b_p1 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) )
      return ret;
    
    return None;
  }

  //computing intersection of triangles a and b

  vgl_vector_3d<double> edge1,edge2;
  vgl_vector_3d<double> a_norm, b_norm, int_line;
//  vgl_vector_3d<double> int_line;

  double a_d, b_d; 
  double d_b1, d_b2, d_b3, d_a1, d_a2, d_a3;
  double isect1[2], isect2[2];
  double d_b1d_b2, d_b1d_b3, d_a1d_a2, d_a1d_a3;

  double p_a1, p_a2, p_a3;
  double p_b1, p_b2, p_b3;
  bool coplanar = false;

  double a, b, c, x0, x1;
  double d, e, f, y0, y1;
  double xx, yy, xxyy, tmp;
  double TRI_TRI_EPS = 100000*vcl_numeric_limits<double>::epsilon();

  //Firstly check if each triangle intersects
  // the plane of the other

  //construct plane equation of triangle a
  edge1 = a_p2 - a_p1;
  edge2 = a_p3 - a_p1;

  a_norm = normalized(cross_product(edge1, edge2));
    
  a_d = -( a_norm.x()*a_p1.x() + a_norm.y()*a_p1.y() +a_norm.z()*a_p1.z() ); 
  //vgl_plane_3d<double> a_plane(a_p1,a_p2,a_p3);

  // get signed distance of triangle b to triangle a's plane
  d_b1 = ( a_norm.x()*b_p1.x() + a_norm.y()*b_p1.y() + a_norm.z()*b_p1.z() ) + a_d;
  d_b2 = ( a_norm.x()*b_p2.x() + a_norm.y()*b_p2.y() + a_norm.z()*b_p2.z() ) + a_d;
  d_b3 = ( a_norm.x()*b_p3.x() + a_norm.y()*b_p3.y() + a_norm.z()*b_p3.z() ) + a_d;
  /*d_b1 = (a_plane.nx()*b_p1.x() + a_plane.ny()*b_p1.y() + a_plane.nz()*b_p1.z() ) + a_plane.d();
  d_b2 = (a_plane.nx()*b_p2.x() + a_plane.ny()*b_p2.y() + a_plane.nz()*b_p2.z() ) + a_plane.d();
  d_b3 = (a_plane.nx()*b_p3.x() + a_plane.ny()*b_p3.y() + a_plane.nz()*b_p3.z() ) + a_plane.d();*/

  // coplanarity robustness check 
  if(vcl_fabs(d_b1) < TRI_TRI_EPS) d_b1 = 0.0;
  if(vcl_fabs(d_b2) < TRI_TRI_EPS) d_b2 = 0.0;
  if(vcl_fabs(d_b3) < TRI_TRI_EPS) d_b3 = 0.0;

  d_b1d_b2 = d_b1*d_b2;
  d_b1d_b3 = d_b1*d_b3;

  // all distances same sign => no intersection
  if(d_b1d_b2 > 0 && d_b1d_b3 > 0) 
  {
    return None;                   
  }

  //construct plane equation of triangle b
  edge1 = b_p2 - b_p1;
  edge2 = b_p3 - b_p1;

  b_norm = normalized(cross_product(edge1,edge2));   

  b_d = -( b_norm.x()*b_p1.x() + b_norm.y()*b_p1.y() + b_norm.z()*b_p1.z() );
  //vgl_plane_3d<double> b_plane(b_p1,b_p2,b_p3);

  // get signed distance of triangle a to triangle b's plane
  d_a1 = ( b_norm.x()*a_p1.x() + b_norm.y()*a_p1.y() + b_norm.z()*a_p1.z() ) + b_d;
  d_a2 = ( b_norm.x()*a_p2.x() + b_norm.y()*a_p2.y() + b_norm.z()*a_p2.z() ) + b_d;
  d_a3 = ( b_norm.x()*a_p3.x() + b_norm.y()*a_p3.y() + b_norm.z()*a_p3.z() ) + b_d;
  /*d_a1 = (b_plane.nx()*a_p1.x() + b_plane.ny()*a_p1.y() + b_plane.nz()*a_p1.z() ) + b_plane.d();
  d_a2 = (b_plane.nx()*a_p2.x() + b_plane.ny()*a_p2.y() + b_plane.nz()*a_p2.z() ) + b_plane.d();
  d_a3 = (b_plane.nx()*a_p3.x() + b_plane.ny()*a_p3.y() + b_plane.nz()*a_p3.z() ) + b_plane.d();*/

  // coplanarity robustness check 
  if(vcl_fabs(d_a1) < TRI_TRI_EPS) d_a1 = 0.0;
  if(vcl_fabs(d_a2) < TRI_TRI_EPS) d_a2 = 0.0;
  if(vcl_fabs(d_a3) < TRI_TRI_EPS) d_a3 = 0.0;

  d_a1d_a2 = d_a1*d_a2;
  d_a1d_a3 = d_a1*d_a3;

  // all distances same sign => no intersection
  if(d_a1d_a2 > 0 && d_a1d_a3 > 0) 
  {
    return None;
  }

  // Now know triangles contain
  // the line of their planes intersection.
  // So...compute each triangles interval of the intersection
  // line and determine if they overlap i.e. if the triangles intersect

  // compute direction of intersection line 
  int_line = cross_product(a_norm,b_norm);
  //int_line = cross_product(a_plane.normal(),b_plane.normal());

  // largest component of int_line?
  // to get a simplified 2D projection 
  int_line.set(vcl_fabs(int_line.x()),vcl_fabs(int_line.y()),vcl_fabs(int_line.z()));

  if (int_line.y()>=int_line.x() && int_line.y()>=int_line.z()) 
  { // Max component is y
    p_a1 = a_p1.y();
    p_a2 = a_p2.y();
    p_a3 = a_p3.y();

    p_b1 = b_p1.y();
    p_b2 = b_p2.y();
    p_b3 = b_p3.y();
  }
  else if (int_line.x()>=int_line.y() && int_line.x()>=int_line.z()) 
  { // Max component is x
    p_a1 = a_p1.x();
    p_a2 = a_p2.x();
    p_a3 = a_p3.x();

    p_b1 = b_p1.x();
    p_b2 = b_p2.x();
    p_b3 = b_p3.x();
  }
  else { // Max component is z
    p_a1 = a_p1.z();
    p_a2 = a_p2.z();
    p_a3 = a_p3.z();

    p_b1 = b_p1.z();
    p_b2 = b_p2.z();
    p_b3 = b_p3.z();
  }

  // compute interval for triangle a
  if(d_a1d_a2 > 0) //a1, a2 on same side of b's plane, a3 on the other side
  { 
    a = p_a3; 
    b = (p_a1-p_a3)*d_a3; 
    c = (p_a2-p_a3)*d_a3; 

    x0 = d_a3-d_a1; 
    x1 = d_a3-d_a2; 
  } 
  else if(d_a1d_a3 > 0) //a1, a3 on same side of b's plane, a2 on the other side
  { 
    a = p_a2; 
    b = (p_a1-p_a2)*d_a2; 
    c = (p_a3-p_a2)*d_a2; 

    x0 = d_a2-d_a1; 
    x1 = d_a2-d_a3; 
  } 
  else if(d_a2*d_a3 > 0 || d_a1 != 0) 
  { 
    a = p_a1; 
    b = (p_a2-p_a1)*d_a1; 
    c = (p_a3-p_a1)*d_a1; 

    x0 = d_a1-d_a2; 
    x1 = d_a1-d_a3; 
  } 
  else if(d_a2 != 0) 
  { 
    a = p_a2; 
    b = (p_a1-p_a2)*d_a2; 
    c = (p_a3-p_a2)*d_a2; 

    x0 = d_a2-d_a1; 
    x1 = d_a2-d_a3; 
  } 
  else if(d_a3 != 0) 
  { 
    a = p_a3; 
    b = (p_a1-p_a3)*d_a3; 
    c = (p_a2-p_a3)*d_a3; 

    x0 = d_a3-d_a1; 
    x1 = d_a3-d_a2;
  } 
  else 
  { 
    // triangles are coplanar 
    coplanar = true;
  } 

  if(!coplanar) 
  {

    // compute interval for triangle b
    if(d_b1d_b2 > 0) //b1, b2 on same side of a's plane, b3 on the other side
    { 
      d = p_b3; 
      e = (p_b1-p_b3)*d_b3; 
      f = (p_b2-p_b3)*d_b3; 

      y0 = d_b3-d_b1; 
      y1 = d_b3-d_b2; 
    } 
    else if(d_b1d_b3 > 0) //b1, b3 on same side of a's plane, b2 on the other side
    { 
      d = p_b2; 
      e=(p_b1-p_b2)*d_b2; 
      f=(p_b3-p_b2)*d_b2; 

      y0=d_b2-d_b1; 
      y1=d_b2-d_b3; 
    } 
    else if(d_b2*d_b3 > 0 || d_b1 != 0) 
    { 
      d = p_b1; 
      e = (p_b2-p_b1)*d_b1; 
      f = (p_b3-p_b1)*d_b1; 

      y0 = d_b1-d_b2; 
      y1 = d_b1-d_b3; 
    } 
    else if(d_b2 != 0) 
    { 
      d = p_b2; 
      e = (p_b1-p_b2)*d_b2; 
      f = (p_b3-p_b2)*d_b2; 

      y0 = d_b2-d_b1; 
      y1 = d_b2-d_b3; 
    } 
    else if(d_b3 != 0) 
    { 
      d = p_b3; 
      e = (p_b1-p_b3)*d_b3; 
      f = (p_b2-p_b3)*d_b3; 

      y0 = d_b3-d_b1; 
      y1 = d_b3-d_b2;
    } 
    else 
    {
      coplanar = true;
    }
  }

  //special case when triangles are coplanar
  if(coplanar) 
  {    

    //check if they intersect in their common plane
    vgl_point_3d<double> i_pnt;
    if(vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1, b_p2, b_p3, i_pnt) || 
      vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1, b_p2, b_p3, i_pnt) ||
      vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p3,a_p1), b_p1, b_p2, b_p3, i_pnt) ) 
    {
      return Coplanar;
    }

    // finally, test if triangle a is totally contained in triangle b or vice versa 
    if(vgl_triangle_3d_test_inside(a_p1, b_p1, b_p2, b_p3) ||
      vgl_triangle_3d_test_inside(b_p1, a_p1, a_p2, a_p3)) 
    {
      return Coplanar;
    }

    return None;
  } 

  // finally, test if the triangles respective intervals
  // of the intersection line overlap
  xx = x0*x1;
  yy = y0*y1;
  xxyy = xx*yy;

  tmp = a*xxyy;
  isect1[0] = tmp+b*x1*yy;
  isect1[1] = tmp+c*x0*yy;

  tmp = d*xxyy;
  isect2[0] = tmp+e*xx*y1;
  isect2[1] = tmp+f*xx*y0;

  if(isect1[0] > isect1[1])    
  {          
    tmp = isect1[0];     
    isect1[0] = isect1[1];     
    isect1[1] = tmp;     
  }

  if(isect2[0] > isect2[1])    
  {          
    tmp = isect2[0];     
    isect2[0] = isect2[1];     
    isect2[1] = tmp;     
  }

  if(isect1[1] < isect2[0] || isect2[1] < isect1[0]) 
  {
    return None;
  }  

  return Skew;
}

//=======================================================================
//: Compute the line of intersection of the given triangle and plane
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return intersection type
//  \note an intersection line is not defined (vgl_nan) for a coplanar intersection
vgl_triangle_3d_intersection_t vgl_triangle_3d_plane_intersection(
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2, 
  const vgl_point_3d<double>& p3,
  const vgl_plane_3d<double>& i_plane,
  vgl_line_segment_3d<double>& i_line)
{
  //Firstly check if the triangle actually intersects the plane
  // by computing the signed distance of its vertices to the plane  

  //all we care about is the sign of the distance
  double p1_d = i_plane.nx()*p1.x() + i_plane.ny()*p1.y() + i_plane.nz()*p1.z() + i_plane.d();
  double p2_d = i_plane.nx()*p2.x() + i_plane.ny()*p2.y() + i_plane.nz()*p2.z() + i_plane.d();
  double p3_d = i_plane.nx()*p3.x() + i_plane.ny()*p3.y() + i_plane.nz()*p3.z() + i_plane.d();

  // coplanarity robustness check 
  if(vcl_fabs(p1_d) < sqrteps) p1_d = 0.0;
  if(vcl_fabs(p2_d) < sqrteps) p2_d = 0.0;
  if(vcl_fabs(p3_d) < sqrteps) p3_d = 0.0;

  double p1_dp2_d = p1_d*p2_d;
  double p1_dp3_d = p1_d*p3_d;

  // all distances same sign => no intersection
  if(p1_dp2_d > 0 && p1_dp3_d > 0) 
    return None;                   

  vgl_point_3d<double> i_pnt1;
  vgl_point_3d<double> i_pnt2;
  vgl_line_3d_2_points<double> edge;
  if(p1_dp2_d > 0) //p1, p2 on same side, p3 on the other
  { 
    edge.set(p1,p3);
    i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p2,p3);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else if(p1_dp3_d > 0) //p1, p3 on same side, p2 on the other
  { 
    edge.set(p1,p2);
    i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p3,p2);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else if(p2_d*p3_d > 0) //p2, p3 on same side, p1 on the other
  { 
    edge.set(p2,p1);
    i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p3,p1);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else if(p1_d == 0 &&
    p2_d == 0) //edge p1,p2 in plane
  {  
    i_pnt1 = p1;
    i_pnt2 = p2;
  }
  else if(p1_d == 0 &&
    p3_d == 0) //edge p1,p3 in plane
  {  
    i_pnt1 = p1;
    i_pnt2 = p3;
  }
  else if(p3_d == 0 &&
    p2_d == 0) //edge p2,p3 in plane
  {  
    i_pnt1 = p2;
    i_pnt2 = p3;
  }
  else if(p1_d == 0) //just p1 in plane
  {  
    i_pnt1 = p1;
    edge.set(p3,p2);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else if(p2_d == 0) //just p2 in plane
  {  
    i_pnt1 = p2;
    edge.set(p3,p1);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else if(p3_d == 0) //just p3 in plane
  {  
    i_pnt1 = p3;
    edge.set(p2,p1);
    i_pnt2 = vgl_intersection(edge, i_plane);
  }
  else //triangle lies in plane 
  {
    i_pnt1.set(nan, nan, nan);
    i_line.set(i_pnt1,i_pnt1);
    return Coplanar; 
  }

  i_line.set(i_pnt1,i_pnt2);

  return Skew;
}


//=======================================================================
// Compute the closest point on a triangle to a reference point
//=======================================================================
vgl_point_3d<double> vgl_triangle_3d_closest_point(
  const vgl_point_3d<double>& q, 
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2, 
  const vgl_point_3d<double>& p3)
{
  // Handle degenerate case.
  if (p1 == p2)
  {
    if (p2 == p3) return p3;
    return vgl_closest_point(vgl_line_3d_2_points<double>(p3, p1), q);
  }
  if (p2 == p3)
    return vgl_closest_point(vgl_line_3d_2_points<double>(p1, p2), q);
  if (p3 == p1)
    return vgl_closest_point(vgl_line_3d_2_points<double>(p2, p3), q);



  // Construct a plane from the 3 vertices of the triangle
  vgl_plane_3d<double> plane(p1,  p2, p3);

  // Find the closest point on the whole plane to the test point 
  vgl_point_3d<double> cp = vgl_closest_point<double>(plane, q);

  // Is this closest point inside the triangle ?
  if (vgl_triangle_3d_test_inside(cp, p1, p2, p3))
  {
    return cp;
  }
  else
  {
    // Find the nearest point on the triangle's boundary by testing each edge

    // Edge 1
    double cp1x, cp1y, cp1z;
    vgl_closest_point_to_linesegment(
      cp1x, cp1y, cp1z, 
      p1.x(), p1.y(), p1.z(), 
      p2.x(), p2.y(), p2.z(), 
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp1(cp1x, cp1y, cp1z);
    double d1 = vgl_distance(cp1, q);

    // Edge 2
    double cp2x, cp2y, cp2z;
    vgl_closest_point_to_linesegment(
      cp2x, cp2y, cp2z, 
      p2.x(), p2.y(), p2.z(), 
      p3.x(), p3.y(), p3.z(), 
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp2(cp2x, cp2y, cp2z);
    double d2 = vgl_distance(cp2, q);

    // Edge 3
    double cp3x, cp3y, cp3z;
    vgl_closest_point_to_linesegment(
      cp3x, cp3y, cp3z, 
      p1.x(), p1.y(), p1.z(), 
      p3.x(), p3.y(), p3.z(), 
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp3(cp3x, cp3y, cp3z);
    double d3 = vgl_distance(cp3, q);

    // Identify nearest edge and return closest point on that edge.
    if (d1<=d2 && d1<=d3) 
      return cp1;
    else if (d2<=d1 && d2<=d3) 
      return cp2;
    else
      return cp3;
  }
}


//=======================================================================
// Compute the distance to the closest point on a triangle from a reference point.
//=======================================================================
double vgl_triangle_3d_distance(const vgl_point_3d<double>& q, 
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2, 
  const vgl_point_3d<double>& p3)
{
  vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
  return vgl_distance(c, q);
}

//=======================================================================
//! Check if the two triangles are coplanar
//  The triangles are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
bool vgl_triangle_3d_triangle_coplanar(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2, 
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2, 
  const vgl_point_3d<double>& b_p3)
{
  return coplanar(a_p1,b_p1,b_p2,b_p3) && coplanar(a_p2,b_p1,b_p2,b_p3) && coplanar(a_p3,b_p1,b_p2,b_p3);
}


