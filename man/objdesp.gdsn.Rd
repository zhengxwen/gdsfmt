\name{objdesp.gdsn}
\alias{objdesp.gdsn}
\title{Variable description}
\description{
	Get the descritpion of a GDS node.
}

\usage{
objdesp.gdsn(node)
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
	Returns a list:
	\item{name}{the variable name of a specified node}
	\item{fullname}{the full name of a specified node}
	\item{storage}{the storage mode in the GDS file}
	\item{type}{a factor indicating the storage mode in R}
	\item{is.array}{indicates whether it is array-type}
	\item{dim}{the dimension of data field}
	\item{compress}{the compression method: "", "ZIP", etc}
	\item{cpratio}{data compression ratio, \code{NaN} indicates no compression}
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{cnt.gdsn}}, \code{\link{name.gdsn}},
	\code{\link{ls.gdsn}}, \code{\link{index.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a vector to "test.gds"
node1 <- add.gdsn(f, name="vector1", val=1:10000, compress="ZIP",
	closezip=TRUE)
objdesp.gdsn(node1)

# add a character to "test.gds"
node2 <- add.gdsn(f, name="vector2", val=c("A", "BC", "DEF"),
	compress="ZIP", closezip=TRUE)
objdesp.gdsn(node2)

closefn.gds(f)
}

\keyword{GDS}
\keyword{utilities}
