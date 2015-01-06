gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
===

[![Build Status](https://travis-ci.org/zhengxwen/gdsfmt.png)](https://travis-ci.org/zhengxwen/gdsfmt)


## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms and include hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers with less than 8 bits, since a single genetic/genomic variant, like single-nucleotide polymorphism, usually occupies fewer bits than a byte. Data compression and decompression are also supported with relatively efficient random access. It is allowed to read a GDS file in parallel with multiple R processes supported by the parallel package.


## License

![LGPLv3](http://www.gnu.org/graphics/lgplv3-88x31.png)
[LGPL-3](https://www.gnu.org/licenses/lgpl.html)


## Citation

Xiuwen Zheng, David Levine, Jess Shen, Stephanie M. Gogarten, Cathy Laurie, Bruce S. Weir. A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. Bioinformatics 2012; [doi:10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606)


## Package Maintainer

Xiuwen Zheng ([zhengx@u.washington.edu](zhengx@u.washington.edu))


## URL

[http://github.com/zhengxwen/gdsfmt](http://github.com/zhengxwen/gdsfmt)


## Unit Testing

Comprehensive unit testing:

[http://github.com/zhengxwen/unittest.gdsfmt](http://github.com/zhengxwen/unittest.gdsfmt)


## Installation

* Development version from Github:
```R
source("http://bioconductor.org/biocLite.R")
biocLite("gdsfmt")
```

## Copyright notice

* CoreArray C++ library, LGPL-3 License, 2007-2015, Xiuwen Zheng
* zlib, zlib License, 1995-2015, Jean-loup Gailly and Mark Adler
* LZ4, BSD 2-clause License, 2011-2015, Yann Collet
