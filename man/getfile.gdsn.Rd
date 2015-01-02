\name{getfile.gdsn}
\alias{getfile.gdsn}
\title{Output a file from a stream container}
\description{
    Get a file from a GDS node of stream container.
}

\usage{
getfile.gdsn(node, out.filename)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{out.filename}{the file name of output stream}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{addfile.gdsn}}
}

\examples{
# save a .RData object
obj <- list(X=1:10, Y=seq(1, 10, 0.1))
save(obj, file="tmp.RData")

# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "double", val=seq(1, 1000, 0.4))
addfile.gdsn(f, "tmp.RData", "tmp.RData")

# open the GDS file
closefn.gds(f)


# open the existing file
(f <- openfn.gds("test.gds"))

getfile.gdsn(index.gdsn(f, "tmp.RData"), "tmp1.RData")
(obj <- get(load("tmp1.RData")))

# open the GDS file
closefn.gds(f)


# delete the temporary files
unlink(c("test.gds", "tmp.RData", "tmp1.RData"), force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
