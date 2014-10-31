\name{compression.gdsn}
\alias{compression.gdsn}
\title{Modify compression mode}
\description{
	Modifie the compression mode of data field in the GDS node.
}

\usage{
compression.gdsn(node, compress=c("", "ZIP", "ZIP_RA"))
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{compress}{the compression method can be "" (no compression),
		"ZIP", "ZIP.fast", "ZIP.default", "ZIP.max" or "ZIP.none" (original
		zlib); "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or
		"ZIP_RA.none" (zlib with efficient random access), see details}
}
\details{
	Z compression algorithm can be used to deflate the data stored in the
GDS file. "ZIP" option is equivalent to "ZIP.default". "ZIP.fast",
"ZIP.default" and "ZIP.max" correspond to different compression levels.

	To support efficient random access of Z stream, "ZIP_RA", "ZIP_RA.fast",
"ZIP_RA.default", "ZIP_RA.max" or "ZIP_RA.none" should be specified. "ZIP_RA"
option is equivalent to "ZIP_RA.default:256K". The block size can be specified
by following colon, and "16K", "32K", "64K", "128K", "256K", "512K" and "1M"
are allowed, such like "ZIP_RA:16K". The compression algorithm tries to keep
each independent compressed data block to be about of the specified block
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
