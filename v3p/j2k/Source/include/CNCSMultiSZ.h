/********************************************************
** Copyright 2000 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
**
** FILE:   	NCSUtil\CNCSMultiSZ.h
** CREATED:	10Apr00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	MultiSZ string manipulation code
** EDITS:
** 01 ny 05May00	Added overridden get() function to
**					retrieve a specific line of text in
**					the MULTI_SZ
 *******************************************************/

#ifndef CNCSMULTISZ_H
#define CNCSMULTISZ_H

#include "NCSDefs.h"

const int MAX_MSZ_NUM_LINE = 256;
const int MAX_MSZ_LINE_LENGTH = 1024;

class NCS_EXPORT CNCSMultiSZ {
public :
  CNCSMultiSZ();
  ~CNCSMultiSZ();
  void print();
  int strstr(char *entryName);
  size_t mszlength();
  void assign(unsigned char *chBuffer);
  void get(unsigned char *ptr);
  BOOLEAN get(int index, wchar_t *ptr); /**[01]**/
  void append(char *newEntryName);
  void replace(char *newEntryName,int index);
  void remove(int entryIndex);
protected :
  void assemble();
  void disassemble(unsigned char *chBuffer);
private :
  char szApps[MAX_MSZ_NUM_LINE][MAX_MSZ_LINE_LENGTH];
  wchar_t wcApps[MAX_MSZ_NUM_LINE][MAX_MSZ_LINE_LENGTH];
  int wcLineCount;
  unsigned char new_chBuffer[1024];
  unsigned char input_multiSZBuffer[2048];
  unsigned char output_multiSZBuffer[2048];
  size_t szlength(unsigned char *chBuffer);
};

#endif
