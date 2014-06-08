\name{sync.gds}
\alias{sync.gds}
\title{Synchronize a GDS file}
\description{
	Write the data cached in memory to disk.
}

\usage{
sync.gds(gdsfile)
}
\arguments{
	\item{gdsfile}{An object of class \code{\link{gds.class}}, a GDS file}
}
\details{
	For better performance, Data in a GDS file are usually cached in memory.
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
	\code{\link{createfn.gds}}, \code{\link{openfn.gds}}
}

\keyword{GDS}
\keyword{utilities}
