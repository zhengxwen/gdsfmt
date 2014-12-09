\name{add.gdsn}
\alias{add.gdsn}
\title{Add a new GDS node}
\description{
	Add a new GDS node to the GDS file.
}

\usage{
add.gdsn(node, name, val=NULL, storage=storage.mode(val), valdim=NULL,
	compress=c("", "ZIP", "ZIP_RA", "LZ4", "LZ4_RA"), closezip=FALSE,
	check=TRUE, replace=FALSE, ...)
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
		factor, logical or raw variable, \code{list} and \code{data.frame}}
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
	\item{compress}{the compression method can be "" (no compression),
		"ZIP", "ZIP.fast", "ZIP.default", "ZIP.max" or "ZIP.none" (original
		zlib); "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or
		"ZIP_RA.none" (zlib with efficient random access); "LZ4", "LZ4.none",
		"LZ4.fast", "LZ4.hc" or "LZ4.max"; "LZ4_RA", "LZ4_RA.none",
		"LZ4_RA.fast", "LZ4_RA.hc" or "LZ4_RA.max" (with efficient random
		access). See details}
	\item{closezip}{if a compression method is specified, get into read mode
		after compression}
	\item{check}{if \code{TRUE}, a warning will be given when \code{val} is
		character and there are missing values in \code{val}. GDS format does
		not support missing characters \code{NA}, and any \code{NA} will be
		converted to a blank string \code{""}}
	\item{replace}{if \code{TRUE}, replace the existing variable silently
		if possible}
	\item{...}{additional parameters for specific \code{storage}, see details}
}

\details{
	\code{val}: if val is \code{list} or \code{data.frame}, the child node(s)
		will be added corresponding to objects in \code{list} or
		\code{data.frame}. If calling \code{add.gdsn(node, name, val=NULL)},
		then a label will be added which does not have any other data except
		the name and attributes. If \code{val} is raw-type, it is interpreted
		as 8-bit signed integer.

	\code{storage}: the default value is \code{storage.mode(val)}, "int"
		denotes signed integer, "uint" denotes unsigned integer, 8, 16, 24,
		32 and 64 denote the number of bits. "bit1" to "bit32" denote the
		packed data types for 1 to 32 bits which are packed on disk, and
		"sbit2" to "sbit32" denote the corresponding signed integers.
		"float32" denotes single-precision number, and "float64" denotes
		double-precision number. "string" represents strings of 8-bit
		characters, "string16" represents strings of 16-bit characters
		following UTF16 industry standard, and "string32" represents a string
		of 32-bit characters following UTF32 industry standard. "folder" is
		to create a folder.

	\code{valdim}: the values in data are taken to be those in the array with
		the leftmost subscript moving fastest. The last entry could be ZERO.
		If the total number of elements is zero, gdsfmt does not allocate
		storage space. \code{NA} is treated as 0.

	\code{compress}:
		Z compression algorithm (\url{http://www.zlib.net/}) can be used to
		deflate the data stored in the GDS file. "ZIP" option is equivalent
		to "ZIP.default". "ZIP.fast", "ZIP.default" and "ZIP.max" correspond
		to different compression levels.

		To support efficient random access of Z stream, "ZIP_RA",
		"ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max" or "ZIP_RA.none" should
		be specified. "ZIP_RA" option is equivalent to "ZIP_RA.default:256K".
		The block size can be specified by following colon, and "16K", "32K",
		"64K", "128K", "256K", "512K" and "1M" are allowed, such like
		"ZIP_RA:16K". The compression algorithm tries to keep each
		independent compressed data block to be about of the specified block
		size, such like 64K.

		LZ4 fast lossless compression algorithm is allowed with
		\code{compress="LZ4"} (\url{http://code.google.com/p/lz4/}). Three
		compression levels can be specified, "LZ4.fast" (LZ4 fast mode),
		"LZ4.hc" (LZ4 high compression mode), "LZ4.max" (maximize the
		compression ratio). The block size can be specified by following colon,
		and "64K", "256K", "1M" and "4M" are allowed according to LZ4 frame
		format. "LZ4" is equivalent to "LZ4.hc:256K".

		To support efficient random access of LZ4 stream, "LZ4_RA",
		"LZ4_RA.fast", "LZ4_RA.hc", "ZIP_RA.max" or "LZ4_RA.none" should
		be specified. "LZ4_RA" option is equivalent to "LZ4_RA.hc:256K".
		The block size can be specified by following colon, and "16K", "32K",
		"64K", "128K", "256K", "512K" and "1M" are allowed, such like
		"ZIP_RA:16K". The compression algorithm tries to keep each
		independent compressed data block to be about of the specified block
		size, such like 64K.

		To finish compressing, you should call \code{\link{readmode.gdsn}} to
		close the writing mode.

	\code{closezip}: if compression option is specified, then enter a read
		mode after deflating the data. see \code{\link{readmode.gdsn}}.

	\code{...}: if \code{storage = "fstring"}, \code{"fstring16"} or
		\code{"fstring32"}, users can set the max length of string in advance
		by \code{maxlen=}.
}

\value{
	An object of class \code{\link{gdsn.class}} of the new node.
}

\references{
	\url{http://github.com/zhengxwen/gdsfmt}
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
