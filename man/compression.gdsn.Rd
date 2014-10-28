\name{compression.gdsn}
\alias{compression.gdsn}
\title{Modify compression mode}
\description{
	Modifie the compression mode of data field in the GDS node.
}

\usage{
compression.gdsn(node, compress =
	c("", "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max"))
}
\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{compress}{specify the compression method}
}
\details{
	Z compression algorithm can be used to deflate the data stored in the
GDS file. "ZIP" option is equivalent to "ZIP.default". "ZIP.fast",
"ZIP.default" and "ZIP.max" correspond to different compression levels.
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
