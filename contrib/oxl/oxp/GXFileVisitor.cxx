// This is oxl/oxp/GXFileVisitor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "GXFileVisitor.h"

#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_fstream.h>

#include <vul/vul_reg_exp.h>
#include <vcl_iostream.h>
#include <vul/vul_awk.h>

bool GXFileVisitor::do_text = true;
bool GXFileVisitor::do_antialias = false;

GXFileVisitor::GXFileVisitor()
{
  color[0] = color[1] = color[2] = 1.0;
  point_radius = 1;
  line_width = 0;
}

//: Open ".gx" file specified by filename, parse, and call the various virtuals.
bool GXFileVisitor::visit(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good()) {
    vcl_cerr << "GXFileVisitor: Could not open [" << filename << "]\n";
    return false;
  }
  return visit(f);
}

// fsm. added 'return false;' in all these to suppress compiler warnings.
// apologies if 'true' was the better choice.
bool GXFileVisitor::point(char const*, float, float) {return false;}
bool GXFileVisitor::polyline(float const*, float const*, int) {return false;}
bool GXFileVisitor::text(float, float, char const*) {return false;}
bool GXFileVisitor::set_color(float, float, float) {return false;}
bool GXFileVisitor::set_point_radius(float) {return false;}
bool GXFileVisitor::set_line_width(float) {return false;}

struct StringToFloat {
  double value;
  bool ok_;

  StringToFloat(const char* s) {
    char * ep;
    value = strtod(s, &ep);
    ok_ = (ep != s);
  }

  double operator() () { return value; }
  bool ok() { return ok_; }
};

bool GXFileVisitor::visit(vcl_istream& s)
{
  vul_reg_exp re("^t +[-+.0-9e]+ +[-+.0-9e]+ +(.+)$");
  for (vul_awk awk(s); awk; ++awk) {
    int NF = awk.NF();
    if (NF == 0)
      continue;
    vcl_string instruction = awk[0];
    StringToFloat instruction_value(instruction.c_str());
    if (instruction == "r")
    {        // Set point radius
      this->point_radius = (float)vcl_atof(awk[1]);
      this->set_point_radius(this->point_radius);
    }
    else if (instruction == "p" || (instruction_value.ok() && NF == 2))
    { // "p" x y, or just x y
      int base = (instruction == "p") ? 1 : 0;
      this->point("p", (float)vcl_atof(awk[base+0]), (float)vcl_atof(awk[base+1]));
    }
    else if (instruction == "+")
    { // + sign
      this->point("+", (float)vcl_atof(awk[1]), (float)vcl_atof(awk[2]));
    }
    else if (instruction == "l" || (instruction_value.ok() && NF == 4))
    { // Polyline
      bool numbers_only = instruction_value.ok();
      int npoints, base;
      if (numbers_only) {
        base = 0;
        npoints = NF/2;
      }
      else {
        base = 1;
        npoints = (NF - 1)/2;
        if (npoints * 2 + 1 != NF) {
          vcl_cerr << "movie: Polyline with odd # of vertices!!!\n";
          return false; //fsm
        }
      }
      if (npoints > 1023) {
        vcl_cerr << "movie: Polyline longer than 1024 points!!\n";
        return false; //fsm
        //npoints = 1023;
      }

      float x[1024];
      float y[1024];
      for (int i = 0; i < npoints; ++i) {
        x[i] = (float)vcl_atof(awk[i*2 + 0 + base]);
        y[i] = (float)vcl_atof(awk[i*2 + 1 + base]);
      }
      this->polyline(x, y, npoints);

    } else if (instruction == "t") { // Text
      if (do_text) {
        char const* text = awk.line();
        // text is of form "t +number +number + ...thetext"
        if (!*text || !re.find(text))
          vcl_cerr << "GXFileVisitor: Bad \"t\" line: [" << text << "]\n";
        else
          this->text((float)vcl_atof(awk[1]), (float)vcl_atof(awk[2]), re.match(1).c_str());
      }
    } else if (instruction == "c") {
      if (awk.NF() == 4) {
        // Assume 0-1 rgb spec
        color[0] = (float)vcl_atof(awk[1]);
        color[1] = (float)vcl_atof(awk[2]);
        color[2] = (float)vcl_atof(awk[3]);
      }
      else
      {
        static struct { char const* s; float c[3]; } colors [] = {
          {"r", {1, 0, 0}},
          {"g", {0, 1, 0}},
          {"b", {0, 0, 1}},
          {"y", {1, 1, 0}},
          {"m", {1, 0, 1}},
          {"c", {0, 1, 1}},
          {"k", {0, 0, 0}},
          {"w", {1, 1, 1}},
          {"red", {1, 0, 0}},
          {"green", {0, 1, 0}},
          {"blue", {0, 0, 1}},
          {"yellow", {1, 1, 0}},
          {"black", {0, 0, 0}},
          {"white", {1, 1, 1}},
          {0,{0,0,0}}
        };
        vcl_string colour = awk[1];
        char const* cs = colour.c_str();
        bool ok = false;
        for (unsigned i = 0; i < sizeof colors / sizeof colors[0]; ++i)
          if (vcl_strcmp(colors[i].s, cs) == 0) {
            //was : color = colors[i].c;
            // fsm. some compilers (SGI native) don't
            // like assignment from float [3] to float [3].
            color[0] = colors[i].c[0];
            color[1] = colors[i].c[1];
            color[2] = colors[i].c[2];
            ok = true;
            break;
          }
        if (!ok) {
          vcl_cerr << "GXFileVisitor: Colour [" << cs << "] not recognised\n";
          return false; //fsm
        }
      }

      this->set_color(color[0], color[1], color[2]);
    }
    else {
      vcl_cerr << "movie: bad gx line " << awk.line() << vcl_endl;
      return false; //fsm
    }
  }
  return true; // fsm. see above
}
