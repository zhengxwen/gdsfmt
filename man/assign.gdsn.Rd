\name{assign.gdsn}
\alias{assign.gdsn}
\title{Assign/append data to a GDS node}
\description{
    Assign data to a GDS node, or append data to a GDS node
}

\usage{
assign.gdsn(dest.obj, src.obj, append=TRUE)
}
\arguments{
    \item{dest.obj}{an object of class \code{\link{gdsn.class}},
        a destination GDS node}
    \item{src.obj}{an object of class \code{\link{gdsn.class}},
        a source GDS node}
    \item{append}{if \code{TRUE}, append data by calling
        \code{\link{append.gdsn}}; otherwise, replace the old one}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{write.gdsn}},
    \code{\link{append.gdsn}}
}

\examples{
f <- createfn.gds("test.gds")

n1 <- add.gdsn(f, "n1", 1:100)
n2 <- add.gdsn(f, "n2", storage="int", valdim=c(20, 0))

assign.gdsn(n2, n1, append=TRUE)

read.gdsn(n1)
read.gdsn(n2)

assign.gdsn(n2, n1, append=TRUE)
append.gdsn(n2, n1)
read.gdsn(n2)

assign.gdsn(n2, n1, append=FALSE); read.gdsn(n2)

closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
