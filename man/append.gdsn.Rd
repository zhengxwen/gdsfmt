\name{append.gdsn}
\alias{append.gdsn}
\title{Append data to a specified variable}
\description{
    Append new data to the data field of a GDS node.
}

\usage{
append.gdsn(node, val, check=TRUE, allow.block=TRUE)
}
\arguments{
    \item{node}{an object of class \code{\link[gdsfmt]{gdsn.class}}}
    \item{val}{R primitive data, like integer; an object of class \code{rle}
        returned by \code{\link[base]{rle}}; or an object of class
        \code{\link[gdsfmt]{gdsn.class}}}
    \item{check}{whether a warning is given, when appended data can not
        match the capability of data field; if \code{val} is character-type,
        a warning will be shown if there is any \code{NA} in \code{val}}
    \item{allow.block}{only applicable when \code{val} is a GDS node: if
        \code{TRUE}, the encoded data of \code{val} can be copied as a block;
        if \code{FALSE}, every element of \code{val} is decoded and encoded
        again; see details}
}
\details{
    \code{storage.mode(val)} should be "integer", "double", "character"
or "logical". GDS format does not support missing characters \code{NA},
and any \code{NA} will be converted to a blank string \code{""}.

    If \code{val} is an object of class \code{rle}, the elements that the runs
stand for are appended, like \code{inverse.rle(val)}, to a node of any storage
type. They are written through a buffer of a fixed size, so the memory needed
does not depend on the number of elements. \code{val$lengths} should consist
of non-negative whole numbers, and factors are not allowed in
\code{val$values}.

    If \code{val} is a GDS node of the same storage type as \code{node}, its
data can be appended without being decoded: the encoded bytes are copied as a
block. When both nodes use the same random-access compression method and
block size (e.g., "ZIP_RA:16K"), whole compressed blocks may be copied as they
are, and they keep the compression level of \code{val} even if \code{node}
is created with another level. \code{allow.block=FALSE} disables the block
copy, so that the appended data are encoded and compressed according to
\code{node}.
}
\value{
    None.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{read.gdsn}}, \code{\link{write.gdsn}},
    \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# commom types
n <- add.gdsn(f, "int", val=matrix(1:10000, nrow=100, ncol=100),
    compress="ZIP")

# no warning, and add a new column
append.gdsn(n, -1:-100)
f

# a warning
append.gdsn(n, -1:-50)
f

# no warning here, and add a new column
append.gdsn(n, -51:-100)
f

# you should call "readmode.gdsn" before reading, since compress="ZIP"
readmode.gdsn(n)

# check the last column
read.gdsn(n, start=c(1, 102), count=c(-1, 1))


# characters
n <- add.gdsn(f, "string", val=as.character(1:100))
append.gdsn(n, as.character(rep(NA, 25)))

read.gdsn(n)


# an object of class "rle"
n <- add.gdsn(f, "runs", rle(rep(c(1L, 5L), c(1000, 2000))))
append.gdsn(n, rle(rep(c(5L, 7L), c(500, 500))))

table(read.gdsn(n))


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
