// This is core/examples/vnl_calc/vnl_calc.cxx

//:
// \file
// \brief vnl_calc - Simple command-line matrix calculator
// \author Andrew W. Fitzgibbon, Oxford RRG 23 Jan 99
//
// Use matcalc to perform simple calculations on the command line.
// It is reverse-polish notation, and matrices are loaded from files.
// e.g.
// \verbatim
// % vnl-calc 1 2 +
// \endverbatim
// prints 3
//
// \verbatim
// Modifications
// 10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

template <class T>
class mystack : public vcl_vector<T>
{
 public:
  void push(const T& t) {
    push_back(t);
  }

  T pop() {
    int n = size();
    if (n == 0) {
      vcl_cerr <<  "ZOKS: Stack underflow\n";
      vcl_exit(1);
    }
    T tmp = (*this)[n-1];
    pop_back();
    return tmp;
  }

  void print() {
    for (unsigned i = 0; i < size(); ++i)
      vcl_cout << (*this)[i] << vcl_endl;
  }
};

#define POP2(expr) Matrix b = stack.pop(); Matrix a = stack.pop(); stack.push(expr);

void cantshift(const vcl_string& arg)
{
  vcl_cerr << "matcalc: Missing argument after \"" << arg << "\".\n";
  vcl_exit (-1);
}

typedef vnl_matrix<double> Matrix;

template class mystack<Matrix>;

void print(mystack<Matrix> const &stack, char const *fmt)
{
  char buf[4096];
  for (unsigned int k=0; k<stack.size(); ++k) {
    Matrix const& M = stack[k];
    for (unsigned int i=0; i<M.rows(); ++i) {
      for (unsigned int j=0; j<M.cols(); ++j) {
        vcl_sprintf(buf, fmt, M[i][j]);
        vcl_cout << ' ' << buf;
      }
      vcl_cout << vcl_endl;
    }
    vcl_cout << vcl_endl;
  }
}

void print(mystack<Matrix> const &stack, vcl_string const& fmt)
{
  print(stack, fmt.c_str());
}

int main(int argc, char ** argv)
{
  mystack<Matrix> stack;
  int cout_precision = 0;

  //Matrix::set_print_format("%20.16e");
  vcl_string print_format = "%20.16e";

  for (int i = 1; i < argc; ++i) {
    vcl_string arg = argv[i];
#define SHIFT { if (++i >= argc) cantshift(arg); else arg = argv[i]; }

    if (arg[0] >= '0' && arg[0] <= '9') {
      stack.push(Matrix(1,1,vcl_atof(arg.c_str())));
    }
    else if (arg == "+") {
      POP2(a+b);
    } else if (arg == "-") {
      POP2(a-b);
    } else if (arg == "*" || arg == "x") {
      POP2(a*b);
    } else if (arg == "/") {

      POP2(element_quotient(a,b));

    } else if (arg == "svd") {
      Matrix a = stack.pop();
      vnl_svd<double> svd(a);
      stack.push(svd.U());
      stack.push(svd.W().asMatrix());
      stack.push(svd.V());

    } else if (arg == "X" || arg == "allx") {
      // Multiply everything
      Matrix out = stack[0];
      for (unsigned k = 1; k < stack.size(); ++k)
        out = out * stack[k];
      stack.clear();
      stack.push(out);

    } else if (arg == "all+") {
      // Add everything
      Matrix out = stack[0];
      for (unsigned k = 1; k < stack.size(); ++k)
        out += stack[k];
      stack.clear();
      stack.push(out);

    } else if (arg == "i") {
      stack.push(vnl_svd<double>(stack.pop()).inverse());

      // Printing:
    } else if (arg == "p") {
      //stack.print();
      print(stack, print_format.c_str());

    } else if (arg == "fmt") {
      SHIFT;
      //Matrix::set_print_format(arg.c_str());
      print_format = arg;

    } else if (arg == "setp") {
      Matrix a = stack.pop();
      cout_precision = int(a(0,0));
      if (cout_precision > 99 || cout_precision < 0)
        cout_precision = 16;
      char t[] = "%20.16e";
      t[4]='0'+cout_precision/10;
      t[5]='0'+cout_precision%10;
      //Matrix::set_print_format(t);
      print_format = t;
    } else { // Load from file
      Matrix m;
      vcl_ifstream f(arg.c_str());
      if (!m.read_ascii(f)) {
        vcl_cerr <<  "Can't read file [" << arg << "]\n";
        return -1;
      }
      stack.push(m);
    }
  }
  //stack.print();
  print(stack, print_format);

  return 0;
}
