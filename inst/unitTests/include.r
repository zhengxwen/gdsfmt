library(RUnit)
library(gdsfmt)

# a list of data types
type.list <- c("int8", "int16", "int24", "int32", "int64",
	paste("sbit", 2:24, sep=""), "sbit64",
	"uint8", "uint16", "uint24", "uint32", "uint64",
	paste("bit", 1:24, sep=""), "bit64",
	"float32", "float64",
	"packedreal8", "packedreal16", "packedreal24", "packedreal32",
	"string", "string16", "string32", "fstring", "fstring16", "fstring32")

# a list of compression algorithms
compress.list <- c(
	"ZIP", "ZIP.fast", "ZIP.default", "ZIP.max",
	"ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max",
	"LZ4", "LZ4.fast", "LZ4.hc", "LZ4.max",
	"LZ4_RA", "LZ4_RA.fast", "LZ4_RA.hc", "LZ4_RA.max")

# unittest.gdsfmt path
base.path <- system.file("unitTests", package="gdsfmt")
