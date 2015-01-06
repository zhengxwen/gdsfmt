\name{get.attr.gdsn}
\alias{get.attr.gdsn}
\title{Get attributes}
\description{
    Get the attributes of a GDS node.
}

\usage{
get.attr.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
    A list of attributes.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{put.attr.gdsn}}, \code{\link{delete.attr.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

node <- add.gdsn(f, "int", val=1:10000)
put.attr.gdsn(node, "missing.value", 10000)

f
get.attr.gdsn(node)

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
