What is knowledge reasoning for model?
Knowledge inference system is a c++ program for modeling complex geological structures, which is used to realize human-computer interaction and construct modeling constraints. The main features of the model include:

Modular design
C++ is used to realize the modular construction of knowledge reasoning, which is used to realize the construction of 3D geological structure modeling. The constraints and visualization of existing data are realized through the construction of topological rule base, and the wireframe model is constructed through expert knowledge. The wireframe model conforms to the existing expert cognition and realizes the network visualization and human-computer interaction with python

mpich or openmpi
PETSc
PETSc is a library that supports parallel linear algebra, krylov solvers, preconditioners and many other things. Note that PETSc must be compiled with MUMPS and HYPRE.
Metis
Metis is a library for partitioning graphs and finite element meshes.
Tetgen
TetGen is a program to generate tetrahedral meshes of any 3D polyhedral domains. EMFEM uses it to refine mesh adaptively.
Eigen
Eigen is a C++ template library for linear algebra.
nanoflann
nanoflann is a C++11 header-only library for building KD-Trees of datasets with different topologies. EMFEM uses it to find the vertex or cell closest to a receiver point efficiently.
The easiest way to install the dependencies of EMFEM is using spack. All the dependencies can be installed with

$ spack install mpi metis tetgen eigen petsc+mumps+hypre+superlu-dist nanoflann
Building
Once installed all the dependencies and unpacked the source code of EMFEM into a directory .


$ make
Note that EMFEM has only been tested on windows systems. 

Usage
To run the forward modeling process, the user needs to provide three files: the model file, the data template file, and the configuration file. Then use the flowing command to run emfem:

$ mpirun -np <# of processes> ./emfem -options_file <configuration-file-name>
For more details on the format of these files, please refer to the documentation.



Contributing
The users are encouraged to open an issue for any questions or bugs. Pull requests for any enhancement are also wellcomed.

Authors
xinran xu, University of Electronic and Science Technology of China, Email: xdsxxr@163.com.
cailu, University of Electronic and Science Technology of China, Email: cailu@uestc.edu.cn.
youming liu, University of Electronic and Science Technology of China, Email: 1354503251@qq.com.
