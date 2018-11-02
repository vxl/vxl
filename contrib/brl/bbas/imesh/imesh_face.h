// This is brl/bbas/imesh/imesh_face.h
#ifndef imesh_face_h_
#define imesh_face_h_
//:
// \file
// \brief A mesh face
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 2, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <utility>
#include <vector>
#include <set>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>

#define imesh_invalid_idx (static_cast<unsigned int>(-1))


//: A mesh face with a fixed number of vertices
template <unsigned s>
class imesh_regular_face
{
 public:
  //: Default Constructor
  imesh_regular_face() { for (unsigned i=0; i<s; ++i) verts_[i]=imesh_invalid_idx; }

  //: Constructor from a vector
  imesh_regular_face(const std::vector<unsigned int>& verts)
  {assert(verts.size()==s); for (unsigned i=0; i<s; ++i) verts_[i]=verts[i];}

  //: return the number of vertices
  unsigned int num_verts() const { return s; }

  void flip_orientation()
  {
    std::reverse(verts_,verts_+s);
  }

  //: Accessor
  unsigned int operator[] (unsigned int i) const { return verts_[i]; }
  unsigned int& operator[] (unsigned int i) { return verts_[i]; }
 protected:
  unsigned int verts_[s];
};


//: A triangle face
class imesh_tri : public imesh_regular_face<3>
{
 public:
  imesh_tri(unsigned int a, unsigned int b, unsigned int c)
  {
    verts_[0] = a;
    verts_[1] = b;
    verts_[2] = c;
  }
};

//: A quadrilateral face
class imesh_quad : public imesh_regular_face<4>
{
 public:
  imesh_quad(unsigned int a, unsigned int b,
             unsigned int c, unsigned int d)
  {
    verts_[0] = a;
    verts_[1] = b;
    verts_[2] = c;
    verts_[3] = d;
  }
};


//: Abstract base class for a collection of faces
class imesh_face_array_base
{
 public:
  //: Destructor
  virtual ~imesh_face_array_base() = default;

  //: returns the number of vertices per face if the same for all faces, zero otherwise
  virtual unsigned int regularity() const = 0;

  //: returns the number of faces
  virtual unsigned int size() const = 0;

  //: returns the number of vertices in face \param f
  virtual unsigned int num_verts(unsigned int f) const = 0;

  //: Access a vertex index by face index and within-face index
  virtual unsigned int operator() (unsigned int f, unsigned int i) const = 0;

  //: Flip a face over, inverting its orientation
  virtual void flip_orientation (unsigned int f)
  {
    if (has_normals())
      normals_[f] *= -1;
  }

  //: Produce a clone of this object (dynamic copy)
  virtual imesh_face_array_base* clone() const = 0;

  //: Append this array of faces (must be the same type)
  //  Optionally shift the indices in \param other by \param ind_shift
  virtual void append(const imesh_face_array_base& other,
                      unsigned int ind_shift=0);

  //: Return true if the faces have normals
  bool has_normals() const { return !normals_.empty(); }

  //: Set the face normals
  void set_normals(const std::vector<vgl_vector_3d<double> >& n)
  { assert(n.size() == this->size()); normals_ = n; }

  //: Access a face normal
  vgl_vector_3d<double>& normal(unsigned int f) { return normals_[f]; }
  const vgl_vector_3d<double>& normal(unsigned int f) const { return normals_[f]; }

  //: Access the entire vector of normals
  const std::vector<vgl_vector_3d<double> >& normals() const { return normals_; }

  //: Returns true if the faces have named groups
  bool has_groups() const { return !groups_.empty(); }

  //: Return the group name for a given face index
  std::string group_name(unsigned int f) const;

  //: Assign a group name to all faces currently unnamed
  //  Return the number of faces in the new group
  unsigned int make_group(const std::string& name);

  //: Return a set of all faces in a group
  std::set<unsigned int> group_face_set(const std::string& name) const;

  //: Access the groups
  const std::vector<std::pair<std::string,unsigned int> >& groups() const { return groups_; }

 protected:
  //: named groups of adjacent faces (a partition of the face array)
  //  Integers mark the group's ending vertex + 1
  std::vector<std::pair<std::string,unsigned int> > groups_;

  //: vectors that are normal to each face
  std::vector<vgl_vector_3d<double> > normals_;
};


//: An array of mesh faces of arbitrary size
class imesh_face_array : public imesh_face_array_base
{
  std::vector<std::vector<unsigned int> > faces_;

 public:
  //: Default Constructor
  imesh_face_array() = default;

  //: Constructor
  imesh_face_array(unsigned int size) : faces_(size) {}

  //: Constructor (from a vector)
  imesh_face_array(std::vector<std::vector<unsigned int> >  faces)
  : faces_(std::move(faces)) {}

  //: Copy Constructor
  imesh_face_array(const imesh_face_array& other)
  : imesh_face_array_base(other), faces_(other.faces_) {}

  //: Construct from base class
  explicit imesh_face_array(const imesh_face_array_base& fb)
  : imesh_face_array_base(fb), faces_(fb.size())
  {
    for (unsigned int i=0; i<fb.size(); ++i)
      for (unsigned int j=0; j<fb.num_verts(i); ++j)
        faces_[i].push_back(fb(i,j));
  }

  //: returns the number of vertices per face if the same for all faces, zero otherwise
  unsigned int regularity() const override { return 0; }

  //: returns the number of faces
  unsigned int size() const override { return faces_.size(); }

  //: returns the number of vertices in face \param f
  unsigned int num_verts(unsigned int f) const override { return faces_[f].size(); }

  //: Access a vertex index by face index and within-face index
  unsigned int operator() (unsigned int f, unsigned int i) const override { return faces_[f][i]; }

  //: Flip a face over, inverting its orientation
  void flip_orientation (unsigned int f) override
  {
    std::reverse(faces_[f].begin(),faces_[f].end());
    imesh_face_array_base::flip_orientation(f);
  }

  //: Produce a clone of this object (dynamic copy)
  imesh_face_array_base* clone() const override
  {
    return new imesh_face_array(*this);
  }

  //: Append this array of faces
  //  Optionally shift the indices in \param other by \param ind_shift
  void append(const imesh_face_array_base& other,
                      unsigned int ind_shift=0) override;

  //: Add a face to the array
  void push_back(const std::vector<unsigned int>& f) { faces_.push_back(f); }

  //: Add a face to the array
  template <unsigned int s>
  void push_back(const imesh_regular_face<s>& f)
  {
    std::vector<unsigned int> f2(s);
    for (unsigned int i=0; i<s; ++i)
      f2[i] = f[i];
    this->push_back(f2);
  }

  //: Access face \param f
  std::vector<unsigned int>& operator[] (unsigned int f) { return faces_[f]; }
  const std::vector<unsigned int>& operator[] (unsigned int f) const { return faces_[f]; }
};


//: An array of mesh faces of arbitrary size
template <unsigned int s>
class imesh_regular_face_array : public imesh_face_array_base
{
  std::vector<imesh_regular_face<s> > faces_;

 public:
  //: Default Constructor
  imesh_regular_face_array<s>() = default;

  //: Constructor
  imesh_regular_face_array<s>(unsigned int size) : faces_(size) {}

  //: Constructor (from a vector)
  imesh_regular_face_array<s>(const std::vector<imesh_regular_face<s> >& faces) : faces_(faces) {}

  //: returns the number of vertices per face if the same for all faces
  //  Returns zero otherwise
  unsigned int regularity() const override { return s; }

  //: returns the number of faces
  unsigned int size() const override { return faces_.size(); }

  //: returns the number of vertices in face \param f
  unsigned int num_verts(unsigned int /*f*/) const override { return s; }

  //: Access a vertex index by face index and within-face index
  unsigned int operator() (unsigned int f, unsigned int i) const override { return faces_[f][i]; }

  //: Flip a face over, inverting its orientation
  void flip_orientation (unsigned int f) override
  {
    faces_[f].flip_orientation();
    imesh_face_array_base::flip_orientation(f);
  }

  //: Produce a clone of this object (dynamic copy)
  imesh_face_array_base* clone() const override
  {
    return new imesh_regular_face_array<s>(*this);
  }

  //: Append this array of faces (must be the same type)
  //  Optionally shift the indices in \param other by \param ind_shift
  void append(const imesh_face_array_base& other,
                      unsigned int ind_shift=0) override
  {
    imesh_face_array_base::append(other,ind_shift);
    assert(other.regularity() == s);
    const imesh_regular_face_array<s>& fs =
        static_cast<const imesh_regular_face_array<s>&>(other);
    const unsigned int new_begin = faces_.size();
    faces_.insert(faces_.end(), fs.faces_.begin(), fs.faces_.end());
    if (ind_shift > 0) {
      for (unsigned int i=new_begin; i<faces_.size(); ++i) {
        imesh_regular_face<s>& f = faces_[i];
        for (unsigned int j=0; j<s; ++j)
          f[j] += ind_shift;
      }
    }
  }

  //: Add a face to the array
  void push_back(const imesh_regular_face<s>& f) { faces_.push_back(f); }

  //: Access face \param f
  imesh_regular_face<s>& operator[] (unsigned int f) { return faces_[f]; }
  const imesh_regular_face<s>& operator[] (unsigned int f) const { return faces_[f]; }

  //=====================================================
  // Face Iterators
  typedef typename std::vector<imesh_regular_face<s> >::iterator iterator;
  typedef typename std::vector<imesh_regular_face<s> >::const_iterator const_iterator;

  iterator begin() { return faces_.begin(); }
  const_iterator begin() const { return faces_.begin(); }

  iterator end() { return faces_.end(); }
  const_iterator end() const { return faces_.end(); }
};


//: Merge the two face arrays
//  Shift the mesh indices in \param f2 by \param ind_shift
std::unique_ptr<imesh_face_array_base>
imesh_merge(const imesh_face_array_base& f1,
            const imesh_face_array_base& f2,
            unsigned int ind_shift=0);

#endif // imesh_face_h_
