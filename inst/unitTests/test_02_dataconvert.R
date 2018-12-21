#############################################################
#
# DESCRIPTION: test data conversion
#

source(system.file("unitTests", "include.r", package="gdsfmt"))


# create a gds file, read and write data
gds_read_write <- function(class.name, data.kind, compress="")
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	if (data.kind == 1)
		dta <- seq(-1000, 999)
	else
		dta <- seq(-499, 299)

	if (class.name %in% c("packedreal8", "packedreal16", "packedreal24", "packedreal32"))
		dta <- dta / 1000
	else if (class.name %in% c("packedreal8u", "packedreal16u", "packedreal24u", "packedreal32u"))
		dta <- abs(dta) / 1000
	else if (class.name == "vl_uint")
		dta <- dta + 1000L

	# create a new gds file
	gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

	# add a 
	node <- add.gdsn(gfile, "data", val=dta, storage=class.name,
		compress=compress, closezip=TRUE)
	# close the gds file
	closefn.gds(gfile)

	# open the gds file
	gfile <- openfn.gds("tmp.gds", allow.duplicate=TRUE)
	node <- index.gdsn(gfile, "data")

	rv <- read.gdsn(node)
 
	# close the gds file
	closefn.gds(gfile)

	rv
}

# create standard dataset
data.create <- function()
{
	dta <- list()
	for (n in type.list)
	{
		# the first dataset
		dta[[sprintf("valid1.%s", n)]] <- gds_read_write(n, 1)

		# the second dataset
		dta[[sprintf("valid2.%s", n)]] <- gds_read_write(n, 2)
	}
	save(dta, file="standard.RData", compress="xz")
}




#############################################################
#
# test functions
#

test.dataconvert <- function()
{
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

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
	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

	for (cp in compress.list)
	{
		for (n in type.list)
		{
			checkEquals(gds_read_write(n, 1, cp), valid.dta[[sprintf("valid1.%s", n)]],
				sprintf("data conversion: %s, compress: %s", n, cp))
			checkEquals(gds_read_write(n, 2, cp), valid.dta[[sprintf("valid2.%s", n)]],
				sprintf("data conversion: %s, compress: %s", n, cp))
		}
	}
}
