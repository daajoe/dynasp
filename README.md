# dynasp
##REQUIREMENTS
### Debian
#### System Packages 
 ```bash
 $ sudo apt-get install git libgmp-dev libtool-bin flex bison
 ```
#### External Packages
- cmake (3.2+)

 ```bash
 $ wget https://cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
 $ tar xf cmake-3.6.1-Linux-x86_64.tar.gz
 ```
 Add cmake bin to path

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
   $ cmake -DCMAKE_INSTALL_PREFIX:PATH=~/.local/usr .
   $ make install
   ```
- build htd

   ```bash
   $ cd dynasp/
   $ cmake -DCMAKE_INSTALL_PREFIX:PATH=~/.local/usr .
   $ make install
    ```
