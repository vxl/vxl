#ifndef vsrl_dynamic_program_h
#define vsrl_dynamic_program_h


// This class will solve a dynamic program which attempts
// to compute a set of assignments between two sets of tokens

#include <vcl_vector.h>
class vsrl_token;
class vsrl_null_token;

typedef  vcl_vector<vsrl_token*> token_list;


class vsrl_dynamic_program
{
 public:
  // constructor
  vsrl_dynamic_program();

  // destructor
  ~vsrl_dynamic_program();

  // set the token lists

  void set_tokens(token_list &l1,
                  token_list &l2);
  // set the inner null cost
  void set_inner_cost(double cost);

  // set the outer cost null cost
  void set_outer_cost(double cost);

  // set the continuity cost
  void set_continuity_cost(double cost);

  // set the search range for each token

  void set_search_range(int range);

  // execute the dynamic program

  double execute();

 private:

 // this structure will keep track of posible assignments
  struct assignment_node{
    int index1; // the first token of this assignment
    int index2; // the second token of this assigment
    int prior_index1; // the first token of the prior assignment
    int prior_index2; // the second token of the prior assignment
    double cost; // the cost of this assignment
    double num_null1; // the number of consecuative null assignments for this assignment
  };

  token_list _list1; // the first set of tokens
  token_list _list2; // the second list of tokens

  double _inner_cost; // the cost of a missing inner assignment
  double _outer_cost; // the cost of a missing outer assignment
  double _continuity_cost; // the cost of a discontinuity
  int _search_range; // the search range for each token;

  vcl_vector<int> _lower_search_range; // the search range for each token
  vcl_vector<int> _upper_search_range; // in the future it could be specific

  void define_search_range();


  assignment_node **_cost_matrix; // structure used to keep track of assignment costs
  int _num_row; // the number of rows in the matrix
  int _num_col; // the number of columns in the matrix

  vsrl_null_token *_inner_null_token;
  vsrl_null_token *_outer_null_token;

  // private functions
  // allocate the matrix
  bool allocate_cost_matrix();

  // deallocate the matrix
  void deallocate_cost_matrix();


  // compute the cost of tokens and j
  void compute_cost(int i, int j);

  // compute the optimal assignment
  double optimum_assignment();

  // **** some tools for analysis  ****
 public:

  void print_direct_cost(int i, int j);
  void print_cost(int i, int j);

  void print_direct_costs(int i);
  void print_costs(int i);
  void print_assignment_node(int i, int j);
  void print_assignment_nodes(int i);

  void print_assignment(int i);
  void print_assignments();
};

#endif
