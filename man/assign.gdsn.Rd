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
    \item{append}{if \code{TRUE}, append data; otherwise, replace the old one}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{write.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

n1 <- add.gdsn(f, "int", val=matrix(1:10000, nrow=100, ncol=100),
    compress="ZIP")

n2 <- add.gdsn(f, "int2", storage="int")

\dontrun{
assign.gdsn(n2, n1)
}

# close the gds file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
