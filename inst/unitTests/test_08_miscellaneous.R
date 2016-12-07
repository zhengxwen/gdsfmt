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
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.digest <<<<\n")

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

	# delete the temporary files
	unlink(c("test.gds", "test.bin"), force=TRUE)
}


test.digest.factor <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.digest.factor <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{	 
		f <- createfn.gds("test.gds")

		v <- sample.int(26, 10000, TRUE)
		v[sample.int(length(v), 10)] <- NA
		vv <- factor(v, labels=letters)

		n1 <- add.gdsn(f, "i1", vv)
		n2 <- add.gdsn(f, "i2", as.character(vv), check=FALSE)

		checkEquals(digest.gdsn(n1, action="Robject"),
			digest.gdsn(n2, action="Robject"), "md5 R object digest")

		closefn.gds(f)
	}

	# delete the temporary file
	unlink("test.gds", force=TRUE)
}


test.zip_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.zip_block.concatenate <<<<\n")

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


test.zip_block_real16.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.zip_block_real16.concatenate <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{
		val <- round(runif(500000), 4)
		val2 <- round(runif(sample.int(256, 1)), 4)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "float", val, storage="packedreal16",
			compress="ZIP_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "float2", storage="packedreal16", compress="ZIP_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "float3", storage="packedreal16", compress="ZIP_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "float4", storage="packedreal16", compress="ZIP_RA:16K")
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
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.lz4_block.concatenate <<<<\n")

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


test.lzma_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.lzma_block.concatenate <<<<\n")

	set.seed(100)

	for (i in 1:10)
	{
		val <- sample.int(2^28, 500000, replace=TRUE)
		val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="LZMA_RA:32K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}
