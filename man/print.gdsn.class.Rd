\name{print.gdsn.class}
\alias{print.gdsn.class}
\title{Show the information of class "gdsn.class"}
\description{
    Display an object of class "gdsn.class", a GDS node.
}

\usage{
\method{print}{gdsn.class}(x, expand=TRUE, all=FALSE, ...)
}
\arguments{
    \item{x}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{expand}{whether enumerate all of child nodes}
    \item{all}{if \code{FALSE}, hide GDS nodes with an attribute
        "R.invisible"}
    \item{...}{the arguments passed to or from other methods}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{\code{\link{print.gds.class}}}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

n <- add.gdsn(f, "int.matrix", matrix(1:50*100, nrow=100, ncol=50))

print(n, all=TRUE)

compression.gdsn(n, "ZIP")

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
