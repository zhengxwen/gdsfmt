\name{showfile.gds}
\alias{showfile.gds}
\title{Enumerate opened GDS files}
\description{
	Enumerate all opened GDS files
}

\usage{
showfile.gds(verbose=TRUE)
}
\arguments{
	\item{verbose}{if \code{TRUE}, show information}
}
\value{
	A list of \code{\link{gds.class}} objects.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int", val=1:10000)

showfile.gds()

# close the GDS file
closefn.gds(f)


unlink("test.gds")
}

\keyword{GDS}
\keyword{utilities}
