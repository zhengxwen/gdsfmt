#############################################################
#
# DESCRIPTION: test data dimension
#

source(system.file("unitTests", "include.r", package="gdsfmt"))



#############################################################
#
# test functions
#

test.data.dimension <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	# create a new gds file
	gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

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
	checkEquals(read.gdsn(node), matrix(seq(0, 1599), nrow=40, ncol=40),
		"data.dimension: two dimensions")

	# three dimensions:
	dta <- array(seq(0, 999), c(20, 10, 5))
	node <- add.gdsn(gfile, "data3", val=dta)
	checkEquals(read.gdsn(node), dta, "data.dimension: three dimensions")
	setdim.gdsn(node, c(15, 10, 5))
	checkEquals(read.gdsn(node), array(0:749, dim=c(15, 10, 5)),
		"data.dimension: three dimensions")

 	# close the gds file
	closefn.gds(gfile)
}


test.data.append <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.data.append <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

	for (n in type.list)
	{
		if (verbose)
			cat(n, "\t", sep="")

		dta1 <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50)
		dta2 <- dta1[, 1:30]

		# create a new gds file
		gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

		# append data
		node <- add.gdsn(gfile, "data", val=dta1)
		append.gdsn(node, dta2)

		checkEquals(read.gdsn(node), cbind(dta1, dta2),
			sprintf("data append: %s", n))

	 	# close the gds file
		closefn.gds(gfile)
	}
}


test.data.append.compress <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.data.append.compress <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

	for (cp in compress.list)
	{
		if (verbose)
			cat("Compression:", cp, "\n", sep="")

		for (n in type.list)
		{
			if (verbose) cat(n, "\t", sep="")

			dta1 <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50)
			dta2 <- dta1[, 1:30]

			# create a new gds file
			gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

			# append data
			node <- add.gdsn(gfile, "data", val=dta1, compress=cp)
			append.gdsn(node, dta2)
			readmode.gdsn(node)

			checkEquals(read.gdsn(node), cbind(dta1, dta2),
				sprintf("data append: %s with compression %s", n, cp))

			# close the gds file
			closefn.gds(gfile)
		}

		if (verbose) cat("\n")
	}
}
