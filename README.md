gdsfmt: R Interface to CoreArray Genomic Data Structure (GDS) files
====

## Features

This package provides a high-level R interface to CoreArray Genomic Data Structure (GDS) data files, which are portable across platforms and include hierarchical structure to store multiple scalable array-oriented data sets with metadata information. It is suited for large-scale datasets, especially for data which are much larger than the available random-access memory. The gdsfmt package offers the efficient operations specifically designed for integers with less than 8 bits, since a single genetic/genomic variant, such like single-nucleotide polymorphism, usually occupies fewer bits than a byte. Data compression and decompression are also supported. It is allowed to read a GDS file in parallel with multiple R processes supported by the parallel package.

## Installation

* From CRAN (stable release 1.0.+)
* Development version from Github:
```
library("devtools")
install_github("gdsfmt", user="zhengxwen")
```
The `install_github()` approach requires that you build from source, i.e. `make` and compilers must be installed on your system -- see the R FAQ for your operating system; you may also need to install dependencies manually.
* Nearly up-to-date development binaries from `gdsfmt` r-forge repository:
```
install.packages("gdsfmt",
   repos=c("http://gdsfmt.r-forge.r-project.org/repos",
          getOption("repos")[["CRAN"]]))
```
