\name{readmode.gdsn}
\alias{readmode.gdsn}
\title{Switch to read mode in the compression settings}
\description{
	Switch to read mode for a GDS node with respect to its compression
settings.
}

\usage{
readmode.gdsn(node)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\details{
	After the compressed data field is created, it is in writing mode.
Users can add new data to the compressed data field, but can not read data
from the data field. Users have to call \code{readmode.gdsn} to finish
writing, before reading any data from the compressed data field.

	Once switch to the read mode, users can not add more data to the data
field. If users would like to append more data or modify the data field,
please call \code{compression.gdsn(node, compress="")} to decompress data
first.
}
\value{
	Return \code{node}.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{compression.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# commom types
n <- add.gdsn(f, "int", val=as.integer(1:100), compress="ZIP")

# you can not read the variable "int" because of writing mode
# read.gdsn(n)

readmode.gdsn(n)

# now you can read "int"
read.gdsn(n)

closefn.gds(f)


unlink("test.gds")
}

\keyword{GDS}
\keyword{utilities}
