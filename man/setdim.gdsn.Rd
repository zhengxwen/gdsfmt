\name{setdim.gdsn}
\alias{setdim.gdsn}
\title{Set the dimension of data field}
\description{
    Assign new dimensions to the data field of a GDS node.
}

\usage{
setdim.gdsn(node, valdim)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{valdim}{the new dimension(s) for the array to be created, which
        is a vector of length one or more giving the maximal indices in
        each dimension. The values in data are taken to be those in the array
        with the leftmost subscript moving fastest. The last entry could be
        ZERO. If the total number of elements is zero, gdsfmt does not allocate
        storage space. \code{NA} is treated as 0.}
}
\value{
    Returns \code{node}.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{write.gdsn}},
    \code{\link{append.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

n <- add.gdsn(f, "int", val=1:100)
f

setdim.gdsn(n, c(120, 4))
f

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
