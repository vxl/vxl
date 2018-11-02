// SimpleTest.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <cstdio>
#include <cstring>
#include "StdAfx.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <expatpplib.h> // resides in b3p/expatpp

class myParser : public expatpp
{
 public:  myParser() : mDepth(0) {}
  virtual void startElement(const XML_Char *name, const XML_Char **atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char *s, int len);

 private:
  void WriteIndent();
  int mDepth;
};


void myParser::WriteIndent()
{
 for (int i=0; i<mDepth; i++)
   std::putchar('\t');
}


void myParser::startElement(const char* name, const char** atts)
{
  WriteIndent();
  std::puts(name);
  if (atts) {  /* write list of attributes indented below element */
    int i;
    for (i=0; atts[i]; i++) {
      WriteIndent();
      std::putchar('-'); std::putchar(' ');
      std::puts(atts[i]);
    }
  }
  mDepth++;
}


void myParser::endElement(const char*)
{
   mDepth--;
}


void myParser::charData(const XML_Char *s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len==leadingSpace)
    return;  // called with whitespace between elements
  WriteIndent();

  /* write out the user data bracketed by ()*/
  std::putchar('(');
  std::fwrite(s, len, 1, stdout);
  std::puts(")");
}


int main(int argc, char* argv[])
{
  myParser parser;
  char filename[80];
  FILE* xmlFile;
  for (;;) {
    int depth = 0;
    std::puts("\n\nXML test: enter filename");
    std::gets(filename);
    if (std::strlen(filename)==0)
      break;
    xmlFile = std::fopen(filename, "r");
    if (!xmlFile)
      break;
    if (!parser.parseFile(xmlFile)) {
      std::fprintf(stderr,
                  "%s at line %d\n",
                  XML_ErrorString(parser.XML_GetErrorCode()),
                  parser.XML_GetCurrentLineNumber()
                 );
      return 1;
    }
  }  // loop asking for and parsing files
  std::puts("\nfinished!");
  return 0;
}
