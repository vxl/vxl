/*
 *
 *  Copyright (C) 1999-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Define alias for cout, cerr and clog
 *
 */

#include "osconfig.h"
#include "ofconsol.h"
#include "ofthread.h"

#define INCLUDE_CASSERT
#include "ofstdinc.h"


#ifdef DCMTK_GUI
  OFOStringStream COUT;
  OFOStringStream CERR;
#endif

OFConsole::OFConsole()
#ifdef DCMTK_GUI
: currentCout(&COUT)
, currentCerr(&CERR)
#else
: currentCout(&cout)
, currentCerr(&cerr)
#endif
, joined(0)
#ifdef _REENTRANT
, coutMutex()
, cerrMutex()
#endif
{
}

ostream *OFConsole::setCout(ostream *newCout)
{
  lockCout();
  ostream *tmpCout = currentCout;
#ifdef DCMTK_GUI
  if (newCout) currentCout = newCout; else currentCout = &COUT;
#else
  if (newCout) currentCout = newCout; else currentCout = &cout;
#endif
  unlockCout();
  return tmpCout;
}

ostream *OFConsole::setCerr(ostream *newCerr)
{
  lockCerr();
  ostream *tmpCerr = currentCerr;
#ifdef DCMTK_GUI
  if (newCerr) currentCerr = newCerr; else currentCerr = &CERR;
#else
  if (newCerr) currentCerr = newCerr; else currentCerr = &cerr;
#endif
  unlockCerr();
  return tmpCerr;
}

void OFConsole::join()
{
  lockCerr();
  if (!joined)
  {
    // changing the state of "joined" requires that both mutexes are locked.
    // Mutexes must always be locked in the same order to avoid deadlocks.
    lockCout();
    joined = 1;
  }

  // now status is joined, so unlockCerr implicitly unlocks both mutexes
  unlockCerr();
  return;
}

void OFConsole::split()
{
  lockCerr();
  if (joined)
  {
    // since status is joined, lockCerr() has locked both mutexes
    joined = 0;

    // now status is unjoined, we have to unlock both mutexes manually
    unlockCout();
  }
  unlockCerr();
  return;
}

OFBool OFConsole::isJoined()
{
  lockCerr();
  // nobody will change "joined" if we have locked either mutex
  int result = joined;
  unlockCerr();
  if (result) return OFTrue; else return OFFalse;
}

OFConsole& OFConsole::instance()
{
  static OFConsole instance_;
  return instance_;
}


class OFConsoleInitializer
{
 public:
  OFConsoleInitializer()
  {
    OFConsole::instance();
  }
};

/* the constructor of this global object makes sure
 * that ofConsole is initialized before main starts.
 * Required to make ofConsole thread-safe.
 */
OFConsoleInitializer ofConsoleInitializer;
