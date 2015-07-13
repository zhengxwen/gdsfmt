\name{write.gdsn}
\alias{write.gdsn}
\title{Write data to a GDS node}
\description{
    Write data to a GDS node.
}

\usage{
write.gdsn(node, val, start=NULL, count=NULL, check=TRUE)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{val}{the data to be written}
    \item{start}{a vector of integers, starting from 1 for each dimension}
    \item{count}{a vector of integers, the length of each dimnension}
    \item{check}{if \code{TRUE}, a warning will be given when \code{val} is
        character and there are missing values in \code{val}}
}
\details{
    \code{start}, \code{count}: The values in data are taken to be those in
the array with the leftmost subscript moving fastest.

    \code{start} and \code{count} should both exist or be missing.
If \code{start} and \code{count} are both missing, the dimensions and values
of \code{val} will be assigned to the data field.

    GDS format does not support missing characters \code{NA}, and any
\code{NA} will be converted to a blank string \code{""}.
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{append.gdsn}}, \code{\link{read.gdsn}},
    \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

###################################################

n <- add.gdsn(f, "matrix", matrix(1:20, ncol=5))
read.gdsn(n)

write.gdsn(n, val=c(NA, NA), start=c(2, 2), count=c(2, 1))
read.gdsn(n)


###################################################

n <- add.gdsn(f, "n", val=1:12)
read.gdsn(n)

write.gdsn(n, matrix(1:24, ncol=6))
read.gdsn(n)

write.gdsn(n, array(1:24, c(4,3,2)))
read.gdsn(n)


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
