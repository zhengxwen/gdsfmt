\name{name.gdsn}
\alias{name.gdsn}
\title{Return the variable name of a node}
\description{
    Get the variable name of a GDS node.
}

\usage{
name.gdsn(node, fullname=FALSE)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{fullname}{if \code{FALSE}, return the node name (by default);
        otherwise the name with a full path}
}
\value{
    Characters.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{cnt.gdsn}}, \code{\link{objdesp.gdsn}},
    \code{\link{ls.gdsn}}, \code{\link{rename.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
add.gdsn(f, name="list", val=list(x=c(1,2), y=c("T","B","C"), z=TRUE))
node <- index.gdsn(f, "list/x")

name.gdsn(node)
# "x"

name.gdsn(node, fullname=TRUE)
# "list/x"

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
