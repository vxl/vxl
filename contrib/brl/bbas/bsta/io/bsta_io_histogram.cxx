#include "bsta_io_histogram.h"
//
//==============  histogram ==========================
//
void vsl_b_write(vsl_b_ostream& os, const bsta_histogram_base* hptr)
{
  if (hptr == nullptr)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    if (hptr->type_ == bsta_histogram_base::HIST_TYPE_FLOAT) {
     const auto* hf =
       static_cast<const bsta_histogram<float>*>(hptr);
      vsl_b_write(os, *hf);
    }
    else if (hptr->type_ == bsta_histogram_base::HIST_TYPE_DOUBLE) {
      const auto* hf =
        static_cast<const bsta_histogram<double>*>(hptr);
      vsl_b_write(os, *hf);
    }
  }
}

void vsl_b_read(vsl_b_istream &is, bsta_histogram_base*& hptr)
{
  delete hptr; hptr = nullptr;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    short ver;
    vsl_b_read(is, ver);
    if (ver != 1)
      return;
    int itype = 0;
    vsl_b_read(is, itype);
    auto type =
      static_cast<bsta_histogram_base::bsta_hist_type>(itype);
    if (type == bsta_histogram_base::HIST_TYPE_FLOAT)
    {
      bsta_histogram<float> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_histogram<float>(h);
    }
    else if (type == bsta_histogram_base::HIST_TYPE_DOUBLE)
    {
      bsta_histogram<double> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_histogram<double>(h);
    }
//  else if (type == bsta_histogram_base::HIST_TYPE_UNKNOWN) ; // do nothing
  }
}

void vsl_b_write(vsl_b_ostream& os, const bsta_histogram_base_sptr& hptr)
{
  vsl_b_write(os, hptr.ptr());
}

void vsl_b_read(vsl_b_istream &is, bsta_histogram_base_sptr& hptr)
{
  bsta_histogram_base* ptr = nullptr;
  vsl_b_read(is, ptr);
  hptr = ptr;
}

//
//============== joint_histogram ==========================
//

void vsl_b_write(vsl_b_ostream &os, const bsta_joint_histogram_base* hptr)
{
  if (hptr == nullptr)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    if (hptr->type_ == bsta_joint_histogram_base::HIST_TYPE_FLOAT) {
      const auto* hf =
        static_cast<const bsta_joint_histogram<float>*>(hptr);
      vsl_b_write(os, *hf);
    }
    else if (hptr->type_ == bsta_joint_histogram_base::HIST_TYPE_DOUBLE) {
      const auto* hf =
        static_cast<const bsta_joint_histogram<double>*>(hptr);
      vsl_b_write(os, *hf);
    }
  }
}

void vsl_b_read(vsl_b_istream &is, bsta_joint_histogram_base*& hptr)
{
  delete hptr; hptr = nullptr;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    short ver;
    vsl_b_read(is, ver);
    if (ver != 1)
      return;
    int itype = 0;
    vsl_b_read(is, itype);
    auto type =
      static_cast<bsta_joint_histogram_base::bsta_joint_hist_type>(itype);
    if (type == bsta_joint_histogram_base::HIST_TYPE_FLOAT)
    {
      bsta_joint_histogram<float> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_joint_histogram<float>(h);
    }
    else if (type == bsta_joint_histogram_base::HIST_TYPE_DOUBLE)
    {
      bsta_joint_histogram<double> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_joint_histogram<double>(h);
    }
//  else if (type == bsta_joint_histogram_base::HIST_TYPE_UNKNOWN) ;// do nothing
  }
}

void vsl_b_write(vsl_b_ostream& os, const bsta_joint_histogram_base_sptr& hptr)
{
  vsl_b_write(os, hptr.ptr());
}

void vsl_b_read(vsl_b_istream &is, bsta_joint_histogram_base_sptr& hptr)
{
  bsta_joint_histogram_base* ptr = nullptr;
  vsl_b_read(is, ptr);
  hptr = ptr;
}
//
//============== joint_histogram_3d ==========================
//

void vsl_b_write(vsl_b_ostream &os, const bsta_joint_histogram_3d_base* hptr)
{
  if (hptr==nullptr)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    if (hptr->type_ == bsta_joint_histogram_3d_base::HIST_TYPE_FLOAT) {
      const auto* hf =
        static_cast<const bsta_joint_histogram_3d<float>*>(hptr);
      vsl_b_write(os, *hf);
    }
    else if (hptr->type_ == bsta_joint_histogram_3d_base::HIST_TYPE_DOUBLE) {
      const auto* hf =
        static_cast<const bsta_joint_histogram_3d<double>*>(hptr);
      vsl_b_write(os, *hf);
    }
  }
}

void vsl_b_read(vsl_b_istream &is, bsta_joint_histogram_3d_base*& hptr)
{
  delete hptr; hptr = nullptr;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    short ver = 0;
    int itype = 0;
    vsl_b_read(is, ver);
    if (ver != 1)
      return;
    vsl_b_read(is, itype);
    auto type =
      static_cast<bsta_joint_histogram_3d_base::bsta_joint_hist_3d_type>(itype);
    if (type == bsta_joint_histogram_3d_base::HIST_TYPE_FLOAT)
    {
      bsta_joint_histogram_3d<float> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_joint_histogram_3d<float>(h);
    }
    if (type == bsta_joint_histogram_3d_base::HIST_TYPE_DOUBLE)
    {
      bsta_joint_histogram_3d<double> h;
      vsl_b_read(is, h, true);
      hptr = new bsta_joint_histogram_3d<double>(h);
    }
//  else if (type == bsta_joint_histogram_3d_base::HIST_TYPE_UNKNOWN) ; // do nothing
  }
}

void vsl_b_write(vsl_b_ostream& os, const bsta_joint_histogram_3d_base_sptr& hptr)
{
  vsl_b_write(os, hptr.ptr());
}

void vsl_b_read(vsl_b_istream &is, bsta_joint_histogram_3d_base_sptr& hptr)
{
  bsta_joint_histogram_3d_base* ptr=nullptr;
  vsl_b_read(is, ptr);
  hptr = ptr;
}
