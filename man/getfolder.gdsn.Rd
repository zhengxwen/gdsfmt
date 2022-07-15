\name{getfolder.gdsn}
\alias{getfolder.gdsn}
\title{Get the folder}
\description{
    Get the folder which contains the specified GDS node.
}

\usage{
getfolder.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}}
}
\value{
    An object of class \code{\link{gdsn.class}}.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{index.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "label", NULL)
add.gdsn(f, "double", seq(1, 1000, 0.4))
add.gdsn(f, "list", list(X=1:10, Y=seq(1, 10, 0.25)))
add.gdsn(f, "data.frame", data.frame(X=1:19, Y=seq(1, 10, 0.5)))

f

getfolder.gdsn(index.gdsn(f, "label"))
getfolder.gdsn(index.gdsn(f, "double"))
getfolder.gdsn(index.gdsn(f, "list/X"))
getfolder.gdsn(index.gdsn(f, "data.frame/Y"))

getfolder.gdsn(f$root)


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
