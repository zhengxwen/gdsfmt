#############################################################
#
# DESCRIPTION: test internal data types
#

library(RUnit)
library(gdsfmt)


# call internal C function
class.nbit <- function(class.name)
{
	.Call("gds_test_Class", class.name, PACKAGE="gdsfmt")
}



#############################################################
#
# test functions
#

test.datatype <- function()
{
	# integers
	checkEquals(class.nbit("integer"), 32, "integer type: int")
	checkEquals(class.nbit("int8"), 8, "integer type: int8")
	checkEquals(class.nbit("uint8"), 8, "integer type: uint8")
	checkEquals(class.nbit("int16"), 16, "integer type: int16")
	checkEquals(class.nbit("uint16"), 16, "integer type: uint16")
	checkEquals(class.nbit("int24"), 24, "integer type: int24")
	checkEquals(class.nbit("uint24"), 24, "integer type: uint24")
	checkEquals(class.nbit("int32"), 32, "integer type: int32")
	checkEquals(class.nbit("uint32"), 32, "integer type: uint32")
	checkEquals(class.nbit("int64"), 64, "integer type: int64")
	checkEquals(class.nbit("uint64"), 64, "integer type: uint64")

	# sbit??
	for (n in 2:16)
	{
		cn <- sprintf("sbit%d", n)
		checkEquals(class.nbit(cn), n, sprintf("integer type: %s", cn))
	}

	# bit??
	for (n in 1:16)
	{
		cn <- sprintf("bit%d", n)
		checkEquals(class.nbit(cn), n, sprintf("integer type: %s", cn))
	}

	# float
	checkEquals(class.nbit("float"), 32, "numeric type: float32")
	checkEquals(class.nbit("float32"), 32, "numeric type: float32")
	checkEquals(class.nbit("double"), 64, "numeric type: double")
	checkEquals(class.nbit("float64"), 64, "numeric type: float64")
	checkEquals(class.nbit("packedreal8"), 8, "numeric type: packedreal8")
	checkEquals(class.nbit("packedreal16"), 16, "numeric type: packedreal16")
	checkEquals(class.nbit("packedreal32"), 32, "numeric type: packedreal32")

	# string
	checkEquals(class.nbit("character"), 8, "character type: character")
	checkEquals(class.nbit("string"), 8, "character type: string")
	checkEquals(class.nbit("string16"), 16, "character type: string16")
	checkEquals(class.nbit("string32"), 32, "character type: string32")

	checkEquals(class.nbit("fstring"), 8, "character type: string")
	checkEquals(class.nbit("fstring16"), 16, "character type: string16")
	checkEquals(class.nbit("fstring32"), 32, "character type: string32")

	# others
	checkEquals(class.nbit("logical"), 32, "integer type: logical")
	checkEquals(class.nbit("factor"), 32, "integer type: factor")
}
