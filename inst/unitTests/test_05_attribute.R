#############################################################
#
# DESCRIPTION: test data dimension
#

library(RUnit)
library(gdsfmt)


#############################################################
#
# test functions
#

test.data.attribute <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	# create a new gds file
	gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

	# add a NULL node
	node <- add.gdsn(gfile, "data", val=NULL)

	# attribute list
	put.attr.gdsn(node, "a")
	put.attr.gdsn(node, "x", "string")
	put.attr.gdsn(node, "y", 1L)
	put.attr.gdsn(node, "z", 1.5)

	# get its attributes
	# check
	val <- list(a=NULL, x="string", y=as.integer(1), z=as.double(1.5))

	checkEquals(get.attr.gdsn(node), val, "data.attribute")

	# close the gds file
	closefn.gds(gfile)
}
