#!/bin/zsh
for i in *.deb ; do
  echo $i
  lintian $i
  linda $i
done
