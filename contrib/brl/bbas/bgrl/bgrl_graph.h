// This is brl/bbas/bgrl/bgrl_graph.h
#ifndef bgrl_graph_h_
#define bgrl_graph_h_
//:
// \file
// \brief A general graph object
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// The graph object maintains pointers to all of the vertices
// in the graph.  It also handles the creation and destruction of
// edges between vertices.  The graph object also contains search
// iterators to iterate through the vertices along edges using search
// algorithms such as depth-first or breadth-first.
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_deque.h>
#include <vcl_set.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include "bgrl_vertex_sptr.h"
#include "bgrl_edge_sptr.h"
#include "bgrl_graph_sptr.h"

//: The graph
class bgrl_graph : public vbl_ref_count
{
 public:

  typedef vcl_set<bgrl_vertex_sptr>::iterator vertex_iterator;
  typedef vcl_set<bgrl_edge_sptr>::iterator edge_iterator;

  //: Constructor
  bgrl_graph();

  //: Copy Constructor
  // \note this provides a deep copy of the graph
  bgrl_graph(const bgrl_graph& graph);

  //: Destructor
  ~bgrl_graph(){}

  //: Adds a new vertex to the graph
  // \retval true if the vertex was added
  // \retval false if the vertex could not be added
  bool add_vertex(const bgrl_vertex_sptr& vertex);

  //: Deletes a vertex in the graph
  // \retval true if the vertex was deleted
  // \retval false if the vertex was not found in the graph
  bool remove_vertex(const bgrl_vertex_sptr& vertex);

  //: Add an edge between \p v1 and \p v2
  bgrl_edge_sptr add_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 );

  //: Add an edge between \p v1 and \p v2
  bool remove_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 );

  //: Remove all edges to NULL vertices and vertices not found in this graph
  // \retval true if any edges have been purged
  // \retval false if all edges were found to be valid
  bool purge();

  //: Returns the number of vertices in the graph
  int size() const;

  //: Returns the beginning iterator to the set of vertices
  vertex_iterator begin();

  //: Returns the end iterator to the set of vertices
  vertex_iterator end();

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bgrl_graph* clone() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

 private:
  //: The vector of vertices
  vcl_set<bgrl_vertex_sptr> vertices_;


 public:
  class iterator
  {
   public:
    //: Constructor
    iterator( bgrl_graph_sptr graph, bgrl_vertex_sptr vertex ) : graph_(graph), curr_vertex_(vertex) {}

    //: Destructor
    virtual ~iterator() {}

    //: Increment
    iterator& operator++ () { next_vertex(); return *this; }

    //: Dereference
    bgrl_vertex_sptr operator -> () const { return curr_vertex_; }
    //: Dereference
    bgrl_vertex_sptr operator * () const { return curr_vertex_; }

    //: Equality comparison
    bool operator == (const iterator& rhs) const { return rhs.curr_vertex_ == this->curr_vertex_; }

    //: Inequality comparison
    bool operator != (const iterator& rhs) const { return rhs.curr_vertex_ != this->curr_vertex_; }

   protected:
    //: Increment the current vertex
    virtual void next_vertex() = 0;

    bgrl_graph_sptr graph_;
    bgrl_vertex_sptr curr_vertex_;
  };

  // Depth first search iterator
  class depth_iterator : public iterator
  {
   public:
    //: Constructor
    depth_iterator( bgrl_graph_sptr graph, bgrl_vertex_sptr vertex ) : iterator(graph, vertex){ visited_.insert(vertex); }

   protected:
    //: Increment the current vertex
    void next_vertex();

    vcl_deque<bgrl_vertex_sptr> eval_queue_;
    vcl_set<bgrl_vertex_sptr> visited_;
  };

  // Breadth first search iterator
  class breadth_iterator : public iterator
  {
   public:
    //: Constructor
    breadth_iterator( bgrl_graph_sptr graph, bgrl_vertex_sptr vertex ) : iterator(graph, vertex){ visited_.insert(vertex); }

   protected:
    //: Increment the current vertex
    void next_vertex();

    vcl_deque<bgrl_vertex_sptr> eval_queue_;
    vcl_set<bgrl_vertex_sptr> visited_;
  };

  //: Depth first search begin iterator
  depth_iterator depth_begin(bgrl_vertex_sptr vertex) { return depth_iterator(this, vertex); }
  //: Depth first search end iterator
  depth_iterator depth_end()   { return depth_iterator(this, NULL); }

  //: Breadth first search begin iterator
  breadth_iterator breadth_begin(bgrl_vertex_sptr vertex) { return breadth_iterator(this, vertex); }
  //: Breadth first search end iterator
  breadth_iterator breadth_end()   { return breadth_iterator(this, NULL); }
};


//: Binary save bgrl_graph to stream.
void vsl_b_write(vsl_b_ostream &os, const bgrl_graph* g);

//: Binary load bgrl_graph from stream.
void vsl_b_read(vsl_b_istream &is, bgrl_graph* &g);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, bgrl_graph* g);


#endif // bgrl_graph_h_
