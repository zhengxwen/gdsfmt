#############################################################
#
# DESCRIPTION: test data dimension
#

source(system.file("unitTests", "include.r", package="gdsfmt"))



#############################################################
#
# test functions
#

test.data.read_write <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.data.read_write <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

	for (n in type.list)
	{
		if (verbose) cat(n, "\t", sep="")
		if (n %in% c("vl_uint", "vl_int")) next

		dta <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50, ncol=40)

		set.seed(1000)
		rows <- as.integer(runif(500) * 50) + 1
		cols <- as.integer(runif(500) * 40) + 1

		r.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
		w.dta <- dta
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			r.dta[i.row, i.col] <- dta[i.row, i.col]
			w.dta[i.row, i.col] <- 0
		}

		# create a new gds file
		gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

		# append data
		node <- add.gdsn(gfile, "data", val=dta, storage=n)
		
		r2.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			r2.dta[i.row, i.col] <- read.gdsn(node, start=c(i.row, i.col),
				count=c(1, 1))
		}
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			write.gdsn(node, 0, start=c(i.row, i.col), count=c(1, 1))
		}

		closefn.gds(gfile)

		gfile <- openfn.gds("tmp.gds", allow.duplicate=TRUE)
		node <- index.gdsn(gfile, "data")

		checkEquals(r2.dta, r.dta, sprintf("data random read: %s", n))
		checkEquals(read.gdsn(node), w.dta, sprintf("data random write: %s", n))

	 	# close the gds file
		closefn.gds(gfile)
	}
}


test.data.read_write.compress.zip <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.data.read_write.compress.zip <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))
	set.seed(1000)

	for (cp in compress.list)
	{
		if (verbose)
			cat("Compression:", cp, "\n", sep="")

		for (n in type.list)
		{
			if (verbose) cat(n, "\t", sep="")
			if (n %in% c("vl_uint", "vl_int")) next

			dta <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50, ncol=40)

			rows <- as.integer(runif(500) * 50) + 1
			cols <- as.integer(runif(500) * 40) + 1

			r.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
			w.dta <- dta
			for (i in 1:200)
			{
				i.row <- rows[i]; i.col <- cols[i]
				r.dta[i.row, i.col] <- dta[i.row, i.col]
				w.dta[i.row, i.col] <- 0
			}

			# create a new gds file
			gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

			# append data
			node <- add.gdsn(gfile, "data", val=dta, storage=n,
				compress=cp, closezip=TRUE)
		
			closefn.gds(gfile)

			gfile <- openfn.gds("tmp.gds", allow.duplicate=TRUE)
			node <- index.gdsn(gfile, "data")

			r2.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
			for (i in 1:200)
			{
				i.row <- rows[i]; i.col <- cols[i]
				r2.dta[i.row, i.col] <- read.gdsn(node,
					start=c(i.row, i.col), count=c(1, 1))
			}

			checkEquals(r2.dta, r.dta,
				sprintf("data random read (%s): %s", cp, n))

			# close the gds file
			closefn.gds(gfile)
		}

		if (verbose) cat("\n")
	}
}


test.data.read_write.file <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink(c("tmp.gds", "tmp.RData"), force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.data.read_write.file <<<<\n")

	# the name of file
	fn <- sprintf("%s/valid/standard.RData", base.path)

	for (cp in compress.list)
	{
		if (verbose) cat(cp, "\t", sep="")

		# create a new gds file
		gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

		node <- addfile.gdsn(gfile, "tmp.RData", filename=fn, compress=cp)
		getfile.gdsn(node, "tmp.RData")

		checkEquals(get(load("tmp.RData")), get(load(fn)),
			sprintf("data stream (%s)", cp))

		# close the gds file
		closefn.gds(gfile)
	}
}


test.data.read_selection <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.data.read_selection <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))
	set.seed(1000)

	for (n in type.list)
	{
		if (verbose) cat(n, "\t", sep="")

		dta <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50, ncol=40)

		# create a new gds file
		gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

		# append data
		node <- add.gdsn(gfile, "data", val=dta, storage=n)

		# for-loop
		for (i in 1:25)
		{
			rsel <- rep(FALSE, nrow(dta))
			rsel[sample.int(length(rsel), 25)] <- TRUE
			csel <- rep(FALSE, ncol(dta))
			csel[sample.int(length(csel), 15)] <- TRUE

			r1.dat <- dta[rsel, csel]
			r2.dat <- readex.gdsn(node, sel=list(rsel, csel))
			checkEquals(r1.dat, r2.dat,
				sprintf("data read with random selection: %s", n))

			# scan by row
			for (j in which(rsel))
			{
				rrsel <- rep(FALSE, length(rsel))
				rrsel[j] <- TRUE
				r1.dat <- dta[rrsel, csel]
				r2.dat <- readex.gdsn(node, sel=list(rrsel, csel))
				checkEquals(r1.dat, r2.dat,
					sprintf("data read with random selection: %s", n))
			}

			# scan by column
			for (j in which(csel))
			{
				ccsel <- rep(FALSE, length(csel))
				ccsel[j] <- TRUE
				r1.dat <- dta[rsel, ccsel]
				r2.dat <- readex.gdsn(node, sel=list(rsel, ccsel))
				checkEquals(r1.dat, r2.dat,
					sprintf("data read with random selection: %s", n))
			}
		}

	 	# close the gds file
		closefn.gds(gfile)
	}
}
