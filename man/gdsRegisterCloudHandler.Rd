\name{gdsRegisterCloudHandler}
\alias{gdsRegisterCloudHandler}
\alias{gdsUnregisterCloudHandler}
\alias{gdsCloudHandlers}
\title{Cloud URL scheme handlers}
\description{
    Register, unregister and list the handler functions used by
\code{\link{openfn.gds}} to open GDS files identified by a URL.
}

\usage{
gdsRegisterCloudHandler(scheme, handler, pkgname=NULL)
gdsUnregisterCloudHandler(scheme)
gdsCloudHandlers()
}
\arguments{
    \item{scheme}{a URL scheme name in lower case and without the trailing
        \code{"://"}, e.g., \code{"s3"}; \code{gdsUnregisterCloudHandler()}
        accepts a character vector of scheme names}
    \item{handler}{a function opening a GDS file from a URL, see details}
    \item{pkgname}{the name of the package registering the handler, used for
        reporting only; or \code{NULL} for unknown}
}
\details{
    \code{gdsfmt} itself has no support for remote storage. Instead, a
companion package (for example \code{gdscloud}) registers a handler function
for each URL scheme it implements. When the file name passed to
\code{\link{openfn.gds}} starts with a registered scheme followed by
\code{"://"}, the call is forwarded to the corresponding handler; when the
scheme is not registered, \code{\link{openfn.gds}} raises an error.

    \code{handler} is called as \code{handler(filename, allow.error=)} where
\code{filename} is the full URL, so it should accept the URL as its first
argument and must have a \code{...} argument to remain forward compatible
with new arguments of \code{\link{openfn.gds}}. It should return an object of
class \code{\link{gds.class}}, or raise an error. Cloud URLs are read-only:
\code{\link{openfn.gds}} rejects \code{readonly=FALSE} before dispatching.

    Registering a scheme that is already registered replaces the previous
handler. A package registering handlers in its \code{.onLoad()} should
unregister them in its \code{.onUnload()}.
}
\value{
    \code{gdsRegisterCloudHandler()} returns the previously registered
handler for \code{scheme} (or \code{NULL}) invisibly.

    \code{gdsUnregisterCloudHandler()} returns invisibly a named logical
vector telling, for each element of \code{scheme}, whether a handler was
registered.

    \code{gdsCloudHandlers()} returns a \code{data.frame} with the columns
\code{scheme} and \code{package}, with one row per registered scheme.
}

\author{Xiuwen Zheng}
\seealso{
    \code{\link{openfn.gds}}
}

\examples{
# the schemes currently registered by companion packages, if any
gdsCloudHandlers()

# a toy handler, for illustration only
my_open <- function(filename, ...)
    stop("Not implemented, this is only an example.")

gdsRegisterCloudHandler("myfs", my_open, pkgname="myPkg")
gdsCloudHandlers()

# openfn.gds() now dispatches "myfs://" URLs to my_open()
try(openfn.gds("myfs://bucket/file.gds"))

gdsUnregisterCloudHandler("myfs")
gdsCloudHandlers()
}

\keyword{GDS}
\keyword{utilities}
