# options(test.verbose=FALSE)
# BiocGenerics:::testPackage("gdsfmt")


library(RUnit)
library(gdsfmt)

# unittest.gdsfmt path
base.path <- system.file("unitTests", package="gdsfmt")


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



valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

n <- "packedreal8"

d <- gds_read_write(n, 1)
print(d)
checkEquals(d, valid.dta[[sprintf("valid1.%s", n)]],
	sprintf("data conversion (1): %s", n))


d <- gds_read_write(n, 2)
print(d)
checkEquals(d, valid.dta[[sprintf("valid2.%s", n)]],
	sprintf("data conversion (2): %s", n))

