// This is core/vul/vul_reg_exp.cxx
#include <iostream>
#include <cstring>
#include <cstddef>
#include "vul_reg_exp.h"
//:
// \file
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Created: MNF Jun 13, 1989 Initial Design and Implementation
// Updated: LGO Aug 09, 1989 Inherit from Generic
// Updated: MBN Sep 07, 1989 Added conditional exception handling
// Updated: MBN Dec 15, 1989 Sprinkled "const" qualifiers all over the place!
// Updated: DLS Mar 22, 1991 New lite version
//
// This  is the header file  for the regular  expression class.   An object of
// this class contains a regular expression,  in  a special "compiled" format.
// This  compiled format consists  of  several slots   all kept as the objects
// private data.  The vul_reg_exp class  provides a convenient  way  to  represent
// regular  expressions.  It makes it easy   to search  for  the  same regular
// expression in many different strings without having to  compile a string to
// regular expression format more than necessary.
//
// A regular  expression allows a programmer to  specify complex patterns that
// can be searched for  and  matched against the  character string of a String
// object.  In  its  simplest case, a   regular expression  is a  sequence  of
// characters with which you can search for exact character matches.  However,
// many times you may not know the exact sequence you want to find, or you may
// only want to find a match at the beginning or end of  a String.  The vul_reg_exp
// object  allows specification of  such patterns by  utilizing the  following
// regular  expression  meta-characters   (note   that  more  one  of  these
// meta-characters  can  be used in a single  regular  expression in  order to
// create complex search patterns):
//
// -   ^  Match at beginning of line
// -   $  Match at end of line
// -   .  Match any single character
// -   [ ]  Match any one character inside the brackets
// -   [^ ] Match any character NOT inside the brackets
// -   -  Match any character in range on either side of dash
// -   *  Match preceding pattern zero or more times
// -   +  Match preceding pattern one or more times
// -   ?  Match preceding pattern zero or once only
// -   ()   Save a matched expression and use it in a further match.
//
// There are three constructors for vul_reg_exp.  One  just creates an empty vul_reg_exp
// object.  Another creates a vul_reg_exp object  and initializes it with a regular
// expression  that is given  in  the form of a   char*.   The  third  takes a
// reference  to  a vul_reg_exp  object  as an  argument  and creates an object
// initialized with the information from the given vul_reg_exp object.
//
// The  find  member function  finds   the  first  occurrence   of  the regular
// expression of that object in the string given to find as an argument.  Find
// returns a boolean, and  if true,  mutates  the private  data appropriately.
// Find sets pointers to the beginning and end of  the thing last  found, they
// are pointers into the actual string  that was searched.   The start and end
// member functions  return indices  into the searched string that  correspond
// to the beginning   and  end pointers  respectively.   The  compile member
// function takes a char* and puts the  compiled version of the char* argument
// into the object's private data fields.  The == and  != operators only check
// the  to see  if   the compiled  regular  expression   is the same, and  the
// deep_equal functions also checks  to see if the  start and end pointers are
// the same.  The is_valid  function returns false if  program is set to NULL,
// (i.e. there is no valid compiled expression). The set_invalid function sets
// the  program to NULL  (Warning: this deletes the compiled  expression). The
// following examples may help clarify regular expression usage:
//
//   *  The regular expression  "^hello" matches  a "hello"  only at  the
//    beginning of a  line.  It would match "hello  there" but not "hi,
//    hello there".
//
//   *  The regular expression "long$" matches a  "long"  only at the end
//    of a line. It would match "so long\0", but not "long ago".
//
//   *  The regular expression "t..t..g"  will match anything that  has a
//    "t" then any two characters, another "t", any  two characters and
//    then a "g".   It will match  "testing", or "test again" but would
//    not match "toasting"
//
//   *  The regular  expression "[1-9ab]" matches any  number one through
//    nine, and the characters  "a" and  "b".  It would match "hello 1"
//    or "begin", but would not match "no-match".
//
//   *  The  regular expression "[^1-9ab]"  matches any character that is
//    not a number one  through nine, or  an "a" or "b".   It would NOT
//    match "hello 1" or "begin", but would match "no-match".
//
//   *  The regular expression "br* " matches  something that begins with
//    a "b", is followed by zero or more "r"s, and ends in a space.  It
//    would match "brrrrr ", and "b ", but would not match "brrh ".
//
//   *  The regular expression "br+ " matches something  that begins with
//    a "b", is followed by one or more "r"s, and ends in  a space.  It
//    would match "brrrrr ",  and  "br ", but would not  match "b  " or
//    "brrh ".
//
//   *  The regular expression "br? " matches  something that begins with
//    a "b", is followed by zero or one "r"s, and ends in  a space.  It
//    would  match  "br ", and "b  ", but would not match  "brrrr "  or
//    "brrh ".
//
//   *  The regular expression "(..p)b" matches  something ending with pb
//    and beginning with whatever the two characters before the first p
//    encountered in the line were. It would find  "repb" in "rep drepa
//    qrepb".  The regular expression "(..p)a"  would find "repa qrepb"
//    in "rep drepa qrepb"
//
//   *  The regular expression "d(..p)" matches something ending  with p,
//    beginning with d, and having  two characters  in between that are
//    the same as the two characters before  the first p encountered in
//    the line.  It would match "drepa qrepb" in "rep drepa qrepb".

#include <vcl_compiler.h>

//: Copies the given regular expression.

vul_reg_exp::vul_reg_exp (vul_reg_exp const& rxp)
{
  int ind;
  this->progsize = rxp.progsize;      // Copy regular expression size
  this->program = new char[this->progsize]; // Allocate storage
  for (ind=this->progsize; ind-- != 0;)   // Copy regular expression
  this->program[ind] = rxp.program[ind];
  this->startp[0] = rxp.startp[0];      // Copy pointers into last
  this->endp[0] = rxp.endp[0];        // Successful "find" operation
  this->regmust = rxp.regmust;        // Copy field
  if (rxp.regmust != nullptr) {
  char* dum = rxp.program;
  ind = 0;
  while (dum != rxp.regmust) {
    ++dum;
    ++ind;
  }
  this->regmust = this->program + ind;
  }
  this->regstart = rxp.regstart;      // Copy starting index
  this->reganch = rxp.reganch;        // Copy remaining private data
  this->regmlen = rxp.regmlen;        // Copy remaining private data
}


//: Returns true if two regular expressions have the same compiled program for pattern matching.

bool vul_reg_exp::operator== (vul_reg_exp const& rxp) const
{
  if (this != &rxp) {           // Same address?
  int ind = this->progsize;     // Get regular expression size
  if (ind != rxp.progsize)      // If different size regexp
    return false;               // Return failure
  while (ind-- != 0)            // Else while still characters
    if (this->program[ind] != rxp.program[ind])// If regexp are different
      return false;             // Return failure
  }
  return true;                  // Else same, return success
}


//: Returns true if have the same compiled regular expressions and the same start and end pointers.

bool vul_reg_exp::deep_equal (vul_reg_exp const& rxp) const
{
  int ind = this->progsize;     // Get regular expression size
  if (ind != rxp.progsize)      // If different size regexp
  return false;                 // Return failure
  while (ind-- != 0)            // Else while still characters
  if (this->program[ind] != rxp.program[ind]) // If regexp are different
    return false;               // Return failure
  return this->startp[0] == rxp.startp[0] &&  // Else if same start/end ptrs,
       this->endp[0] == rxp.endp[0];    // Return true
}


// The remaining code in this file is derived from the  regular expression code
// whose  copyright statement appears  below.  It has been  changed to work
// with the class concepts of C++ and COOL.

//
// compile and find
//
// Copyright (c) 1986 by University of Toronto.
// Written by Henry Spencer.  Not derived from licensed software.
//
// Permission is granted to anyone to use this software for any
// purpose on any computer system, and to redistribute it freely,
// subject to the following restrictions:
//
// 1. The author is not responsible for the consequences of use of
//  this software, no matter how awful, even if they arise
//  from defects in it.
//
// 2. The origin of this software must not be misrepresented, either
//  by explicit claim or by omission.
//
// 3. Altered versions must be plainly marked as such, and must not
//  be misrepresented as being the original software.
//
// Beware that some of this code is subtly aware of the way operator
// precedence is structured in regular expressions.  Serious changes in
// regular-expression syntax might require a total rethink.
//

//
// The "internal use only" fields in regexp.h are present to pass info from
// compile to execute that permits the execute phase to run lots faster on
// simple cases.  They are:
//
// regstart  char that must begin a match; '\0' if none obvious
// reganch   is the match anchored (at beginning-of-line only)?
// regmust   string (pointer into program) that match must include, or NULL
// regmlen   length of regmust string
//
// Regstart and reganch permit very fast decisions on suitable starting points
// for a match, cutting down the work a lot.  Regmust permits fast rejection
// of lines that cannot possibly match.  The regmust tests are costly enough
// that compile() supplies a regmust only if the r.e. contains something
// potentially expensive (at present, the only such thing detected is * or +
// at the start of the r.e., which can involve a lot of backup).  Regmlen is
// supplied because the test in find() needs it and compile() is computing
// it anyway.
//

//
// Structure for regexp "program".  This is essentially a linear encoding
// of a nondeterministic finite-state machine (aka syntax charts or
// "railroad normal form" in parsing technology).  Each node is an opcode
// plus a "next" pointer, possibly plus an operand.  "Next" pointers of
// all nodes except BRANCH implement concatenation; a "next" pointer with
// a BRANCH on both ends of it is connecting two alternatives.  (Here we
// have one of the subtle syntax dependencies:  an individual BRANCH (as
// opposed to a collection of them) is never concatenated with anything
// because of operator precedence.)  The operand of some types of node is
// a literal string; for others, it is a node leading into a sub-FSM.  In
// particular, the operand of a BRANCH node is the first node of the branch.
// (NB this is \e not a tree structure:  the tail of the branch connects
// to the thing following the set of BRANCHes.)  The opcodes are:
//

// definition   number  opnd?   meaning
#define END   0  // no   End of program.
#define BOL   1  // no   Match "" at beginning of line.
#define EOL   2  // no   Match "" at end of line.
#define ANY   3  // no   Match any one character.
#define ANYOF   4  // str  Match any character in this string.
#define ANYBUT  5  // str  Match any character not in this string.
#define BRANCH  6  // node Match this alternative, or the next...
#define BACK  7  // no   Match "", "next" ptr points backward.
#define EXACTLY 8  // str  Match this string.
#define NOTHING 9  // no   Match empty string.
#define STAR  10   // node Match this (simple) thing 0 or more times.
#define PLUS  11   // node Match this (simple) thing 1 or more times.
#define OPEN  20   // no   Mark this point in input as start of #n.
// OPEN+1 is number 1, etc.
#define CLOSE   30   // no   Analogous to OPEN.

//
// Opcode notes:
//
// BRANCH     The set of branches constituting a single choice are hooked
//        together with their "next" pointers, since precedence prevents
//        anything being concatenated to any individual branch.  The
//        "next" pointer of the last BRANCH in a choice points to the
//        thing following the whole choice.  This is also where the
//        final "next" pointer of each individual branch points; each
//        branch starts with the operand node of a BRANCH node.
//
// BACK     Normal "next" pointers all implicitly point forward; BACK
//        exists to make loop structures possible.
//
// STAR,PLUS  '?', and complex '*' and '+', are implemented as circular
//        BRANCH structures using BACK.  Simple cases (one character
//        per match) are implemented with STAR and PLUS for speed
//        and to minimize recursive plunges.
//
// OPEN,CLOSE   ...are numbered at compile time.
//

//
// A node is one char of opcode followed by two chars of "next" pointer.
// "Next" pointers are stored as two 8-bit pieces, high order first.  The
// value is a positive offset from the opcode of the node containing it.
// An operand, if any, simply follows the node.  (Note that much of the
// code generation knows about this implicit relationship.)
//
// Using two bytes for the "next" pointer is vast overkill for most things,
// but allows patterns to get big without disasters.
//

#define OP(p)       (*(p))
#define NEXT(p)     (((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define OPERAND(p)    ((p) + 3)

constexpr unsigned char MAGIC = 0234;

//
// Utility definitions.
//
#define UCHARAT(p)    ((const unsigned char*)(p))[0]

#define FAIL(m) { regerror(m); return NULL; }
#define ISMULT(c)     ((c) == '*' || (c) == '+' || (c) == '?')
#define META  "^$.[()|?+*\\"

//
// Flags to be passed up and down.
//
#define HASWIDTH    01    // Known never to match null string.
#define SIMPLE      02    // Simple enough to be STAR/PLUS operand.
#define SPSTART     04    // Starts with * or +.
#define WORST       0     // Worst case.


//: Return an expression that will match precisely c
// The returned string is owned by the function, and
// will be overwritten in subsequent calls.
const char * vul_reg_exp::protect(char c)
{
  //: This should be in thread local storage.
  static char pattern[3];

  if (std::strchr(META, c) != nullptr)
  {
    pattern[0] = '\\';
    pattern[1] = c;
    pattern[2] = 0;
  }
  else
  {
    pattern[0] = c;
    pattern[1] = 0;
  }
  return pattern;
}


/////////////////////////////////////////////////////////////////////////
//
//  COMPILE AND ASSOCIATED FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////


//
// Global work variables for compile().
//
static const char* regparse; // Input-scan pointer.
static       int   regnpar; // () count.
static       char  regdummy;
static       char* regcode; // Code-emit pointer; &regdummy = don't.
static       long  regsize; // Code size.

//
// Forward declarations for compile()'s friends.
//
static       char* reg (int, int*);
static       char* regbranch (int*);
static       char* regpiece (int*);
static       char* regatom (int*);
static       char* regnode (char);
static const char* regnext ( const char*);
static       char* regnext ( char*);
static void        regc (unsigned char);
static void        reginsert (char, char*);
static void        regtail (char*, const char*);
static void        regoptail (char*, const char*);


//
// We can't allocate space until we know how big the compiled form will be,
// but we can't compile it (and thus know how big it is) until we've got a
// place to put the code.  So we cheat:  we compile it twice, once with code
// generation turned off and size counting turned on, and once "for real".
// This also means that we don't allocate space until we are sure that the
// thing really will compile successfully, and we never have to move the
// code and thus invalidate pointers into it.  (Note that it has to be in
// one piece because free() must be able to free it all.)
//
// Beware that the optimization-preparation code in here knows about some
// of the structure of the compiled regexp.
//


//: Compile a regular expression into internal code for later pattern matching.

void vul_reg_exp::compile (char const* exp)
{
  const char*   scan;
  const char*   longest;
  unsigned long len;
  int           flags;

  if (exp == nullptr) {
    //RAISE Error, SYM(vul_reg_exp), SYM(No_Expr),
    std::cout << "vul_reg_exp::compile(): No expression supplied.\n";
    return;
  }

  // First pass: determine size, legality.
  regparse = exp;
  regnpar = 1;
  regsize = 0L;
  regcode = &regdummy;
  regc(MAGIC);
  if (!reg(0, &flags))
  {
    std::cout << "vul_reg_exp::compile(): Error in compile.\n";
    return;
  }
  this->startp[0] = this->endp[0] = this->searchstring = nullptr;

  // Small enough for pointer-storage convention?
  if (regsize >= 32767L) // Probably could be 65535L.
  {
    //RAISE Error, SYM(vul_reg_exp), SYM(Expr_Too_Big),
    std::cout << "vul_reg_exp::compile(): Expression too big.\n";
    return;
  }

  // Allocate space.
//#ifndef VCL_WIN32
  if (this->program != nullptr) delete [] this->program;
//#endif
  this->program = new char[regsize];
  this->progsize = (int) regsize;

  if (this->program == nullptr) {
    //RAISE Error, SYM(vul_reg_exp), SYM(Out_Of_Memory),
    std::cout << "vul_reg_exp::compile(): Out of memory.\n";
    return;
  }

  // Second pass: emit code.
  regparse = exp;
  regnpar = 1;
  regcode = this->program;
  regc(MAGIC);
  reg(0, &flags);

  // Dig out information for optimizations.
  this->regstart = '\0'; // Worst-case defaults.
  this->reganch = 0;
  this->regmust = nullptr;
  this->regmlen = 0;
  scan = this->program + 1; // First BRANCH.
  if (OP(regnext(scan)) == END) // Only one top-level choice.
  {
    scan = OPERAND(scan);

    // Starting-point info.
    if (OP(scan) == EXACTLY)
      this->regstart = *OPERAND(scan);
    else if (OP(scan) == BOL)
      this->reganch++;

     //
     // If there's something expensive in the r.e., find the longest
     // literal string that must appear and make it the regmust.  Resolve
     // ties in favor of later strings, since the regstart check works
     // with the beginning of the r.e. and avoiding duplication
     // strengthens checking.  Not a strong reason, but sufficient in the
     // absence of others.
     //
    if (flags & SPSTART) {
      longest = nullptr;
      len = 0L;
      for (; scan != nullptr; scan = regnext(scan))
        if (OP(scan) == EXACTLY && std::strlen(OPERAND(scan)) >= len) {
          longest = OPERAND(scan);
          len = (unsigned long)std::strlen(OPERAND(scan));
        }
      this->regmust = longest;
      this->regmlen = (int)len;
    }
  }
}


// regular expression, i.e. main body or parenthesized thing
//
// Caller must absorb opening parenthesis.
//
// Combining parenthesis handling with the base level of regular expression
// is a trifle forced, but the need to tie the tails of the branches to what
// follows makes it hard to avoid.
//
static char* reg (int paren, int *flagp)
{
  char* ret;
  char* br;
  char* ender;
  int   parno =0;
           int   flags;

  *flagp = HASWIDTH; // Tentatively.

  // Make an OPEN node, if parenthesized.
  if (paren) {
    if (regnpar >= vul_reg_exp_nsubexp) {
      //RAISE Error, SYM(vul_reg_exp), SYM(Too_Many_Parens),
      std::cout << "vul_reg_exp::compile(): Too many parentheses.\n";
      return nullptr;
    }
    parno = regnpar;
    regnpar++;
    ret = regnode(char(OPEN + parno));
  }
  else
    ret = nullptr;

  // Pick up the branches, linking them together.
  br = regbranch(&flags);
  if (br == nullptr)
    return nullptr;
  if (ret != nullptr)
    regtail(ret, br); // OPEN -> first.
  else
    ret = br;
  if (!(flags & HASWIDTH))
    *flagp &= ~HASWIDTH;
  *flagp |= flags & SPSTART;
  while (*regparse == '|')
  {
    regparse++;
    br = regbranch(&flags);
    if (br == nullptr)
      return nullptr;
    regtail(ret, br); // BRANCH -> BRANCH.
    if (!(flags & HASWIDTH))
      *flagp &= ~HASWIDTH;
    *flagp |= flags & SPSTART;
  }

  // Make a closing node, and hook it on the end.
  ender = regnode((paren) ? char(CLOSE + parno) : char(END));
  regtail(ret, ender);

  // Hook the tails of the branches to the closing node.
  for (br = ret; br != nullptr; br = regnext(br))
    regoptail(br, ender);

  // Check for proper termination.
  if (paren && *regparse++ != ')') {
    //RAISE Error, SYM(vul_reg_exp), SYM(Unmatched_Parens),
    std::cout << "vul_reg_exp::compile(): Unmatched parentheses.\n";
    return nullptr;
  }
  else if (!paren && *regparse != '\0') {
    if (*regparse == ')') {
      //RAISE Error, SYM(vul_reg_exp), SYM(Unmatched_Parens),
      std::cout << "vul_reg_exp::compile(): Unmatched parentheses.\n";
      return nullptr;
    }
    else {
      //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
      std::cout << "vul_reg_exp::compile(): Internal error.\n";
      return nullptr;
    }
    // NOTREACHED
  }
  return ret;
}


// one alternative of an | operator
//
// Implements the concatenation operator.
//
static char* regbranch (int *flagp)
{
  char* ret;
  char* chain;
  char* latest;
  int            flags;

  *flagp = WORST; // Tentatively.

  ret = regnode(BRANCH);
  chain = nullptr;
  while (*regparse != '\0' && *regparse != '|' && *regparse != ')')
  {
    latest = regpiece(&flags);
    if (latest == nullptr)
      return nullptr;
    *flagp |= flags & HASWIDTH;
    if (chain == nullptr) // First piece.
      *flagp |= flags & SPSTART;
    else
      regtail(chain, latest);
    chain = latest;
  }
  if (chain == nullptr) // Loop ran zero times.
    regnode(NOTHING);

  return ret;
}


//
// something followed by possible [*+?]
//
// Note that the branching code sequences used for ? and the general cases
// of * and + are somewhat optimized:  they use the same NOTHING node as
// both the endmarker for their branch list and the body of the last branch.
// It might seem that this node could be dispensed with entirely, but the
// endmarker role is not redundant.
//
static char* regpiece (int *flagp)
{
  char* ret;
  char  op;
  char* next;
  int            flags;

  ret = regatom(&flags);
  if (ret == nullptr)
    return nullptr;

  op = *regparse;
  if (!ISMULT(op)) {
    *flagp = flags;
    return ret;
  }

  if (!(flags & HASWIDTH) && op != '?') {
    //RAISE Error, SYM(vul_reg_exp), SYM(Empty_Operand),
    std::cout << "vul_reg_exp::compile() : *+ operand could be empty.\n";
    return nullptr;
  }
  *flagp = (op != '+') ? (WORST | SPSTART) : (WORST | HASWIDTH);

  if (op == '*' && (flags & SIMPLE))
    reginsert(STAR, ret);
  else if (op == '*') {
    // Emit x* as (x&|), where & means "self".
    reginsert(BRANCH, ret); // Either x
    regoptail(ret, regnode(BACK)); // and loop
    regoptail(ret, ret); // back
    regtail(ret, regnode(BRANCH)); // or
    regtail(ret, regnode(NOTHING)); // null.
  }
  else if (op == '+' && (flags & SIMPLE))
    reginsert(PLUS, ret);
  else if (op == '+') {
    // Emit x+ as x(&|), where & means "self".
    next = regnode(BRANCH); // Either
    regtail(ret, next);
    regtail(regnode(BACK), ret); // loop back
    regtail(next, regnode(BRANCH)); // or
    regtail(ret, regnode(NOTHING)); // null.
  }
  else if (op == '?') {
    // Emit x? as (x|)
    reginsert(BRANCH, ret); // Either x
    regtail(ret, regnode(BRANCH)); // or
    next = regnode(NOTHING);// null.
    regtail(ret, next);
    regoptail(ret, next);
  }
  regparse++;
  if (ISMULT(*regparse)) {
    //RAISE Error, SYM(vul_reg_exp), SYM(Nested_Operand),
    std::cout << "vul_reg_exp::compile(): Nested *?+.\n";
    return nullptr;
  }
  return ret;
}


// the lowest level
//
// Optimization:  gobbles an entire sequence of ordinary characters so that
// it can turn them into a single node, which is smaller to store and
// faster to run.  Backslashed characters are exceptions, each becoming a
// separate node; the code is simpler that way and it's not worth fixing.
//
static char* regatom (int *flagp)
{
  char* ret;
  int   flags;

  *flagp = WORST; // Tentatively.

  switch (*regparse++)
  {
   case '^':
    ret = regnode(BOL);
    break;
   case '$':
    ret = regnode(EOL);
    break;
   case '.':
    ret = regnode(ANY);
    *flagp |= HASWIDTH | SIMPLE;
    break;
   case '[':
   {
    int  rxpclass;
    int  rxpclassend;

    if (*regparse == '^') { // Complement of range.
      ret = regnode(ANYBUT);
      regparse++;
    }
    else
      ret = regnode(ANYOF);
    if (*regparse == ']' || *regparse == '-')
      regc(*regparse++);
    while (*regparse != '\0' && *regparse != ']')
    {
      if (*regparse == '-')
      {
        regparse++;
        if (*regparse == ']' || *regparse == '\0')
          regc('-');
        else {
          rxpclass = UCHARAT(regparse - 2) + 1;
          rxpclassend = UCHARAT(regparse);
          if (rxpclass > rxpclassend + 1) {
             //RAISE Error, SYM(vul_reg_exp), SYM(Invalid_Range),
             std::cout << "vul_reg_exp::compile(): Invalid range in [].\n";
             return nullptr;
          }
          for (; rxpclass <= rxpclassend; rxpclass++)
            regc(static_cast<unsigned char>(rxpclass));
          regparse++;
        }
      }
      else
        regc(*regparse++);
    }
    regc('\0');
    if (*regparse != ']') {
      //RAISE Error, SYM(vul_reg_exp), SYM(Unmatched_Bracket),
      std::cout << "vul_reg_exp::compile(): Unmatched [].\n";
      return nullptr;
    }
    regparse++;
    *flagp |= HASWIDTH | SIMPLE;
    break;
   }
   case '(':
    ret = reg(1, &flags);
    if (ret == nullptr)
      return nullptr;
    *flagp |= flags & (HASWIDTH | SPSTART);
    break;
   case '\0':
   case '|':
   case ')':
    //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
    std::cout << "vul_reg_exp::compile(): Internal error.\n"; // Never here
    return nullptr;
   case '?':
   case '+':
   case '*':
    //RAISE Error, SYM(vul_reg_exp), SYM(No_Operand),
    std::cout << "vul_reg_exp::compile(): ?+* follows nothing.\n";
    return nullptr;
   case '\\':
    if (*regparse == '\0') {
      //RAISE Error, SYM(vul_reg_exp), SYM(Trailing_Backslash),
      std::cout << "vul_reg_exp::compile(): Trailing backslash.\n";
      return nullptr;
    }
    ret = regnode(EXACTLY);
    regc(*regparse++);
    regc('\0');
    *flagp |= HASWIDTH | SIMPLE;
    break;
   default:
   {
    int  len;
    char   ender;

    regparse--;
    len = (int)std::strcspn(regparse, META);
    if (len <= 0) {
      //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
      std::cout << "vul_reg_exp::compile(): Internal error.\n";
      return nullptr;
    }
    ender = *(regparse + len);
    if (len > 1 && ISMULT(ender))
      len--; // Back off clear of ?+* operand.
    *flagp |= HASWIDTH;
    if (len == 1)
      *flagp |= SIMPLE;
    ret = regnode(EXACTLY);
    while (len > 0) {
      regc(*regparse++);
      len--;
    }
    regc('\0');
    break;
   }
  }
  return ret;
}


// emit a node
// Location.
//
static char* regnode (char op)
{
  char* ret;
  char* ptr;

  ret = regcode;
  if (ret == &regdummy) {
    regsize += 3;
    return ret;
  }

  ptr = ret;
  *ptr++ = op;
  *ptr++ = '\0'; // Null "next" pointer.
  *ptr++ = '\0';
  regcode = ptr;

  return ret;
}


// emit (if appropriate) a byte of code
//
static void regc (unsigned char b)
{
  if (regcode != &regdummy)
    *regcode++ = b;
  else
    regsize++;
}


// insert an operator in front of already-emitted operand
//
// Means relocating the operand.
//
static void reginsert (char op, char* opnd)
{
  char* src;
  char* dst;
  char* place;

  if (regcode == &regdummy) {
    regsize += 3;
    return;
  }

  src = regcode;
  regcode += 3;
  dst = regcode;
  while (src > opnd)
    *--dst = *--src;

  place = opnd; // Op node, where operand used to be.
  *place++ = op;
  *place++ = '\0';
  *place   = '\0';
}


// set the next-pointer at the end of a node chain
//
static void regtail (char* p, const char* val)
{
  char* scan;
  char* temp;
  std::ptrdiff_t  offset;

  if (p == &regdummy)
    return;

  // Find last node.
  scan = p;
  for (;;) {
    temp = regnext(scan);
    if (temp == nullptr)
      break;
    scan = temp;
  }

  if (OP(scan) == BACK)
    offset = (const char*)scan - val;
  else
    offset = val - scan;
  *(scan + 1) = (char)((offset >> 8) & 0377);
  *(scan + 2) = (char)(offset & 0377);
}


// regtail on operand of first argument; nop if operandless
//
static void regoptail (char* p, const char* val)
{
  // "Operandless" and "op != BRANCH" are synonymous in practice.
  if (p == nullptr || p == &regdummy || OP(p) != BRANCH)
    return;
  regtail(OPERAND(p), val);
}


////////////////////////////////////////////////////////////////////////
//
//  find and friends
//
////////////////////////////////////////////////////////////////////////


//
// Global work variables for find().
//
static const char*  reginput; // String-input pointer.
static const char*  regbol; // Beginning of input, for ^ check.
static const char* *regstartp; // Pointer to startp array.
static const char* *regendp; // Ditto for endp.

//
// Forwards.
//
static int regtry (const char*, const char* *, const char* *, const char*);
static int regmatch (const char*);
static int regrepeat (const char*);

#ifdef DEBUG
int      regnarrate = 0;
void     regdump ();
static char* regprop ();
#endif

bool vul_reg_exp::find (std::string const& s)
{
  return find(s.c_str());
}


//: Matches the regular expression to the given string.
// Returns true if found, and sets start and end indexes accordingly.

bool vul_reg_exp::find (char const* string)
{
  const char* s;

  this->searchstring = string;

   // Check validity of program.
  if (!this->program || UCHARAT(this->program) != MAGIC) {
    //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
    std::cout << "vul_reg_exp::find(): Compiled regular expression corrupted.\n";
    return false;
  }

  // If there is a "must appear" string, look for it.
  if (this->regmust != nullptr)
  {
    s = string;
    while ((s = std::strchr(s, this->regmust[0])) != nullptr) {
      if (std::strncmp(s, this->regmust, this->regmlen) == 0)
        break; // Found it.
      s++;
    }
    if (s == nullptr) // Not present.
      return false;
  }

  // Mark beginning of line for ^ .
  regbol = string;

  // Simplest case:  anchored match need be tried only once.
  if (this->reganch)
    return regtry(string, this->startp, this->endp, this->program) != 0;

  // Messy cases:  unanchored match.
  s = string;
  if (this->regstart != '\0')
    // We know what char it must start with.
    while ((s = std::strchr(s, this->regstart)) != nullptr) {
      if (regtry(s, this->startp, this->endp, this->program))
        return true;
      s++;
    }
  else
    // We don't - general case.
    do {
      if (regtry(s, this->startp, this->endp, this->program))
        return true;
    } while (*s++ != '\0');

  // Failure.
  return false;
}


// try match at specific point
// 0 failure, 1 success
//
static int regtry(const char* string, const char* *start,
                  const char* *end, const char* prog)
{
  int   i;
  const char* *sp1;
  const char* *ep;

  reginput = string;
  regstartp = start;
  regendp = end;

  sp1 = start;
  ep = end;
  for (i = vul_reg_exp_nsubexp; i > 0; i--) {
    *sp1++ = nullptr;
    *ep++ = nullptr;
  }
  if (regmatch(prog + 1)) {
    start[0] = string;
    end[0] = reginput;
    return 1;
  }
  else
    return 0;
}


// main matching routine
//
// Conceptually the strategy is simple:  check to see whether the current
// node matches, call self recursively to see whether the rest matches,
// and then act accordingly.  In practice we make some effort to avoid
// recursion, in particular by going through "ordinary" nodes (that don't
// need to know whether the rest of the match failed) by a loop instead of
// by recursion.
// 0 failure, 1 success
//
static int regmatch(const char* prog)
{
   const char* scan; // Current node.
  const char* next; // Next node.

  scan = prog;

  while (scan != nullptr)
  {
    next = regnext(scan);

    switch (OP(scan))
    {
     case BOL:
      if (reginput != regbol)
        return 0;
      break;
     case EOL:
      if (*reginput != '\0')
        return 0;
      break;
     case ANY:
      if (*reginput == '\0')
        return 0;
      reginput++;
      break;
     case EXACTLY:
     {
      int     len;
      const char* opnd;

      opnd = OPERAND(scan);
      // Inline the first character, for speed.
      if (*opnd != *reginput)
        return 0;
      len = (int)std::strlen(opnd);
      if (len > 1 && std::strncmp(opnd, reginput, len) != 0)
        return 0;
      reginput += len;
      break;
     }
     case ANYOF:
      if (*reginput == '\0' || std::strchr(OPERAND(scan), *reginput) == nullptr)
        return 0;
      reginput++;
      break;
     case ANYBUT:
      if (*reginput == '\0' || std::strchr(OPERAND(scan), *reginput) != nullptr)
        return 0;
      reginput++;
      break;
     case NOTHING:
      break;
     case BACK:
      break;
     case OPEN+1: case OPEN+2: case OPEN+3: case OPEN+4: case OPEN+5: case OPEN+6: case OPEN+7: case OPEN+8: case OPEN+9:
     {
      int no;
      const char* save;

      no = OP(scan) - OPEN;
      save = reginput;

      if (regmatch(next))
      {
        //
        // Don't set startp if some later invocation of the
        // same parentheses already has.
        //
        if (regstartp[no] == nullptr)
          regstartp[no] = save;
        return 1;
      }
      else
        return 0;
     }
     case CLOSE+1: case CLOSE+2: case CLOSE+3: case CLOSE+4: case CLOSE+5: case CLOSE+6: case CLOSE+7: case CLOSE+8: case CLOSE+9:
     {
      int  no;
      const char* save;

      no = OP(scan) - CLOSE;
      save = reginput;

      if (regmatch(next))
      {
        //
        // Don't set endp if some later invocation of the
        // same parentheses already has.
        //
        if (regendp[no] == nullptr)
          regendp[no] = save;
        return 1;
      }
      else
        return 0;
     }
     case BRANCH:
     {
      const char* save;

      if (OP(next) != BRANCH) // No choice.
        next = OPERAND(scan); // Avoid recursion.
      else {
        do {
          save = reginput;
          if (regmatch(OPERAND(scan)))
            return 1;
          reginput = save;
          scan = regnext(scan);
        } while (scan != nullptr && OP(scan) == BRANCH);
        return 0;
        // NOTREACHED
      }
      break;
     }
     case STAR:
     case PLUS:
     {
      char   nextch;
      int    no;
      const char* save;
      int    min_no;

      //
      // Lookahead to avoid useless match attempts when we know
      // what character comes next.
      //
      nextch = '\0';
      if (OP(next) == EXACTLY)
        nextch = *OPERAND(next);
      min_no = (OP(scan) == STAR) ? 0 : 1;
      save = reginput;
      no = regrepeat(OPERAND(scan));
      while (no >= min_no) {
        // If it could work, try it.
        if (nextch == '\0' || *reginput == nextch)
          if (regmatch(next))
            return 1;
        // Couldn't or didn't - back up.
        no--;
        reginput = save + no;
      }
      return 0;
     }
     case END:
      return 1; // Success!

     default:
      //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
      std::cout << "vul_reg_exp::find(): Internal error -- memory corrupted.\n";
      return 0;
    }
    scan = next;
  }

  //
  //  We get here only if there's trouble - normally "case END" is the
  //  terminating point.
  //
  //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
  std::cout << "vul_reg_exp::find(): Internal error -- corrupted pointers.\n";
  return 0;
}


// repeatedly match something simple, report how many
//
static int regrepeat(const char* p)
{
  int     count = 0;
  const char* scan;
  const char* opnd;

  scan = reginput;
  opnd = OPERAND(p);
  switch (OP(p))
  {
   case ANY:
    count = (int)std::strlen(scan);
    scan += count;
    break;
   case EXACTLY:
    while (*opnd == *scan) {
      count++;
      scan++;
    }
    break;
   case ANYOF:
    while (*scan != '\0' && std::strchr(opnd, *scan) != nullptr) {
      count++;
      scan++;
    }
    break;
   case ANYBUT:
    while (*scan != '\0' && std::strchr(opnd, *scan) == nullptr) {
      count++;
      scan++;
    }
    break;
   default: // Oh dear.  Called inappropriately.
    //RAISE Error, SYM(vul_reg_exp), SYM(Internal_Error),
    std::cout << "vul_reg_exp::find(): Internal error.\n";
    return 0;
  }
  reginput = scan;
  return count;
}


// dig the "next" pointer out of a node
//
static const char* regnext(const char* p)
{
  int offset;

  if (p == &regdummy)
    return nullptr;

  offset = NEXT(p);
  if (offset == 0)
    return nullptr;

  if (OP(p) == BACK)
    return p - offset;
  else
    return p + offset;
}


static char* regnext(char* p)
{
  int offset;

  if (p == &regdummy)
    return nullptr;

  offset = NEXT(p);
  if (offset == 0)
    return nullptr;

  if (OP(p) == BACK)
    return p - offset;
  else
    return p + offset;
}
