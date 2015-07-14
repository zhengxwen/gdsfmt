\name{read.gdsn}
\alias{read.gdsn}
\title{Read data field of a GDS node}
\description{
    Get data from a GDS node.
}

\usage{
read.gdsn(node, start=NULL, count=NULL,
    simplify=c("auto", "none", "force"), .useraw=FALSE, .value=NULL,
    .substitute=NULL)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{start}{a vector of integers, starting from 1 for each dimension
        component}
    \item{count}{a vector of integers, the length of each dimnension. As a
        special case, the value "-1" indicates that all entries along that
        dimension should be written}
    \item{simplify}{if \code{"auto"}, the result is collapsed to be a vector
        if possible; \code{"force"}, the result is forced to be a vector}
    \item{.useraw}{use R RAW storage mode if integers can be stored in a byte,
        to reduce memory usage}
    \item{.value}{a vector of values to be replaced in the original data array,
        or NULL for nothing}
    \item{.substitute}{a vector of values after replacing, or NULL for
        nothing; \code{length(.substitute)} should be one or
        \code{length(.value)}; if \code{length(.substitute)} =
        \code{length(.value)}, it is a mapping from \code{.value} to
        \code{.substitute}}
}
\details{
    \code{start}, \code{count}: the values in data are taken to be those
in the array with the leftmost subscript moving fastest.
}
\value{
    Return an array, \code{list}, or \code{data.frame}.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
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

read.gdsn(index.gdsn(f, "matrix"), start=c(2,2), count=c(-1,1),
    simplify="none")
#      [,1]  <- a matrix
# [1,]    5
# [2,]    6

read.gdsn(index.gdsn(f, "matrix"), start=c(2,2), count=c(-1,3))
read.gdsn(index.gdsn(f, "matrix"), start=c(2,2), count=c(-1,3),
    .value=c(12,5), .substitute=NA)


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
