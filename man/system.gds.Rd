\name{system.gds}
\alias{system.gds}
\title{Get the parameters in the GDS system}
\description{
	Get a list of parameters in the GDS system
}

\usage{
system.gds()
}
\value{
	A list including
	\item{num.logical.core}{the number of logical cores}
	\item{l1i.cache.size}{L1 instruction cache}
	\item{l1d.cache.size}{L1 data cache}
	\item{l2.cache.size}{L2 data cache}
	\item{l3.cache.size}{L3 data cache}
	\item{l4.cache.size}{L4 data cache}
	\item{compression.encoder}{compression/decompression algorithms}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\examples{
system.gds()
}

\keyword{GDS}
\keyword{utilities}
