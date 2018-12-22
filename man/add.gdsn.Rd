\name{add.gdsn}
\alias{add.gdsn}
\title{Add a new GDS node}
\description{
    Add a new GDS node to the GDS file.
}

\usage{
add.gdsn(node, name, val=NULL, storage=storage.mode(val), valdim=NULL,
    compress=c("", "ZIP", "ZIP_RA", "LZMA", "LZMA_RA", "LZ4", "LZ4_RA"),
    closezip=FALSE, check=TRUE, replace=FALSE, visible=TRUE, ...)
}

\arguments{
    \item{node}{an object of class \code{\link{gdsn.class}} or
        \code{\link{gds.class}}:
        \code{"gdsn.class"} -- the node of hierarchical structure;
        \code{"gds.class"} -- the root of hieracrchical structure}
    \item{name}{the variable name; if it is not specified, a temporary name
        is assigned}
    \item{val}{the R value can be integers, real numbers, characters,
        factor, logical or raw variable, \code{list} and \code{data.frame}}
    \item{storage}{to specify data type (not case-sensitive),
        signed integer:
            "int8", "int16", "int24", "int32", "int64",
            "sbit2", "sbit3", ..., "sbit16", "sbit24", "sbit32", "sbit64",
            "vl_int" (encoding variable-length signed integer);
        unsigned integer:
            "uint8", "uint16", "uint24", "uint32", "uint64",
            "bit1", "bit2", "bit3", ..., "bit15", "bit16", "bit24", "bit32",
            "bit64", "vl_uint" (encoding variable-length unsigned integer);
        floating-point number ( "float32", "float64" );
        packed real number ( "packedreal8", "packedreal16", "packedreal24",
            "packedreal32": pack a floating-point number to a signed
            8/16/24/32-bit integer with two attributes "offset" and "scale",
            representing ``(signed int)*scale + offset'', where the minimum of
            the signed integer is used to represent NaN; "packedreal8u",
            "packedreal16u", "packedreal24u", "packedreal32u": pack a
            floating-point number to an unsigned 8/16/24/32-bit integer with
            two attributes "offset" and "scale", representing
            ``(unsigned int)*scale + offset'', where the maximum of the unsigned
            integer is used to represent NaN );
        string (variable-length: "string", "string16", "string32";
            C [null-terminated] string: "cstring", "cstring16", "cstring32";
            fixed-length: "fstring", "fstring16", "fstring32");
        Or "char" (="int8"), "int"/"integer" (="int32"), "single" (="float32"),
            "float" (="float32"), "double" (="float64"),
            "character" (="string"), "logical", "list", "factor", "folder";
        Or a \code{\link{gdsn.class}} object, the storage mode is set to be
            the same as the object specified by \code{storage}.}
    \item{valdim}{the dimension attribute for the array to be created, which
        is a vector of length one or more giving the maximal indices in each
        dimension}
    \item{compress}{the compression method can be "" (no compression),
        "ZIP", "ZIP.fast", "ZIP.def", "ZIP.max" or "ZIP.none" (original
        zlib); "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.def", "ZIP_RA.max" or
        "ZIP_RA.none" (zlib with efficient random access); "LZ4", "LZ4.none",
        "LZ4.fast", "LZ4.hc" or "LZ4.max" (LZ4 compression/decompression
        library); "LZ4_RA", "LZ4_RA.none", "LZ4_RA.fast", "LZ4_RA.hc" or
        "LZ4_RA.max" (with efficient random access); "LZMA", "LZMA.fast",
        "LZMA.def", "LZMA.max", "LZMA_RA", "LZMA_RA.fast", "LZMA_RA.def",
        "LZMA_RA.max" (lzma compression/decompression algorithm).
        See details}
    \item{closezip}{if a compression method is specified, get into read mode
        after compression}
    \item{check}{if \code{TRUE}, a warning will be given when \code{val} is
        character and there are missing values in \code{val}. GDS format does
        not support missing characters \code{NA}, and any \code{NA} will be
        converted to a blank string \code{""}}
    \item{replace}{if \code{TRUE}, replace the existing variable silently
        if possible}
    \item{visible}{\code{FALSE} -- invisible/hidden, except
        \code{print(, all=TRUE)}}
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
        to "ZIP.def". "ZIP.fast", "ZIP.def" and "ZIP.max" correspond to
        different compression levels.

        To support efficient random access of Z stream, "ZIP_RA", "ZIP_RA.fast",
        "ZIP_RA.def" or "ZIP_RA.max" should be specified. "ZIP_RA" option is
        equivalent to "ZIP_RA.def:256K". The block size can be specified by
        following colon, and "16K", "32K", "64K", "128K", "256K", "512K", "1M",
        "2M", "4M" and "8M" are allowed, like "ZIP_RA:64K". The compression
        algorithm tries to keep each independent compressed data block to be
        about of the specified block size, like 64K.

        LZ4 fast lossless compression algorithm is allowed when
        \code{compress="LZ4"} (\url{http://code.google.com/p/lz4/}). Three
        compression levels can be specified, "LZ4.fast" (LZ4 fast mode),
        "LZ4.hc" (LZ4 high compression mode), "LZ4.max" (maximize the
        compression ratio). The block size can be specified by following colon,
        and "64K", "256K", "1M" and "4M" are allowed according to LZ4 frame
        format. "LZ4" is equivalent to "LZ4.hc:256K".

        To support efficient random access of LZ4 stream, "LZ4_RA",
        "LZ4_RA.fast", "LZ4_RA.hc" or "ZIP_RA.max" should be specified.
        "LZ4_RA" option is equivalent to "LZ4_RA.hc:256K". The block size can
        be specified by following colon, and "16K", "32K", "64K", "128K",
        "256K", "512K", "1M", "2M", "4M" and "8M" are allowed, like
        "LZ4_RA:64K". The compression algorithm tries to keep each independent
        compressed data block to be about of the specified block size, like 64K.

        LZMA compression algorithm (\url{http://tukaani.org/xz/}) is available
        since gdsfmt_v1.7.18, which has a higher compression ratio than ZIP
        algorithm. "LZMA", "LZMA.fast", "LZMA.def" and "LZMA.max" available.
        To support efficient random access of LZMA stream, "LZMA_RA",
        "LZMA_RA.fast", "LZMA_RA.def" and "LZMA_RA.max" can be used. The block
        size can be specified by following colon. "LZMA_RA" is equivalent to
        "LZMA_RA.def:256K".

        To finish compressing, you should call \code{\link{readmode.gdsn}} to
        close the writing mode.

        the parameter details with equivalent command lines can be found at
        \code{\link{compression.gdsn}}.

    \code{closezip}: if compression option is specified, then enter a read
        mode after deflating the data. see \code{\link{readmode.gdsn}}.

    \code{...}: if \code{storage = "fstring"}, \code{"fstring16"} or
        \code{"fstring32"}, users can set the max length of string in advance
        by \code{maxlen=}. If \code{storage = "packedreal8"},
        \code{"packedreal8u"}, \code{"packedreal16"}, \code{"packedreal16u"},
        \code{"packedreal32"} or \code{"packedreal32u"}, users can define
        \code{offset} and \code{scale} to represent real numbers by
        ``val*scale + offset'' where ``val'' is a 8/16/32-bit integer.
        By default, \code{offset=0}, \code{scale=0.01} for \code{"packedreal8"}
        and \code{"packedreal8u"}, \code{scale=0.0001} for \code{"packedreal16"}
        and \code{"packedreal16u"}, \code{scale=0.00001} for
        \code{"packedreal24"} and \code{"packedreal24u"},
        \code{scale=0.000001} for \code{"packedreal32"} and
        \code{"packedreal32u"}. For example,
        \code{packedreal8:scale=1/127,offset=0},
        \code{packedreal16:scale=1/32767,offset=0} for correlation [-1, 1];
        \code{packedreal8u:scale=1/254,offset=0},
        \code{packedreal16u:scale=1/65534,offset=0} for a probability [0, 1].
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


# close the GDS file
closefn.gds(f)


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
