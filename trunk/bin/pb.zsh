#!/bin/zsh
pushd .
for i in *.dsc ; do
  {sudo pbuilder build $i && echo "BUILD OK"} | tee logfile.txt
  popd
done
