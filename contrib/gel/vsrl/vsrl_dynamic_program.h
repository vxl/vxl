#ifndef vsrl_dynamic_program_h
#define vsrl_dynamic_program_h


// This class will solve a dynamic program which attempts
// to compute a set of assignments between two sets of tokens

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>

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

  void define_search_range();
    
  void define_search_range(vnl_vector<int > curr_row);



 protected:

 // this structure will keep track of possible assignments
  struct assignment_node{
    int index1; // the first token of this assignment
    int index2; // the second token of this assignment
    int prior_index1; // the first token of the prior assignment
    int prior_index2; // the second token of the prior assignment
    double cost; // the cost of this assignment
    double num_null1; // the number of consecutive null assignments for this assignment
  };

  token_list list1_; // the first set of tokens
  token_list list2_; // the second list of tokens

  double inner_cost_; // the cost of a missing inner assignment
  double outer_cost_; // the cost of a missing outer assignment
  double continuity_cost_; // the cost of a discontinuity
  int search_range_; // the search range for each token;

  vcl_vector<int> lower_search_range_; // the search range for each token
  vcl_vector<int> upper_search_range_; // in the future it could be specific



  assignment_node **cost_matrix_; // structure used to keep track of assignment costs
  int num_row_; // the number of rows in the matrix
  int num_col_; // the number of columns in the matrix

  vsrl_null_token *inner_null_token_;
  vsrl_null_token *outer_null_token_;

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
