\name{digest.gdsn}
\alias{digest.gdsn}
\title{create hash function digests}
\description{
    Create hash function digests for a GDS node.
}

\usage{
digest.gdsn(node, algo=c("md5", "sha256", "sha512"),
    attr.action=c("none", "add", "remove"))
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{algo}{the algorithm to be used; currently available choices are
        "md5" (by default)}
    \item{attr.action}{"none": nothing (by default); "add": add a barcode
        attribute; "remove": remove any hash barcode}
}
\details{
    The R package \code{digest} should be installed to perform hash function
digests.
}
\value{
    A character or \code{NA_character_} when the hash algorithm is not
available.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}

\examples{
library(digest)
library(tools)

# cteate a GDS file
f <- createfn.gds("test.gds")

val <- as.raw(rep(1:128, 1024))
n <- add.gdsn(f, "raw", val)
digest.gdsn(n)
digest.gdsn(n, attr.action="add")
writeBin(read.gdsn(n, .useraw=TRUE), con="test1.bin")

write.gdsn(n, 0, start=1027, count=1)
digest.gdsn(n, attr.action="add")
digest.gdsn(n, "sha256", attr.action="add")
digest.gdsn(n, "sha512", attr.action="add")
writeBin(read.gdsn(n, .useraw=TRUE), con="test2.bin")

print(n, attribute=TRUE)
digest.gdsn(n, attr.action="remove")
print(n, attribute=TRUE)

# close the GDS file
closefn.gds(f)

# check with other program
md5sum(c("test1.bin", "test2.bin"))


# delete the temporary file
unlink(c("test.gds", "test1.bin", "test2.bin"), force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
