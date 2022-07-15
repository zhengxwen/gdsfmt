\name{permdim.gdsn}
\alias{permdim.gdsn}
\title{Array Transposition}
\description{
    Transpose an array by permuting its dimensions.
}

\usage{
permdim.gdsn(node, dimidx, target=NULL)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{dimidx}{the subscript permutation vector, and it should be a
        permutation of the integers '1:n', where 'n' is the number of
        dimensions}
    \item{target}{if it is not \code{NULL}, the transposed data are saved to
        \code{target}}
}
\value{
    None.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{setdim.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

(node <- add.gdsn(f, "matrix", val=matrix(1:48, nrow=6),
    compress="ZIP", closezip=TRUE))
read.gdsn(node)

permdim.gdsn(node, c(2,1))
read.gdsn(node)


(node <- add.gdsn(f, "array", val=array(1:120, dim=c(5,4,3,2)),
    compress="ZIP", closezip=TRUE))
read.gdsn(node)

mat <- read.gdsn(node)
permdim.gdsn(node, c(1,2,3,4))
stopifnot(identical(mat, read.gdsn(node)))

mat <- read.gdsn(node)
permdim.gdsn(node, c(4,2,1,3))
stopifnot(identical(aperm(mat, c(4,2,1,3)), read.gdsn(node)))

mat <- read.gdsn(node)
permdim.gdsn(node, c(3,2,4,1))
stopifnot(identical(aperm(mat, c(3,2,4,1)), read.gdsn(node)))

mat <- read.gdsn(node)
permdim.gdsn(node, c(2,3,1,4))
stopifnot(identical(aperm(mat, c(2,3,1,4)), read.gdsn(node)))


# close the GDS file
closefn.gds(f)

# remove unused space after permuting dimensions
cleanup.gds("test.gds")


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
