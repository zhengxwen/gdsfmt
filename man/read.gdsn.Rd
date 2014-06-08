\name{read.gdsn}
\alias{read.gdsn}
\title{Read data field of a GDS node}
\description{
	Get data from a GDS node.
}

\usage{
read.gdsn(node, start=NULL, count=NULL, simplify=TRUE)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{start}{a vector of integers, starting from 1 for each dimension
		component}
	\item{count}{a vector of integers, the length of each dimnension. As a
		special case, the value "-1" indicates that all entries along that
		dimension should be written}
	\item{simplify}{if \code{TRUE}, the results are reduced to a vector
		if possible}
}
\details{
    \code{start}, \code{count}: the values in data are taken to be those
in the array with the leftmost subscript moving fastest.
}
\value{
	Return an array, \code{list}, or \code{data.frame}.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{readex.gdsn}}, \code{\link{append.gdsn}},
	\code{\link{write.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "vector", 1:128)
add.gdsn(f, "list", list(X=1:10, Y=seq(1, 10, 0.25)))
add.gdsn(f, "data.frame", data.frame(X=1:19, Y=seq(1, 10, 0.5)))
add.gdsn(f, "matrix", matrix(1:12, ncol=4))

f

read.gdsn(index.gdsn(f, "vector"))
read.gdsn(index.gdsn(f, "list"))
read.gdsn(index.gdsn(f, "data.frame"))


# the effects of 'simplify'
read.gdsn(index.gdsn(f, "matrix"), start=c(2,2), count=c(-1,1))
# [1] 5 6  <- a vector

read.gdsn(index.gdsn(f, "matrix"), start=c(2,2), count=c(-1,1), simplify=FALSE)
#      [,1]  <- a matrix
# [1,]    5
# [2,]    6


closefn.gds(f)


unlink("test.gds")
}

\keyword{GDS}
\keyword{utilities}
