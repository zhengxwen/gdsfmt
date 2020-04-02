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
    A \code{data.frame} with the columns "FileName", "ReadOnly" and "State",
or \code{NULL} if there is no opened gds file.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "int", val=1:10000)

showfile.gds()

showfile.gds(closeall=TRUE)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
