git clone --single-branch --branch master https://github.com/NablaVM/libnabla 

cd libnabla

mkdir build
cd build

cmake ../lib -DCMAKE_BUILD_TYPE=Release
make -j4 

sudo make install
