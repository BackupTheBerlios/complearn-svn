#!/bin/zsh
sudo rm /var/cache/pbuilder/result/*.(deb|dsc|changes|gz)
for i in *.dsc ; do
  echo "pbuilder build $i"
  sudo pbuilder build $i || echo "ERROR building $i"
done
