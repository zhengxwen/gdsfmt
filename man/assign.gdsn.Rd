\name{assign.gdsn}
\alias{assign.gdsn}
\title{Assign/append data to a GDS node}
\description{
	Assign data to a GDS node, or append data to a GDS node
}

\usage{
assign.gdsn(dest.obj, src.obj, append=TRUE)
}
\arguments{
	\item{dest.obj}{an object of class \code{\link{gdsn.class}},
		a destination GDS node}
	\item{src.obj}{an object of class \code{\link{gdsn.class}},
		a source GDS node}
	\item{append}{if TRUE, append data; otherwise, replace the old one}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{read.gdsn}}, \code{\link{write.gdsn}}
}

\keyword{GDS}
\keyword{utilities}
