\name{digest.gdsn}
\alias{digest.gdsn}
\title{create hash function digests}
\description{
    Create hash function digests for a GDS node.
}

\usage{
digest.gdsn(node, algo=c("md5", "sha1", "sha256", "sha384", "sha512"),
    action=c("none", "Robject", "add", "add.Robj", "clear", "verify", "return"))
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
    \item{algo}{the algorithm to be used; currently available choices are
        "md5" (by default), "sha1", "sha256", "sha384", "sha512"}
    \item{action}{"none": nothing (by default); "Robject": convert to R object,
        i.e., raw, integer, double or character before applying hash digests;
        "add": add a barcode attribute; "add.Robj": add a barcode attribute
        generated from R object; "clear": remove all hash barcodes;
        "verify": verify data integrity if there is any hash code in the
        attributes, and stop if any fails; "return": compare the existing hash
        code in the attributes, and return \code{FALSE} if fails, \code{NA} if
        no hash code, and \code{TRUE} if the verification succeeds}
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
n1 <- add.gdsn(f, "raw1", val)
n2 <- add.gdsn(f, "int1", as.integer(val))
n3 <- add.gdsn(f, "int2", as.integer(val), compress="ZIP", closezip=TRUE)

digest.gdsn(n1)
digest.gdsn(n1, action="Robject")
digest.gdsn(n1, action="add")
digest.gdsn(n1, action="add.Robj")
writeBin(read.gdsn(n1, .useraw=TRUE), con="test1.bin")

write.gdsn(n1, 0, start=1027, count=1)
digest.gdsn(n1, action="add")
digest.gdsn(n1, action="add.Robj")
digest.gdsn(n1, "sha1", action="add")
digest.gdsn(n1, "sha256", action="add")
digest.gdsn(n1, "sha384", action="add")
digest.gdsn(n1, "sha512", action="add")
writeBin(read.gdsn(n1, .useraw=TRUE), con="test2.bin")

print(n1, attribute=TRUE)
digest.gdsn(n1, action="verify")

digest.gdsn(n1, action="clear")
print(n1, attribute=TRUE)


digest.gdsn(n2)
digest.gdsn(n2, action="Robject")

# using R object
digest.gdsn(n2) == digest.gdsn(n3)  # FALSE
digest.gdsn(n2, action="Robject") == digest.gdsn(n3, action="Robject")  # TRUE

# close the GDS file
closefn.gds(f)

# check with other program
md5sum(c("test1.bin", "test2.bin"))


# delete the temporary file
unlink(c("test.gds", "test1.bin", "test2.bin"), force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
