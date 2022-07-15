\name{delete.gdsn}
\alias{delete.gdsn}
\title{Delete a GDS node}
\description{
    Delete a specified GDS node.
}

\usage{
delete.gdsn(node, force=FALSE)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{force}{if \code{FALSE}, it is not allowed to delete a non-empty
        folder}
}
\value{
    None.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, name="list", val=list(x=c(1,2), y=c("T", "B", "C"), z=TRUE))
f

\dontrun{
# delete "node", but an error occurs
delete.gdsn(node)
}

# delete "node"
delete.gdsn(node, TRUE)

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
