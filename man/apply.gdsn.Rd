\name{apply.gdsn}
\alias{apply.gdsn}
\title{Apply functions over margins}
\description{
	Return a vector or list of values obtained by applying a function to
margins of a GDS matrix or array.
}

\usage{
apply.gdsn(node, margin, FUN, selection=NULL,
	as.is = c("list", "integer", "double", "character", "none"), ...)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, or a
		list of objects of class \code{\link{gdsn.class}}}
	\item{margin}{an integer giving the subscripts which the function will
		be applied over. E.g., for a matrix 1 indicates rows, 2 indicates
		columns}
	\item{FUN}{the function to be applied}
	\item{selection}{a list or NULL; if a list, it is a list of logical
		vectors according to dimensions indicating selection; if NULL, uses
		all data}
	\item{as.is}{returned value: a list, an integer vector, etc}
	\item{...}{optional arguments to \code{FUN}}
}
\details{
	The algorithm is optimized by blocking the computations to exploit the
high-speed memory instead of disk.
}
\value{
	A vector or list of values.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{read.gdsn}}, \code{\link{readex.gdsn}},
	\code{\link{clusterApply.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

(n <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))
read.gdsn(index.gdsn(f, "matrix"))

# Apply functions over rows of matrix
tmp <- apply.gdsn(n, margin=1, FUN=function(x) print(x))
tmp <- apply.gdsn(n, margin=1,
	selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
	FUN=function(x) print(x))

# Apply functions over columns of matrix
tmp <- apply.gdsn(n, margin=2, FUN=function(x) print(x))
tmp <- apply.gdsn(n, margin=2,
	selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
	FUN=function(x) print(x))

# close
closefn.gds(f)



########################################################
#
# Multiple variables
#

# cteate a GDS file
f <- createfn.gds("test.gds")

X <- matrix(1:50, nrow=10)
Y <- matrix((1:50)/100, nrow=10)
Z1 <- factor(c(rep(c("ABC", "DEF", "ETD"), 3), "TTT"))
Z2 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)

node.X <- add.gdsn(f, "X", X)
node.Y <- add.gdsn(f, "Y", Y)
node.Z1 <- add.gdsn(f, "Z1", Z1)
node.Z2 <- add.gdsn(f, "Z2", Z2)

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z1), margin=c(1, 1, 1),
	FUN=function(x) { print(x) }, as.is="none")

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
	FUN=function(x) print(x))


# with selection

s1 <- rep(c(FALSE, TRUE), 5)
s2 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z1), margin=c(1, 1, 1),
	selection = list(list(s1, s2), list(s1, s2), list(s1)),
	FUN=function(x) print(x))

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
	selection = list(list(s1, s2), list(s1, s2), list(s2)),
	FUN=function(x) print(x))


# close
closefn.gds(f)


unlink("test.gds")
}

\keyword{GDS}
\keyword{utilities}
