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
    \item{stream}{summary of byte streams, one row per stream: \code{id},
        \code{size}, \code{capacity}, \code{num_chunk} and \code{path}.
        The path names the node and what the stream holds for it:
        \code{$head$} for the node header, \code{$data$} for the data, and
        \code{$index$} for an offset index kept by the variable-length
        strings and integers and the sparse arrays. A stream that has no
        block in the file and exists in memory only is marked
        \code{$data<memory>$} or \code{$index<memory>$}; it may appear in
        a file written by an earlier version, and \code{\link{cleanup.gds}}
        with \code{sort=TRUE} gives it a block. The last row,
        \code{$unused$}, sums the free fragments}
    \item{log}{event log records}

    If \code{gds} is a \code{"gdsn.class"} object, the function returns a list
with components, like:
    \item{head}{total_size, chunk_offset, chunk_size}
    \item{data}{total_size, chunk_offset, chunk_size}
    \item{index}{total_size, chunk_offset, chunk_size; present only for a
        storage type that keeps an offset index -- the variable-length strings
        and integers and the sparse arrays -- naming the stream that indexes
        \code{data}. A node keeping more than one is reported as
        \code{index1}, \code{index2}, ...}
    \item{ra_stream}{raw, compression; present only when \code{data} is
        compressed with a random-access coder ("ZIP_RA", "LZMA_RA" or
        "LZ4_RA"). Two equal-length vectors giving the uncompressed and the
        compressed size of each independently compressed block. Note that the
        block size bounds the \emph{compressed} size, so the more compressible
        the data the fewer and the coarser the blocks}
    \item{...}{}
}

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
