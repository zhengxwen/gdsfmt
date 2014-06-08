\name{print.gdsn.class}
\alias{print.gdsn.class}
\title{Show the information of class "gdsn.class"}
\description{
	Display an object of class "gdsn.class", a GDS node.
}

\usage{
\method{print}{gdsn.class}(x, expand=TRUE, all=FALSE, ...)
}
\arguments{
	\item{x}{an object of class \code{\link{gdsn.class}}, a GDS node}
	\item{expand}{whether enumerate all of child nodes}
	\item{all}{if \code{FALSE}, hide GDS nodes with an attribute
		"R.invisible"}
	\item{...}{the arguments passed to or from other methods}
}

\references{\url{http://sourceforge.net/projects/corearray/}}
\author{Xiuwen Zheng}
\seealso{\code{\link{print.gds.class}}}

\keyword{GDS}
\keyword{utilities}
