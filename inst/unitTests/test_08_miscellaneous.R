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

.test.digest <- function()
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
