\name{createfn.gds}
\alias{createfn.gds}
\title{Create a GDS file}
\description{
    Create a new CoreArray Genomic Data Structure (GDS) file.
}

\usage{
createfn.gds(filename, allow.duplicate=FALSE)
}
\arguments{
    \item{filename}{the file name of a new GDS file to be created}
    \item{allow.duplicate}{if \code{TRUE}, it is allowed to open a GDS file
        with read-only mode when it has been opened in the same R session}
}
\details{
    Keep in mind that the new file may not actually be written to disk until
\code{\link{closefn.gds}} or \code{\link{sync.gds}} is called.
}
\value{
    Return an object of class \code{\link{gds.class}}:
    \item{filename}{the file name to be created}
    \item{id}{internal file id}
    \item{root}{an object of class \code{\link{gdsn.class}},
        the root of hierachical structure}
    \item{readonly}{whether it is read-only or not}
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{openfn.gds}}, \code{\link{closefn.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, val=list(x=c(1,2), y=c("T", "B", "C"), z=TRUE))

f

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
