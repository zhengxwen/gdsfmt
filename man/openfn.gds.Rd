\name{openfn.gds}
\alias{openfn.gds}
\title{Open a GDS file}
\description{
	Open an existing file of CoreArray Genomic Data Structure (GDS) for
reading or writing.
}

\usage{
openfn.gds(filename, readonly = TRUE)
}
\arguments{
	\item{filename}{the file name of a GDS file to be opened}
	\item{readonly}{if \code{TRUE}, the file is opened read-only;
		otherwise, it is allowed to write data to the file}
}
\details{
	This function opens an existing GDS file for reading (or, if
\code{readonly = FALSE}, for writing). To create a new GDS file, use
\code{\link{createfn.gds}} instead.

	If the file is opened read-only, all data in the file are not allowed to
change, including hierachical structure, variable names, data fields, etc.
}
\value{
	Return an object of class \code{\link{gds.class}}.
	\item{filename}{the file name to be created}
	\item{id}{internal file id}
	\item{root}{an object of class \code{\link{gdsn.class}},
		the root of hierachical structure}
	\item{readonly}{whether it is read-only or not}
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{createfn.gds}}, \code{\link{closefn.gds}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a list to "test.gds"
node <- add.gdsn(f, name="list", val=list(x=c(1,2), y=c("T", "B", "C"), z=TRUE))
# close
closefn.gds(f)

# open the same file
f <- openfn.gds("test.gds")

# read
(node <- index.gdsn(f, "list"))
read.gdsn(node)

# close
closefn.gds(f)
}

\keyword{GDS}
\keyword{utilities}
