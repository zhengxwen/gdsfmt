\name{put.attr.gdsn}
\alias{put.attr.gdsn}
\title{Add an attribute into a GDS node}
\description{
	Add an attribute to a GDS node.
}

\usage{
put.attr.gdsn(node, name, val=NULL)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{name}{the name of an attribute}
	\item{val}{the value of an attribute}
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{get.attr.gdsn}}, \code{\link{delete.attr.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

node <- add.gdsn(f, "int", val=as.integer(1:10000))
put.attr.gdsn(node, "missing.value", 10000)
put.attr.gdsn(node, "one.value", 1L)
put.attr.gdsn(node, "string", c("ABCDEF", "THIS"))
put.attr.gdsn(node, "bool", c(TRUE, TRUE, FALSE))

f
get.attr.gdsn(node)

delete.attr.gdsn(node, "one.value")
get.attr.gdsn(node)

closefn.gds(f)
}

\keyword{GDS}
\keyword{utilities}
