// This is core/vgl/vgl_pointset_3d.h
#ifndef vgl_pointset_3d_h_
#define vgl_pointset_3d_h_
//:
// \file
// \brief A 3-d pointset. points can have normals to represent sampled surface patches
// \author  J.L. Mundy
//
// \verbatim
// Modifications
// Initial version August 29,  2015
// \endverbatim

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
template <class Type>
class vgl_pointset_3d
{
  //: members
  bool has_normals_;
  bool has_scalars_;
  std::vector<vgl_point_3d<Type> > points_;
  std::vector<vgl_vector_3d<Type> > normals_;
  std::vector< Type > scalars_;
 public:
  //: Default constructor
 vgl_pointset_3d(): has_normals_(false), has_scalars_(false){}

  //: Construct from a list
 vgl_pointset_3d(std::vector<vgl_point_3d<Type> >  points): has_normals_(false), has_scalars_(false), points_(std::move(points)){}

 vgl_pointset_3d(std::vector<vgl_point_3d<Type> >  points, std::vector<vgl_vector_3d<Type> >  normals):
  has_normals_(true), has_scalars_(false), points_(std::move(points)), normals_(std::move(normals)){}

 vgl_pointset_3d(std::vector<vgl_point_3d<Type> >  points,  std::vector< Type >  scalars):
  has_normals_(false), has_scalars_(true), points_(std::move(points)), scalars_(std::move(scalars)){}

 vgl_pointset_3d(std::vector<vgl_point_3d<Type> >  points,
                 std::vector<vgl_vector_3d<Type> >  normals,
                 std::vector< Type >  scalars):
  has_normals_(true), has_scalars_(true), points_(std::move(points)), normals_(std::move(normals)), scalars_(std::move(scalars)){}

  //: incrementally grow points, duplicate points are allowed
  void add_point(vgl_point_3d<Type> const& p){
    points_.push_back(p); has_normals_=false; has_scalars_ = false;
  }
  //: incrementally grow points and normals duplicate pairs are allowed
  void add_point_with_normal(vgl_point_3d<Type> const& p, vgl_vector_3d<Type> const& normal){
    points_.push_back(p); normals_.push_back(normal); has_normals_ = true, has_scalars_ = false;
  }
  void add_point_with_scalar(vgl_point_3d<Type> const& p, Type sc){
    points_.push_back(p); scalars_.push_back(sc); has_scalars_ = true;
  }
  void add_point_with_normal_and_scalar(vgl_point_3d<Type> const& p, vgl_vector_3d<Type> const& normal, Type sc){
    points_.push_back(p); normals_.push_back(normal), scalars_.push_back(sc); has_normals_ = true; has_scalars_ = true;
  }
  //: accessors
  bool has_normals() const {return has_normals_;}
  bool has_scalars() const {return has_scalars_;}
  size_t npts() const {return points_.size();}
  size_t size() const {return points_.size();}
  vgl_point_3d<Type> p(unsigned i) const {return points_[i];}
  vgl_vector_3d<Type> n(unsigned i) const
  {if(has_normals_) return normals_[i]; return vgl_vector_3d<Type>();}
  Type sc(unsigned i) const {if(has_scalars_) return scalars_[i]; return Type(0);}

  std::vector<vgl_point_3d<Type> > points() const {return points_;}
  std::vector<vgl_vector_3d<Type> > normals() const {return normals_;}
  std::vector< Type > scalars() const {return scalars_;}
  void clear(){points_.clear(); normals_.clear();}
  void set_points(std::vector<vgl_point_3d<Type> > const& points)
  { points_ = points; has_normals_=false; has_scalars_ = false;}

  void set_points_with_normals(std::vector<vgl_point_3d<Type> > const& points,
                               std::vector<vgl_vector_3d<Type> > const& normals)
  { points_ = points; normals_ = normals; has_normals_=true; has_scalars_ = false;}

  void set_points_with_scalars(std::vector<vgl_point_3d<Type> > const& points,
                               std::vector< Type > const& scalars)
  { points_ = points; scalars_ = scalars; has_scalars_=true; }

  void set_points_with_normals_and_scalars(std::vector<vgl_point_3d<Type> > const& points,
                                           std::vector<vgl_vector_3d<Type> > const& normals,
                                           std::vector< Type > const& scalars)
  { points_ = points; normals_ = normals; scalars_ = scalars; has_normals_=true; has_scalars_ = true;}


  void append_pointset(vgl_pointset_3d<Type> const& ptset){
    if(this->has_normals_ != ptset.has_normals())
      return; // can't be done
    unsigned npts = ptset.npts();
    for(unsigned i = 0; i<npts; ++i){
      if(!this->has_normals_&& !this->has_scalars_)
        this->add_point(ptset.p(i));
      else if(!this->has_scalars_)
        this->add_point_with_normal(ptset.p(i), ptset.n(i));
      else if(!this->has_normals_)
        this->add_point_with_scalar(ptset.p(i), ptset.sc(i));
      else
        this->add_point_with_normal_and_scalar(ptset.p(i), ptset.n(i), ptset.sc(i));
    }
  }

  bool set_point(unsigned i, vgl_point_3d<Type> const& p){
    if(i>=static_cast<unsigned>(points_.size())) return false;
    points_[i].set(p.x(), p.y(), p.z()); return true;
  }
  bool set_normal(unsigned i, vgl_vector_3d<Type> const& n){
    if(has_normals_&& i<static_cast<unsigned>(normals_.size())){
                normals_[i].set(n.x(), n.y(), n.z()); return true;}
    else return false;}

  bool set_scalar(unsigned i, Type sc){
    if(has_scalars_&& i<static_cast<unsigned>(scalars_.size())){
      scalars_[i] = sc; return true;}
    else return false;}

  //: Equality operator
  bool operator==(const vgl_pointset_3d<Type> &spl) const;

  bool operator!=(vgl_pointset_3d<Type>const& spl) const { return !operator==(spl); }

};

template <class Type>
  bool vgl_pointset_3d<Type>::operator==(const vgl_pointset_3d<Type>& pointset) const{
  unsigned n = pointset.npts();
  if(n != this->npts())
    return false;
  std::vector<vgl_point_3d<Type> > pts = pointset.points();
  for(unsigned i =0; i<n; ++i)
    if(pts[i] != points_[i])
      return false;
  if((has_normals() && !pointset.has_normals()) || (!has_normals() && pointset.has_normals()))
    return false;
  if(has_normals_){
    if(static_cast<unsigned>(normals_.size()) != n)
      return false;
    std::vector<vgl_vector_3d<Type> > normals = pointset.normals();
    for(unsigned i =0; i<n; ++i)
      if(normals[i] != normals_[i])
        return false;
  }
  if((has_scalars() && !pointset.has_scalars()) || (!has_scalars() && pointset.has_scalars()))
    return false;
  if(has_scalars_){
    if(static_cast<unsigned>(scalars_.size()) != n)
      return false;
    std::vector< Type > scalars = pointset.scalars();
    for(unsigned i =0; i<n; ++i)
      if(scalars[i] != scalars_[i])
        return false;
  }
  return true;
}

template <class Type>
std::ostream&  operator<<(std::ostream& ostr, vgl_pointset_3d<Type> const& ptset){
  if(!ostr){
    std::cout << "Bad ostream in write vgl_pointset_3d to stream\n";
    return ostr;
  }
  std::vector<vgl_point_3d<Type> > pts = ptset.points();
  if(!ptset.has_normals()&&!ptset.has_scalars()){
    for(unsigned i =0; i<static_cast<unsigned>(pts.size()); i++){
    const vgl_point_3d<Type>& p = pts[i];
    ostr << p.x() << ',' << p.y() << ',' << p.z() << '\n';
    }
  }else if(!ptset.has_normals()&&ptset.has_scalars()){
    std::vector< Type > scalars = ptset.scalars();
    for(unsigned i =0; i<static_cast<unsigned>(pts.size()); i++){
    const vgl_point_3d<Type>& p = pts[i];
    Type sc = scalars[i];
    ostr << p.x() << ',' << p.y() << ',' << p.z()<< ',' << sc << '\n';
    }
  }else if(ptset.has_normals()&& !ptset.has_scalars()){
    std::vector<vgl_vector_3d<Type> > normals = ptset.normals();
    for(unsigned i =0; i<static_cast<unsigned>(pts.size()); i++){
    const vgl_point_3d<Type>& p = pts[i];
    const vgl_vector_3d<Type>& n = normals[i];
    ostr << p.x() << ',' << p.y() << ',' << p.z()<< ',' << n.x() << ',' << n.y() << ',' << n.z() << '\n';
    }
  }else if(ptset.has_normals()&& ptset.has_scalars()){
    std::vector< Type > scalars = ptset.scalars();
    std::vector<vgl_vector_3d<Type> > normals = ptset.normals();
    for(unsigned i =0; i<static_cast<unsigned>(pts.size()); i++){
      const vgl_point_3d<Type>& p = pts[i];
      const vgl_vector_3d<Type>& n = normals[i];
      Type sc = scalars[i];
      ostr << p.x() << ',' << p.y() << ',' << p.z()<< ',' << n.x() << ',' << n.y() << ',' << n.z() << ',' << sc << '\n';
    }
  }
  return ostr;
}

template <class Type>
std::istream&  operator>>(std::istream& istr, vgl_pointset_3d<Type>& ptset){
  if(!istr){
    std::cout << "Bad istream in read vgl_pointset_3d from stream\n";
    return istr;
  }
  std::vector<vgl_point_3d<Type> > pts;
  std::vector<vgl_vector_3d<Type> > normals;
  std::vector<Type > scalars;
  char buf[100];
  bool has_normals = false;
  bool has_scalars = false;
  //determine the number of comma or space separators
  // 2: points
  // 3: points and scalars
  // 5: points and normals
  // 6: points, normals and scalars
  bool first_line = true;
  while(istr.getline(buf,100)){
      std::string buf_str;
      if(first_line){
        bool done = false;
        unsigned comma_count = 0;
        for(unsigned i =0; i<100&&!done; ++i){
          char c = buf[i];
          if(c == '\0'||c == '\n'){
            done = true;
            continue;
          }else{
            buf_str.push_back(c);
            if(c == ','||c==' ')
              comma_count++;
          }
        }
        has_scalars = (comma_count == 3 || comma_count == 6) ;
        has_normals = (comma_count == 5 || comma_count == 6) ;
        first_line = false;
      }else{
        // scan in the file line by line
        bool done = false;
        for(unsigned i =0; i<100&&!done; ++i){
          char c = buf[i];
          if(c == '\0'||c == '\n'){
            done = true;
            continue;
          }
          buf_str.push_back(c);
        }
      }
      // create a stream from the valid line characters
      std::stringstream isstr(buf_str);
      isstr >>std::noskipws;//accept spaces as separators
      // parse the line for point and normal values
      Type x, y, z, nx, ny, nz, sc;
      unsigned char c;
      isstr >> x >> c;
      if(!(c==','||c==' ')){
        std::cout << "stream parse error\n";
        return istr;
      }
      isstr >> y >> c;
      if(!(c==','||c==' ')){
        std::cout << "stream parse error\n";
        return istr;
      }
      if(!has_normals&&!has_scalars)
        isstr >> z;
      else if(has_normals || has_scalars){
        isstr >> z >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
      }
          if(has_normals&&!has_scalars){
        isstr >> nx >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
        isstr >> ny >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
        isstr >> nz;
      }else if(has_scalars&& !has_normals)
        isstr >> sc;
      else if( has_scalars && has_normals){
        isstr >> nx >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
        isstr >> ny >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
        isstr >> nz >> c;
        if(!(c==','||c==' ')){
          std::cout << "stream parse error\n";
          return istr;
        }
        isstr >> sc;
      }
      vgl_point_3d<Type> p(x,y,z);
      pts.push_back(p);
      if(has_normals){
        vgl_vector_3d<Type> n(nx, ny, nz);
        normals.push_back(n);
      }
      if(has_scalars)
        scalars.push_back(sc);
  }
  if(!has_normals&&has_scalars){
    ptset = vgl_pointset_3d<Type>(pts, scalars);
    return istr;
  }else if(has_normals&&!has_scalars){
    ptset = vgl_pointset_3d<Type>(pts, normals);
    return istr;
  }else if(has_scalars&&has_normals){
    ptset = vgl_pointset_3d<Type>(pts, normals, scalars);
    return istr;
  }
  ptset = vgl_pointset_3d<Type>(pts);
  return istr;
};

#endif // vgl_pointset_3d_h_
