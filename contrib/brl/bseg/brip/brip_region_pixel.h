// This is brl/bseg/brip/brip_region_pixel.h
#ifndef brip_region_pixel_h_
#define brip_region_pixel_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A region pixel class for the watershed algorithm.
//
// \verbatim
//  Modifications
//   Initial version July 5, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_iosfwd.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <brip/brip_region_pixel_sptr.h>

class brip_region_pixel : public vbl_ref_count
{
 public:
  vgl_point_2d<int> location_;
  vgl_point_2d<int> nearest_;
  float cost_;
  int count_;
  unsigned int label_;
  int dist_;

  brip_region_pixel()
    : vbl_ref_count(), location_(0,0), nearest_(0,0), cost_(0.0f),
      count_(0), label_(0), dist_(0) {}
  brip_region_pixel(brip_region_pixel const& p)
    : vbl_ref_count(), location_(p.location_), nearest_(p.nearest_),
      cost_(p.cost_), count_(p.count_), label_(p.label_), dist_(p.dist_) {}
  brip_region_pixel(vgl_point_2d<int> const& location,
                    vgl_point_2d<int> const& nearest,
                    float const& cost,
                    int const count,
                    unsigned int const& label)
    : vbl_ref_count(), location_(location), nearest_(nearest), cost_(cost),
      count_(count), label_(label)
  {
    int dx = location_.x() - nearest_.x();
    int dy = location_.y() - nearest_.y();
    dist_ = dx * dx + dy * dy;
  }

  void set(vgl_point_2d<int> const& location,
           vgl_point_2d<int> const& nearest,
           float const& cost,
           int   const& count,
           int   const& label)
  {
    location_ = location;
    nearest_ = nearest;
    cost_ = cost;
    count_ = count;
    label_ = label;
    int dx = location_.x() - nearest_.x();
    int dy = location_.y() - nearest_.y();
    dist_ = dx * dx + dy * dy;
  }

  // must implement comparison since priority_queue looks for largest element
  struct compare
  {
    bool operator()(brip_region_pixel const& l,
                    brip_region_pixel const& r) const
    {
      if (r.cost_ == l.cost_)
      {
        if (r.dist_ == l.dist_) return r.count_ < l.count_;

        return r.dist_ < l.dist_;
      }

      return r.cost_ < l.cost_;
    }


    bool operator()(brip_region_pixel_sptr const& l,
                    brip_region_pixel_sptr const& r) const
    {
      if (r->cost_ == l->cost_)
      {
        if (r->dist_ == l->dist_) return r->count_ < l->count_;

        return r->dist_ < l->dist_;
      }

      return r->cost_ < l->cost_;
    }
  };
};

vcl_ostream& operator<<(vcl_ostream& s, brip_region_pixel const& sd);

#endif // brip_region_pixel_h_
