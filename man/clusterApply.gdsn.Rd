\name{clusterApply.gdsn}
\alias{clusterApply.gdsn}
\title{Apply functions over matrix margins in parallel}
\description{
	Return a vector or list of values obtained by applying a function to
margins of a GDS matrix in parallel.
}

\usage{
clusterApply.gdsn(cl, gds.fn, node.name, margin, FUN, selection=NULL,
	as.is = c("list", "integer", "double", "character", "none"),
	var.index = c("none", "relative", "absolute"), ...)
}
\arguments{
	\item{cl}{a cluster object, created by this package or by the package
		\link[parallel]{parallel}}
	\item{gds.fn}{the file name of a GDS file}
	\item{node.name}{a character vector indicating GDS node path}
	\item{margin}{an integer giving the subscripts which the function will be
		applied over. E.g., for a matrix 1 indicates rows, 2 indicates columns}
	\item{FUN}{the function to be applied}
	\item{selection}{a list or NULL; if a list, it is a list of logical vectors
		according to dimensions indicating selection; if NULL, uses all data}
	\item{as.is}{returned value: a list, an integer vector, etc}
	\item{var.index}{if \code{"none"}, call \code{FUN(x, ...)} without an
		index; if \code{"relative"} or \code{"absolute"}, add an argument to
		the user-defined function \code{FUN} like \code{FUN(index, x, ...)}
		where \code{index} in the function is an index starting from 1:
		\code{"relative"} for indexing in the selection defined by
		\code{selection}, \code{"absolute"} for indexing with respect to
		all data}
	\item{...}{optional arguments to \code{FUN}}
}
\details{
	The algorithm of applying is optimized by blocking the computations to
exploit the high-speed memory instead of disk.
}
\value{
	A vector or list of values.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{apply.gdsn}}
}

\examples{
###########################################################
# prepare a GDS file

# cteate a GDS file
f <- createfn.gds("test1.gds")

(n <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))
read.gdsn(index.gdsn(f, "matrix"))

closefn.gds(f)


# cteate the GDS file "test2.gds"
(f <- createfn.gds("test2.gds"))

X <- matrix(1:50, nrow=10)
Y <- matrix((1:50)/100, nrow=10)
Z1 <- factor(c(rep(c("ABC", "DEF", "ETD"), 3), "TTT"))
Z2 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)

node.X <- add.gdsn(f, "X", X)
node.Y <- add.gdsn(f, "Y", Y)
node.Z1 <- add.gdsn(f, "Z1", Z1)
node.Z2 <- add.gdsn(f, "Z2", Z2)
f

closefn.gds(f)



###########################################################
# apply in parallel

library(parallel)

# Use option cl.core to choose an appropriate cluster size.
cl <- makeCluster(getOption("cl.cores", 2))


# Apply functions over rows or columns of matrix

clusterApply.gdsn(cl, "test1.gds", "matrix", margin=1, FUN=function(x) x)

clusterApply.gdsn(cl, "test1.gds", "matrix", margin=2, FUN=function(x) x)

clusterApply.gdsn(cl, "test1.gds", "matrix", margin=1,
	selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
	FUN=function(x) x)

clusterApply.gdsn(cl, "test1.gds", "matrix", margin=2,
	selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
	FUN=function(x) x)



# Apply functions over rows or columns of multiple data sets

clusterApply.gdsn(cl, "test2.gds", c("X", "Y", "Z1"), margin=c(1, 1, 1),
	FUN=function(x) x)

# with variable names
clusterApply.gdsn(cl, "test2.gds", c(X="X", Y="Y", Z="Z2"), margin=c(2, 2, 1),
	FUN=function(x) x)


# stop clusters
stopCluster(cl)


# delete the temporary file
unlink(c("test1.gds", "test2.gds"), force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
