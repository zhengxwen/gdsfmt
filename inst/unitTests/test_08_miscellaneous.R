#############################################################
#
# DESCRIPTION: miscellaneous
#

library(RUnit)
library(digest)
library(tools)
library(gdsfmt)


#############################################################
#
# test functions
#

test.digest <- function()
{
	set.seed(1000)

	for (i in 1:10)
	{    
		len <- sample.int(100000, 1)
		val <- as.raw(sample.int(256, len, replace=TRUE) - 1L)
		writeBin(val, con="test.bin")

		# cteate a GDS file
		f <- createfn.gds("test.gds")
		add.gdsn(f, "raw", val)
		closefn.gds(f)

		f <- openfn.gds("test.gds")
		hash1 <- unname(digest.gdsn(index.gdsn(f, "raw")))
		closefn.gds(f)

		# check with other program
		hash2 <- unname(md5sum("test.bin"))

		checkEquals(hash1, hash2, paste("md5 digest", i))
	}

	# delete the temporary file
	unlink(c("test.gds", "test.bin"), force=TRUE)
}



test.zip_block.concatenate <- function()
{
	set.seed(1000)

	for (i in 1:10)
	{
	    val <- sample.int(2^28, 500000, replace=TRUE)
	    val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="ZIP_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}



test.lz4_block.concatenate <- function()
{
	set.seed(100)

	for (i in 1:10)
	{
	    val <- sample.int(2^28, 500000, replace=TRUE)
	    val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="LZ4_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}
