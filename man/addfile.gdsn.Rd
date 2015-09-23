\name{addfile.gdsn}
\alias{addfile.gdsn}
\title{Add a GDS node with a file}
\description{
    Add a file to a GDS file as a node.
}

\usage{
addfile.gdsn(node, name, filename,
    compress=c("ZIP", "ZIP_RA", "LZ4", "LZ4_RA"), replace=FALSE, visible=TRUE)
}

\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}} or
        \code{\link{gds.class}}}
    \item{name}{the variable name; if it is not specified, a temporary name
        is assigned}
    \item{filename}{the file name of input stream.}
    \item{compress}{the compression method can be "" (no compression),
        "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max" or "ZIP.none" (original
        zlib); "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or
        "ZIP_RA.none" (zlib with efficient random access); "LZ4", "LZ4.none",
        "LZ4.fast", "LZ4.hc" or "LZ4.max"; "LZ4_RA", "LZ4_RA.none",
        "LZ4_RA.fast", "LZ4_RA.hc" or "LZ4_RA.max" (with efficient random
        access). See details}
    \item{replace}{if \code{TRUE}, replace the existing variable silently
        if possible}
    \item{visible}{\code{FALSE} -- invisible/hidden, except
        \code{print(, all=TRUE)}}
}

\details{
    \code{compress}:
        Z compression algorithm (\url{http://www.zlib.net/}) can be used to
        deflate the data stored in the GDS file. "ZIP" option is equivalent
        to "ZIP.default". "ZIP.fast", "ZIP.default" and "ZIP.max" correspond
        to different compression levels.

        To support efficient random access of Z stream, "ZIP_RA",
        "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or "ZIP_RA.none" should
        be specified. "ZIP_RA" option is equivalent to "ZIP_RA.default:256K".
        The block size can be specified by following colon, and "16K", "32K",
        "64K", "128K", "256K", "512K", "1M", "2M", "4M" and "8M" are allowed,
        like "ZIP_RA:64K". The compression algorithm tries to keep each
        independent compressed data block to be about of the specified block
        size, like 64K.

        LZ4 fast lossless compression algorithm is allowed when
        \code{compress="LZ4"} (\url{http://code.google.com/p/lz4/}). Three
        compression levels can be specified, "LZ4.fast" (LZ4 fast mode),
        "LZ4.hc" (LZ4 high compression mode), "LZ4.max" (maximize the
        compression ratio). The block size can be specified by following colon,
        and "64K", "256K", "1M" and "4M" are allowed according to LZ4 frame
        format. "LZ4" is equivalent to "LZ4.hc:256K".

        To support efficient random access of LZ4 stream, "LZ4_RA",
        "LZ4_RA.fast", "LZ4_RA.hc", "ZIP_RA.max" or "LZ4_RA.none" should
        be specified. "LZ4_RA" option is equivalent to "LZ4_RA.hc:256K".
        The block size can be specified by following colon, and "16K", "32K",
        "64K", "128K", "256K", "512K", "1M", "2M", "4M" and "8M" are allowed,
        like "LZ4_RA:64K". The compression algorithm tries to keep each
        independent compressed data block to be about of the specified block
        size, like 64K.
}

\value{
    An object of class \code{\link{gdsn.class}}.
}

\references{
    \url{http://github.com/zhengxwen/gdsfmt}
}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{getfile.gdsn}}, \code{\link{add.gdsn}}
}

\examples{
# save a .RData object
obj <- list(X=1:10, Y=seq(1, 10, 0.1))
save(obj, file="tmp.RData")

# cteate a GDS file
f <- createfn.gds("test.gds")

add.gdsn(f, "double", val=seq(1, 1000, 0.4))
addfile.gdsn(f, "tmp.RData", "tmp.RData")

# open the GDS file
closefn.gds(f)


# open the existing file
(f <- openfn.gds("test.gds"))

getfile.gdsn(index.gdsn(f, "tmp.RData"), "tmp1.RData")
(obj <- get(load("tmp1.RData")))

# open the GDS file
closefn.gds(f)


# delete the temporary files
unlink(c("test.gds", "tmp.RData", "tmp1.RData"), force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
