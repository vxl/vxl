// This is brl/bbas/imesh/imesh_half_edge.h
#ifndef imesh_half_edge_h_
#define imesh_half_edge_h_

//:
// \file
// \brief A simple indexed half-edge
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 2, 2008

#include <iostream>
#include <iterator>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#define imesh_invalid_idx (static_cast<unsigned int>(-1))

class imesh_half_edge
{
    friend class imesh_half_edge_set;
  public:
    imesh_half_edge(unsigned int e, unsigned int n, unsigned int v, unsigned int f)
    : next_(n), edge_(e), vert_(v), face_(f) {}

    //: return the next half-edge index
    unsigned int next_index() const { return next_; }
    //: return the pair half-edge index
    unsigned int pair_index() const { return edge_^1; }

    //: return the index of the full edge
    unsigned int edge_index() const { return edge_>>1; }
    //: return the index of the half-edge
    unsigned int half_edge_index() const { return edge_; }

    //: return the vertex index
    unsigned int vert_index() const { return vert_; }
    //: return the face index
    unsigned int face_index() const { return face_; }

    bool is_boundary() const { return face_ == imesh_invalid_idx; }

  private:
    unsigned int next_;
    unsigned int edge_;

    unsigned int vert_;
    unsigned int face_;
};


//: A collection of indexed half edges
class imesh_half_edge_set
{
  public:
    //: Default Constructor
    imesh_half_edge_set() = default;
    //: Construct from a face index list
    imesh_half_edge_set(const std::vector<std::vector<unsigned int> >& face_list);

    //: Build the half edges from an indexed face set
    void build_from_ifs(const std::vector<std::vector<unsigned int> >& face_list);

    //: Access by index
    const imesh_half_edge& operator [] (unsigned int i) const { return half_edges_[i]; }
    //: Access by index
    imesh_half_edge& operator [] (unsigned int i)  { return half_edges_[i]; }

    //: number of half edges
    unsigned int size() const { return half_edges_.size(); }

    //: clear the edges
    void clear()
    {
      half_edges_.clear();
      vert_to_he_.clear();
      face_to_he_.clear();
    }

    // forward declare iterators
    class f_iterator;
    class f_const_iterator;
    class v_iterator;
    class v_const_iterator;

    //=====================================================
    // Mesh Face Iterators - each half edge touches the same face

    //: An iterator of half edges adjacent to a face
    class f_iterator : public std::iterator<std::forward_iterator_tag,imesh_half_edge>
    {
      friend class f_const_iterator;
      friend class v_iterator;
      public:
        //: Constructor
        f_iterator(unsigned int hei, imesh_half_edge_set& edge_set)
          :half_edge_index_(hei), edge_set_(edge_set) {}

        //: Constructor from vertex iterator
        explicit f_iterator(const v_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Assignment
        f_iterator& operator = (const f_iterator& other)
        {
          if (this != &other){
            assert(&edge_set_ == &other.edge_set_);
            half_edge_index_ = other.half_edge_index_;
          }
          return *this;
        }

        imesh_half_edge & operator*() const { return edge_set_[half_edge_index_]; }
        imesh_half_edge * operator->() const { return &**this; }
        imesh_half_edge & pair() const { return edge_set_[half_edge_index_^1]; }
        f_iterator & operator++ () // pre-inc
        {
          half_edge_index_ = edge_set_[half_edge_index_].next_index();
          return *this;
        }
        f_iterator operator++(int) // post-inc
        {
          f_iterator old = *this;
          ++*this;
          return old;
        }

        bool operator == (const f_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const f_iterator& other) const
        {
          return !(*this == other);
        }

        bool operator == (const f_const_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const f_const_iterator& other) const
        {
          return !(*this == other);
        }

      private:
        unsigned int half_edge_index_;
        imesh_half_edge_set& edge_set_;
    };

    //: A const iterator of half edges adjacent to a face
    class f_const_iterator : public std::iterator<std::forward_iterator_tag,imesh_half_edge>
    {
        friend class f_iterator;
        friend class v_const_iterator;
      public:
        //: Constructor
        f_const_iterator(unsigned int hei, const imesh_half_edge_set& edge_set)
          :half_edge_index_(hei), edge_set_(edge_set) {}

        //: Constructor from non-const iterator
        f_const_iterator(const f_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Constructor from vertex iterator
        explicit f_const_iterator(const v_const_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Assignment
        f_const_iterator& operator = (const f_const_iterator& other)
        {
          if (this != &other){
            assert(&edge_set_ == &other.edge_set_);
            half_edge_index_ = other.half_edge_index_;
          }
          return *this;
        }

        const imesh_half_edge & operator*() const { return edge_set_[half_edge_index_]; }
        const imesh_half_edge * operator->() const { return &**this; }
        const imesh_half_edge & pair() const { return edge_set_[half_edge_index_^1]; }
        f_const_iterator & operator++ () // pre-inc
        {
          half_edge_index_ = edge_set_[half_edge_index_].next_index();
          return *this;
        }
        f_const_iterator operator++(int) // post-inc
        {
          f_const_iterator old = *this;
          ++*this;
          return old;
        }

        bool operator == (const f_const_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const f_const_iterator& other) const
        {
          return !(*this == other);
        }

      private:
        unsigned int half_edge_index_;
        const imesh_half_edge_set& edge_set_;
    };


    //=====================================================
    // Mesh Vertex Iterators - each half edge touches the same vertex

    //: An iterator of half edges adjacent to a vertex
    class v_iterator : public std::iterator<std::forward_iterator_tag,imesh_half_edge>
    {
        friend class v_const_iterator;
        friend class f_iterator;
      public:
        //: Constructor
        v_iterator(unsigned int hei, imesh_half_edge_set& edge_set)
          :half_edge_index_(hei), edge_set_(edge_set) {}

        //: Constructor from face iterator
        explicit v_iterator(const f_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Assignment
        v_iterator& operator = (const v_iterator& other)
        {
          if (this != &other){
            assert(&edge_set_ == &other.edge_set_);
            half_edge_index_ = other.half_edge_index_;
          }
          return *this;
        }

        imesh_half_edge & operator*() const { return edge_set_[half_edge_index_]; }
        imesh_half_edge * operator->() const { return &**this; }
        imesh_half_edge & pair() const { return edge_set_[half_edge_index_^1]; }
        v_iterator & operator++ () // pre-inc
        {
          half_edge_index_ = half_edge_index_ ^ 1; // pair index
          half_edge_index_ = edge_set_[half_edge_index_].next_index();
          return *this;
        }
        v_iterator operator++(int) // post-inc
        {
          v_iterator old = *this;
          ++*this;
          return old;
        }

        bool operator == (const v_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const v_iterator& other) const
        {
          return !(*this == other);
        }

        bool operator == (const v_const_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const v_const_iterator& other) const
        {
          return !(*this == other);
        }

      private:
        unsigned int half_edge_index_;
        imesh_half_edge_set& edge_set_;
    };

    //: A const iterator of half edges adjacent to a vertex
    class v_const_iterator : public std::iterator<std::forward_iterator_tag,imesh_half_edge>
    {
        friend class v_iterator;
        friend class f_const_iterator;
      public:
        //: Constructor
        v_const_iterator(unsigned int hei, const imesh_half_edge_set& edge_set)
          :half_edge_index_(hei), edge_set_(edge_set) {}

        //: Constructor from non-const iterator
        v_const_iterator(const v_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Constructor from face iterator
        explicit v_const_iterator(const f_const_iterator& other)
          :half_edge_index_(other.half_edge_index_), edge_set_(other.edge_set_) {}

        //: Assignment
        v_const_iterator& operator = (const v_const_iterator& other)
        {
          if (this != &other){
            assert(&edge_set_ == &other.edge_set_);
            half_edge_index_ = other.half_edge_index_;
          }
          return *this;
        }

        const imesh_half_edge & operator*() const { return edge_set_[half_edge_index_]; }
        const imesh_half_edge * operator->() const { return &**this; }
        const imesh_half_edge & pair() const { return edge_set_[half_edge_index_^1]; }
        v_const_iterator & operator++ () // pre-inc
        {
          half_edge_index_ = half_edge_index_ ^ 1; // pair index
          half_edge_index_ = edge_set_[half_edge_index_].next_index();
          return *this;
        }
        v_const_iterator operator++(int) // post-inc
        {
          v_const_iterator old = *this;
          ++*this;
          return old;
        }

        bool operator == (const v_const_iterator& other) const
        {
          return this->half_edge_index_ == other.half_edge_index_ &&
                &(this->edge_set_) == &(other.edge_set_);
        }

        bool operator != (const v_const_iterator& other) const
        {
          return !(*this == other);
        }

      private:
        unsigned int half_edge_index_;
        const imesh_half_edge_set& edge_set_;
    };

    //: Access a face iterator for face \param f
    f_const_iterator face_begin(unsigned int f) const { return f_const_iterator(face_to_he_[f],*this); }
    f_iterator face_begin(unsigned int f) { return f_iterator(face_to_he_[f],*this); }

    //: Access a vertex iterator for vertex \param v
    v_const_iterator vertex_begin(unsigned int v) const { return v_const_iterator(vert_to_he_[v],*this); }
    v_iterator vertex_begin(unsigned int v) { return v_iterator(vert_to_he_[v],*this); }

    //: Count the number of vertices pointed to by these edges
    unsigned int num_verts() const;

    //: Count the number of faces pointed to by these edges
    unsigned int num_faces() const;

  private:
    std::vector<imesh_half_edge> half_edges_;
    std::vector<unsigned int> vert_to_he_;
    std::vector<unsigned int> face_to_he_;
};


#endif // imesh_half_edge_h_
