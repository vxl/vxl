// In a dynamic progamming problem we are given the task of 
// assigning one set of tokens to another set of tokens.
// This class represents the token.

#include <vsrl/vsrl_token.h>


// constructor

vsrl_token::vsrl_token()
{
  _index = 0-1;
  _assigned_token = 0;
  _x=0;
  _y=0;
}

// destructor
  
vsrl_token::~vsrl_token()
{
  
}

// get the index of this token
int vsrl_token::get_index()
{
  return _index;
}

// set the index of this token
void vsrl_token::set_index(int index)
{
  _index = index;
}

// get the assigned token 

vsrl_token* vsrl_token::get_assigned_token()
{
  return _assigned_token;
}

// set the assigned token 

void vsrl_token::set_assigned_token(vsrl_token *tok)
{
  _assigned_token = tok;
}

// set the position

void vsrl_token::set_position(double x, double y)
{
  _x=x;
  _y=y;
}

void vsrl_token::get_position(double &x, double &y)
{
  x= _x;
  y= _y;
}

//: what is the direct cost of assigning this token to tok
double vsrl_token::cost(vsrl_token *tok)
{
  return 0;
}

//: what is the incremental cost of going from from this -> tok2 given 
//: the assignment going from tok_p -> tok_2p

double vsrl_token::incremental_cost(vsrl_token *tok2, vsrl_token *tok_p, vsrl_token *tok_2p)
{
  return 0;
}




