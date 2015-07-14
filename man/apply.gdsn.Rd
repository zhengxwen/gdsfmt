\name{apply.gdsn}
\alias{apply.gdsn}
\title{Apply functions over margins}
\description{
    Return a vector or list of values obtained by applying a function to
margins of a GDS matrix or array.
}

\usage{
apply.gdsn(node, margin, FUN, selection=NULL,
    as.is=c("list", "none", "integer", "double", "character", "logical",
    "raw", "gdsnode"), var.index=c("none", "relative", "absolute"),
    target.node=NULL, .useraw=FALSE, .value=NULL, .val.replaced=NULL, ...)
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
    \item{as.is}{returned value: a list, an integer vector, etc;
        \code{"gdsnode"} -- the returned value from the user-defined function
        will be appended to \code{target.node}.}
    \item{var.index}{if \code{"none"}, call \code{FUN(x, ...)} without an
        index; if \code{"relative"} or \code{"absolute"}, add an argument to
        the user-defined function \code{FUN} like \code{FUN(index, x, ...)}
        where \code{index} in the function is an index starting from 1:
        \code{"relative"} for indexing in the selection defined by
        \code{selection}, \code{"absolute"} for indexing with respect to
        all data}
    \item{target.node}{NULL, an object of class \code{\link{gdsn.class}}
        or a list of \code{\link{gdsn.class}}: output to the target GDS node(s)
        when \code{as.is="gdsnode"}. See details}
    \item{.useraw}{use R RAW storage mode if integers can be stored in a byte,
        to reduce memory usage}
    \item{.value}{a vector of values to be replaced in the original data array,
        or NULL for nothing}
    \item{.val.replaced}{a vector of values after replacing, or NULL for
        nothing; \code{length(.val.replaced)} should be one or
        \code{length(.value)}; if \code{length(.val.replaced)} =
        \code{length(.value)}, it is a mapping from \code{.value} to
        \code{.val.replaced}}
    \item{...}{optional arguments to \code{FUN}}
}
\details{
    The algorithm is optimized by blocking the computations to exploit the
high-speed memory instead of disk.

    When \code{as.is="gdsnode"} and there are more than one
\code{\link{gdsn.class}} object in \code{target.node}, the user-defined
function should return a list with elements corresponding to
\code{target.node}, or \code{NULL} indicating no appending.
}
\value{
    A vector or list of values.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{readex.gdsn}},
    \code{\link{clusterApply.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

(n1 <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))
read.gdsn(index.gdsn(f, "matrix"))

(n2 <- add.gdsn(f, "string",
    val=matrix(paste("L", 1:(10*6), sep=","), nrow=10)))
read.gdsn(index.gdsn(f, "string"))

# Apply functions over rows of matrix
apply.gdsn(n1, margin=1, FUN=function(x) print(x), as.is="none")
apply.gdsn(n1, margin=1,
    selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
    FUN=function(x) print(x), as.is="none")
apply.gdsn(n1, margin=1, var.index="relative",
    selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
    FUN=function(i, x) { cat("index: ", i, ", ", sep=""); print(x) },
    as.is="none")
apply.gdsn(n1, margin=1, var.index="absolute",
    selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
    FUN=function(i, x) { cat("index: ", i, ", ", sep=""); print(x) },
    as.is="none")
apply.gdsn(n2, margin=1, FUN=function(x) print(x), as.is="none")


# Apply functions over columns of matrix
apply.gdsn(n1, margin=2, FUN=function(x) print(x), as.is="none")
apply.gdsn(n1, margin=2,
    selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
    FUN=function(x) print(x), as.is="none")
apply.gdsn(n2, margin=2,
    selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
    FUN=function(x) print(x), as.is="none")


apply.gdsn(n1, margin=1, FUN=function(x) print(x), as.is="none",
    .value=16:40, .val.replaced=NA)
apply.gdsn(n1, margin=2, FUN=function(x) print(x), as.is="none",
    .value=16:40, .val.replaced=NA)


# close
closefn.gds(f)



########################################################
#
# Append to a target GDS node
#

# cteate a GDS file
f <- createfn.gds("test.gds")

(n2 <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))

(n2 <- add.gdsn(f, "string",
    val=matrix(paste("L", 1:(10*6), sep=","), nrow=10)))
read.gdsn(index.gdsn(f, "string"))

n2.1 <- add.gdsn(f, "transpose.matrix", storage="int", valdim=c(6,0))
n2.1 <- add.gdsn(f, "transpose.string", storage="string", valdim=c(6,0))

# Apply functions over rows of matrix
apply.gdsn(n2, margin=1, FUN=`c`, as.is="gdsnode", target.node=n2.1)

# matrix transpose
read.gdsn(n2)
read.gdsn(n2.1)


# Apply functions over rows of matrix
apply.gdsn(n2, margin=1, FUN=`c`, as.is="gdsnode", target.node=n2.1)

# matrix transpose
read.gdsn(n2)
read.gdsn(n2.1)

# close
closefn.gds(f)



########################################################
#
# Append to multiple target GDS node
#

# cteate a GDS file
f <- createfn.gds("test.gds")

(n2 <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))

n2.1 <- add.gdsn(f, "transpose.matrix", storage="int", valdim=c(6,0))
n2.2 <- add.gdsn(f, "n.matrix", storage="int", valdim=c(0))

# Apply functions over rows of matrix
apply.gdsn(n2, margin=1, FUN=function(x) list(x, x[1]),
    as.is="gdsnode", target.node=list(n2.1, n2.2))

# matrix transpose
read.gdsn(n2)
read.gdsn(n2.1)
read.gdsn(n2.2)

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
    FUN=print, as.is="none")

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
    FUN=print)

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
    FUN=print, .value=35:45, .val.replaced=NA)

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
    FUN=print, .value=35:45, .val.replaced=NA)



# with selection

s1 <- rep(c(FALSE, TRUE), 5)
s2 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z1), margin=c(1, 1, 1),
    selection = list(list(s1, s2), list(s1, s2), list(s1)),
    FUN=function(x) print(x))

v <- apply.gdsn(list(X=node.X, Y=node.Y, Z=node.Z2), margin=c(2, 2, 1),
    selection = list(list(s1, s2), list(s1, s2), list(s2)),
    FUN=function(x) print(x))


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
