// This is core/vgl/vgl_pointset_3d.h
#ifndef vgl_pointset_3d_h_
#define vgl_pointset_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
// :
// \file
// \brief A 3-d pointset. points can have normals to represent sampled surface patches
// \author  J.L. Mundy
//
// \verbatim
// Modifications
// Initial version August 29,  2015
// \endverbatim

#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
template <class Type>
class vgl_pointset_3d
{
  // : members
  bool                             has_normals_;
  vcl_vector<vgl_point_3d<Type> >  points_;
  vcl_vector<vgl_vector_3d<Type> > normals_;
public:
  // : Default constructor
  vgl_pointset_3d() : has_normals_(false) {}

  // : Construct from a list
  vgl_pointset_3d(vcl_vector<vgl_point_3d<Type> > const& points) : has_normals_(false), points_(points) {}
  vgl_pointset_3d(vcl_vector<vgl_point_3d<Type> > const& points, vcl_vector<vgl_vector_3d<Type> > const& normals) :
    has_normals_(true), points_(points), normals_(normals) {}

  // : incrementally grow points, duplicate points are allowed
  void add_point(vgl_point_3d<Type> const& p)
  {
    points_.push_back(p); has_normals_ = false;
  }

  // : incrementally grow points and normals duplicate pairs are allowed
  void add_point_with_normal(vgl_point_3d<Type> const& p, vgl_vector_3d<Type> const& normal)
  {
    points_.push_back(p); normals_.push_back(normal); has_normals_ = true;
  }

  // : accessors
  bool has_normals() const {return has_normals_; }
  unsigned npts() const {return static_cast<unsigned>(points_.size() ); }
  vgl_point_3d<Type> p(unsigned i) const {return points_[i]; }
  vgl_vector_3d<Type> n(unsigned i) const
  {if( has_normals_ ) {return normals_[i]; } return vgl_vector_3d<Type>(); }

  vcl_vector<vgl_point_3d<Type> > points() const {return points_; }
  vcl_vector<vgl_vector_3d<Type> > normals() const {return normals_; }

  void set_points(vcl_vector<vgl_point_3d<Type> > const& points)
  { points_ = points; has_normals_ = false; }

  void set_points_with_normals(vcl_vector<vgl_point_3d<Type> > const& points,
                               vcl_vector<vgl_vector_3d<Type> > const& normals)
  { points_ = points; normals_ = normals; has_normals_ = true; }

  bool set_point(unsigned i, vgl_point_3d<Type> const& p)
  {
    if( i >= static_cast<unsigned>(points_.size() ) ) {return false; }
    points_[i].set(p.x(), p.y(), p.z() ); return true;
  }

  bool set_normal(unsigned i, vgl_vector_3d<Type> const& n)
  {
    if( has_normals_ && i < static_cast<unsigned>(normals_.size() ) )
      {
      normals_[i].set(n.x(), n.y(), n.z() ); return true;
      }
    else {return false; }}

  // : Equality operator
  bool operator==(const vgl_pointset_3d<Type> & spl) const;

  bool operator!=(vgl_pointset_3d<Type> const& spl) const { return !operator==(spl); }

};

template <class Type>
bool vgl_pointset_3d<Type>::operator==(const vgl_pointset_3d<Type>& pointset) const
{
  unsigned n = pointset.npts();

  if( n != this->npts() )
    {
    return false;
    }
  vcl_vector<vgl_point_3d<Type> > pts = pointset.points();
  for( unsigned i = 0; i < n; ++i )
    {
    if( pts[i] != points_[i] )
      {
      return false;
      }
    }
  if( has_normals_ )
    {
    if( static_cast<unsigned>(normals_.size() ) != n )
      {
      return false;
      }
    vcl_vector<vgl_vector_3d<Type> > normals = pointset.normals();
    for( unsigned i = 0; i < n; ++i )
      {
      if( normals[i] != normals_[i] )
        {
        return false;
        }
      }
    }
  return true;
}

template <class Type>
vcl_ostream &  operator<<(vcl_ostream& ostr, vgl_pointset_3d<Type> const& ptset)
{
  if( !ostr )
    {
    vcl_cout << "Bad ostream in write vgl_pointset_3d to stream\n";
    return ostr;
    }
  vcl_vector<vgl_point_3d<Type> > pts = ptset.points();
  if( !ptset.has_normals() )
    {
    for( unsigned i = 0; i < static_cast<unsigned>(pts.size() ); i++ )
      {
      const vgl_point_3d<Type>& p = pts[i];
      ostr << p.x() << ',' << p.y() << ',' << p.z() << '\n';
      }
    }
  else
    {
    vcl_vector<vgl_vector_3d<Type> > normals = ptset.normals();
    for( unsigned i = 0; i < static_cast<unsigned>(pts.size() ); i++ )
      {
      const vgl_point_3d<Type>&  p = pts[i];
      const vgl_vector_3d<Type>& n = normals[i];
      ostr << p.x() << ',' << p.y() << ',' << p.z() << ',' << n.x() << ',' << n.y() << ',' << n.z() << '\n';
      }
    }
  return ostr;
}

template <class Type>
vcl_istream &  operator>>(vcl_istream& istr, vgl_pointset_3d<Type>& ptset)
{
  if( !istr )
    {
    vcl_cout << "Bad istream in read vgl_pointset_3d from stream\n";
    return istr;
    }
  vcl_vector<vgl_point_3d<Type> >  pts;
  vcl_vector<vgl_vector_3d<Type> > normals;
  char                             buf[100];
  bool                             has_normals = false;
  // determine if file has three comma-separated double values or six
  bool first_line = true;
  while( istr.getline(buf, 100) )
    {
    vcl_string buf_str;
    if( first_line )
      {
      bool     done = false;
      unsigned comma_count = 0;
      for( unsigned i = 0; i < 100 && !done; ++i )
        {
        char c = buf[i];
        if( c == '\0' || c == '\n' )
          {
          done = true;
          continue;
          }
        else
          {
          buf_str.push_back(c);
          if( c == ',' )
            {
            comma_count++;
            }
          }
        }
      // three values have two separating commas
      has_normals = comma_count > 2;
      first_line = false;
      }
    else
      {
      bool done = false;
      for( unsigned i = 0; i < 100 && !done; ++i )
        {
        char c = buf[i];
        if( c == '\0' || c == '\n' )
          {
          done = true;
          continue;
          }
        buf_str.push_back(c);
        }
      }
    vcl_stringstream isstr(buf_str);
    Type             x, y, z, nx, ny, nz;
    unsigned char    c;
    isstr >> x >> c;
    if( c != ',' )
      {
      vcl_cout << "stream parse error\n";
      return istr;
      }
    isstr >> y >> c;
    if( c != ',' )
      {
      vcl_cout << "stream parse error\n";
      return istr;
      }
    if( !has_normals )
      {
      isstr >> z;
      }
    else
      {
      isstr >> z >> c;
      if( c != ',' )
        {
        vcl_cout << "stream parse error\n";
        return istr;
        }
      }
    if( has_normals )
      {
      isstr >> nx >> c;
      if( c != ',' )
        {
        vcl_cout << "stream parse error\n";
        return istr;
        }
      isstr >> ny >> c;
      if( c != ',' )
        {
        vcl_cout << "stream parse error\n";
        return istr;
        }
      isstr >> nz;
      }
    vgl_point_3d<Type> p(x, y, z);
    pts.push_back(p);
    if( has_normals )
      {
      vgl_vector_3d<Type> n(nx, ny, nz);
      normals.push_back(n);
      }
    }

  if( has_normals )
    {
    ptset = vgl_pointset_3d<Type>(pts, normals);
    return istr;
    }
  ptset = vgl_pointset_3d<Type>(pts);
  return istr;
};

#endif // vgl_pointset_3d_h_
