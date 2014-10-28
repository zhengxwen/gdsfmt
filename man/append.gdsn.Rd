\name{append.gdsn}
\alias{append.gdsn}
\title{Append data to a specified variable}
\description{
	Append new data to the data field of a GDS node.
}

\usage{
append.gdsn(node, val, check=TRUE)
}
\arguments{
	\item{node}{an object of class \code{\link[gdsfmt]{gdsn.class}}}
	\item{val}{new data to be appended}
	\item{check}{whether a warning is given, when appended data can not
		match the capability of data field; if \code{val} is character-type,
		a warning will be shown if there is any \code{NA} in \code{val}}
}
\details{
	\code{storage.mode(val)} should be "integer", "double", "character"
or "logical". GDS format does not support missing characters \code{NA},
and any \code{NA} will be converted to a blank string \code{""}.
}
\value{
	None.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
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


# close the gds file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
