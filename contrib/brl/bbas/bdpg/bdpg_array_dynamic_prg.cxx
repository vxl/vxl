//:
// \file
#include <vnl/vnl_numeric_traits.h>
#include <bdpg/bdpg_array_dynamic_prg.h>

bdpg_array_dynamic_prg::bdpg_array_dynamic_prg(const unsigned rows, const unsigned cols)
{
  num_rows_ = rows;
  num_cols_ = cols;
  array_.resize(rows, cols);
}

//:find if an old column exists in the path at (row, col).
bool bdpg_array_dynamic_prg::find(const unsigned row, const unsigned col, const unsigned old_col)
{
  if (col==old_col)
    return true;
  bdpg_node_sptr n = array_[row][col];
  if (!n)
    return false;
  int prow = n->previous_row();
  if (prow<0)
    return false;
  //trace out the path
  int temp = old_col;
  while (prow>=0)
  {
    int pcol = n->previous_col();
    if (temp == pcol)
      return true;
    n = array_[prow] [pcol];
    prow = n->previous_row();
  }
  return false;
}

void bdpg_array_dynamic_prg::print_path(unsigned row, unsigned col)
{
  bdpg_node_sptr n = array_[row][col];
  if (!n)
  {
    vcl_cout << "NULL PATH\n";
    return;
  }
  vcl_cout << "PATH\n";
  vcl_cout << '(' << row << ' ' << col << ")\n";
  int prow = n->previous_row();
  if (prow<0)
    return;
  //trace out the path
  while (prow>=0)
  {
    int pcol = n->previous_col();
    vcl_cout << '(' << prow << ' ' << pcol << ")\n";
    n = array_[prow] [pcol];
    prow = n->previous_row();
  }
}

//: find the column index that is most probable in the previous row
bool bdpg_array_dynamic_prg::maxp(unsigned row, unsigned col, int & best_col)
{
  best_col = -1;
  if (!row)
    return false;
  unsigned rm = row-1;
  double log_pmax = -vnl_numeric_traits<double>::maxval;
  for (unsigned c = 0; c<num_cols_; ++c)
  {
    if (this->find(rm, c, col))
      continue;
    bdpg_node_sptr n = array_[rm][c];
    if (!n)
      return false;
    if (n->log_p()>log_pmax)
    {
      log_pmax = n->log_p();
      best_col = c;
    }
  }
  vcl_cout << '(' << row << ' ' << col << ")->(" << rm << ' '
           << best_col << "): " << log_pmax << '\n';
  return true;
}

bool bdpg_array_dynamic_prg::scan()
{
  for (unsigned r = 1; r<num_rows_; ++r)
    for (unsigned c = 0; c<num_cols_; ++c)
    {
      bdpg_node_sptr n = array_[r][c];
      if (!n)
      return false;
      if (n->p() > 0)
      {
        int pcol = 0;
        if (!this->maxp(r, c, pcol))
          return false;//program fails
        //it may be that the current row can't be assigned to c
        //pcol<0 indicates that no path exists in the previous row
        //that doesn't already have c on it.
        if (pcol<0)
        {
          n->update_prob(0.0);
          continue;
        }
        n->set_previous_row(r-1);
        n->set_previous_col(pcol);
        bdpg_node_sptr m = array_[r-1][pcol];
        n->update_log_prob(m->log_p());
      }
    }
  return true;
}

unsigned bdpg_array_dynamic_prg::best_assign_col()
{
  unsigned best_col = 0;
  double lpmax = -vnl_numeric_traits<double>::maxval;
  for (unsigned c = 0; c<num_cols_; ++c)
  {
    bdpg_node_sptr n = array_[num_rows_-1][c];
    if (!n)
      continue;
    double lp = n->log_p();
    if (lp>lpmax)
    {
      lpmax = lp;
      best_col = c;
    }
  }
  return best_col;
}

vcl_vector<unsigned> bdpg_array_dynamic_prg::assignment()
{
  //each row must be assigned to some column.
  vcl_vector<unsigned> path(num_rows_,0);
  unsigned col = this->best_assign_col();
  path[num_rows_-1]=col;
  bdpg_node_sptr n = array_[num_rows_-1][col];
  if (!n)
    return path;
  int prow = n->previous_row();
  if (prow<0)
    return path;
  //trace out the path
  while (prow>=0)
  {
    int pcol = n->previous_col();
    path[prow]=pcol;
    n = array_[prow] [pcol];
    prow = n->previous_row();
  }
  return path;
}

void bdpg_array_dynamic_prg::print_array()
{
  vcl_cout << "Dynamic program array\n";
  for (unsigned r=0; r<num_rows_; ++r)
  {
    for (unsigned c=0; c<num_cols_; ++c)
    {
      if (!array_[r][c])
        vcl_cout << -1 << ' ';
      else
        vcl_cout << array_[r][c]->log_p() << ' ';
    }
    vcl_cout << '\n';
  }
  vcl_cout << '\n';
}
