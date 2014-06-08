\name{is.element.gdsn}
\alias{is.element.gdsn}
\title{whether the elements are in a set}
\description{
	Determine whether the elements are in a specified set.
}

\usage{
is.element.gdsn(node, set)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}} (a GDS node)}
	\item{set}{the specified set of elements}
}
\value{
	A logical vector or array.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{read.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int", val=1:100)
add.gdsn(f, "mat", val=matrix(1:12, nrow=4, ncol=3))
add.gdsn(f, "double", val=seq(1, 10, 0.1))
add.gdsn(f, "character", val=c("int", "double", "logical", "factor"))

is.element.gdsn(index.gdsn(f, "int"), c(1, 10, 20))
is.element.gdsn(index.gdsn(f, "mat"), c(2, 8, 12))
is.element.gdsn(index.gdsn(f, "double"), c(1.1, 1.3, 1.5))
is.element.gdsn(index.gdsn(f, "character"), c("int", "factor"))


closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
