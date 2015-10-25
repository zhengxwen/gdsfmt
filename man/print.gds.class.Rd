\name{print.gds.class}
\alias{print.gds.class}
\alias{print.gdsn.class}
\title{Show the information of class "gds.class" and "gdsn.class"}
\description{
    Displays the contents of "gds.class" (a GDS file) and "gdsn.class"
(a GDS node).
}
\usage{
\method{print}{gds.class}(x, ...)
\method{print}{gdsn.class}(x, expand=TRUE, all=FALSE, attribute=FALSE,
    attribute.trim=FALSE, ...)
}
\arguments{
    \item{x}{an object of class \code{\link{gds.class}}, a GDS file; or
        \code{\link{gdsn.class}}, a GDS node}
    \item{expand}{whether enumerate all of child nodes}
    \item{all}{if FALSE, hide GDS nodes with an attribute "R.invisible"}
    \item{attribute}{if TRUE, show the attribute(s)}
    \item{attribute.trim}{if TRUE, trim the attribute information if it is too
        long}
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
put.attr.gdsn(index.gdsn(f, "int.matrix"), "int", 1:10)

print(f, all=TRUE)
print(f, all=TRUE, attribute=TRUE)
print(f, all=TRUE, attribute=TRUE, attribute.trim=FALSE)

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
