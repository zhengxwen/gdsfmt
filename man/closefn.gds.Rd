\name{closefn.gds}
\alias{closefn.gds}
\title{Close a GDS file}
\description{
	Close a CoreArray Genomic Data Structure (GDS) file.
}

\usage{
closefn.gds(gds)
}
\arguments{
	\item{gds}{an object of class \code{\link{gds.class}}, a GDS file}
}
\details{
	For better performance, data in a GDS file are usually cached in memory.
Keep in mind that the new file may not actually be written to disk, until
\code{\link{closefn.gds}} or \code{\link{sync.gds}} is called. Anyway, when
R shuts down, all GDS files created or opened would be automatically closed.
}
\value{
	None.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{createfn.gds}}, \code{\link{openfn.gds}},
	\code{\link{sync.gds}}
}

\keyword{GDS}
\keyword{utilities}
