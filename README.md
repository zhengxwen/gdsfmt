gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
===

Version: 1.1.2

[![Build Status](https://travis-ci.org/zhengxwen/gdsfmt.png)](https://travis-ci.org/zhengxwen/gdsfmt)


## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms and include hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers with less than 8 bits, since a single genetic/genomic variant, such like single-nucleotide polymorphism, usually occupies fewer bits than a byte. Data compression and decompression are also supported. It is allowed to read a GDS file in parallel with multiple R processes supported by the parallel package.


## Importance

The version 1.1.2 should be installed immediately, if you see the error like
```
Invalid Zip Deflate Stream operation 'Seek'!
```

Changes in v1.1.2:

	* minor fixes
	* support efficient random access of compressed data, which are composed of independent compressed blocks

Changes in v1.1.0:

	* fully support big-endian systems



## License

LGPL-3


## Citation

Xiuwen Zheng, David Levine, Jess Shen, Stephanie M. Gogarten, Cathy Laurie, Bruce S. Weir. A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. Bioinformatics 2012; [doi:10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606)


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
[download the source code](https://github.com/zhengxwen/gdsfmt/tarball/master)
```
wget --no-check-certificate https://github.com/zhengxwen/gdsfmt/tarball/master -O gdsfmt_1.1.2.tar.gz
** Or **
curl -L https://github.com/zhengxwen/gdsfmt/tarball/master/ -o gdsfmt_1.1.2.tar.gz

** Install **
R CMD INSTALL gdsfmt_1.1.2.tar.gz
```
