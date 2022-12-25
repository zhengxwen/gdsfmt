gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
===

![LGPLv3](http://www.gnu.org/graphics/lgplv3-88x31.png)
[GNU Lesser General Public License, LGPL-3](https://www.gnu.org/licenses/lgpl.html)

[![Availability](http://www.bioconductor.org/shields/availability/release/gdsfmt.svg)](http://www.bioconductor.org/packages/gdsfmt)
[![Years-in-BioC](http://www.bioconductor.org/shields/years-in-bioc/gdsfmt.svg)](http://www.bioconductor.org/packages/release/bioc/html/gdsfmt.html)
[![Build status](https://ci.appveyor.com/api/projects/status/6ussam0n65o32r0j?svg=true)](https://ci.appveyor.com/project/zhengxwen/gdsfmt)


## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms with hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers of less than 8 bits, since a diploid genotype, like single-nucleotide polymorphism (SNP), usually occupies fewer bits than a byte. Data compression and decompression are available with relatively efficient random access. It is also allowed to read a GDS file in parallel with multiple R processes supported by the package parallel.


## Bioconductor:

Release Version: v1.34.0

[http://www.bioconductor.org/packages/release/bioc/html/gdsfmt.html](http://www.bioconductor.org/packages/release/bioc/html/gdsfmt.html)

[Help Documents](https://rdrr.io/bioc/gdsfmt/man)

[News](./NEWS): v1.34.0


## Package Vignettes

[http://bioconductor.org/packages/release/bioc/vignettes/gdsfmt/inst/doc/gdsfmt.html](http://bioconductor.org/packages/release/bioc/vignettes/gdsfmt/inst/doc/gdsfmt.html)


## Citations

Zheng X, Levine D, Shen J, Gogarten SM, Laurie C, Weir BS (2012). A High-performance Computing Toolset for Relatedness and Principal Component Analysis of SNP Data. *Bioinformatics*. [DOI: 10.1093/bioinformatics/bts606](http://dx.doi.org/10.1093/bioinformatics/bts606).

Zheng X, Gogarten S, Lawrence M, Stilp A, Conomos M, Weir BS, Laurie C, Levine D (2017). SeqArray -- A storage-efficient high-performance data format for WGS variant calls. *Bioinformatics*. [DOI: 10.1093/bioinformatics/btx145](http://dx.doi.org/10.1093/bioinformatics/btx145).


## Package Maintainer

Dr. Xiuwen Zheng ([zhengxwen@gmail.com](zhengxwen@gmail.com))


## URL

[https://bioconductor.org/packages/gdsfmt](https://bioconductor.org/packages/gdsfmt)

[https://github.com/zhengxwen/gdsfmt](https://github.com/zhengxwen/gdsfmt)


## Installation

* Bioconductor repository:
```R
if (!requireNamespace("BiocManager", quietly=TRUE))
    install.packages("BiocManager")
BiocManager::install("gdsfmt")
```

* Development version from Github (for developers/testers only):
```R
library("devtools")
install_github("zhengxwen/gdsfmt")
```
The `install_github()` approach requires that you build from source, i.e. `make` and compilers must be installed on your system -- see the [R FAQ](https://cran.r-project.org/faqs.html) for your operating system; you may also need to install dependencies manually.


## Copyright Notice

* CoreArray C++ library, LGPL-3 License, 2007-2021, Xiuwen Zheng
* zlib, zlib License, 1995-2017, Jean-loup Gailly and Mark Adler
* LZ4, BSD 2-clause License, 2011-2019, Yann Collet
* liblzma, public domain, 2005-2018, Lasse Collin and other xz contributors
* [README](./inst/COPYRIGHTS)


## GDS Command-line Tools

In the R environment,
```R
install.packages("getopt", repos="http://cran.r-project.org")
install.packages("optparse", repos="http://cran.r-project.org")
install.packages("crayon", repos="http://cran.r-project.org")

if (!requireNamespace("BiocManager", quietly=TRUE))
    install.packages("BiocManager")
BiocManager::install("gdsfmt")
```

[See More...](https://github.com/zhengxwen/Documents/tree/master/Program)

### *viewgds*

`viewgds` is a shell script written in R ([viewgds.R](https://github.com/zhengxwen/Documents/blob/master/Program/viewgds.R)), to view the contents of a GDS file. The R packages `gdsfmt`, `getopt` and `optparse` should be installed before running `viewgds`, and the package `crayon` is optional.

```
Usage: viewgds [options] file
```

Installation with command line,
```sh
curl -L https://raw.githubusercontent.com/zhengxwen/Documents/master/Program/viewgds.R > viewgds
chmod +x viewgds

## Or
wget -qO- --no-check-certificate https://raw.githubusercontent.com/zhengxwen/Documents/master/Program/viewgds.R > viewgds
chmod +x viewgds
```


### *diffgds*

`diffgds` is a shell script written in R ([diffgds.R](https://github.com/zhengxwen/Documents/blob/master/Program/diffgds.R)), to compare two files GDS files. The R packages `gdsfmt`, `getopt` and `optparse` should be installed before running `diffgds`.

```
Usage: diffgds [options] file1 file2
```

Installation with command line,
```sh
curl -L https://raw.githubusercontent.com/zhengxwen/Documents/master/Program/diffgds.R > diffgds
chmod +x diffgds

## Or
wget -qO- --no-check-certificate https://raw.githubusercontent.com/zhengxwen/Documents/master/Program/diffgds.R > diffgds
chmod +x diffgds
```


## Examples

```R
library(gdsfmt)

# create a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int", val=1:10000)
add.gdsn(f, "double", val=seq(1, 1000, 0.4))
add.gdsn(f, "character", val=c("int", "double", "logical", "factor"))
add.gdsn(f, "logical", val=rep(c(TRUE, FALSE, NA), 50))
add.gdsn(f, "factor", val=as.factor(c(NA, "AA", "CC")))
add.gdsn(f, "bit2", val=sample(0:3, 1000, replace=TRUE), storage="bit2")

# list and data.frame
add.gdsn(f, "list", val=list(X=1:10, Y=seq(1, 10, 0.25)))
add.gdsn(f, "data.frame", val=data.frame(X=1:19, Y=seq(1, 10, 0.5)))

folder <- addfolder.gdsn(f, "folder")
add.gdsn(folder, "int", val=1:1000)
add.gdsn(folder, "double", val=seq(1, 100, 0.4))

# show the contents
f

# close the GDS file
closefn.gds(f)
```

```
File: test.gds (1.1K)
+    [  ]
|--+ int   { Int32 10000, 39.1K }
|--+ double   { Float64 2498, 19.5K }
|--+ character   { Str8 4, 26B }
|--+ logical   { Int32,logical 150, 600B } *
|--+ factor   { Int32,factor 3, 12B } *
|--+ bit2   { Bit2 1000, 250B }
|--+ list   [ list ] *
|  |--+ X   { Int32 10, 40B }
|  \--+ Y   { Float64 37, 296B }
|--+ data.frame   [ data.frame ] *
|  |--+ X   { Int32 19, 76B }
|  \--+ Y   { Float64 19, 152B }
\--+ folder   [  ]
   |--+ int   { Int32 1000, 3.9K }
   \--+ double   { Float64 248, 1.9K }
```


## Also See

[pygds](https://github.com/CoreArray/pygds): Python interface to CoreArray Genomic Data Structure (GDS) files

[jugds.jl](https://github.com/CoreArray/jugds.jl): Julia interface to CoreArray Genomic Data Structure (GDS) files
