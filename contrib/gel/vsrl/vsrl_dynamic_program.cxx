#include "vsrl_dynamic_program.h"
#include <vsrl/vsrl_token.h>
#include <vsrl/vsrl_null_token.h>
#include <vsrl/vsrl_parameters.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

// this class will perform a dynamic program

// constructor
vsrl_dynamic_program::vsrl_dynamic_program()
{
  // get the global parameters

  search_range_= vsrl_parameters::instance()->correlation_range; // probably 10
  inner_cost_= vsrl_parameters::instance()->inner_cost; // probably 1.0
  outer_cost_= vsrl_parameters::instance()->outer_cost; // probably 0.5
  continuity_cost_= vsrl_parameters::instance()->continuity_cost; // probably 0.1
  num_row_=0;
  num_col_=0;

  inner_null_token_ = new vsrl_null_token();
  outer_null_token_ = new vsrl_null_token();
  inner_null_token_->set_cost(inner_cost_);
  outer_null_token_->set_cost(outer_cost_);

  cost_matrix_=0;
}

// destructor
vsrl_dynamic_program::~vsrl_dynamic_program()
{
  delete inner_null_token_;
  delete outer_null_token_;

  // deallocate the matrix

  deallocate_cost_matrix();
}

void vsrl_dynamic_program::set_tokens(token_list &l1,
                                      token_list &l2)
{
  // set the tokens for the assignments
  if (l1.empty() || l2.empty())
  {
    vcl_cout << "warning empty list\n";
    return;
  }

  // the first list is a direct copy of l1
  list1_.clear();
  token_list::iterator ti;

  for (ti=l1.begin();ti!=l1.end();ti++)
  {
    list1_.push_back(*ti);
  }

  // the second list has alternating sets of tokens
  // from l2 and null tokens

  list2_.clear();
  // set the first token to an outer null token

  list2_.push_back(outer_null_token_);
  for (ti=l2.begin();ti!=l2.end();ti++)
  {
    // push the real token from l2
    list2_.push_back(*ti);
    // push an inner null token
    list2_.push_back(inner_null_token_);
  }

  // replace the last inner null token with an outer null token
  list2_[list2_.size()-1]=outer_null_token_;

  // set the number of rows and columns of the assignment matrix
  num_row_= list1_.size();
  num_col_= list2_.size();

  // set the search range for each token

  define_search_range();
}

// define the search range for the tokens in list1

void vsrl_dynamic_program::define_search_range()
{
  for (unsigned int i=0;i<list1_.size();i++)
  {
    int low=i*2+1 - 2*search_range_;
    int high=i*2+1 + 2*search_range_;

    if (low<0)
      low=0;
    if ((unsigned int)low>=list2_.size())
      low=list2_.size()-1;
    if (high<0)
      high=0;
    if ((unsigned int)high>list2_.size())
      high=list2_.size();

    lower_search_range_.push_back(low);
    upper_search_range_.push_back(high);
  }
}


// set the inner null cost
void vsrl_dynamic_program::set_inner_cost(double cost)
{
  inner_cost_ = cost;
  inner_null_token_->set_cost(cost);
}

// set the outer null cost
void vsrl_dynamic_program::set_outer_cost(double cost)
{
  outer_cost_ = cost;
  outer_null_token_->set_cost(cost);
}


// set the continuity cost
void vsrl_dynamic_program::set_continuity_cost(double cost)
{
  continuity_cost_ = cost;
}

// set the search range for each token
void vsrl_dynamic_program::set_search_range(int range)
{
  search_range_ = range;
}

// allocate the data matrix

bool vsrl_dynamic_program::allocate_cost_matrix()
{
  // we wish to make a two dimensional of assignment_nodes
  // to keep track of the assignment costs

  if (!num_row_ || !num_col_)
  {
    vcl_cout << "must set new token lists\n";
    cost_matrix_ = 0;
    return false;
  }

  cost_matrix_ = (assignment_node**)(malloc(num_row_ * sizeof(*cost_matrix_)));

  for (int i=0;i<num_row_;i++)
    cost_matrix_[i]=(assignment_node*)(malloc(num_col_ * sizeof(**cost_matrix_)));

  // initialize the matrix

  for (int i=0;i<num_row_;i++)
    for (int j=0;j<num_col_;j++)
    {
      cost_matrix_[i][j].index1=i;
      cost_matrix_[i][j].index2=j;
      cost_matrix_[i][j].prior_index1=0;
      cost_matrix_[i][j].prior_index2=0;
      cost_matrix_[i][j].cost=1000000;
      cost_matrix_[i][j].num_null1=0;
    }
  return true;
}

void vsrl_dynamic_program::deallocate_cost_matrix()
{
  // free the memory associated with matrix

  for (int i=0;i<num_row_;i++)
    free((char*)(cost_matrix_[i]));

  free((char*)cost_matrix_);

  cost_matrix_=0;
  num_row_=0;
  num_col_=0;
}

double vsrl_dynamic_program::execute()
{
  // we can now perform the dynamic program

  // allocate the cost matrix

  if (!allocate_cost_matrix())
    return 0.0;


  // compute the cost matrix that will allow us to
  // find the cheepest set of assignments

  for (int i=0;i<num_row_;i++)
    for (int j=lower_search_range_[i];j<upper_search_range_[i];j++)
      compute_cost(i,j);


  // we can now set the optimum assignment

  double cost= optimum_assignment();

  // print_assignments();

   return cost;
}

void vsrl_dynamic_program::compute_cost(int i, int j)
{
  // we wish to compute the cost between tokens i and j;

  // get these tokens
  vsrl_token *tok1 = list1_[i];
  vsrl_token *tok2 = list2_[j];


  // compute the direct cost of associating these tokens

  double direct_cost = tok1->cost(tok2);

  // special cases were token i is either the first or last token

  if (i==0)
  {
    // if j > 1 then we must consider all the unassigned tokens
    // in list2 that are below j;

    for (int k=0;k<j;k++)
    {
      if (!list2_[k]->null_token())
      {
        // this is a real token in list2 which must now
        // be allocated to the null assignment
        direct_cost = direct_cost + outer_cost_;
      }
    }
  }


  // this is the last token
  if (i+1==int(list1_.size()))
  {
    // this is the last token in list 1
    // so consider the rest of the tokens in list2 which
    // would now be doomed to a null assignment

    for (unsigned int k=j+1;k<list2_.size();k++)
    {
      if (!list2_[k]->null_token())
      {
        // this is a real token in list2 which must now
        // be allocated to the null assignment
        direct_cost = direct_cost + outer_cost_;
      }
    }
  }

  //  we can now compute the minimum prior cost

  double new_num_null1 = 0;
  double prior_cost = 0;

  int p_i =i-1;
  int p_j =0;

  if (i>0)
  {
    // determine the upper search limit on list 2
    // the key is that since the assignments must
    // be increasing, we must have an assignment below
    // j there for

    int upper_j = j;
    if (tok2->null_token())
    {
      // however it appears that token j is the null token
      // which can have mulltiple assignments so we will
      // increment upper_j;

      upper_j++;
    }

    // initiallly set the prior cost really high
    prior_cost = 100000;

    // the number of null assignments in a row

    double num_null1;

    // now compute the prior cost;

    for (int k=lower_search_range_[p_i];k<upper_j;k++)
    {
      // this is the cost of the prior assignmentt

      double cost = cost_matrix_[p_i][k].cost;

      // in some cases the geometry of the prior set of assignments
      // is important so we must consider this situation

      cost = cost + tok1->incremental_cost(tok2,list1_[p_i],list2_[k]);


      // compute the penalty for assigning too many tokens the same null assignment

      if (j==k)
      {
        // we have yet another null assignment so using the number of
        // consecuative null assignmens for the node p_i->k we can compute
        // how many consecuative null assignments now exist

        num_null1=cost_matrix_[p_i][k].num_null1 + 1;
      }
      else
      {
        // there is no continuity of null assignments but this
        // could be the start of a new gap
        //
        if (list2_[j]->null_token())
        {
          // this is the the start of a new gap
          num_null1=1;
        }
        else
        {
          // this is a valid token
          num_null1=0;
        }
      }


      // add the cost for this gap  - note that as the number of
      // consecuative null assignments increases, the penalty becomes more
      // and more severe

      cost = cost + num_null1 * continuity_cost_;


      // if there is a gap between k and j then all these
      // tokens in list will get the null assignment so  this
      // must be compensated for. We must also penalize the second
      // kind of gap

      double num_null2=0;

      for (int l=k+1;l<j;l++)
      {
        if (!list2_[l]->null_token())
        {
          // this is a real token in list2 which must now
          // be allocated to the null assignment
          cost = cost + inner_cost_;
          num_null2++;
          cost=cost+num_null2 * continuity_cost_;
        }
      }

      if (cost<prior_cost)
      {
        // we have a new prior cost
        prior_cost = cost;
        p_j=k;
        new_num_null1=num_null1;
      }
    }
  }


  // we can now determine the cost of assigning node i to node j

  cost_matrix_[i][j].cost= direct_cost + prior_cost;
  cost_matrix_[i][j].num_null1 = new_num_null1;

  // record the prior assignments used to compute cost

  cost_matrix_[i][j].prior_index1=p_i;
  cost_matrix_[i][j].prior_index2=p_j;
}

double vsrl_dynamic_program::optimum_assignment()
{
  // we can now compute the optimum assignment
  // this is pretty easy we just find the lowest
  // cost assignment for the last token in list 2
  // and then follow the path back.
  // start by setting the assigned tokens of all lists to null;

  token_list::iterator ti;
  for (ti=list1_.begin();ti!=list1_.end();ti++)
    (*ti)->set_assigned_token(0);

  for (ti=list2_.begin();ti!=list2_.end();ti++)
    (*ti)->set_assigned_token(0);

  // now find the best assignment for token list1
  vsrl_token *tok1;
  vsrl_token *tok2;

  int i = num_row_-1;
  int j=0;
  double min_cost=1000000;

  for (int k=0;k<num_col_;k++)
  {
    if (cost_matrix_[i][k].cost < min_cost)
    {
      min_cost = cost_matrix_[i][k].cost;
      j=k;
    }
  }

  // Ok we know were to start so lets snake down the
  // optimal path

  double total_cost=0;

  while (i>=0)
  {
    // add the total cost
    total_cost = total_cost + cost_matrix_[i][j].cost;
    tok1= list1_[i];
    tok2= list2_[j];

    if (!tok2->null_token())
    {
      // this is a real assignment so assign it
      tok1->set_assigned_token(tok2);
      tok2->set_assigned_token(tok1);
    }

    // get the next j;

    j=cost_matrix_[i][j].prior_index2;

    i=i-1;
  }

  // ok we are done

  return total_cost;
}


// Some tools for diagnosis

void vsrl_dynamic_program::print_direct_cost(int i, int j)
{
  // we want to print the direct cost between matcjing token i and j

  assert(i>=0);
  assert(j>=0);
  if ((unsigned int)i < list1_.size() && (unsigned int)j< list2_.size())
  {
    vsrl_token *tok1 = list1_[i];
    vsrl_token *tok2 = list2_[j];

    double direct_cost = tok1->cost(tok2);

    vcl_cout << "Direct cost " << i << " -> " << j << " " << direct_cost <<'\n';
  }
}


void vsrl_dynamic_program::print_cost(int i, int j)
{
  // we want to print the direct cost between matcjing token i and j

  assert(i>=0);
  assert(j>=0);
  if ((unsigned int)i < list1_.size() && (unsigned int)j< list2_.size())
    vcl_cout << "cost "<< i <<" -> "<< j <<" "<<cost_matrix_[i][j].cost <<'\n';
}

void vsrl_dynamic_program::print_direct_costs(int i)
{
  // print all the direct costs for token i on list1
  vcl_cout << "Direct costs for token " << i << vcl_endl;
  for (int j=lower_search_range_[i];j<upper_search_range_[i];j++)
    print_direct_cost(i,j);
}

void vsrl_dynamic_program::print_costs(int i)
{
  // print all the costs for token i on list1
  vcl_cout << "Printing the costs for token " << i << " in the range " << lower_search_range_[i]
           << " to " << upper_search_range_[i] << vcl_endl;

  for (int j=lower_search_range_[i];j<upper_search_range_[i];j++)
    print_cost(i,j);
}

void vsrl_dynamic_program::print_assignment_nodes(int i)
{
  for (int j=lower_search_range_[i];j<upper_search_range_[i];j++)
    print_assignment_node(i,j);
}


void vsrl_dynamic_program::print_assignment_node(int i, int j)
{
  vcl_cout << " assignment for node " << i << " -> " << j << vcl_endl
           << "   prior assignment: " << cost_matrix_[i][j].prior_index1 << " -> "
           << cost_matrix_[i][j].prior_index2 << vcl_endl
           << "   cost: " << cost_matrix_[i][j].cost << vcl_endl << vcl_endl;
}

void vsrl_dynamic_program::print_assignment(int i)
{
  // we want to print the assignment for the i'th token

  vcl_cout << "Token " << i << " goes to ";
  vsrl_token *tok1 = (list1_[i]);
  vsrl_token *tok2 = tok1->get_assigned_token();
  if (tok2)
    vcl_cout << "token " << tok2->get_x() << vcl_endl;
  else
    vcl_cout << "the null token\n";
}

void vsrl_dynamic_program::print_assignments()
{
  for (unsigned int j=0;j<list1_.size();j++)
    print_assignment(j);
}
