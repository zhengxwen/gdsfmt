\name{rename.gdsn}
\alias{rename.gdsn}
\title{Rename a GDS node}
\description{
	Rename a GDS node.
}

\usage{
rename.gdsn(node, newname)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{newname}{the new name of a specified node}
}
\details{
	CoreArray hierarchical structure does not allow duplicate names in the
same folder.
}
\value{
	None.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{name.gdsn}}, \code{\link{ls.gdsn}},
	\code{\link[gdsfmt]{index.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")
n <- add.gdsn(f, "old.name", val=1:10)
f

rename.gdsn(n, "new.name")
f

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
