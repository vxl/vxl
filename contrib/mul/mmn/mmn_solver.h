#ifndef  mmn_solver_h_
#define mmn_solver_h_
//:
// \file
// \brief Base for classes which solve Markov Random Field problems
// \author Tim Cootes

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mmn/mmn_arc.h>
#include <vnl/vnl_fwd.h>

#include <vsl/vsl_binary_io.h>
#include <cassert>

//: Base for classes which solve Markov Random Field problems.
class mmn_solver
{
protected:

public:

    //: Dflt ctor
  mmn_solver();

    //: Destructor
  virtual ~mmn_solver();

    //: Input the arcs that define the graph
  virtual void set_arcs(unsigned num_nodes,
                        const std::vector<mmn_arc>& arcs) = 0;

  //: Find values for each node with minimise the total cost
  //  \param node_cost: node_cost[i][j] is cost of selecting value j for node i
  //  \param pair_cost: pair_cost[a](i,j) is cost of selecting values (i,j) for nodes at end of arc a.
  //  \param x: On exit, x[i] gives choice for node i
  // NOTE: If arc a connects nodes v1,v2, the associated pair_cost is ordered
  // with the node with the lowest index being the first parameter.  Thus if
  // v1 has value i1, v2 has value i2, then the cost of this choice is
  // (v1<v2?pair_cost(i1,i2):pair_cost(i2,i1))
  // Returns the minimum cost
  virtual double solve(
                 const std::vector<vnl_vector<double> >& node_cost,
                 const std::vector<vnl_matrix<double> >& pair_cost,
                 std::vector<unsigned>& x) =0;

  //: Initialise from a text stream
  virtual bool set_from_stream(std::istream &is);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual std::string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual mmn_solver* clone() const = 0;

    //: Print class to os
  virtual void print_summary(std::ostream& os) const =0;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

  static std::unique_ptr<mmn_solver>
    create_from_stream(std::istream &is);
};

  //: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mmn_solver& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mmn_solver& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mmn_solver& b);

  //: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mmn_solver& b);

  //: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const mmn_solver* b);

#endif
