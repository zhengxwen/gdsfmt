\name{diagnosis.gds}
\alias{diagnosis.gds}
\title{Diagnose the GDS file}
\description{
    Diagnose the GDS file and data information.
}

\usage{
diagnosis.gds(gds, log.only=FALSE)
}
\arguments{
    \item{gds}{an object of class \code{\link{gdsn.class}} or
        \code{\link{gds.class}}}
    \item{log.only}{if \code{TRUE}, return a character vector of log only}
}
\value{
    A list with stream and chunk information.

    If \code{gds} is a \code{"gds.class"} object (i.e., a GDS file), the
function returns a list with components, like:
    \item{stream}{summary of byte stream}
    \item{log}{event log records}

    If \code{gds} is a \code{"gdsn.class"} object, the function returns a list
with components, like:
    \item{head}{total_size, chunk_offset, chunk_size}
    \item{data}{total_size, chunk_offset, chunk_size}
    \item{...}{}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

set.seed(1000)
rawval <- as.raw(rep(0:99, 50))

add.gdsn(f, "label", NULL)
add.gdsn(f, "raw", rawval)

closefn.gds(f)

##

f <- openfn.gds("test.gds")

diagnosis.gds(f)
diagnosis.gds(f$root)
diagnosis.gds(index.gdsn(f, "label"))
diagnosis.gds(index.gdsn(f, "raw"))

closefn.gds(f)

## remove fragments

cleanup.gds("test.gds")

##

f <- openfn.gds("test.gds")

diagnosis.gds(f$root)
diagnosis.gds(index.gdsn(f, "label"))
(adr <- diagnosis.gds(index.gdsn(f, "raw")))

closefn.gds(f)


## read binary data directly

f <- file("test.gds", "rb")

dat <- NULL
for (i in seq_len(length(adr$data$chunk_offset)))
{
    seek(f, adr$data$chunk_offset[i])
    dat <- c(dat, readBin(f, "raw", adr$data$chunk_size[i]))
}

identical(dat, rawval)  # should be TRUE

close(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
