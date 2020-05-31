#
#   Get the version of Flex that Nabla has been built with and install it
#
cd /tmp
wget https://github.com/westes/flex/files/981163/flex-2.6.4.tar.gz
tar -zxvf flex-2.6.4.tar.gz
cd flex-2.6.4
./configure
sudo make 
sudo make install 
sudo rm /usr/bin/flex
sudo ln -s /usr/local/bin/flex /usr/bin/flex

#
#   Get the version of Bison that Nabla has been build with and install it
#
cd /tmp
wget https://ftp.gnu.org/gnu/bison/bison-3.5.1.tar.gz
tar -zxvf bison-3.5.1.tar.gz
cd bison-3.5.1
./configure
sudo make
sudo make install
sudo rm /usr/bin/bison
sudo ln -s /usr/local/bin/bison /usr/bin/bison