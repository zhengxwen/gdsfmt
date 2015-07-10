\name{assign.gdsn}
\alias{assign.gdsn}
\title{Assign/append data to a GDS node}
\description{
    Assign data to a GDS node, or append data to a GDS node
}

\usage{
assign.gdsn(dest.obj, src.obj, append=FALSE, seldim=NULL)
}
\arguments{
    \item{dest.obj}{an object of class \code{\link{gdsn.class}},
        a destination GDS node}
    \item{src.obj}{an object of class \code{\link{gdsn.class}},
        a source GDS node}
    \item{append}{if \code{TRUE}, append data by calling
        \code{\link{append.gdsn}}; otherwise, replace the old one}
    \item{seldim}{the selection of \code{src.obj}, or \code{NULL} for all
        data}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{readex.gdsn}},
    \code{\link{apply.gdsn}}, \code{\link{write.gdsn}},
    \code{\link{append.gdsn}}
}

\examples{
f <- createfn.gds("test.gds")

n1 <- add.gdsn(f, "n1", 1:100)
n2 <- add.gdsn(f, "n2", storage="int", valdim=c(20, 0))
n3 <- add.gdsn(f, "n3", storage="int", valdim=c(0))

assign.gdsn(n2, n1, append=TRUE)

read.gdsn(n1)
read.gdsn(n2)

assign.gdsn(n2, n1, append=TRUE)
append.gdsn(n2, n1)
read.gdsn(n2)

assign.gdsn(n3, n2, seldim=
    list(rep(c(TRUE, FALSE), 10), c(rep(c(TRUE, FALSE), 7), TRUE)))
read.gdsn(n3)

setdim.gdsn(n2, c(25,0))
assign.gdsn(n2, n1, append=TRUE, seldim=rep(c(TRUE, FALSE), 50))
read.gdsn(n2)

assign.gdsn(n2, n1); read.gdsn(n2)

closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
