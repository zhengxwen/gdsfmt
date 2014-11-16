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
		"LZ4.fast", "LZ4.hc" or "LZ4.max"; "LZ4_RA", "LZ4_RA.none",
		"LZ4_RA.fast", "LZ4_RA.hc" or "LZ4_RA.max" (with efficient random
		access). See details}
}
\details{
	Z compression algorithm (\url{http://www.zlib.net/}) can be used to
deflate the data stored in the GDS file. "ZIP" option is equivalent
to "ZIP.default". "ZIP.fast", "ZIP.default" and "ZIP.max" correspond
to different compression levels.

	To support efficient random access of Z stream, "ZIP_RA",
"ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or "ZIP_RA.none" should
be specified. "ZIP_RA" option is equivalent to "ZIP_RA.default:256K".
The block size can be specified by following colon, and "16K", "32K",
"64K", "128K", "256K", "512K" and "1M" are allowed, such like
"ZIP_RA:16K". The compression algorithm tries to keep each
independent compressed data block to be about of the specified block
size, such like 64K.

	LZ4 fast lossless compression algorithm is allowed with
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
"64K", "128K", "256K", "512K" and "1M" are allowed, such like
"ZIP_RA:16K". The compression algorithm tries to keep each
independent compressed data block to be about of the specified block
size, such like 64K.
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

\keyword{GDS}
\keyword{utilities}
