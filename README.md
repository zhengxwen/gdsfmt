gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
===

Version: 1.1.3

[![Build Status](https://travis-ci.org/zhengxwen/gdsfmt.png)](https://travis-ci.org/zhengxwen/gdsfmt)


## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms and include hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers with less than 8 bits, since a single genetic/genomic variant, like single-nucleotide polymorphism, usually occupies fewer bits than a byte. Data compression and decompression are also supported with relatively efficient random access. It is allowed to read a GDS file in parallel with multiple R processes supported by the parallel package.


## Importance

The version 1.1.3 should be installed immediately, if you see the error like
```
Invalid Zip Deflate Stream operation 'Seek'!
```

Changes in v1.1.1 - 1.1.3:

* minor fixes
* support efficient random access of zlib compressed data, which are composed of independent compressed blocks
* support LZ4 compression format (http://code.google.com/p/lz4/), based on "experimental lz4frame API" of r124
* include zlib_1.2.8 and LZ4 source codes
* allow R RAW data (interpreted as 8-bit signed integer) to replace 32-bit integer, with 'read.gdsn', 'readex.gdsn', 'apply.gdsn', 'clusterApply.gdsn', 'write.gdsn', 'append.gdsn'

Changes in v1.1.0:

	* fully support big-endian systems


## License

[LGPL-3](https://www.gnu.org/licenses/lgpl.html)


## Citation

Xiuwen Zheng, David Levine, Jess Shen, Stephanie M. Gogarten, Cathy Laurie, Bruce S. Weir. A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. Bioinformatics 2012; [doi:10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606)


## Package Author & Maintainer

Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com) / [zhengx@u.washington.edu](zhengx@u.washington.edu))


## Copyright
* CoreArray/gdsfmt, LGPL-3 License, Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com))
* zlib 1.2.8, ZLIB License, Jean-loup Gailly ([jloup@gzip.org](jloup@gzip.org)), Mark Adler ([madler@alumni.caltech.edu](madler@alumni.caltech.edu))
* LZ4, BSD License, Yann Collet ([https://code.google.com/p/lz4/](https://code.google.com/p/lz4/))


## URL

[http://github.com/zhengxwen/gdsfmt](http://github.com/zhengxwen/gdsfmt)


## Unit Testing

Comprehensive unit testing: [http://github.com/zhengxwen/unittest.gdsfmt](http://github.com/zhengxwen/unittest.gdsfmt)


## Examples

1. [Limited random-access reading on compressed data](https://github.com/zhengxwen/gdsfmt/wiki/Limited-random-access-reading-on-compressed-data)
2. [Transpose a matrix](https://github.com/zhengxwen/gdsfmt/wiki/Transpose-a-matrix)


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
wget --no-check-certificate https://github.com/zhengxwen/gdsfmt/tarball/master -O gdsfmt_latest.tar.gz
** Or **
curl -L https://github.com/zhengxwen/gdsfmt/tarball/master/ -o gdsfmt_latest.tar.gz

** Install **
R CMD INSTALL gdsfmt_latest.tar.gz
```
