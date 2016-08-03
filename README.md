# dynasp
##REQUIREMENTS
### Debian
#### System Packages 
 ```bash
 $ sudo apt-get install git libgmp-dev libtool-bin flex
 ```
#### External Packages
- cmake (3.2+)

 ```bash
 $ wget https://cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
 $ tar xf cmake-3.6.1-Linux-x86_64.tar.gz
 ```
 Add cmake bin to path
- automake (1.15+)

  ```bash
  $ wget http://ftp.gnu.org/gnu/automake/automake-1.15.tar.gz
  $ cd automake-1.15/
  $ ./configure --prefix=$HOME/.local && make -j5 install
  ```
  
- libtool (2.2+)
 ```bash
 $ http://mirror2.klaus-uwe.me/gnu/libtool/libtool-2.2.tar.bz2
 $ tar xjf libtool-2.2.tar.bz2 
 $ cd libtool-2.2/
 $ ./configure --prefix=$HOME/.local/usr
 $ make install
 ```
  
##INSTALLATION
  Just clone the repository including all the submodules via
  ```bash
  $ git clone --recursive https://github.com/daajoe/dynasp.git
  ```
  
  or
  
  ```bash
  $ git clone https://github.com/daajoe/dynasp.git && cd dynasp/
  $ git submodule init
  $ git submodule update --checkout --recursive
  ```
  
## BUILD
- prepare local root

  ```bash
  $ mkdir ~/.local
  ```
- build htd

   ```bash
   $ cd dynasp/htd
   $ cmake -DCMAKE_INSTALL_PREFIX:PATH=~/.local/usr && make -j 5 all install
   ```
- build sharp 

   ```bash
   $ cd dynasp/sharp
   $ autoreconf --force --install
   $ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/usr/lib LIBRARY_PATH=$LIBRARY_PATH:~/.local/usr/lib CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:~/.local/usr/include C_INCLUDE_PATH=$C_INCLUDE_PATH:~/.local/usr/include ./configure --prefix=$HOME/.local/usr
   $ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/usr/lib LIBRARY_PATH=$LIBRARY_PATH:~/.local/usr/lib CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:~/.local/usr/include C_INCLUDE_PATH=$C_INCLUDE_PATH:~/.local/usr/include make -j5 install
   ```
- build htd

   ```bash
   $ cd dynasp/
   $ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/usr/lib LIBRARY_PATH=$LIBRARY_PATH:~/.local/usr/lib CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:~/.local/usr/include C_INCLUDE_PATH=$C_INCLUDE_PATH:~/.local/usr/include ./configure --prefix=$HOME/.local/usr
   $ PATH=$PATH:~/.local/usr/bin LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.local/usr/lib LIBRARY_PATH=$LIBRARY_PATH:~/.local/usr/lib CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:~/.local/usr/include C_INCLUDE_PATH=$C_INCLUDE_PATH:~/.local/usr/include make install
    ```
