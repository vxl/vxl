// This is contrib/prip/vdtop/vdtop_freeman_code.cxx

#include "vdtop_freeman_code.h"

const vxl_byte vdtop_freeman_code::_reduction [8][8] = {{0x00, 0x01, 0xF1, 0xF2, 0xFF, 0xF6, 0xF7, 0xF7},
		     {0x10, 0x11, 0x12, 0x22, 0xF2, 0xFF, 0xF0, 0x00},
		     {0xF1, 0x21, 0x22, 0x23, 0xF3, 0xF4, 0xFF, 0xF0},
		     {0xF2, 0x22, 0x32, 0x33, 0x34, 0x44, 0xF4, 0xFF},
		     {0xFF, 0xF2, 0xF3, 0x43, 0x44, 0x45, 0xF5, 0xF6},
		     {0xF6, 0xFF, 0xF4, 0x44, 0x54, 0x55, 0x56, 0x66},
		     {0xF7, 0xF0, 0xFF, 0xF4, 0xF5, 0x65, 0x66, 0x67},
		     {0x70, 0x00, 0xF0, 0xFF, 0xF6, 0x66, 0x76, 0x77}};

const vxl_byte vdtop_freeman_code::_normalized[8] = { 0, 1, 2, 3, 0, 1, 2, 3 } ;
const vxl_byte vdtop_freeman_code::_opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 } ;
const vxl_byte vdtop_freeman_code::_orthogonal[8] = { 2, 3, 4, 5, 6, 7, 0, 1} ;
const bool vdtop_freeman_code::_are_orthogonal[8][8] = {	{false, false, true, false, false, false, true, false},
			{false, false, false, true, false, false, false, true},
			{true, false, false, false, true, false, false, false},
			{false, true, false, false, false, true, false, false},
			{false, false, true, false, false, false, true, false},
			{false, false, false, true, false, false, false, true},
			{true, false, false, false, true, false, false, false},
			{false, true, false, false, false, true, false, false}};
//const vxl_byte vdtop_freeman_code::pc_difcd[3][3] = { {3, 2, 1},
//		       {4, 8, 0},
//		       {5, 6, 7} };
const vxl_byte vdtop_freeman_code::_mask [8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
const int vdtop_freeman_code::_di[8] = {1, 1, 0, -1, -1, -1, 0, 1} ;
const int vdtop_freeman_code::_dj[8] = {0, -1, -1, -1, 0, 1, 1, 1} ;
