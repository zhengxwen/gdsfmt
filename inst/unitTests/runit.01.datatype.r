#############################################################
#
# DESCRIPTION: test internal data types
#

library(RUnit)
library(gdsfmt)


# call internal C function
class.nbit <- function(class.name)
{
	rv <- .C("gds_Internal_Class", as.character(class.name),
		out_nbit = integer(1), err=integer(1), NAOK=TRUE, PACKAGE="gdsfmt")
	if (rv$err != 0) stop(lasterr.gds())

	rv$out_nbit
}



#############################################################
# test function
#

test.datatype <- function()
{
	# integers
	checkEquals(class.nbit("integer"), 32, "numeric type: integer")
	checkEquals(class.nbit("int8"), 8, "numeric type: int8")
	checkEquals(class.nbit("uint8"), 8, "numeric type: uint8")
	checkEquals(class.nbit("int16"), 16, "numeric type: int16")
	checkEquals(class.nbit("uint16"), 16, "numeric type: uint16")
	checkEquals(class.nbit("int24"), 24, "numeric type: int24")
	checkEquals(class.nbit("uint24"), 24, "numeric type: uint24")
	checkEquals(class.nbit("int32"), 32, "numeric type: int32")
	checkEquals(class.nbit("uint32"), 32, "numeric type: uint32")
	checkEquals(class.nbit("int64"), 64, "numeric type: int64")
	checkEquals(class.nbit("uint64"), 64, "numeric type: uint64")

	# sbit??
	for (n in 2:16)
	{
		cn <- sprintf("sbit%d", n)
		checkEquals(class.nbit(cn), n, sprintf("numeric type: %s", cn))
	}

	# bit??
	for (n in 1:16)
	{
		cn <- sprintf("bit%d", n)
		checkEquals(class.nbit(cn), n, sprintf("numeric type: %s", cn))
	}

	# float
	checkEquals(class.nbit("float"), 32, "numeric type: float32")
	checkEquals(class.nbit("float32"), 32, "numeric type: float32")
	checkEquals(class.nbit("double"), 64, "numeric type: double")
	checkEquals(class.nbit("float64"), 64, "numeric type: float64")

	# string
	checkEquals(class.nbit("character"), 8, "numeric type: character")
	checkEquals(class.nbit("string"), 8, "numeric type: string")
	checkEquals(class.nbit("string16"), 16, "numeric type: string16")
	checkEquals(class.nbit("string32"), 32, "numeric type: string32")

	checkEquals(class.nbit("fstring"), 8, "numeric type: string")
	checkEquals(class.nbit("fstring16"), 16, "numeric type: string16")
	checkEquals(class.nbit("fstring32"), 32, "numeric type: string32")

	# others
	checkEquals(class.nbit("logical"), 32, "numeric type: logical")
}
