#!/bin/bash
sudo apt-get update
sudo apt-get install build-essential -y
sudo apt-get install gcc -y
sudo apt-get install g++ -y
sudo apt-get install bison -y 
sudo apt-get install flex -y
sudo apt-get install perl -y
sudo apt-get install qt5-default -y
sudo apt-get install tcl-dev -y
sudo apt-get install tk-dev -y
sudo apt-get install libxml2-dev -y
sudo apt-get install zlib1g-dev -y
sudo apt-get install default-jre -y
sudo apt-get install doxygen -y
sudo apt-get install graphviz -y
sudo apt-get install libwebkitgtk-3.0-0 -y
sudo apt-get install libopenscenegraph-3.4-dev -y
sudo apt-get install libopenscenegraph-dev -y
sudo apt-get install openscenegraph-plugin-osgearth -y
sudo apt-get install libosgearth-dev -y
sudo apt-get install openmpi-bin -y
sudo apt-get install libopenmpi-dev -y
sudo apt-get install python -y

mv ./lib/omnetpp-5.2.1-src-core.tgz ~
cd ~

tar -xzf omnetpp-5.2.1-src-core.tgz
cd ~/omnetpp-5.2.1

make cleanall

'. setenv'

./configure WITH_TKENV=no WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no

make

x=$(grep -c 'omnet' .bashrc)
if [ $x -le 0 ]; then
  echo "export PATH="$HOME/omnetpp-5.2.1/bin:$PATH"" >> ~/.bashrc
fi

source ~/.bash
source ~/.bashrc


cd simulator-stream-processing
make clean
make
