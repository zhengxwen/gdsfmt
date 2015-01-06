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
however how to cache data depends on the configuration of operating system,
including system memory and caching strategy. Note that this function does
not explicitly allocate memory for the data.

    If the data has been compressed, caching strategy almost has no effect
on random access, since the data has to be decompressed serially.
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

n <- add.gdsn(f, "int.matrix", matrix(1:50*100, nrow=100, ncol=50))
n

cache.gdsn(n)

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
