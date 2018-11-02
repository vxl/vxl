// This is brl/bbas/imesh/imesh_vertex.h
#ifndef imesh_vertex_h_
#define imesh_vertex_h_
//:
// \file
// \brief A mesh vertex
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 5, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#define imesh_invalid_idx (static_cast<unsigned int>(-1))


//: A mesh face with a fixed number of vertices
template <unsigned d>
class imesh_vertex
{
 public:
  //: Default Constructor
  imesh_vertex() { for (unsigned i=0; i<d; ++i) coords_[i]=0.0; }

  //: Constructor from a vector
  imesh_vertex(const std::vector<double>& coords)
  {assert(coords.size()==d); for (unsigned i=0; i<d; ++i) coords_[i]=coords[i];}

  //: return the dimension of the vertex
  unsigned int dim() const { return d; }

  //: Accessor
  double operator[] (unsigned int i) const { return coords_[i]; }
  double& operator[] (unsigned int i) { return coords_[i]; }

 protected:
  double coords_[d];
};


//: A 2d vertex specialization with extra capabilities
template <>
class imesh_vertex<2>
{
 public:
  //: Default Constructor
  imesh_vertex() { coords_[0]=0.0; coords_[1]=0.0;}

  //: Constructor (from 2 doubles)
  imesh_vertex(double x, double y)
  {
    coords_[0] = x;
    coords_[1] = y;
  }

  //: Constructor (from vgl point)
  imesh_vertex(const vgl_point_2d<double>& pt)
  {
    coords_[0] = pt.x();
    coords_[1] = pt.y();
  }

  //: convert to a vgl point
  operator vgl_point_2d<double>() const
  {
    return {coords_[0],coords_[1]};
  }

  //: Constructor from a vector
  imesh_vertex(const std::vector<double>& coords)
  {assert(coords.size()==2); coords_[0]=coords[0]; coords_[1]=coords[1];}

  //: return the dimension of the vertex
  unsigned int dim() const { return 2; }

  //: Accessor
  double operator[] (unsigned int i) const { return coords_[i]; }
  double& operator[] (unsigned int i) { return coords_[i]; }

 protected:
  double coords_[2];
};


//: A 3d vertex specialization with extra capabilities
template <>
class imesh_vertex<3>
{
 public:
  //: Default Constructor
  imesh_vertex() {coords_[0]=0.0; coords_[1]=0.0; coords_[2]=0.0; }

  //: Constructor (from 2 doubles)
  imesh_vertex(double x, double y, double z)
  {
    coords_[0] = x;
    coords_[1] = y;
    coords_[2] = z;
  }

  //: Constructor (from vgl point)
  imesh_vertex(const vgl_point_3d<double>& pt)
  {
    coords_[0] = pt.x();
    coords_[1] = pt.y();
    coords_[2] = pt.z();
  }

  //: Constructor from a vector
  imesh_vertex(const std::vector<double>& coords)
  {
    assert(coords.size()==3);
    coords_[0]=coords[0];
    coords_[1]=coords[1];
    coords_[2]=coords[2];
  }

  //: convert to a vgl point
  operator vgl_point_3d<double>() const
  {
    return {coords_[0],coords_[1],coords_[2]};
  }

  //: return the dimension of the vertex
  unsigned int dim() const { return 3; }

  //: Accessor
  double operator[] (unsigned int i) const { return coords_[i]; }
  double& operator[] (unsigned int i) { return coords_[i]; }

 protected:
  double coords_[3];
};


//: Abstract base class for a collection of vertices
class imesh_vertex_array_base
{
 public:
  //: Destructor
  virtual ~imesh_vertex_array_base() = default;

  //: returns the number of vertices
  virtual unsigned int size() const = 0;

  //: returns the dimension of the vertices
  virtual unsigned int dim() const = 0;

  //: Access a vertex coordinate by vertex index and coordinate index
  virtual double operator() (unsigned int v, unsigned int i) const = 0;

  //: Produce a clone of this object (dynamic copy)
  virtual imesh_vertex_array_base* clone() const = 0;

  //: Append these vertices (assuming the same type)
  virtual void append(const imesh_vertex_array_base& verts)
  {
    if (this->has_normals() && verts.has_normals())
      normals_.insert(normals_.end(), verts.normals_.begin(), verts.normals_.end());
    else
      normals_.clear();
  }

  //: Return true if the vertices have normals
  bool has_normals() const { return !normals_.empty(); }

  //: Set the vertex normals
  void set_normals(const std::vector<vgl_vector_3d<double> >& n)
  { assert(n.size() == this->size()); normals_ = n; }

  //: Access a vertex normal
  vgl_vector_3d<double>& normal(unsigned int v) { return normals_[v]; }
  const vgl_vector_3d<double>& normal(unsigned int v) const { return normals_[v]; }

  //: Access the normals
  const std::vector<vgl_vector_3d<double> >& normals() const { return normals_; }

 protected:
  std::vector<vgl_vector_3d<double> > normals_;
};


//: An array of vertices of dimension d
template <unsigned int d>
class imesh_vertex_array : public imesh_vertex_array_base
{
  std::vector<imesh_vertex<d> > verts_;

 public:
  //: Default Constructor
  imesh_vertex_array<d>() = default;

  //: Constructor (from size)
  imesh_vertex_array<d>(unsigned int size)
  : verts_(size) {}

  //: Constructor (from vector)
  imesh_vertex_array<d>(const std::vector<imesh_vertex<d> >& verts)
  : verts_(verts) {}

  //: Produce a clone of this object (dynamic copy)
  imesh_vertex_array_base* clone() const override
  {
    return new imesh_vertex_array<d>(*this);
  }

  //: returns the number of vertices
  unsigned int size() const override { return verts_.size(); }

  //: returns the dimension of the vertices
  unsigned int dim() const override { return d; }

  //: Access a vertex coordinate by vertex index and coordinate index
  double operator() (unsigned int v, unsigned int i) const override { return verts_[v][i]; }

  //: Append these vertices (assuming the same type)
  void append(const imesh_vertex_array_base& verts) override
  {
    assert(verts.dim() == d);
    const imesh_vertex_array<d>& v = static_cast<const imesh_vertex_array<d>&>(verts);
    verts_.insert(verts_.end(), v.verts_.begin(), v.verts_.end());
    imesh_vertex_array_base::append(verts);
  }

  //: Add a vertex to the array
  void push_back(const imesh_vertex<d>& v) { verts_.push_back(v); }

  //: Access a vertex
  imesh_vertex<d>& operator[] (unsigned int v) { return verts_[v]; }
  const imesh_vertex<d>& operator[] (unsigned int v) const { return verts_[v]; }

  //=====================================================
  // Vertex Iterators
  typedef typename std::vector<imesh_vertex<d> >::iterator iterator;
  typedef typename std::vector<imesh_vertex<d> >::const_iterator const_iterator;

  iterator begin() { return verts_.begin(); }
  const_iterator begin() const { return verts_.begin(); }

  iterator end() { return verts_.end(); }
  const_iterator end() const { return verts_.end(); }
};


//: compute the vector normal to the plane defined by 3 vertices
vgl_vector_3d<double> imesh_tri_normal(const imesh_vertex<3>& a,
                                       const imesh_vertex<3>& b,
                                       const imesh_vertex<3>& c);

#endif // imesh_vertex_h_
