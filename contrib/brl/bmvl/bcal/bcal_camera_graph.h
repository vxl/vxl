//:
// \file
// \brief a graph with source
// \author Kongbin Kang (Kongbin_Kang@Brown.edu)
// \date   4/3/2003

#ifndef AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_
#define AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

template<class S, class V, class E>
class bcal_camera_graph
{
 protected:
  struct vertex_node
  {
   public:
    int id_;
    V *v_; // distinguish source with other node
    S *s_;
   public:
    vertex_node(int id)
    {
      id_ = id;
      v_ = 0;
      s_ = 0;
    }

    ~vertex_node()
    {
      delete v_; v_ = 0;
      delete s_; s_ = 0;
    }
  };


  struct edge_node
  {
    vertex_node* v_; // recode which vertex attached with this edge
    E* e_;
   public:
    edge_node(vertex_node* v) { v_ = v; e_ = new E;}
    virtual ~edge_node() { delete e_; }
  };

 protected:
  int init_graph()
  {
    source_ = 0;
    num_vertice_ = 0;
    // add a source
    add_vertex(0);

    source_ = vertice_[0]->s_;
    return 0;
  }

  vertex_node* malloc_new_node(bool is_source)
  {
    vertex_node *v = new vertex_node(num_vertice_++);

    if (is_source){
      v->s_ = new S;
      v->v_ = 0;
    }
    else{
      v->s_ = 0;
      v->v_ = new V;
    }

    return v;
  }

 public:
#if 0
  class iterator
  {
   public:
    iterator() : ptr_(0), pos_(0) {}
    iterator(vcl_vector<vertex_node*> *p, int pos = 0) : ptr_(p), pos_(pos) {}
    iterator(const iterator& x) : ptr_( x.ptr_ ), pos_(x.pos_) {}
    V& operator*() const {return *((*ptr_)[pos_]->v_); }
    V* operator->() const {return &(* *this); }

    iterator& operator=(iterator const& x)
    {
      pos_ = x.pos_;
      ptr_ = x.ptr_;
      return *this;
    }

    iterator& operator++()
    {
      increment();
      return *this;
    }

    iterator operator++(int)
    {
      iterator t = *this;
      increment();
      return t;
    }

    iterator& operator--()
    {
      decrement();
      return *this;
    }

    iterator operator--(int )
    {
      iterator t = *this;
      decrement();
      return t;
    }

    bool operator==(const iterator& x) const { return ptr_ == x.ptr_ && pos_ = x.pos_; }

    bool operator!=(const iterator& x) const { return !(*this == x); }

    void decrement() { if (pos_>=0)  pos_--; }
    void increment() { pos_++; }

    vertex_node* node() const { return (*ptr_)[pos]; }

    int get_vertex_id() const { return (*ptr_)[pos_]->id_; }

   protected:
    vcl_vector<vertex_node*>* ptr_;
    int pos_;
  };
#endif // 0

 public: // constructor and destructor

  bcal_camera_graph() { init_graph(); }

  virtual ~bcal_camera_graph() { erase_graph(); }

 public: // operations
  S* get_source() {  return source_;}
  int get_source_id() { return 0;}

  // get vertex at position of
  V* get_vertex_from_pos(int pos)
  {
    assert(num_vertice_ > pos);
    return vertice_[pos + 1]->v_;
  }

  V* get_vertex_from_id(int id)
  {
    assert(id > 0 && id <= num_vertice_);
    return get_vertex_from_pos(id - 1);
  }

  // return a id of vertex at position pos;
  int get_vertex_id(int pos)
  {
    assert(pos >= 0 && pos < num_vertice_);
    return pos + 1;
  }

  // add a new vertex and edge from the neighbour v to it
  // it return the position of the new vertex in the array
  int add_vertex(int neighbour = 0)
  {
    // currently only edge from source can be added
    assert(neighbour == 0);

    // the neighbour should exist already
    assert(neighbour <= num_vertice_);

    vertex_node *v;
    if (source_){
      v = malloc_new_node(false); // allocate an ordinary vertex
    }
    else
      v = malloc_new_node(true); // allocate a source vertex

    vertice_.push_back(v);

    // add a neighbour list into edges
    vcl_vector<edge_node*> *neighbour_list = new vcl_vector<edge_node*>;
    edges_.push_back(neighbour_list);

    // update old adjacent neighbour list
    if (source_){ // if not source vertex is added
      edge_node *e = new edge_node(v);
      edges_[neighbour]->push_back(e);
    }

    return v->id_;
  }

  // id is the same of it is position in array
  inline V* get_vertex(int i)
  {
    assert(i>0);
    return vertice_[i]->v_;
  }

#if 0
  // return a iterator pointer to first vertex
  iterator begin() { return iterator(&vertice_, 1); }
#endif // 0

  // get edge from v1 to v2
  inline E* get_edge(int v1, int v2)
  {
    assert(v1 == 0 && v2 <= num_vertice_); // only from souce to camera is avaible
    vcl_vector<edge_node*>* plist = edges_[v1];
    for (unsigned int i=0; i < plist->size(); i++){
      edge_node *e = (*plist)[i];
      vertex_node *v = e->v_;
      assert(v != 0); // no single edge exist

      if (v->id_ == v2){
        return e->e_;
      }
      else
        continue;
    }

    return 0; // cannot find edge
  }

  int num_vertice() { return vertice_.size() - 1; }


  // for debug
  void print(vcl_ostream& out = vcl_cerr)
  {
    out<<"print graph\n";
    for (int i=0; i<num_vertice_; i++){
      out<<"vertex id is: "<<vertice_[i]->id_<<"\t v is: "
         <<vertice_[i]->v_ <<"\ts is: "<<vertice_[i]->s_<<'\n';

      int num_neighbours = edges_[i]->size();
      for (int j=0; j< num_neighbours; j++){
        (*edges_[i])[j]->e_->print(out);
      }
    }

    //print edge
  }

  int erase_graph()
  {
    num_vertice_ = vertice_.size();

    if (num_vertice_ == 0) // empty graph
      return 0; // no error

    for (int i=0; i<num_vertice_; i++){
      // delete each vertex
      delete vertice_[i]; vertice_[i] = 0;

      // delete each edge node
      if (edges_[i])
      {
        int list_length = edges_[i]->size();
        for (int j=0; j<list_length; j++)
          delete (*(edges_[i]))[j];

        delete edges_[i];
      }
      edges_[i] = 0;
    }
    vertice_.clear();
    edges_.clear();

    source_ = 0;

    return 0; // no error
  }

 private:
  vcl_vector<vertex_node*> vertice_;
  vcl_vector<vcl_vector<edge_node*>* > edges_; // adjacent neigbhour list
  S* source_;
  int num_vertice_;
};

#endif // AFX_CAMERAGRAPH_H__8810B4C3_E6C7_42CE_8C7F_D9F8F201F6F6__INCLUDED_
