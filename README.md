gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
===

Version: 1.1.2

[![Build Status](https://travis-ci.org/zhengxwen/gdsfmt.png)](https://travis-ci.org/zhengxwen/gdsfmt)


## Importance

The version 1.1.2 should be installed immediately, if you see the error like
```
Invalid Zip Deflate Stream operation 'Seek'!
```

Changes in 1.1.1 - 1.1.2:

	* minor fixes
	* support efficient random access of compressed data, which are composed of independent compressed blocks

Changes in 1.1.0:

	* fully support big-endian systems



## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms and include hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers with less than 8 bits, since a single genetic/genomic variant, such like single-nucleotide polymorphism, usually occupies fewer bits than a byte. Data compression and decompression are also supported. It is allowed to read a GDS file in parallel with multiple R processes supported by the parallel package.


## License

LGPL-3


## Package Author & Maintainer

Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com) / [zhengx@u.washington.edu](zhengx@u.washington.edu))


## URL

[http://github.com/zhengxwen/gdsfmt](http://github.com/zhengxwen/gdsfmt)


## Installation

* From CRAN (stable release 1.1.+)

* Development version from Github:
```
library("devtools")
install_github("zhengxwen/gdsfmt")
```
The `install_github()` approach requires that you build from source, i.e. `make` and compilers must be installed on your system -- see the R FAQ for your operating system; you may also need to install dependencies manually.

* Nearly up-to-date development binaries from `gdsfmt` r-forge repository:
```
install.packages("gdsfmt", repos="http://R-Forge.R-project.org")
```

* Install the package from the source code:
[download the source code](http://codeload.github.com/zhengxwen/gdsfmt/tar.gz/v1.1.2)
```
wget http://codeload.github.com/zhengxwen/gdsfmt/tar.gz/v1.1.2 -O gdsfmt_1.1.2.tar.gz
** Or **
curl http://codeload.github.com/zhengxwen/gdsfmt/tar.gz/v1.1.2 -o gdsfmt_1.1.2.tar.gz

** Install **
R CMD INSTALL gdsfmt_1.1.2.tar.gz
```
