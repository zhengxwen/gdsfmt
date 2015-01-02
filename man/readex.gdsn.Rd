\name{readex.gdsn}
\alias{readex.gdsn}
\title{Read data field of a GDS node with a selection}
\description{
    Get data from a GDS node with subset selection.
}

\usage{
readex.gdsn(node, sel=NULL, simplify=c("auto", "none", "force"), .useraw=FALSE)
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

# read matrix
readex.gdsn(index.gdsn(f, "matrix"))
readex.gdsn(index.gdsn(f, "matrix"),
    sel=list(d1=rep(c(TRUE, FALSE), 5), d2=rep(c(TRUE, FALSE), 3)))

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
