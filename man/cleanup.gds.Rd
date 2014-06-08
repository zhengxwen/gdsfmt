\name{cleanup.gds}
\alias{cleanup.gds}
\title{Clean up fragments}
\description{
	Clean up the fragments of a CoreArray Genomic Data Structure (GDS) file.
}

\usage{
cleanup.gds(filename, verbose=TRUE)
}
\arguments{
	\item{filename}{the file name of a GDS file to be opened}
	\item{verbose}{if \code{TRUE}, show information}
}
\value{
	None.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{openfn.gds}}, \code{\link{createfn.gds}},
	\code{\link{closefn.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# commom types
add.gdsn(f, "int", val=1:10000)
L <- -2500:2499
add.gdsn(f, "int.matrix", val=matrix(L, nrow=100, ncol=50))

# save a .RData object
obj <- list(X=1:10, Y=seq(1, 10, 0.1))
save(obj, file="tmp.RData")
addfile.gdsn(f, "tmp.RData", filename="tmp.RData")

f

closefn.gds(f)


# clean up fragments
cleanup.gds("test.gds")


# open ...
(f <- openfn.gds("test.gds"))
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
