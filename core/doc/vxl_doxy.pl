#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Script to change the perceps documentation format to Doxygen (JavaDoc) format
# Authors:
#         Dave Cooper
#         Maarten Vergauwen
# Date:
#      17/02/2000
# Modified:
# 11 April 2001 Ian Scott.   Remove support for old perceps commands
#  5 May   2001 Geoff Cross. Correctly handle end of verbatim blocks. Allow two contiguous comments
#  10 May  2001 Ian Scott. Merged Geoffs and my changes


# patterns to be matched
$verbpatt = '\\verbatim';
$endverbpatt = '\\endverbatim';
$slashslashpatt = '^\s*//';
$slashslashcolonpatt = '^\s*//:';
$slashstarstarpatt = '/**';
$spacespacepatt = '  ';
$starpatt = '*';
$starslashpatt = '*/';

# variables that keep state:

# comment found -> first line should start with /**, next lines with *, last line with */
$comment = 0;

# verbatim found -> lines should not start with * (visible in Doxygen)
$verbatim = 0;
# finish verbatim mode at the end of this line.
$should_end_verbatim = 0;

$debug = 0;

# mainloop
while (<>)
{
    if ( $should_end_verbatim )
    {
        $verbatim = 0;
        $should_end_verbatim = 0;
    }

    # found verbatim ?
    if ( m/$verbpatt/ ) { $verbatim = 1; };

    # found endverbatim ?
    if ( m/$endverbpatt/ ) { $should_end_verbatim = 1; };

    # found start of comment: "//:"  ?
    if ( s!$slashslashcolonpatt!$slashstarstarpatt! )
    {
        chomp;
        # escape all dots, and add a dot at the end:
        s/\./\\\./g; s/\\\.\s*$//; s/$/.\n/;
        if ($comment)
        {
            # Previous comment hasn't ended--two contiguous comment blocks.
            # (Happens at the top of .txx files, for example.)
            print "*\/ \n";
        }
        $comment = 1;
        print; next;
    }

    # found continuation of comment WITH verbatim -> no "*"
    if ( m!$slashslashpatt! && $verbatim && $comment)
    {
        s!$slashslashpatt!$spacespacepatt!;
        print; next;
    }

    # found continuation of comment WITHOUT verbatim -> start line with "*"
    if ( m!$slashslashpatt! && $comment )
    {
        s!$slashslashpatt!$starpatt!;
        print; next;
    }

    # found end of comment -> start line with */
    # NOTE that *every* line within a comment (also empty lines) *must* start with // !
    # (In an earlier version of this script, empty lines were allowed inside comments.)
    if ( $comment && ! m!$slashslashpatt!  )
    {
        print "$starslashpatt\n";
        $comment = 0;
        print; next;
    }

    # just print line if not in comment or in file
    if ( !$comment ) { print; next; }

    # debug - print unprocessed lines (s.b. none)
    if ($debug) { print "LNP:\t"; print; }
}
