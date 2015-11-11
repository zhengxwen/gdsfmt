\name{summarize.gdsn}
\alias{summarize.gdsn}
\title{GDS object Summaries}
\description{
    Get the summaries of a GDS node.
}

\usage{
summarize.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
    A list including
    \item{min}{the minimum value}
    \item{max}{the maximum value}
    \item{num_na}{the number of invalid numbers or NA}
    \item{decimal}{the count of each decimal (integer, 0.1, 0.01, ..., or
        other)}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}

\examples{
f <- createfn.gds("test.gds")

n1 <- add.gdsn(f, "x", seq(1, 10, 0.1), storage="float")
n2 <- add.gdsn(f, "y", seq(1, 10, 0.1), storage="double")
n3 <- add.gdsn(f, "int", c(1:100, NA, 112, NA), storage="int")
n4 <- add.gdsn(f, "int8", c(1:100, NA, 112, NA), storage="int8")

summarize.gdsn(n1)
summarize.gdsn(n2)
summarize.gdsn(n3)
summarize.gdsn(n4)

closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
