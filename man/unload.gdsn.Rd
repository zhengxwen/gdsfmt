\name{unload.gdsn}
\alias{unload.gdsn}
\title{Unload a GDS node}
\description{
    Unload a specified GDS node.
}

\usage{
unload.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{delete.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, "val", 1:1000)
node

\dontrun{
unload.gdsn(node)
# Error in print.gdsn.class(x):
#   Invalid GDS node object (it was unloaded or deleted).
node
}

index.gdsn(f, "val")

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
