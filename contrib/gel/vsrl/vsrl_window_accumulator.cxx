#include <vsrl/vsrl_window_accumulator.h>
#include <vcl_iostream.h>
#include <vsrl/vsrl_parameters.h>

// constructor 

vsrl_window_accumulator::vsrl_window_accumulator()
{
  // initial values
  _in_mat=0;
  _out_mat=0;

  _win_width= (vsrl_parameters::instance()->correlation_window_width)/2; // probably 10
  _win_height= (vsrl_parameters::instance()->correlation_window_height)/2; // probably 10

  _current_row=0;
  _current_col=0;
}

// destructor
vsrl_window_accumulator::~vsrl_window_accumulator()
{
}

void vsrl_window_accumulator::set_in_matrix(vnl_matrix<double> *in_mat)
{
  // set the in matrix
  _in_mat=in_mat;
}


void vsrl_window_accumulator::set_out_matrix(vnl_matrix<double> *out_mat)
{
  // set the out matrix
  _out_mat=out_mat;
}


void vsrl_window_accumulator::set_window_width(int width)
{
  // the width of the accumulation window
  _win_width=width/2;
}

void vsrl_window_accumulator::set_window_height(int height)
{
  // the height of the accumulation window
  _win_height=height/2;
}


void vsrl_window_accumulator::execute()
{
  // run the accumulation 

  _current_row= 0-1;

  // the column sums 

  vnl_vector<double> vec(_in_mat->cols());
  double sum=0;
  double N;

  int num_row = (_in_mat->rows()-1);
  int num_col = (_in_mat->cols()-1);

  while((num_row >  _current_row)){

    // get the column sums 
    next_column_sums(vec);

    while(num_col > _current_col){

      // get the sum;
      sum = next_accumulation(vec,sum);

      // save the accumulation 
      N=num_samples(_current_row,_current_col);

      (*_out_mat)(_current_row, _current_col) = sum/N;
    }
  }
}


void vsrl_window_accumulator::initial_column_sums(vnl_vector<double> &vec)
{
  // we want to find the column sums for the first row of the matrix 
  // for the given window

  // set the current row and column to zero

  _current_row=0;
  _current_col=0-1;

  // find out how far to accumulate;

  unsigned int sum_length=_win_height;

  if(sum_length >= _in_mat->rows()){
    sum_length=_in_mat->rows()-1;
  }

  // initialize the vector of column sums 

  vec.fill(0.0);

  // start to accumulate

  int i,j;
  double sum;

  for(i=0;i<_in_mat->columns();i++){
    sum=0;
    for(j=0;j<=sum_length;j++){
      sum=sum+(*_in_mat)(j,i);
    }
    vec(i)=sum;
  }
}

void vsrl_window_accumulator::next_column_sums(vnl_vector<double> &vec)
{
  // using the column sums from the previous calculation, update the vec
  // by going to the next region

  if(_current_row<0){
    // initialize the column vectors
    initial_column_sums(vec);
    return;
  }

  _current_col=0-1;
  _current_row++;

  int add_row = _current_row + _win_height;
  int delete_row= _current_row - _win_height -1;

  int add_row_flag=1;
  int delete_row_flag=1;

  if(add_row>=_in_mat->rows()){
    add_row_flag=0;
  }

  if(delete_row <0){
    delete_row_flag=0;
  }

  // update the vector sums 

  for(int i=0;i<_in_mat->cols();i++){
    if(add_row_flag){
      vec(i)=vec(i) + (*_in_mat)(add_row,i);
    }
    if(delete_row_flag){
      vec(i)=vec(i) - (*_in_mat)(delete_row,i);
    }
  }
}

double vsrl_window_accumulator::initial_accumulation(vnl_vector<double> &vec)
{
  // perform an initial accumulation using the column sums

  _current_col=0;

  int sum_width=_win_width;
  if(sum_width >= _in_mat->cols()){
    sum_width = _in_mat->cols() - 1;
  }

  double sum=0;
  int i;
  for(i=0;i<= sum_width;i++){
    sum = sum+ vec(i);
  }

  return sum;
}

double vsrl_window_accumulator::next_accumulation(vnl_vector<double> &vec, double old_sum)
{
  if(_current_col<0){
    // initialize the accumulation
    return initial_accumulation(vec);
  }

  // update the accumulation using the old sum and the column sums 
  _current_col++;

  int add_col=_current_col + _win_width;
  int delete_col= _current_col - _win_width -1;

  if(add_col<_in_mat->cols()){
    old_sum=old_sum + vec(add_col);
  }
  if(delete_col >=0){
    old_sum=old_sum - vec(delete_col);
  }

  return old_sum;
}


double vsrl_window_accumulator::num_samples(int r, int c)
{
  // we want to find the number of samples in the region r,c

  int l1 = (c > _win_width) ? _win_width : c;
  int l2 = _in_mat->cols() -1 -c;
  if(l2 > _win_width) l2 = _win_width;

  double w = l1+l2+1;

  l1 = (r > _win_height) ? _win_height : r;
  l2 = _in_mat->rows() -1 -r;
  if(l2 > _win_height) l2 = _win_height;

  double h = l1+l2+1;

  return w*h;
}

