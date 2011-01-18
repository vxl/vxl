/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	include\NCSnet.h
** CREATED:	
** AUTHOR: 	Doug Mansell
** PURPOSE:	cnet/snet shared private header
** EDITS:
 *******************************************************/

#ifndef NCSNET_H
#define NCSNET_H

/*
** protocol commands 
*/

#define NCSNET_PACKET			0
#define NCSNET_CHUNKED_PACKET	1
#define NCSNET_CHUNK			2

#define NCSNET_COMMAND(command,length) ((UINT32)((command)<<24)|((length)&0xffffff))

#endif
