\name{diagnosis.gds}
\alias{diagnosis.gds}
\title{Diagnose the GDS file}
\description{
	Diagnose the GDS file and block information
}

\usage{
diagnosis.gds(gdsfile)
}
\arguments{
	\item{gdsfile}{An object of class \code{\link{gds.class}}, a GDS file}
}
\value{
	A list with stream and chunk information.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{createfn.gds}}, \code{\link{openfn.gds}}
}
\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")
L <- -2500:2499

# commom types

add.gdsn(f, "label", NULL)
add.gdsn(f, "int", 1:10000, compress="ZIP", closezip=TRUE)
add.gdsn(f, "int.matrix", matrix(L, nrow=100, ncol=50))

closefn.gds(f)


###############################################################

f <- openfn.gds("test.gds", FALSE)

diagnosis.gds(f)

delete.gdsn(index.gdsn(f, "int"))

diagnosis.gds(f)

closefn.gds(f)


###############################################################

cleanup.gds("test.gds")

f <- openfn.gds("test.gds", FALSE)
diagnosis.gds(f)
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
