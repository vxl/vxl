#include "easy2D_sequencer.h"

easy2D_sequencer::easy2D_sequencer(vcl_vector<vgui_easy2D_tableau_sptr> seq)
{
  sequence_ = seq;
  pos_ = seq.begin();
}
vgui_easy2D_tableau_sptr easy2D_sequencer::next() 
{
  if (pos_ == sequence_.end())
    pos_ = sequence_.begin();
  else
    pos_++;
  return *pos_;
}
vgui_easy2D_tableau_sptr easy2D_sequencer::prev() 
{
  if (pos_ == sequence_.begin())
    pos_ = sequence_.end();
  else
    pos_--;
  return *pos_;
}
