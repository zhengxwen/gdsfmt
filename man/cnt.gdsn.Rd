\name{cnt.gdsn}
\alias{cnt.gdsn}
\title{Return the number of child nodes}
\description{
    Return the number of child nodes for a GDS node.
}

\usage{
cnt.gdsn(node, include.hidden=FALSE)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{include.hidden}{whether including hidden variables or folders}
}
\value{
    If \code{node} is a folder, return the numbers of variables in the folder
including child folders. Otherwise, return 0.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{objdesp.gdsn}}, \code{\link{ls.gdsn}},
    \code{\link{index.gdsn}}, \code{\link{delete.gdsn}},
    \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, name="list", val=list(x=c(1,2), y=c("T","B","C"), z=TRUE))
cnt.gdsn(node)
# 3

closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
