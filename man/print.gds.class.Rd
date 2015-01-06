\name{print.gds.class}
\alias{print.gds.class}
\title{Show the information of class "gds.class"}
\description{
    Displays an object of class "gds.class", a GDS file.
}
\usage{
\method{print}{gds.class}(x, all=FALSE, ...)
}
\arguments{
    \item{x}{an object of class \code{\link{gds.class}}, a GDS file}
    \item{all}{if FALSE, hide GDS nodes with an attribute "R.invisible"}
    \item{...}{the arguments passed to or from other methods}
}
\value{
    None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{\code{\link{print.gdsn.class}}}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int.matrix", matrix(1:50*100, nrow=100, ncol=50))

print(f, all=TRUE)

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
