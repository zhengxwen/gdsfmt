\name{is.sparse.gdsn}
\alias{is.sparse.gdsn}
\title{whether a sparse array or not}
\description{
    Determine whether the node is a sparse array or not.
}

\usage{
is.sparse.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}} (a GDS node)}
}
\value{
    TRUE / FALSE.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

cnt <- matrix(0, nrow=4, ncol=8)
set.seed(100)
cnt[sample.int(length(cnt), 8)] <- rpois(8, 4)
cnt

add.gdsn(f, "mat", val=cnt)
add.gdsn(f, "sp.mat", val=cnt, storage="sp.real")
f

is.sparse.gdsn(index.gdsn(f, "mat"))
is.sparse.gdsn(index.gdsn(f, "sp.mat"))

read.gdsn(index.gdsn(f, "sp.mat"))

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
