\name{gds.class}
\docType{class}
\alias{gds.class}

\title{the class of GDS file}
\description{
	The class of a CoreArray Genomic Data Structure (GDS) file.
}

\value{
	There are three components:
	\item{filename}{the file name to be created}
	\item{id}{internal file id, an integer}
	\item{root}{an object of class \code{\link{gdsn.class}},
		the root of hierachical structure}
	\item{readonly}{whether it is read-only or not}
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{createfn.gds}}, \code{\link{openfn.gds}},
	\code{\link{closefn.gds}}
}

\keyword{GDS}
\keyword{utilities}
