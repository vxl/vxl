#! /bin/sh

cvs=/data/target/46/objects/cvs-1.10-hacked-solaris-build/bin/cvs

#
# fsm
#

for d in $@; do
  if test -f $d; then
    echo "$d exists and is a file"
    exit 1
  fi

  if test -d $d; then
    echo "directory $d already exists"
    exit 1
  fi

  # update from Oxford
  $cvs -repo oxford up -dP $d || exit 1
  mv $d $d.oxo

  # update from Leuven
  $cvs -repo leuven up -dP $d || exit 1
  mv $d $d.leu

  # make unique list of files:
  (
    ( cd $d.oxo; find . )
    ( cd $d.leu; find . )
  ) | sort | uniq > ff

  # inspect all files in turn:
  for f in `cat ff`; do
    # -d in Oxford
    if test -d $d.oxo/$f; then
      # -d in Leuven
      if test -d $d.leu/$f; then
        echo "$d/$f : directory"
      # -f in Leuven
      elif test -f $d.leu/$f; then
        echo "$d.oxo/$f : directory"
        echo "$d.leu/$f : file"
        exit 1
      # not in Leuven
      else
        echo "$d.oxo/$f : no counterpart"
      fi

    # -f in Oxford
    elif test -f $d.oxo/$f; then
      # -d in Leuven
      if test -d $d.leu/$f; then
        echo "$d.oxo/$f : file"
        echo "$d.leu/$f : directory"
        exit 1
      # -f in Leuven
      elif test -f $d.leu/$f; then
        # compare them.
        if diff $d.oxo/$f $d.leu/$f >/dev/null; then
          echo "$d/$f : ok"
        else
          echo "$d/$f : conflict"
          mv $d.oxo/$f $d.oxo/$f.oxo
          mv $d.leu/$f $d.leu/$f.leu
        fi
      # not in Leuven
      else
        echo "$d.oxo/$f : no counterpart"
      fi

    # not in Oxford
    else
      echo "$d.leu/$f : no counterpart"
    fi
  done
  rm -f ff

  # now we can merge the two directories:
  for f in `cd $d.leu; find .`; do
    mv $d.leu/$f $d.oxo/$f
  done
  mv $d.oxo $d
  rmdir $d.leu

done
