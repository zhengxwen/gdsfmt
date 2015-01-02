\name{sync.gds}
\alias{sync.gds}
\title{Synchronize a GDS file}
\description{
    Write the data cached in memory to disk.
}

\usage{
sync.gds(gdsfile)
}
\arguments{
    \item{gdsfile}{An object of class \code{\link{gds.class}}, a GDS file}
}
\details{
    For better performance, Data in a GDS file are usually cached in memory.
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
    \code{\link{createfn.gds}}, \code{\link{openfn.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

node <- add.gdsn(f, "int", val=1:10000)
put.attr.gdsn(node, "missing.value", 10000)

sync.gds(f)

f
get.attr.gdsn(node)

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
