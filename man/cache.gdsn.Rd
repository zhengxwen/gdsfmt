\name{cache.gdsn}
\alias{cache.gdsn}
\title{Caching variable data}
\description{
	Caching the data associated with a GDS variable
}

\usage{
cache.gdsn(node)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\details{
	If random access of array-based data is required, it is possible to
speed up the access time by caching data in memory. This function tries to
force the operating system to cache the data associated with the GDS node,
however how to cache data depends on the configure of operating system,
including memory size and caching strategy.

	If the data has been compressed, caching strategy almost has no effect
on random access, since the data has to be decompressed serially.
}
\value{
	None.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{read.gdsn}}
}

\keyword{GDS}
\keyword{utilities}
