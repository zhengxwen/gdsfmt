\name{assign.gdsn}
\alias{assign.gdsn}
\title{Assign/append data to a GDS node}
\description{
    Assign data to a GDS node, or append data to a GDS node
}

\usage{
assign.gdsn(node, src.node=NULL, resize=TRUE, seldim=NULL, append=FALSE,
    .value=NULL, .substitute=NULL)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}},
        a target GDS node}
    \item{src.node}{an object of class \code{\link{gdsn.class}},
        a source GDS node}
    \item{resize}{whether call \code{\link{setdim.gdsn}} to reset the
        dimension(s)}
    \item{seldim}{the selection of \code{src.obj} with numeric or logical
        indicators, or \code{NULL} for all data}
    \item{append}{if \code{TRUE}, append data by calling
        \code{\link{append.gdsn}}; otherwise, replace the old one}
    \item{.value}{a vector of values to be replaced in the original data array,
        or \code{NULL} for nothing}
    \item{.substitute}{a vector of values after replacing, or NULL for
        nothing; \code{length(.substitute)} should be one or
        \code{length(.value)}; if \code{length(.substitute)} =
        \code{length(.value)}, it is a mapping from \code{.value} to
        \code{.substitute}}
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
n4 <- add.gdsn(f, "n4", matrix(1:48, 6))
f

assign.gdsn(n2, n1, resize=FALSE, append=TRUE)

read.gdsn(n1)
read.gdsn(n2)

assign.gdsn(n2, n1, resize=FALSE, append=TRUE)
append.gdsn(n2, n1)
read.gdsn(n2)

assign.gdsn(n3, n2, seldim=
    list(rep(c(TRUE, FALSE), 10), c(rep(c(TRUE, FALSE), 7), TRUE)))
read.gdsn(n3)

setdim.gdsn(n2, c(25,0))
assign.gdsn(n2, n1, append=TRUE, seldim=rep(c(TRUE, FALSE), 50))
read.gdsn(n2)

assign.gdsn(n2, n1); read.gdsn(n2)
f

##

read.gdsn(n4)

# substitute
assign.gdsn(n4, .value=c(3:8,35:40), .substitute=NA); read.gdsn(n4)

# subset
assign.gdsn(n4, seldim=list(c(4,2,6,NA), c(5,6,NA,2,8,NA,4))); read.gdsn(n4)


n4 <- add.gdsn(f, "n4", matrix(1:48, 6), replace=TRUE)
read.gdsn(n4)
# sort into descending order
assign.gdsn(n4, seldim=list(6:1, 8:1)); read.gdsn(n4)


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
