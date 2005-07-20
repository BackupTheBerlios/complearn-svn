#!/bin/zsh
if [[ $# < 2 ]] ; then
  echo "Usage: $0 oldpattern newpattern files ..."
  exit 1
fi
progname=$0
firstpat=$1
shift
secondpat=$1
shift
echo "Doing <$firstpat> ===> <$secondpat>"
for i in $* ; do
  echo -n "... processing $i: "
  if ruby -00 -pe "gsub!(/$firstpat/, '$secondpat')" $i >$i.nw3 && mv $i.nw3 $i ; then
  echo ok
  else
  echo bad
  fi
done
echo "Done."
