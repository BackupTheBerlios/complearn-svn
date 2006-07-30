#!/bin/zsh
echo "INVOKING FULLREB with $*"
firstarg=$1
secondarg=$2
allargs=$*
export T=0

function doupdate () {
  echo Updating...
(cd /var/cache/pbuilder/result ; sudo apt-ftparchive packages . >/tmp/t$$.Packages ; sudo mv /tmp/t$$.Packages ./Packages ; sudo gzip -f -9 Packages )
  sudo pbuilder update
}

function docleaning () {
  echo Cleaning directory
  cd /var/cache/pbuilder/result
  sudo rm * || echo "(no files in result dir)"
  exit 0
}

function buildallof () {
tobuild=$*
echo "Trying to build"
for i in `echo $tobuild` ; do
  echo $i
done
echo "Pre-build condition:"

(cd /var/cache/pbuilder/result ; ls *.dsc || echo "(no debsrc)"; ls *.deb || echo "(no deb)")

(cd /var/cache/pbuilder/result ; sudo apt-ftparchive packages . >/tmp/t$$.Packages ; sudo mv /tmp/t$$.Packages ./Packages ; sudo gzip -f -9 Packages )

sudo pbuilder update

echo "BUILDING $allargs"
for i in `echo $tobuild` ; do
  echo "pbuilder build $i"
  sudo pbuilder build $i || echo "ERROR building $i"
done
}

if [ "x$firstarg" = "xup" ] ; then
  doupdate
  exit 0
fi

if [ "x$firstarg" = "xclean" ] ; then
  docleaning
  exit 0
fi

if [ "x$firstarg" = "xbuild" ] ; then
  sudo apt-get update
(
if [ "x$secondarg" = "xcl" ] ; then
echo "
libcsoap
libcomplearn
complearn-gui complearn-mpi libcomplearn-ruby
"
exit 0
fi
if [ "x$secondarg" = "xsvm" ] ; then
echo "
libsvm
libsvm-ruby
"
exit 0
fi
echo "Error, unknown project $secondarg" >&2
exit 1
) | while read line ; do
pushd .
T=`expr $T + 1` && dname=s$$-$T
sudo rm -rf $dname
mkdir $dname && cd $dname
echo "WORKING ON " `pwd` with "$line"
for pkgname in `echo $line` ; do
sudo apt-get source $pkgname
done
popd
done
#docleaning
echo "ABOUT TO BEGIN"
for dsc in `echo s${$}*/*.dsc` ; do
  echo "Doing $dsc"
  cd `dirname $dsc`
  if [ -e .builtyet ] ; then
    echo "Already built " `dirname $dsc`
  else
    echo "BUILDING in " `dirname $dsc` which is `pwd`
    ls
    echo "There's the stuff"
    buildallof *.dsc
    echo "Any left?"
    touch .builtyet
  fi
  cd ..
done
sudo rm -rf s$$*
exit 0
fi
echo "Trying to build $firstarg"
buildallof $firstarg
exit 0
