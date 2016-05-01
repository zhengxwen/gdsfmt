library(RUnit)
library(gdsfmt)

# a list of data types
type.list <- c("int8", "int16", "int24", "int32", "int64",
	paste("sbit", 2:16, sep=""), "sbit64",
	"uint8", "uint16", "uint24", "uint32", "uint64",
	paste("bit", 1:16, sep=""), "bit64",
	"float32", "float64",
	"packedreal8", "packedreal16", "packedreal24", "packedreal32",
	"string", "string16", "string32",
	"cstring", "cstring16", "cstring32",
	"fstring", "fstring16", "fstring32")

# a list of compression algorithms
compress.list <- c("ZIP", "ZIP_RA:16K", "LZ4", "LZ4.fast",
	"LZ4_RA:16K", "LZ4_RA.fast:16K", "LZMA", "LZMA_RA:16K")

# unittest.gdsfmt path
base.path <- system.file("unitTests", package="gdsfmt")
