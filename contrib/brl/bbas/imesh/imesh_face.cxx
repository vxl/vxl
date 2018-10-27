// This is brl/bbas/imesh/imesh_face.cxx
#include "imesh_face.h"
//:
// \file


//: Return the group name for a given face index
std::string imesh_face_array_base::group_name(unsigned int f) const
{
  if (groups_.empty())
    return "";

  unsigned int i=0;
  for (; i<groups_.size() && groups_[i].second<f; ++i)
    ;

  if (i>=groups_.size())
    return "";

  return groups_[i].first;
}


//: Return a set of all faces in a group
std::set<unsigned int>
imesh_face_array_base::group_face_set(const std::string& name) const
{
  std::set<unsigned int> face_set;
  unsigned int start = 0, end;
  for (const auto & group : groups_) {
    end = group.second;
    if (group.first == name) {
      for (unsigned int i=start; i<end; ++i)
        face_set.insert(i);
    }
    start = end;
  }
  return face_set;
}


//: Assign a group name to all faces currently unnamed
//  Return the number of faces in the new group
unsigned int imesh_face_array_base::make_group(const std::string& name)
{
  unsigned int start_idx = 0;
  if (!groups_.empty())
    start_idx = groups_.back().second;

  if (start_idx < this->size())
    groups_.emplace_back(name,this->size());

  return this->size() - start_idx;
}


//: Append this array of faces (must be the same type)
//  Optionally shift the indices in \param other by \param ind_shift
void imesh_face_array_base::append(const imesh_face_array_base& other,
                                   unsigned int )
{
  if (this->has_normals() && other.has_normals())
    normals_.insert(normals_.end(), other.normals_.begin(), other.normals_.end());
  else
    normals_.clear();

  if (other.has_groups()) {
    // group any ungrouped faces in this array
    this->make_group("ungrouped");
    unsigned int offset = this->size();
    for (const auto & group : other.groups_) {
      groups_.push_back(group);
      groups_.back().second += offset;
    }
  }
}


//: Append this array of faces
//  Optionally shift the indices in \param other by \param ind_shift
void imesh_face_array::append(const imesh_face_array_base& other,
                              unsigned int ind_shift)
{
  imesh_face_array_base::append(other,ind_shift);

  const unsigned int new_begin = faces_.size();

  if (other.regularity() == 0) {
    const auto& fs = static_cast<const imesh_face_array&>(other);
    faces_.insert(faces_.end(), fs.faces_.begin(), fs.faces_.end());

    if (ind_shift > 0) {
      for (unsigned int i=new_begin; i<faces_.size(); ++i) {
        std::vector<unsigned int>& f = faces_[i];
        for (unsigned int & j : f)
          j += ind_shift;
      }
    }
  }
  else {
    for (unsigned int i=0; i<other.size(); ++i) {
      std::vector<unsigned int> f(other.num_verts(i));
      for (unsigned int j=0; j<other.num_verts(i); ++j)
        f[j] = other(i,j) + ind_shift;
      faces_.push_back(f);
    }
  }
}


//: Merge the two face arrays
std::unique_ptr<imesh_face_array_base>
imesh_merge(const imesh_face_array_base& f1,
            const imesh_face_array_base& f2,
            unsigned int ind_shift)
{
  std::unique_ptr<imesh_face_array_base> f;
  // if both face sets are regular with the same number of vertices per face
  if (f1.regularity() == f2.regularity() || f1.regularity() == 0) {
    f.reset(f1.clone());
  }
  else {
    f.reset(new imesh_face_array(f1));
  }
  f->append(f2,ind_shift);
  return f;
}
