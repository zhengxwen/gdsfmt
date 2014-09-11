\name{add.gdsn}
\alias{add.gdsn}
\title{Add a new GDS node}
\description{
	Add a new GDS node to the GDS file.
}

\usage{
add.gdsn(node, name, val=NULL, storage=storage.mode(val),
	valdim=NULL, compress=c("", "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max"),
	closezip=FALSE, check=TRUE, replace=FALSE)
}

\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}} or
		\code{\link{gds.class}}:
		\code{"gdsn.class"} -- the node of hierarchical structure;
		\code{"gds.class"} -- the root of hieracrchical structure}
	\item{name}{the variable name of the added node; if it is not specified,
		``\code{Item} \code{N}'' is assigned to \code{name}, where \code{N}
		is the number of child nodes plus one}
	\item{val}{the R value can be integers, real numbers, characters,
		factor, logical variable, \code{list} and \code{data.frame}}
	\item{storage}{to specify data type (not case-sensitive): integer (
		signed:
			"int8", "int16", "int24", "int32", "int64",
			"sbit2", "sbit3", "sbit4", ..., "sbit32", "sbit64" ;
		unsigned:
			"uint8", "uint16", "uint24", "uint32", "uint64",
			"bit1", "bit2", "bit3", ..., "bit32", "bit64" );
		float ("float32", "float64");
		string (variable-length: "string", "string16", "string32" ;
			fixed-length: "fstring", "fstring16", "fstring32").
		Or "char" (="int8"), "int"/"integer" (="int32"), "float" (="float32"),
			"double" (="float64"), "character" (="string"),
			"logical", "list", "factor", "folder"}
	\item{valdim}{the dimension attribute for the array to be created, which
		is a vector of length one or more giving the maximal indices in each
		dimension}
	\item{compress}{the compression method can be "", "ZIP", "ZIP.fast",
		"ZIP.default", or "ZIP.max"}
	\item{closezip}{if a compression method is specified, get into read mode
		after compression}
	\item{check}{if \code{TRUE}, a warning will be given when \code{val} is
		character and there are missing values in \code{val}. GDS format does
		not support missing characters \code{NA}, and any \code{NA} will be
		converted to a blank string \code{""}}
	\item{replace}{if \code{TRUE}, replace the existing variable silently
		if possible}
}

\details{
	\code{val}: if val is \code{list} or \code{data.frame}, the child node(s)
		will be added corresponding to objects in \code{list} or
		\code{data.frame}. If calling \code{add.gdsn(node, name, val=NULL)},
		then a label will be added which does not have any other data except
		the name and attributes.

	\code{storage}: the default value is \code{storage.mode(val)}, "int"
		denotes signed integer, "uint" denotes unsigned integer, 8, 16, 24,
		32 and 64 denote the number of bits. "bit1" to "bit32" denote the
		packed data types for 1 to 32 bits which are packed on disk, and
		"sbit2" to "sbit32" denote the corresponding signed integers.
		"float32" denotes single-precision number, and "float64" denotes
		double-precision number. "string" represents strings of 8-bit
		characters, "wstring" represents strings of 16-bit characters
		following UTF16 industry standard, and "dwstring" represents strings
		of 32-bit characters following UTF32 industry standard. "folder" is
		to create a folder.

	\code{valdim}: the values in data are taken to be those in the array with
		the leftmost subscript moving fastest. Some of entries could be ZERO.
		If the total number of elements is zero, gdsfmt does not allocate
		storage space.

	\code{compress}: Z compression algorithm can be used to deflate the data
		stored in the GDS file. "ZIP" option is equivalent to "ZIP.default".
		"ZIP.fast", "ZIP.default" and "ZIP.max" correspond to different
		compression levels. In the comprssion mode, you can not change the
		non-first dimension and read data field. Call
		\code{\link{readmode.gdsn}} to return read mode.

	\code{closezip}: if compression option is specified, then enter a read
		mode after deflating the data. see \code{\link{readmode.gdsn}}.
}

\value{
	An object of class \code{\link{gdsn.class}} of the new node.
}

\references{
	\url{http://sourceforge.net/projects/corearray/},
	\url{http://www.zlib.net/}
}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{addfile.gdsn}}, \code{\link{addfolder.gdsn}},
	\code{\link{index.gdsn}}, \code{\link{objdesp.gdsn}},
	\code{\link{read.gdsn}}, \code{\link{readex.gdsn}},
	\code{\link{write.gdsn}}, \code{\link{append.gdsn}}
}

\examples{
# cteate a GDS file
f <- createfn.gds("test.gds")
L <- -2500:2499

##########################################################################
# commom types

add.gdsn(f, "label", NULL)
add.gdsn(f, "int", 1:10000, compress="ZIP", closezip=TRUE)
add.gdsn(f, "int.matrix", matrix(L, nrow=100, ncol=50))
add.gdsn(f, "double", seq(1, 1000, 0.4))
add.gdsn(f, "character", c("int", "double", "logical", "factor"))
add.gdsn(f, "logical", rep(c(TRUE, FALSE, NA), 50))
add.gdsn(f, "factor", as.factor(c(letters, NA, "AA", "CC")))
add.gdsn(f, "NA", rep(NA, 10))
add.gdsn(f, "NaN", c(rep(NaN, 20), 1:20))
add.gdsn(f, "bit2-matrix", matrix(L[1:5000], nrow=50, ncol=100),
	storage="bit2")
# list and data.frame
add.gdsn(f, "list", list(X=1:10, Y=seq(1, 10, 0.25)))
add.gdsn(f, "data.frame", data.frame(X=1:19, Y=seq(1, 10, 0.5)))


##########################################################################
# save a .RData object

obj <- list(X=1:10, Y=seq(1, 10, 0.1))
save(obj, file="tmp.RData")
addfile.gdsn(f, "tmp.RData", filename="tmp.RData")

f

read.gdsn(index.gdsn(f, "list"))
read.gdsn(index.gdsn(f, "list/Y"))
read.gdsn(index.gdsn(f, "data.frame"))


##########################################################################
# allocate the disk spaces

n1 <- add.gdsn(f, "n1", 1:100, valdim=c(10, 20))
read.gdsn(index.gdsn(f, "n1"))

n2 <- add.gdsn(f, "n2", matrix(1:100, 10, 10), valdim=c(15, 20))
read.gdsn(index.gdsn(f, "n2"))


##########################################################################
# replace variables

f

add.gdsn(f, "double", 1:100, storage="float", replace=TRUE)
f
read.gdsn(index.gdsn(f, "double"))


closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
