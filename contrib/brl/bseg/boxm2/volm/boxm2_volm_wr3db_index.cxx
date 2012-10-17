#include "boxm2_volm_wr3db_index.h"
#include <bbas/volm/volm_spherical_container.h>

void boxm2_volm_wr3db_index::allocate_mem()
{
  if (vis_values_)
    delete vis_values_;
  vis_values_ = new char[size_];
  if (!only_vis_) {
    if (prob_values_)
      delete prob_values_;
    prob_values_ = new char[size_];
  }
}

boxm2_volm_wr3db_index::~boxm2_volm_wr3db_index()
{
  if (vis_values_)
    delete vis_values_;
  if (prob_values_)
    delete prob_values_;
}

