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
* allow R RAW data (interpreted as 8-bit signed integer) to replace 32-bit integer, with 'read.gdsn', 'readex.gdsn', 'apply.gdsn', 'clusterApply.gdsn', 'write.gdsn', 'append.gdsn'

Changes in v1.1.0:

	* fully support big-endian systems


## License

[LGPL-3](https://www.gnu.org/licenses/lgpl.html)


## Citation

Xiuwen Zheng, David Levine, Jess Shen, Stephanie M. Gogarten, Cathy Laurie, Bruce S. Weir. A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. Bioinformatics 2012; [doi:10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606)


## Package Author & Maintainer

Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com) / [zhengx@u.washington.edu](zhengx@u.washington.edu))


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


## Copyright notice

* CoreArray/gdsfmt, LGPL-3 License, Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com))
```
CoreArray C/C++ library
Copyright (C) 2007-2014    Xiuwen Zheng

gdsfmt R package
Copyright (C) 2011-2014    Xiuwen Zheng

All rights reserved.

CoreArray is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License Version 3 as
published by the Free Software Foundation.

CoreArray is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with CoreArray.
If not, see <http://www.gnu.org/licenses/>.
```

* LZ4, BSD 2-clause License, Yann Collet ([https://code.google.com/p/lz4/](https://code.google.com/p/lz4/))
```
LZ4 Library
Copyright (c) 2011-2014, Yann Collet
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
