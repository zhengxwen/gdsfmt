\name{closefn.gds}
\alias{closefn.gds}
\title{Close a GDS file}
\description{
    Close a CoreArray Genomic Data Structure (GDS) file.
}

\usage{
closefn.gds(gdsfile)
}
\arguments{
    \item{gdsfile}{an object of class \code{\link{gds.class}}, a GDS file}
}
\details{
    For better performance, data in a GDS file are usually cached in memory.
Keep in mind that the new file may not actually be written to disk, until
\code{\link{closefn.gds}} or \code{\link{sync.gds}} is called. Anyway, when
R shuts down, all GDS files created or opened would be automatically closed.
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{createfn.gds}}, \code{\link{openfn.gds}},
    \code{\link{sync.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int.matrix", matrix(1:50*100, nrow=100, ncol=50))

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
