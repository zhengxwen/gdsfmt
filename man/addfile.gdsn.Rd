\name{addfile.gdsn}
\alias{addfile.gdsn}
\title{Add a GDS node with a file}
\description{
	Add a file to a GDS file as a node.
}

\usage{
addfile.gdsn(node, name, filename,
	compress=c("ZIP", "ZIP.fast", "ZIP.default", "ZIP.max", ""))
}

\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}} or
		\code{\link{gds.class}}}
	\item{name}{the variable name of the added node}
	\item{filename}{the file name of input stream.}
	\item{compress}{the compression method can be "", "ZIP", "ZIP.fast",
		"ZIP.default", or "ZIP.max".}
}

\details{
	\code{name}  if missing, ``Item n'' is assigned to \code{name}, where
		\code{n} is the number of child nodes + 1.

	\code{compress}  Z compression algorithm can be used to deflate the data
		stored in the GDS file. "ZIP" option is equivalent to "ZIP.default".
		"ZIP.fast", "ZIP.default" and "ZIP.max" correspond to different
		compression levels. In the comprssion mode, you can not change the
		non-first dimension and read data field. Call
		\code{\link{readmode.gdsn}} to return read mode.
}

\value{
	An object of class \code{\link{gdsn.class}}.
}

\references{\url{http://sourceforge.net/projects/corearray/}}
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

f

getfile.gdsn(index.gdsn(f, "tmp.RData"), "tmp1.RData")
(obj <- get(load("tmp1.RData")))

# open the GDS file
closefn.gds(f)
}

\keyword{GDS}
\keyword{utilities}
