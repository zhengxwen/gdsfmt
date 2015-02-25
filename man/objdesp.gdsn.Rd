\name{objdesp.gdsn}
\alias{objdesp.gdsn}
\title{Variable description}
\description{
    Get the descritpion of a GDS node.
}

\usage{
objdesp.gdsn(node)
}
\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}}, a GDS node}
}
\value{
    Returns a list:
    \item{name}{the variable name of a specified node}
    \item{fullname}{the full name of a specified node}
    \item{storage}{the storage mode in the GDS file}
    \item{type}{a factor indicating the storage mode in R:
        Label -- a label node,
        Folder -- a directory,
        VFolder -- a virtual folder linking to another GDS file,
        Raw -- raw data (\code{\link{addfile.gdsn}}),
        Integer -- integers,
        Factor -- factor values,
        Logical -- logical values (FALSE, TRUE and NA),
        Real -- floating numbers,
        String -- characters,
        Unknown -- unknown type}
    \item{is.array}{indicates whether it is array-type}
    \item{dim}{the dimension of data field}
    \item{encoder}{encoder for compressed data, such like "ZIP"}
    \item{compress}{the compression method: "", "ZIP.max", etc}
    \item{cpratio}{data compression ratio, \code{NaN} indicates no compression}
    \item{size}{the size of data stored in the GDS file}
    \item{good}{logical, indicates the state of GDS file, e.g.,
        FALSE if the virtual folder fails to link the target GDS file}
    \item{message}{if applicable, messages of the GDS node, such like error
        messages, log information}
    \item{param}{the parameters, used in \code{\link{add.gdsn}}, like
    	"maxlen", "offset", "scale"}
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
    \code{\link{cnt.gdsn}}, \code{\link{name.gdsn}},
    \code{\link{ls.gdsn}}, \code{\link{index.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")

# add a vector to "test.gds"
node1 <- add.gdsn(f, name="vector1", val=1:10000)
objdesp.gdsn(node1)

# add a vector to "test.gds"
node2 <- add.gdsn(f, name="vector2", val=1:10000, compress="ZIP.max",
    closezip=FALSE)
objdesp.gdsn(node2)

# add a character to "test.gds"
node3 <- add.gdsn(f, name="vector3", val=c("A", "BC", "DEF"),
    compress="ZIP", closezip=TRUE)
objdesp.gdsn(node3)

# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
