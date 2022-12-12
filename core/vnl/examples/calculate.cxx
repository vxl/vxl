//:
// \file
// \brief arbitrary precision calculator
// Accepts a C-style numeric expression that is to be parsed (preferably within
// quotes to escape spaces, brackets and other special symbols from the shell);
// it may contain the following:
//  optional spaces (are ignored)
//  integer and floating point numbers, possibly with scientific notation
//  but without decimal point
//  operators: + - * / % !
//  brackets ()
//  the function calls abs(), floor(), ceil(), round(), min(,), max(,), binom(,), pow(,).
// The normal operator precedence applies: (5) brackets(), (4) function call,
// (3) unary + and -, (2) *, / and %, (1) + and -.
// On the same level, the order is left-to-right.
//
// Usage examples:
//  calculate '(1e99-1)/9'
//  calculate << EOF
//    123!
//    10 * 11 / 12
//    binom(100,50)
//    EOF
//  echo '33333333333333333333333333333e-5%12345' | calculate
//
// \author Peter Vanroose, ABIS Leuven
// \date  August 2011
// History: Originally written for double precision arithmetic (1996).

#include <iostream>

class node
{
public:
  fptr1 func1{ nullptr };
  fptr2 func2{ nullptr };
  fptr3 func3{ nullptr };
  void * param1{ nullptr };
  void * param2{ nullptr };
  node() = default;
};

void
ErrorExit(std::string const & expr, char const * t, unsigned long s)
{
  std::cerr << "Error parsing expression -- " << t << ":\n" << expr << '\n';
  while (s--)
    std::cerr << ' ';
  std::cerr << "^\n";
  std::exit(1);
}

//: find end of argument, i.e., comma or ')'
int
arglength(std::string const & expr, unsigned long s, int l)
{
  int i = 0;
  int level = 0;
  while (i < l)
  {
    if (expr[s + i] == '(')
      ++level;
    if (level == 0 && (expr[s + i] == ',' || expr[s + i] == ')'))
      break;
    if (expr[s + i] == ')')
      --level;
    ++i;
  }
  if (i >= l)
    ErrorExit(expr, "argument not finished here", s + i);
  return i;
}

//: find matching bracket or other end of block
int
blocklength(std::string const & expr, unsigned long s, int l)
{
  int i = 0;
  if (expr[s] == '-' || expr[s] == '+') // unary minus or plus
    return 1 + blocklength(expr, s + 1, l - 1);
  if (expr[s] == '!')
    return 1;
  if (expr[s] == '(')
  { // bracketed subsequence
    int level = 1;
    while (++i < l)
    {
      if (expr[s + i] == '(')
        ++level;
      else if (expr[s + i] == ')')
        --level;
      if (level == 0)
        break;
    }
    if (i >= l)
      ErrorExit(expr, "no matching closing bracket", s);
    return i + 1;
  }
  if (expr[s] >= 'a' && expr[s] <= 'z')
  { // function call
    for (i = 1; i < l && ((expr[s + i] >= 'a' && expr[s + i] <= 'z') || (expr[s + i] >= '0' && expr[s + i] <= '9'));
         ++i)
      ;
    if (i >= l || expr[s + i] != '(')
      ErrorExit(expr, "function call without parameters", s + i);
    return i + blocklength(expr, s + i, l - i);
  }
  if (expr[s] >= '0' && expr[s] <= '9')
  { // a number
    for (i = 0; i < l && expr[s + i] >= '0' && expr[s + i] <= '9'; ++i)
      ;
    if (i < l && expr[s + i] == 'e')
    { // optional exponent
      ++i;
      if (i < l && (expr[s + i] == '-' || expr[s + i] == '+'))
        ++i;
      if (i >= l)
        ErrorExit(expr, "Exponent missing", s + i - 1);
      while (i < l && expr[s + i] >= '0' && expr[s + i] <= '9')
        ++i;
    }
    return i;
  }
  if (expr[s] == ')')
  {
    std::cerr << "Warning: length of closing bracket!\n";
    return 0;
  }
  std::cerr << "Warning: length 1 returned on " << expr[s] << '\n';
  return 1;
}

//: put one pair of brackets around expr[s]--expr[s+l-1], except if already bracketed
// THIS IS THE ONLY ROUTINE THAT INSERTS SOMETING (namely a pair of brackets) INTO expr !!
int
enbracket(std::string & expr, unsigned long s, int l)
{
  if (l <= 0)
    return l;
  if (expr[s] == '(' && blocklength(expr, s, l) == l)
    return l;
  expr.insert(s + l, 1, ')');
  expr.insert(s, 1, '(');
#ifdef DEBUG
  std::cerr << "--> " << expr << '\n';
#endif
  return l + 2;
}

int
isoperator(char c)
{ // true if c is not a-z or 0-9.
  return (c < 'a' || c > 'z') && (c < '0' || c > '9');
}

//: add all level brackets into expr[s]--expr[s+l-1]
// REMARK: expr should not contain spaces or uppercase letters
int
put_brackets(std::string & expr, unsigned long s, int l)
{
  int k;
  if (l <= 0)
    return l; // do nothing on empty string
  if (expr[s] == ')')
    ErrorExit(expr, "string starts with closing bracket", s);
  int j = blocklength(expr, s, l);
  if (expr[s] == '(')
  {
    k = 2 + put_brackets(expr, s + 1, j - 2); // put brackets inside
    l += k - j;
    j = k;
    if (j < l && !isoperator(expr[s + j]))
      ErrorExit(expr, "expecting an operator here", s + j);
    k = put_brackets(expr, s + j + 1, l - j - 1); // put brackets after
    return k + j + 1;
  }

  // remove unary +:
  if (expr[s] == '+')
    expr.erase(s, 1L);

  // process unary minus:
  if (expr[s] == '-' && l < 2)
    ErrorExit(expr, "nothing following unary minus", s);
  if (l < 1)
    ErrorExit(expr, "something missing", s);
  if (expr[s] == '-' && expr[s + 1] != '(')
  {
    if (expr[s + 1] == ')')
      ErrorExit(expr, "closing bracket after unary -", s + 1);
    j = blocklength(expr, s + 1, l - 1);
    l += enbracket(expr, s + 1, j) - j;
  }
  if (expr[s] == '-')
  { // now, expr[s+1] == '('
    j = blocklength(expr, s + 1, l - 1);
    k = 2 + put_brackets(expr, s + 2, j - 2); // put brackets inside
    l += k - j;
    j = k;
    if (j + 1 < l && !isoperator(expr[s + j + 1]))
      ErrorExit(expr, "expecting an operator here", s + j + 1);
    k = put_brackets(expr, s + j + 2, l - j - 2); // put brackets after
    return k + j + 2;
  }

  j = blocklength(expr, s, l);

  // process numbers: are always put between brackets, so scanning is only to be done once
  if (expr[s] >= '0' && expr[s] <= '9')
  {
    k = enbracket(expr, s, j);
    l += k - j;
    j = k;
    if (j < l && !isoperator(expr[s + j]))
      ErrorExit(expr, "expecting an operator here", s + j);
    k = put_brackets(expr, s + j + 1, l - j - 1); // put brackets after
    return k + j + 1;
  }

  // process function calls
  if (expr[s] >= 'a' && expr[s] <= 'z')
  {
    for (k = 1; k < j; ++k)
      if ((expr[s + k] < 'a' || expr[s + k] > 'z') && (expr[s + k] < '0' || expr[s + k] > '9'))
        break;
    if (expr[s + k] != '(')
      ErrorExit(expr, "opening bracket missing", s + k);
    while (k < j - 1)
    { // bracket inside each argument
      int i = k + 1 + arglength(expr, s + k + 1, j - k - 1);
      k += 1 + put_brackets(expr, s + k + 1, i - k - 1);
      l += k - i;
      j += k - i;
    }
    if (j < l && !isoperator(expr[s + j]))
      ErrorExit(expr, "expecting an operator here", s + j);
    k = put_brackets(expr, s + j + 1, l - j - 1); // put brackets after
    return k + j + 1;
  }

  ErrorExit(expr, "invalid symbol", s);
  return -1;
}

//: add operator hierarchy brackets into expr[s]--expr[s+l-1]
// REMARK: expr should already been bracketed by put_brackets()
int
hierarchy_brackets(std::string & expr, unsigned long s, int l)
{
  int i, previ, bl, strength = 0;

  if (expr[s] == ')')
    ErrorExit(expr, "substring starts with closing bracket", s);
  int j = blocklength(expr, s, l);
  int is = 0;
  while (l > 0 && expr[s] == '(' && j == l && (expr[s + 1] < '0' || expr[s + 1] > '9'))
  {
    if (expr[s + (++is)] == ')')
      ErrorExit(expr, "empty () brackets", s + is);
    j = blocklength(expr, s + is, l - 2 * is);
  }
  if (l - 2 * is <= 0)
    return l; // do nothing on empty string

  // find highest precedence operator on outer level
  int first = -2, second = -2;
  for (previ = -1, i = is; i < l - is; previ = i, i += 1 + bl)
  {
    if (expr[s + i] == ')')
      ErrorExit(expr, "closing bracket after operator", s + i);
    bl = blocklength(expr, s + i, l - is - i);
    if (previ >= 0)
    {
      if (strength < 6 && (expr[s + i - 1] == '*' || expr[s + i - 1] == '/' || expr[s + i - 1] == '%'))
        strength = 6, second = i, first = previ;
      if (strength < 5 && (expr[s + i - 1] == '+' || expr[s + i - 1] == '-'))
        strength = 5, second = i, first = previ;
    }
    if (expr[s + i] == '(' && (expr[s + i + 1] < '0' || expr[s + i + 1] > '9'))
    { // no number
      int k = hierarchy_brackets(expr, s + i, bl);
      l += k - bl;
      bl = k;
    }
    if (expr[s + i] >= 'a' && expr[s + i] <= 'z')
    { // function call
      int m = i;
      while (expr[s + m] != '(' && m < bl + i)
        ++m;
      ++m;
      while (m < bl + i)
      {
        j = arglength(expr, s + m, bl + i - m);
        int k = hierarchy_brackets(expr, s + m, j);
        bl += k - j;
        l += k - j;
        m += 1 + k;
      }
    }
  }
  if (strength <= 0)
    return l; // no operators in outer level
  if (first == is && second == previ)
    return l; // only one prefix operator
  if (expr[s + second] == ')')
    ErrorExit(expr, "closing bracket after operator", s + second);
  second += blocklength(expr, s + second, l - is - second);
  l += enbracket(expr, s + first, second - first) - (second - first);
  l = hierarchy_brackets(expr, s, l);
  return l;
}

//: Return str, but after removing spaces, making lowercase, and adding ()
std::string
simplify(char const * str)
{
  std::string expr = "";
  // remove all spaces and tabs:
  for (int i = 0; str[i]; ++i)
    if (str[i] != ' ' && str[i] != '\t')
      expr.push_back(str[i]);
  int l = expr.length();
  // make lowercase
  for (int i = 0; i < l; ++i)
  {
    if (expr[i] >= 'A' && expr[i] <= 'Z')
      expr[i] += 'a' - 'A';
  }
  l = put_brackets(expr, 0L, l); // all grouping except operator precedence
  // EXAMPLE:
  //     -2 * abs (32*pow(32,2)) + 7*32 -(8-32)
  // --> (-(2))*abs(32*pow(32,(2)))+(7)*32-((8)-32)
  hierarchy_brackets(expr, 0L, l); // put brackets according to o.p.
  // EXAMPLE:
  //     (-(2))*abs(32*pow(32,(2)))+(7)*32-((8)-32)
  // --> (((-(2))*abs(32*pow(32,(2))))+((7)*32))-((8)-32)
  return expr;
}
