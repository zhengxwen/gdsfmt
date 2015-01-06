\name{showfile.gds}
\alias{showfile.gds}
\title{Enumerate opened GDS files}
\description{
    Enumerate all opened GDS files
}

\usage{
showfile.gds(closeall=FALSE, verbose=TRUE)
}
\arguments{
    \item{closeall}{if \code{TRUE}, close all GDS files}
    \item{verbose}{if \code{TRUE}, show information}
}
\value{
    A list of \code{\link{gds.class}} objects.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int", val=1:10000)

showfile.gds()

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
