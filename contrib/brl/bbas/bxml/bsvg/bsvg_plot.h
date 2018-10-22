#ifndef bsvg_plot_h_
#define bsvg_plot_h_
//:
// \file
// \brief Various plots - as SVG documents
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - July 08, 09 - ported to vxl from local repository - minor fixes
//   Yi Dong - May 20, 2013 - add a mothod to add axes given a scale_factor
// \endverbatim

#include "bsvg_document.h"
#include "bsvg_element.h"

//: Coordinate system of SVG is the same as an image coordinate system: upper left corner of the browser page is the origin
//  Plot axes will be placed so that lower_left corner of the browser page will be the plot origin
//  Plot only draws lines of x and y axes if they are added to the plot.
//  scales the x and y axes according to x_min, x_max values so that it spans the document area

const float default_stroke_width = 2.0f;
constexpr int default_font_size = 15;
const float default_margin = 20.0f;

class bsvg_plot : public bsvg_document
{
 public:

    bsvg_plot(float w, float h) : bsvg_document(w, h), margin_(default_margin), font_size_(default_font_size) {}
    bsvg_plot(float w, float h, float viewBox_x, float viewBox_y, float viewBox_w, float viewBox_h) : bsvg_document(w,h,viewBox_x, viewBox_y, viewBox_w, viewBox_h), margin_(default_margin), font_size_(default_font_size) {}

    void add_axes(float x_min, float x_max, float y_min, float y_max, float stroke_width = default_stroke_width);
    void add_axes(float x_min, float x_max, float y_min, float y_max, bool is_scale_x, float stroke_width);
    //: assumes add_axes have been called
    void add_x_increments(float x_inc, float stroke_width = default_stroke_width);
    void add_y_increments(float y_inc, float stroke_width = default_stroke_width);

    void set_margin(float m) { margin_ = m; }
    void set_font_size(int s) { font_size_ = s; }
    void add_title(const std::string& t);

    void add_line(const std::vector<float>& xs, const std::vector<float>& ys, const std::string& color, float stroke_width = default_stroke_width);

    //: add equally spaced and equal width bars with the given heights
    void add_bars(const std::vector<float>& heights, const std::string& color);
    void add_bars(const std::vector<float>& heights, const std::vector<float>& x_labels, bool vertical_labels, const std::string& color);
    void add_bars(const std::vector<float>& heights, const std::vector<std::string>& x_labels, bool vertical_labels, const std::string& color);

    //:return the number of bars in the plot (counts the number of element with name "rect")
    int number_of_bars();

    //: add bars sequentially with a fixed interval and width.
    //  use margin_ as the width of each bar and leave margin_/3 intervals in between
    //  the total width of the plot needs to be adjusted during initialization to contain all desired number of bars
    int add_bar(const float height, const std::string& color);
    int add_bar(const float height, const float x_label, bool vertical_label, const std::string& color);
    int add_bar(const float height, const std::string& label, bool vertical_label, const std::string& color);

    bsvg_group* add_bars_helper(const std::vector<float>& heights, const std::string& color);
    bsvg_group* add_x_labels_helper(const std::vector<std::string>& x_labels, const std::string& color, bool vertical_labels);

    //: add splices for a pie chart
    //  angle is zero at the x-axis and is positive counter-clockwise, use angles in range [0,2*pi]
    void add_splice(float center_x, float center_y, float radius, float start_angle, float end_angle, const std::string& color);
    //  angle is zero at the x-axis and is positive counter-clockwise, use angles in range [0,2*pi], colors red, green, blue are each in range [0,255]
    void add_splice(float center_x, float center_y, float radius, float start_angle, float end_angle, unsigned red, unsigned green, unsigned blue);

 protected:

    float margin_;
    int font_size_;

    float scale_factor_;
    float axes_orig_x_;
    float axes_orig_y_;
    float h2_x, h2_y;
};

#endif // bsvg_plot_h_
