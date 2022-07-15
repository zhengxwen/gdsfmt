\name{copyto.gdsn}
\alias{copyto.gdsn}
\title{Copy GDS nodes}
\description{
    Copy GDS node(s) to a folder with a new name
}

\usage{
copyto.gdsn(node, source, name=NULL)
}
\arguments{
    \item{node}{a folder of class \code{\link{gdsn.class}} or
        \code{\link{gds.class}}}
    \item{source}{an object of class \code{\link{gdsn.class}} or
        \code{\link{gds.class}}}
    \item{name}{a specified name; if \code{NULL}, it is determined by
        \code{source}}
}
\value{
    None.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{moveto.gdsn}}
}
\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "label", NULL)
add.gdsn(f, "int", 1:100, compress="ZIP", closezip=TRUE)
add.gdsn(f, "int.matrix", matrix(1:100, nrow=20))
addfolder.gdsn(f, "folder1")
addfolder.gdsn(f, "folder2")


for (nm in c("label", "int", "int.matrix"))
    copyto.gdsn(index.gdsn(f, "folder1"), index.gdsn(f, nm))
f

copyto.gdsn(index.gdsn(f, "folder2"), index.gdsn(f, "folder1"))
f

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
