\name{readex.gdsn}
\alias{readex.gdsn}
\title{Read data field of a GDS node with a selection}
\description{
    Get data from a GDS node with subset selection.
}

\usage{
readex.gdsn(node, sel=NULL, simplify=c("auto", "none", "force"),
    .useraw=FALSE, .value=NULL, .val.replaced=NULL)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{sel}{a list of \code{m} logical vectors, where \code{m} is the
        number of dimensions of \code{node} and each logical vector should
        have the same size of dimension in \code{node}}
    \item{simplify}{if \code{"auto"}, the result is collapsed to be a vector
        if possible; \code{"force"}, the result is forced to be a vector}
    \item{.useraw}{use R RAW storage mode if integers can be stored in a byte,
        to reduce memory usage}
    \item{.value}{a vector of values to be replaced in the original data array,
        or NULL for nothing}
    \item{.val.replaced}{a vector of values after replacing, or NULL for
        nothing; \code{length(.val.replaced)} should be one or
        \code{length(.value)}; if \code{length(.val.replaced)} =
        \code{length(.value)}, it is a mapping from \code{.value} to
        \code{.val.replaced}}
}
\details{
    It is suggested to use logical vectors to extract data from the GDS node
instead of numeric vectors, if the dimension is preserved as data order saved
in the GDS file. If \code{sel} is a list of numeric vectors, the internal
method converts the numeric vectors to logical vectors first, extract data
with logical vectors, and finally call \code{\link{[}} to reorder or
expend data.
}
\value{
    Return an array.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{append.gdsn}},
    \code{\link{write.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "vector", 1:128)
add.gdsn(f, "matrix", matrix(as.character(1:(10*6)), nrow=10))
f

# read vector
readex.gdsn(index.gdsn(f, "vector"), sel=rep(c(TRUE, FALSE), 64))
readex.gdsn(index.gdsn(f, "vector"), sel=c(4:8, 1, 2, 12))
readex.gdsn(index.gdsn(f, "vector"), sel=-1:-10)

readex.gdsn(index.gdsn(f, "vector"), sel=c(4, 1, 10, NA, 12, NA))
readex.gdsn(index.gdsn(f, "vector"), sel=c(4, 1, 10, NA, 12, NA),
    .value=c(NA, 1, 12), .val.replaced=c(6, 7, NA))


# read matrix
readex.gdsn(index.gdsn(f, "matrix"))
readex.gdsn(index.gdsn(f, "matrix"),
    sel=list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)))
readex.gdsn(index.gdsn(f, "matrix"), sel=list(NULL, c(1,3,6)))
readex.gdsn(index.gdsn(f, "matrix"),
    sel=list(rep(c(TRUE, FALSE), 5), c(1,3,6)))
readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(1,3,6,10), c(1,3,6)))
readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(-1,-3), -6))

readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(1,3,NA,10), c(1,3,NA,5)))
readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(1,3,NA,10), c(1,3,NA,5)),
    simplify="force")

readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(1,3,NA,10), c(1,3,NA,5)))
readex.gdsn(index.gdsn(f, "matrix"), sel=list(c(1,3,NA,10), c(1,3,NA,5)),
    .value=NA, .val.replaced="X")


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
