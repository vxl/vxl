// This is gel/vsrl/vsrl_raster_dp_setup.cxx
#include "vsrl_raster_dp_setup.h"
//:
// \file
#include <vsrl/vsrl_dynamic_program.h>
#include <vsrl/vsrl_parameters.h>
#include <vsrl/vsrl_token.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>

// constructor
vsrl_raster_dp_setup::vsrl_raster_dp_setup(int raster_line, vsrl_image_correlation *image_correlation)
{
  tok_list1.clear();
  tok_list2.clear();
  raster_line_ = raster_line;
  image_correlation_ = image_correlation;
  search_range_=image_correlation->get_correlation_range();
  prior_raster_=0;
  bias_cost_=vsrl_parameters::instance()->bias_cost; // probably 0.2
  inner_cost_=1.0;
  outer_cost_=0.5;
}


// destructor
vsrl_raster_dp_setup::~vsrl_raster_dp_setup()
{
  clear_token_list(tok_list1);
  clear_token_list(tok_list2);
}


int vsrl_raster_dp_setup::get_image1_width()
{
  return image_correlation_->get_image1_width();
}


int vsrl_raster_dp_setup::get_image2_width()
{
  return image_correlation_->get_image2_width();
}


int vsrl_raster_dp_setup::get_assignment(int x)
{
  if (x<0 || (unsigned int)x >= tok_list1.size())
  {
    return -1;
  }

  vsrl_token *tok2= (tok_list1[x]->get_assigned_token());

  if (!tok2)
    return -1;
  else
    return (int)(tok2->get_x());
}


void vsrl_raster_dp_setup::create_token_list(int width,
                                             vsrl_image_correlation *image_correlation,
                                             vcl_vector<vsrl_intensity_token*> &tok_list,
                                             double /*step*/)
{
  // we wish to create the token lists

  clear_token_list(tok_list);

  //  get the number of tokens that are required


  double x,y;
  y= raster_line_;
  int index = 0;

  for (x=0;x<width;x++)
  {
    // create the intensity token

    vsrl_intensity_token *tok = new vsrl_intensity_token();

    // set the image correlation

    tok->set_image_correlation(image_correlation);

    // set the position

    tok->set_position(x,y);

    tok->set_index(index);
    index++;

    // put the token on the list
    tok_list.push_back(tok);
  }
}


void vsrl_raster_dp_setup::clear_token_list(vcl_vector<vsrl_intensity_token*> &tok_list)
{
  // clear the token list

  vcl_vector<vsrl_intensity_token*>::iterator i;

  for (i=tok_list.begin();i<tok_list.end();i++)
  {
    delete *i;
  }
  tok_list.clear();
}


double vsrl_raster_dp_setup::execute()
{
  // create the token lists

  create_token_list(get_image1_width(),image_correlation_,tok_list1,1.0);
  create_token_list(get_image2_width(),0,tok_list2,1.0);

  // set the bias information for token list1 based the previous raster
  set_token_biases();

  // create the right kind of list

  vcl_vector<vsrl_token*> list1;
  vcl_vector<vsrl_token*> list2;

  vcl_vector<vsrl_intensity_token*>::iterator i;

  for (i=tok_list1.begin();i<tok_list1.end();i++)
  {
    list1.push_back(*i);
  }

  for (i=tok_list2.begin();i<tok_list2.end();i++)
  {
    list2.push_back(*i);
  }


  // perform the dynamic program

  vsrl_dynamic_program dyn_prog;

  if (search_range_)
  {
    dyn_prog.set_search_range(search_range_);
  }

  dyn_prog.set_tokens(list1,list2);
  dyn_prog.define_search_range();

  dyn_prog.set_inner_cost(inner_cost_);
  dyn_prog.set_inner_cost(outer_cost_);
  double total_cost = dyn_prog.execute();

  vcl_cout << " Total cost is " << total_cost << vcl_endl;

  // dyn_prog.print_direct_costs(375);
  // dyn_prog.print_costs(375);

  return total_cost;
}


double vsrl_raster_dp_setup::execute(vnl_vector<int > curr_row)
{
  // create the token lists

  create_token_list(get_image1_width(),image_correlation_,tok_list1,1.0);
  create_token_list(get_image2_width(),0,tok_list2,1.0);

  // set the bias information for token list1 based the previous raster
  set_token_biases();

  // create the right kind of list

  vcl_vector<vsrl_token*> list1;
  vcl_vector<vsrl_token*> list2;

  vcl_vector<vsrl_intensity_token*>::iterator i;

  for (i=tok_list1.begin();i<tok_list1.end();i++)
  {
    list1.push_back(*i);
  }

  for (i=tok_list2.begin();i<tok_list2.end();i++)
  {
    list2.push_back(*i);
  }


  // perform the dynamic program

  vsrl_dynamic_program dyn_prog;

  if (search_range_)
  {
    dyn_prog.set_search_range(search_range_);
  }

  dyn_prog.set_tokens(list1,list2);
  dyn_prog.define_search_range(curr_row);

  double total_cost = dyn_prog.execute();

  vcl_cout << " Total cost is " << total_cost << vcl_endl;

  // dyn_prog.print_direct_costs(375);
  // dyn_prog.print_costs(375);

  return total_cost;
}


void vsrl_raster_dp_setup::set_search_range(int range)
{
  search_range_=range;
}

void vsrl_raster_dp_setup::set_prior_raster(vsrl_raster_dp_setup *prior_raster)
{
  prior_raster_=prior_raster;
}

void vsrl_raster_dp_setup::set_bias_cost(double bias_cost)
{
  bias_cost_=bias_cost;
}


void vsrl_raster_dp_setup::set_token_biases()
{
  // the idea here is to set the bias for each token based on the
  // the assignments made in the prior raster

  if (!prior_raster_)
  {
    // no prior information
    return;
  }

  int y1=this->get_raster_line();
  int y2=prior_raster_->get_raster_line();


  vcl_vector<vsrl_intensity_token*>::iterator i;

  double bias_diff = vsrl_parameters::instance()->common_intensity_diff;

  for (i=tok_list1.begin();i<tok_list1.end();i++)
  {
    // find the position of i'th token

    int x = (int)((*i)->get_x());

    // if the intensity values are the same, set the bias function

    double int_diff = image_correlation_->get_image_value1(x,y1) - image_correlation_->get_image_value1(x,y2);

    if (int_diff*int_diff <= bias_diff*bias_diff)
    {
      int bias = prior_raster_->get_assignment(x);
      if (bias>0)
      {
        // we can now set the bias for token i
        (*i)->set_bias(bias);
        (*i)->set_bias_cost(bias_cost_);
      }
    }
  }

  // set the first and last pixel bias to 0
  (tok_list1[0])->set_bias(0);
  (tok_list1[0])->set_bias_cost(100.0);
  int last_tok = tok_list1.size() - 1;
  (tok_list1[last_tok])->set_bias(0);
  (tok_list1[last_tok])->set_bias_cost(100.0);
}


//: get the raster line that this dynamic program uses
int vsrl_raster_dp_setup::get_raster_line()
{
  return raster_line_;
}
