#############################################################
#
# DESCRIPTION: test data dimension
#

library(RUnit)
library(gdsfmt)


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




#############################################################
#
# test function
#

test.data.dimension <- function()
{
	# create a new gds file
	gfile <- createfn.gds("tmp.gds")

	# one dimension:
	dta <- seq(0, 1999)
	node <- add.gdsn(gfile, "data1", val=dta)
	checkEquals(read.gdsn(node), dta, "data.dimension: one dimension")
	setdim.gdsn(node, 100)
	checkEquals(read.gdsn(node), dta[1:100], "data.dimension: one dimension")

	# two dimensions:
	dta <- matrix(seq(0, 1999), nrow=50, ncol=40)
	node <- add.gdsn(gfile, "data2", val=dta)
	checkEquals(read.gdsn(node), dta, "data.dimension: two dimensions")
	setdim.gdsn(node, c(40, 40))
	checkEquals(read.gdsn(node), dta[1:40, ], "data.dimension: two dimensions")

	# three dimensions:
	dta <- array(seq(0, 999), c(20, 10, 5))
	node <- add.gdsn(gfile, "data3", val=dta)
	checkEquals(read.gdsn(node), dta, "data.dimension: three dimensions")
	setdim.gdsn(node, c(15, 10, 5))
	checkEquals(read.gdsn(node), dta[1:15,,], "data.dimension: three dimensions")

 	# close the gds file
	closefn.gds(gfile)
	unlink("tmp.gds")
}


test.data.append <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	for (n in type.list)
	{
		dta1 <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50)
		dta2 <- dta1[, 1:30]

		# create a new gds file
		gfile <- createfn.gds("tmp.gds")

		# append data
		node <- add.gdsn(gfile, "data", val=dta1)
		append.gdsn(node, dta2)

		checkEquals(read.gdsn(node), cbind(dta1, dta2), sprintf("data append: %s", n))

	 	# close the gds file
		closefn.gds(gfile)
		unlink("tmp.gds")
	}
}


test.data.append.compress <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	for (n in type.list)
	{
		dta1 <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50)
		dta2 <- dta1[, 1:30]

		# create a new gds file
		gfile <- createfn.gds("tmp.gds")

		# append data
		node <- add.gdsn(gfile, "data", val=dta1, compress="ZIP")
		append.gdsn(node, dta2)
		readmode.gdsn(node)

		checkEquals(read.gdsn(node), cbind(dta1, dta2), sprintf("data append: %s", n))

	 	# close the gds file
		closefn.gds(gfile)
		unlink("tmp.gds")
	}
}

