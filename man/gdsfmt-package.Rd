\name{gdsfmt-package}
\alias{gdsfmt-package}
\alias{gdsfmt}
\docType{package}
\title{CoreArray Genomic Data Structure (GDS) R Interface}
\description{
	R interface of CoreArray GDS is based on the CoreArray project initiated
and developed from 2007.
}
\details{
\tabular{ll}{
	Package: \tab gdsfmt\cr
	Type: \tab Package\cr
	Version: \tab 1.0.1\cr
	License: \tab LGPL version 3\cr
}
	R interface of CoreArray GDS is based on the CoreArray project initiated
and developed from 2007 (\url{http://corearray.sourceforge.net}). The CoreArray
project is to develop portable, scalable, bioinformatics data visualization
and storage technologies.

	R is the most popular statistical environment, but one not necessarily
optimized for high performance or parallel computing which ease the burden of
large-scale calculations. To support efficient data management in parallel for
numerical genomic data, we developed the Genomic Data Structure (GDS) file
format. \code{gdsfmt} provides fundamental functions to support accessing data
in parallel, and allows future R packages to call these functions.

	Forums:	\url{http://sourceforge.net/projects/corearray/forums}
}
\references{
	\url{http://corearray.sourceforge.net}

	Xiuwen Zheng, David Levine, Jess Shen, Stephanie M. Gogarten,
Cathy Laurie, Bruce S. Weir. A High-performance Computing Toolset for
Relatedness and Principal Component Analysis of SNP Data.
Bioinformatics 2012; doi: 10.1093/bioinformatics/bts606
}
\author{Xiuwen Zheng \email{zhengx@u.washington.edu}}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")
L <- -2500:2499

# commom types
add.gdsn(f, "label", NULL)
add.gdsn(f, "int", val=as.integer(1:10000), compress="ZIP", closezip=TRUE)
add.gdsn(f, "int.matrix", val=matrix(L, nrow=100, ncol=50))
add.gdsn(f, "mat", val=matrix(1:(10*6), nrow=10))
add.gdsn(f, "double", val=seq(1, 1000, 0.4))
add.gdsn(f, "character", val=c("int", "double", "logical", "factor"))
add.gdsn(f, "logical", val=rep(c(TRUE, FALSE, NA), 50))
add.gdsn(f, "factor", val=as.factor(c(letters, NA, "AA", "CC")))
add.gdsn(f, "NA", val=rep(NA, 10))
add.gdsn(f, "NaN", val=c(rep(NaN, 20), 1:20))
add.gdsn(f, "bit2-matrix", val=matrix(L[1:5000], nrow=50, ncol=100), storage="bit2")
# list and data.frame
add.gdsn(f, "list", val=list(X=1:10, Y=seq(1, 10, 0.25)))
add.gdsn(f, "data.frame", val=data.frame(X=1:19, Y=seq(1, 10, 0.5)))

# save a .RData object
obj <- list(X=1:10, Y=seq(1, 10, 0.1))
save(obj, file="tmp.RData")
addfile.gdsn(f, "tmp.RData", filename="tmp.RData")

f

read.gdsn(index.gdsn(f, "list"))
read.gdsn(index.gdsn(f, "list/Y"))
read.gdsn(index.gdsn(f, "data.frame"))
read.gdsn(index.gdsn(f, "mat"))

# Apply functions over columns of matrix
tmp <- apply.gdsn(index.gdsn(f, "mat"), margin=2, FUN=function(x) print(x))
tmp <- apply.gdsn(index.gdsn(f, "mat"), margin=2,
	selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
	FUN=function(x) print(x))

closefn.gds(f)
}

\keyword{GDS}
\keyword{utilities}
\keyword{IO}
\keyword{interface}
\keyword{file}
\keyword{database}
