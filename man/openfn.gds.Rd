\name{openfn.gds}
\alias{openfn.gds}
\title{Open a GDS file}
\description{
    Open an existing file of CoreArray Genomic Data Structure (GDS) for
reading or writing.
}

\usage{
openfn.gds(filename, readonly=TRUE, allow.duplicate=FALSE, allow.fork=FALSE,
    allow.error=FALSE)
}
\arguments{
    \item{filename}{the file name of a GDS file to be opened}
    \item{readonly}{if \code{TRUE}, the file is opened read-only;
        otherwise, it is allowed to write data to the file}
    \item{allow.duplicate}{if \code{TRUE}, it is allowed to open a GDS file
        with read-only mode when it has been opened in the same R session}
    \item{allow.fork}{\code{TRUE} for parallel environment using forking,
        see details}
    \item{allow.error}{\code{TRUE} for data recovery from a crashed GDS file}
}
\details{
    This function opens an existing GDS file for reading (or, if
\code{readonly=FALSE}, for writing). To create a new GDS file, use
\code{\link{createfn.gds}} instead.

    If the file is opened read-only, all data in the file are not allowed to
be changed, including hierachical structure, variable names, data fields, etc.

    \code{\link{mclapply}} and \code{\link{mcmapply}} in
the R package \code{parallel} rely on unix forking. However, the forked child
process inherits copies of the parent's set of open file descriptors. Each
file descriptor in the child refers to the same open file description as the
corresponding file descriptor in the parent. This means that the two
descriptors share open file status flags, current file offset, and
signal-driven I/O attributes. The sharing of file description can cause a
serious problem (wrong reading, even program crashes), when child processes
read or write the same GDS file simultaneously.
    \code{allow.fork=TRUE} adds additional file operations to avoid any
conflict using forking. The current implementation does not support writing
in forked processes.
}
\value{
    Return an object of class \code{\link{gds.class}}.
    \item{filename}{the file name to be created}
    \item{id}{internal file id, an integer}
    \item{root}{an object of class \code{\link{gdsn.class}},
        the root of hierachical structure}
    \item{readonly}{whether it is read-only or not}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{createfn.gds}}, \code{\link{closefn.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, name="list", val=list(x=c(1,2), y=c("T","B","C"), z=TRUE))
# close
closefn.gds(f)

# open the same file
f <- openfn.gds("test.gds")

# read
(node <- index.gdsn(f, "list"))
read.gdsn(node)

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
