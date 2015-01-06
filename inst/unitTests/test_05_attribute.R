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
	# create a new gds file
	gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

	# add a NULL node
	node <- add.gdsn(gfile, "data", val=NULL)

	# attribute list
	put.attr.gdsn(node, "a")
	put.attr.gdsn(node, "x", "string")
	put.attr.gdsn(node, "y", as.integer(1))
	put.attr.gdsn(node, "z", as.double(1.5))

	# get its attributes
	# check
	val <- list(a=NULL, x="string", y=as.integer(1), z=as.double(1.5))

	checkEquals(get.attr.gdsn(node), val, "data.attribute")


	# close the gds file
	closefn.gds(gfile)
	unlink("tmp.gds")
}
