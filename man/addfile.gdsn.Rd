\name{addfile.gdsn}
\alias{addfile.gdsn}
\title{Add a GDS node with a file}
\description{
	Add a file to a GDS file as a node.
}

\usage{
addfile.gdsn(node, name, filename,
	compress=c("ZIP", "ZIP.fast", "ZIP.default", "ZIP.max", ""),
	replace=FALSE)
}

\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}} or
		\code{\link{gds.class}}}
	\item{name}{the variable name of the added node; if it is not specified,
		``\code{Item} \code{N}'' is assigned to \code{name}, where \code{N}
		is the number of child nodes plus one}
	\item{filename}{the file name of input stream.}
	\item{compress}{the compression method can be "", "ZIP", "ZIP.fast",
		"ZIP.default", or "ZIP.max".}
	\item{replace}{if \code{TRUE}, replace the existing variable silently
		if possible}
}

\details{
	\code{compress}: Z compression algorithm can be used to deflate the data
		stored in the GDS file. "ZIP" option is equivalent to "ZIP.default".
		"ZIP.fast", "ZIP.default" and "ZIP.max" correspond to different
		compression levels. In the comprssion mode, you can not change the
		non-first dimension and read data field. Call
		\code{\link{readmode.gdsn}} to return read mode.
}

\value{
	An object of class \code{\link{gdsn.class}}.
}

\references{
	\url{http://sourceforge.net/projects/corearray/},
	\url{http://www.zlib.net/}
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
