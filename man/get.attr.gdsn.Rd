\name{get.attr.gdsn}
\alias{get.attr.gdsn}
\title{Get attributes}
\description{
	Get the attributes of a GDS node.
}

\usage{
get.attr.gdsn(node)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
	A list of attributes.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{put.attr.gdsn}}, \code{\link{delete.attr.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

node <- add.gdsn(f, "int", val=as.integer(1:10000))
put.attr.gdsn(node, "missing.value", 10000)

f
get.attr.gdsn(node)

closefn.gds(f)


unlink("test.gds")
}

\keyword{GDS}
\keyword{utilities}
