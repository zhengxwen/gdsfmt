\name{compression.gdsn}
\alias{compression.gdsn}
\title{Modify compression mode}
\description{
    Modifie the compression mode of data field in the GDS node.
}

\usage{
compression.gdsn(node, compress=c("", "ZIP", "ZIP_RA", "LZ4", "LZ4_RA"))
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{compress}{the compression method can be "" (no compression),
        "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max" or "ZIP.none" (original
        zlib); "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or
        "ZIP_RA.none" (zlib with efficient random access); "LZ4", "LZ4.none",
        "LZ4.fast", "LZ4.hc" or "LZ4.max" (LZ4 compression/decompression
        library); "LZ4_RA", "LZ4_RA.none", "LZ4_RA.fast", "LZ4_RA.hc" or
        "LZ4_RA.max" (with efficient random access). See details}
}
\details{
    Z compression algorithm (\url{http://www.zlib.net/}) can be used to
deflate the data stored in the GDS file. "ZIP" option is equivalent
to "ZIP.def". "ZIP.fast", "ZIP.def" and "ZIP.max" correspond to
different compression levels.

    To support efficient random access of Z stream, "ZIP_RA", "ZIP_RA.fast",
"ZIP_RA.def" or "ZIP_RA.max" should be specified. "ZIP_RA" option is
equivalent to "ZIP_RA.def:256K". The block size can be specified by
following colon, and "16K", "32K", "64K", "128K", "256K", "512K", "1M",
"2M", "4M" and "8M" are allowed, like "ZIP_RA:64K". The compression
algorithm tries to keep each independent compressed data block to be
about of the specified block size, like 64K.

    LZ4 fast lossless compression algorithm is allowed when
\code{compress="LZ4"} (\url{http://code.google.com/p/lz4/}). Three
compression levels can be specified, "LZ4.fast" (LZ4 fast mode),
"LZ4.hc" (LZ4 high compression mode), "LZ4.max" (maximize the
compression ratio). The block size can be specified by following colon,
and "64K", "256K", "1M" and "4M" are allowed according to LZ4 frame
format. "LZ4" is equivalent to "LZ4.hc:256K".

    To support efficient random access of LZ4 stream, "LZ4_RA",
"LZ4_RA.fast", "LZ4_RA.hc" or "ZIP_RA.max" should be specified.
"LZ4_RA" option is equivalent to "LZ4_RA.hc:256K". The block size can
be specified by following colon, and "16K", "32K", "64K", "128K",
"256K", "512K", "1M", "2M", "4M" and "8M" are allowed, like
"LZ4_RA:64K". The compression algorithm tries to keep each independent
compressed data block to be about of the specified block size, like 64K.

    LZMA compression algorithm (\url{http://tukaani.org/xz/}) is available
since gdsfmt_v1.7.18, which has a higher compression ratio than ZIP
algorithm. "LZMA", "LZMA.fast", "LZMA.def" and "LZMA.max" available.
To support efficient random access of LZMA stream, "LZMA_RA",
"LZMA_RA.fast", "LZMA_RA.def" and "LZMA_RA.max" can be used. The block
size can be specified by following colon. "LZMA_RA" is equivalent to
"LZMA_RA.def:256K".
}
\value{
    Return \code{node}.
}

\references{
    \url{http://github.com/zhengxwen/gdsfmt},
    \url{http://zlib.net/}
}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{readmode.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

n <- add.gdsn(f, "int.matrix", matrix(1:50*100, nrow=100, ncol=50))
n

compression.gdsn(n, "ZIP")

# close the GDS file
closefn.gds(f)

# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
