#include <vsrl/vsrl_token.h>
#include <vsrl/vsrl_null_token.h>
#include <vsrl/vsrl_dynamic_program.h>
#include <vsrl/vsrl_parameters.h>

// this class will perform a dynamic program

// constructor
vsrl_dynamic_program::vsrl_dynamic_program()
{
  // get the global parameters

  _search_range= vsrl_parameters::instance()->correlation_range; // probably 10
  _inner_cost= vsrl_parameters::instance()->inner_cost; // probably 1.0
  _outer_cost= vsrl_parameters::instance()->outer_cost; // probably 0.5
  _continuity_cost= vsrl_parameters::instance()->continuity_cost; // probably 0.1
  _num_row=0;
  _num_col=0;

  _inner_null_token = new vsrl_null_token();
  _outer_null_token = new vsrl_null_token();
  _inner_null_token->set_cost(_inner_cost);
  _outer_null_token->set_cost(_outer_cost);

  _cost_matrix=0;
}

// destructor
vsrl_dynamic_program::~vsrl_dynamic_program()
{
  delete _inner_null_token;
  delete _outer_null_token;

  // deallocate the matrix

  deallocate_cost_matrix();
}

void vsrl_dynamic_program::set_tokens(token_list &l1,
                                      token_list &l2)
{
  // set the tokens for the assignments
  if(l1.empty() || l2.empty())
  {
    vcl_cout << "warning empty list" << vcl_endl;
    return;
  }

  // the first list is a direct copy of l1
  _list1.clear();
  token_list::iterator ti;

  for(ti=l1.begin();ti!=l1.end();ti++)
  {
    _list1.push_back(*ti);
  }

  // the second list has alternating sets of tokens
  // from l2 and null tokens

  _list2.clear();
  // set the first token to an outer null token

  _list2.push_back(_outer_null_token);
  for(ti=l2.begin();ti!=l2.end();ti++)
  {
    // push the real token from l2
    _list2.push_back(*ti);
    // push an inner null token
    _list2.push_back(_inner_null_token);
  }

  // replace the last inner null token with an outer null token
  _list2[_list2.size()-1]=_outer_null_token;

  // set the number of rows and columns of the assignment matrix
  _num_row= _list1.size();
  _num_col= _list2.size();

  // set the search range for each token

  define_search_range();
}

// define the search range for the tokens in list1

void vsrl_dynamic_program::define_search_range()
{
  for(unsigned int i=0;i<_list1.size();i++)
  {
    int low=i*2+1 - 2*_search_range;
    int high=i*2+1 + 2*_search_range;

    if(low<0)
    {
      low=0;
    }
    if(low>=_list2.size())
    {
      low=_list2.size()-1;
    }
    if(high<0)
    {
      high=0;
    }
    if(high>_list2.size())
    {
      high=_list2.size();
    }

    _lower_search_range.push_back(low);
    _upper_search_range.push_back(high);
  }
}


// set the inner null cost
void vsrl_dynamic_program::set_inner_cost(double cost)
{
  _inner_cost = cost;
  _inner_null_token->set_cost(cost);
}

// set the outer null cost
void vsrl_dynamic_program::set_outer_cost(double cost)
{
  _outer_cost = cost;
  _outer_null_token->set_cost(cost);
}


// set the continuity cost
void vsrl_dynamic_program::set_continuity_cost(double cost)
{
  _continuity_cost = cost;
}

// set the search range for each token
void vsrl_dynamic_program::set_search_range(int range)
{
  _search_range = range;
}

// allocate the data matrix

bool vsrl_dynamic_program::allocate_cost_matrix()
{
  // we wish to make a two dimensional of assignement_nodes
  // to keep track of the assignment costs

  if(!_num_row || !_num_col)
  {
    vcl_cout << "must set new token lists" << vcl_endl;
    _cost_matrix = 0;
    return false;
  }

  _cost_matrix = (assignment_node**)(malloc(_num_row * sizeof(*_cost_matrix)));

  int i,j;
  for(i=0;i<_num_row;i++)
  {
    _cost_matrix[i]=(assignment_node*)(malloc(_num_col * sizeof(**_cost_matrix)));
  }

  // initialize the matrix

  for(i=0;i<_num_row;i++)
  {
    for(j=0;j<_num_col;j++)
    {
      _cost_matrix[i][j].index1=i;
      _cost_matrix[i][j].index2=j;
      _cost_matrix[i][j].prior_index1=0;
      _cost_matrix[i][j].prior_index2=0;
      _cost_matrix[i][j].cost=1000000;
      _cost_matrix[i][j].num_null1=0;
    }
  }
  return true;
}

void vsrl_dynamic_program::deallocate_cost_matrix()
{
  // free the memory associated with matrix

  int i;
  for(i=0;i<_num_row;i++)
  {
    free((char*)(_cost_matrix[i]));
  }

  free((char*)_cost_matrix);

  _cost_matrix=0;
  _num_row=0;
  _num_col=0;
}

double vsrl_dynamic_program::execute()
{
  // we can now perfrom the dynamic program

  // allocate the cost matrix

  if(!allocate_cost_matrix())
  {
    return 0.0;
  }


  // compute the cost matrix that will allow us to
  // find the cheepest set of assignments

  int i,j;
  for(i=0;i<_num_row;i++)
  {
    for(j=_lower_search_range[i];j<_upper_search_range[i];j++)
    {
       compute_cost(i,j);
    }
  }


  // we can now set the optimum assignment

  double cost= optimum_assignment();

  // print_assignments();

   return cost;
}

void vsrl_dynamic_program::compute_cost(int i, int j)
{
  // we wish to compute the cost between tokens i and j;

  // get these tokens
  vsrl_token *tok1 = _list1[i];
  vsrl_token *tok2 = _list2[j];


  // compute the direct cost of associating these tokens

  double direct_cost = tok1->cost(tok2);

  // special cases were token i is either the first or last token

  int k,l;
  if(i==0)
  {
    // if j > 1 then we must consider all the unassigned tokens
    // in list2 that are below j;

    for(k=0;k<j;k++)
    {
      if(!_list2[k]->null_token())
      {
        // this is a real token in list2 which must now
        // be allocated to the null assignment
        direct_cost = direct_cost + _outer_cost;
      }
    }
  }


  // this is the last token
  if(i==_list1.size()-1)
  {
    // this is the last token in list 1
    // so consider the rest of the tokens in list2 which
    // would now be doomed to a null assignment

    for(k=j+1;k<_list2.size();k++)
    {
      if(!_list2[k]->null_token())
      {
        // this is a real token in list2 which must now
        // be allocated to the null assignment
        direct_cost = direct_cost + _outer_cost;
      }
    }
  }

  //  we can now compute the minimum prior cost

  double new_num_null1 = 0;
  double prior_cost = 0;

  int p_i =i-1;
  int p_j =0;

  if(i>0)
  {
    // determine the upper search limit on list 2
    // the key is that since the assignements must
    // be increasing, we must have an assignment below
    // j there for

    int upper_j = j;
    if(tok2->null_token())
    {
      // however it appears that token j is the null token
      // which can have mulltiple assignments so we will
      // increment upper_j;

      upper_j++;
    }

    // initiallly set the prior cost really high
    prior_cost = 100000;

    // the number of null assignments in a row

    double num_null1=0;

    // now compute the prior cost;

    for(k=_lower_search_range[p_i];k<upper_j;k++)
    {
      // this is the cost of the prior assignmentt

      double cost = _cost_matrix[p_i][k].cost;

      // compute the penalty for assigning too many tokens the same null assignment

      if(j==k)
      {
        // we have yet another null assignment so using the number of
        // consecuative null assignmens for the node p_i->k we can compute
        // how many consecuative null assignments now exist

        num_null1=_cost_matrix[p_i][k].num_null1 + 1;
      }
      else
      {
        // there is no continuity of null assignments but this
        // could be the start of a new gap
        //
        if(_list2[j]->null_token())
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

      cost = cost + num_null1 * _continuity_cost;




      // if there is a gap between k and j then all these
      // tokens in list will get the null assignement so  this
      // must be compensated for. We must also penalize the second
      // kind of gap

      double num_null2=0;

      for(l=k+1;l<j;l++)
      {
        if(!_list2[l]->null_token())
        {
          // this is a real token in list2 which must now
          // be allocated to the null assignment
          cost = cost + _inner_cost;
          num_null2++;
          cost=cost+num_null2 * _continuity_cost;
        }
      }

      if(cost<prior_cost)
      {
        // we have a new prior cost
        prior_cost = cost;
        p_j=k;
        new_num_null1=num_null1;
      }
    }
  }


  // we can now determine the cost of assigning node i to node j

  _cost_matrix[i][j].cost= direct_cost + prior_cost;
  _cost_matrix[i][j].num_null1 = new_num_null1;

  // record the prior assignments used to compute cost

  _cost_matrix[i][j].prior_index1=p_i;
  _cost_matrix[i][j].prior_index2=p_j;
}

double vsrl_dynamic_program::optimum_assignment()
{
  // we can now compute the optimum assignment
  // this is pretty easy we just find the lowest
  // cost assignement for the last token in list 2
  // and then follow the path back.
  // start by setting the assigned tokens of all lists to null;



  token_list::iterator ti;
  for(ti=_list1.begin();ti!=_list1.end();ti++)
  {
    (*ti)->set_assigned_token(0);
  }

  for(ti=_list2.begin();ti!=_list2.end();ti++)
  {
    (*ti)->set_assigned_token(0);
  }

  // now find the best assignment for token list1
  vsrl_token *tok1;
  vsrl_token *tok2;

  int i = _num_row-1;
  int j=0;
  int k;
  double min_cost=1000000;

  for(k=0;k<_num_col;k++)
  {
    if(_cost_matrix[i][k].cost < min_cost)
    {
      min_cost = _cost_matrix[i][k].cost;
      j=k;
    }
  }

  // Ok we know were to start so lets snake down the
  // optimal path

  double total_cost=0;

  while(i>=0)
  {
    // add the total cost
    total_cost = total_cost + _cost_matrix[i][j].cost;
    tok1= _list1[i];
    tok2= _list2[j];

    if(!tok2->null_token())
    {
      // this is a real assignment so assign it
      tok1->set_assigned_token(tok2);
      tok2->set_assigned_token(tok1);
    }

    // get the next j;

    j=_cost_matrix[i][j].prior_index2;

    i=i-1;
  }



  // ok we are done

  return total_cost;
}


// Some tools for diagnosis

void vsrl_dynamic_program::print_direct_cost(int i, int j)
{
  // we want to print the direct cost between matcjing token i and j

  if(i>=0 && i < _list1.size() && j>=0 && j< _list2.size())
  {
    vsrl_token *tok1 = _list1[i];
    vsrl_token *tok2 = _list2[j];

    double direct_cost = tok1->cost(tok2);

    vcl_cout << "Direct cost " << i << " -> " << j << " " << direct_cost << vcl_endl;
  }
}


void vsrl_dynamic_program::print_cost(int i, int j)
{
  // we want to print the direct cost between matcjing token i and j

  if(i>=0 && i < _list1.size() && j>=0 && j< _list2.size())
  {
    vcl_cout << "cost " << i << " -> " << j << " " << _cost_matrix[i][j].cost << vcl_endl;
  }
}

void vsrl_dynamic_program::print_direct_costs(int i)
{
  // print all the direct costs for token i on list1

  int j;
  vcl_cout << "Direct costs for token " << i << vcl_endl;
  for(j=_lower_search_range[i];j<_upper_search_range[i];j++)
  {
    print_direct_cost(i,j);
  }
}

void vsrl_dynamic_program::print_costs(int i)
{
  // print all the costs for token i on list1

  vcl_cout << "Printing the costs for token " << i << " in the range " << _lower_search_range[i]
           << " to " << _upper_search_range[i] << vcl_endl;

  int j;
  for(j=_lower_search_range[i];j<_upper_search_range[i];j++)
  {
    print_cost(i,j);
  }
}

void vsrl_dynamic_program::print_assignment_nodes(int i)
{
  int j;
  for(j=_lower_search_range[i];j<_upper_search_range[i];j++)
  {
    print_assignment_node(i,j);
  }
}


void vsrl_dynamic_program::print_assignment_node(int i, int j)
{
  vcl_cout << " assignement for node " << i << " -> " << j << vcl_endl;
  vcl_cout << "   prior assignment: " << _cost_matrix[i][j].prior_index1 << " -> ";
  vcl_cout << _cost_matrix[i][j].prior_index2 << vcl_endl;
  vcl_cout << "   cost: " << _cost_matrix[i][j].cost << vcl_endl;
  vcl_cout << vcl_endl;
}

void vsrl_dynamic_program::print_assignment(int i)
{
  // we want to print the assignment for the i'th token

  vcl_cout << "Token " << i << " goes to ";
  vsrl_token *tok1 = (_list1[i]);
  vsrl_token *tok2 = tok1->get_assigned_token();
  if(tok2)
  {
    vcl_cout << "token " << tok2->get_x() << vcl_endl;
  }
  else
  {
    vcl_cout << "the null token" << vcl_endl;
  }
}

void vsrl_dynamic_program::print_assignments()
{
  int j;
  for(j=0;j<_list1.size();j++)
  {
    print_assignment(j);
  }
}
