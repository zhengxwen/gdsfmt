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
# test functions
#

test.data.read_write <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	for (n in type.list)
	{
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
		gfile <- createfn.gds("tmp.gds")

		# append data
		node <- add.gdsn(gfile, "data", val=dta)
		
		r2.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			r2.dta[i.row, i.col] <- read.gdsn(node, start=c(i.row, i.col), count=c(1, 1))
		}
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			write.gdsn(node, 0, start=c(i.row, i.col), count=c(1, 1))
		}

		checkEquals(r2.dta, r.dta, sprintf("data random read: %s", n))
		checkEquals(read.gdsn(node), w.dta, sprintf("data random write: %s", n))

	 	# close the gds file
		closefn.gds(gfile)
		unlink("tmp.gds")
	}
}


test.data.read_write.compress.zip <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	set.seed(1000)

	for (n in type.list)
	{
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
		gfile <- createfn.gds("tmp.gds")

		# append data
		node <- add.gdsn(gfile, "data", val=dta, compress="ZIP", closezip=TRUE)
		
		r2.dta <- matrix(0, nrow=nrow(dta), ncol=ncol(dta))
		for (i in 1:200)
		{
			i.row <- rows[i]; i.col <- cols[i]
			r2.dta[i.row, i.col] <- read.gdsn(node,
				start=c(i.row, i.col), count=c(1, 1))
		}

		checkEquals(r2.dta, r.dta,
			sprintf("data random read (zip stream): %s", n))

		# close the gds file
		closefn.gds(gfile)
		unlink("tmp.gds")
	}
}


test.data.read_write.file <- function()
{
	# the name of file
	fn <- sprintf("%s/valid/standard.RData", gdsfmt.path)

	# create a new gds file
	gfile <- createfn.gds("tmp.gds")

	node <- addfile.gdsn(gfile, "tmp.RData", filename=fn)
	getfile.gdsn(node, "tmp.RData")

	checkEquals(get(load("tmp.RData")), get(load(fn)), "data stream")

	# close the gds file
	closefn.gds(gfile)
	unlink("tmp.gds")
	unlink("tmp.RData")
}


test.data.read_selection <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", gdsfmt.path)))

	set.seed(1000)

	for (n in type.list)
	{
		dta <- matrix(valid.dta[[sprintf("valid1.%s", n)]], nrow=50, ncol=40)

		# create a new gds file
		gfile <- createfn.gds("tmp.gds")

		# append data
		node <- add.gdsn(gfile, "data", val=dta)

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

		unlink("tmp.gds")
	}
}
