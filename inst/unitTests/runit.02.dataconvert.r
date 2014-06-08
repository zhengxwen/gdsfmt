#############################################################
#
# DESCRIPTION: test data conversion
#

library(RUnit)
library(gdsfmt)


# create a gds file, read and write data
gds_read_write <- function(class.name, data.kind, zip="")
{
	if (data.kind == 1)
		dta <- seq(-1000, 999)
	else
		dta <- seq(-499, 299)

	# create a new gds file
	gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

	# add a 
	node <- add.gdsn(gfile, "data", val=dta, storage=class.name,
		compress=zip, closezip=TRUE)
	rv <- read.gdsn(node)
 
	# close the gds file
	closefn.gds(gfile)
	unlink("tmp.gds")

	rv
}



#############################################################
# list of data types
#

type.list <- c("int8", "int16", "int24", "int32", "int64",

	"sbit2", "sbit3", "sbit4", "sbit5", "sbit6", "sbit7", "sbit8", "sbit9",
	"sbit10", "sbit11", "sbit12", "sbit13", "sbit14", "sbit15", "sbit16",
	"sbit24", "sbit32", "sbit64",

	"uint8", "uint16", "uint24", "uint32", "uint64",
	"bit1", "bit2", "bit3", "bit4", "bit5", "bit6", "bit7", "bit8", "bit9",
	"bit10", "bit11", "bit12", "bit13", "bit14", "bit15", "bit16", "bit24",
	"bit32", "bit64",

	"float32", "float64")

# gdsfmt path
gdsfmt.path <- system.file("unitTests", package="gdsfmt")


# create standard dataset
# dta <- list()
# for (n in type.list)
# {
#	# the first dataset
#	dta[[sprintf("valid1.%s", n)]] <- gds_read_write(n, 1)

#	# the second dataset
#	dta[[sprintf("valid2.%s", n)]] <- gds_read_write(n, 2)
# }
# save(dta, file="/Users/foxsts/Documents/Codes/Rpackages/gdsfmt/inst/unitTests/valid/standard.RData")




#############################################################
#
# test functions
#

test.dataconvert <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	for (n in type.list)
	{
		checkEquals(gds_read_write(n, 1), valid.dta[[sprintf("valid1.%s", n)]],
			sprintf("data conversion: %s", n))
		checkEquals(gds_read_write(n, 2), valid.dta[[sprintf("valid2.%s", n)]],
			sprintf("data conversion: %s", n))
	}
}


test.dataconvert.compress <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	for (n in type.list)
	{
		checkEquals(gds_read_write(n, 1, "ZIP"), valid.dta[[sprintf("valid1.%s", n)]],
			sprintf("data conversion: %s", n))
		checkEquals(gds_read_write(n, 2, "ZIP"), valid.dta[[sprintf("valid2.%s", n)]],
			sprintf("data conversion: %s", n))
	}
}
