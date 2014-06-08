#############################################################
#
# DESCRIPTION: test data dimension
#

library(RUnit)
library(gdsfmt)


#############################################################
#
# test function
#

test.apply.gdsn <- function()
{
	# data
	dat1 <- matrix(1:(10*6), nrow=10)
	dat2 <- array(1:(3*4*5), dim=c(3,4,5))


	###################################################################
	# a matrix
	#

	# cteate the GDS file "test.gds"
	f <- createfn.gds("tmp.gds")

	# add a data variable
	node <- add.gdsn(f, "matrix", val=dat1)


	####  apply functions over rows of matrix  ####

	tmp <- simplify2array(apply.gdsn(node, margin=1, FUN=function(x) x))
	checkEquals(tmp, t(dat1), "apply.gdsn, matrix test 1")

	tmp <- simplify2array(apply.gdsn(node, margin=1,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x))
	checkEquals(tmp, t(dat1[rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)]),
		"apply.gdsn, matrix test 2")


	####  apply functions over columns of matrix  ####

	tmp <- simplify2array(apply.gdsn(node, margin=2, FUN=function(x) x))
	checkEquals(tmp, dat1, "apply.gdsn, matrix test 3")

	tmp <- simplify2array(apply.gdsn(node, margin=2,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x))
	checkEquals(tmp, dat1[rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)],
		"apply.gdsn, matrix test 4")


	# close
	closefn.gds(f)



	###################################################################
	# a 3d array
	#

	# cteate the GDS file "test.gds"
	f <- createfn.gds("tmp.gds")

	# add a data variable
	node <- add.gdsn(f, "array", val=dat2)


	####  apply functions over dimensions  ####

	tmp <- simplify2array(apply.gdsn(node, margin=1, FUN=function(x) x))
	checkEquals(tmp, array(c(dat2[1,,], dat2[2,,], dat2[3,,]), dim=dim(tmp)),
		"apply.gdsn, 3d array test 1")

	tmp <- simplify2array(apply.gdsn(node, margin=2, FUN=function(x) x))
	checkEquals(tmp, array(c(dat2[,1,], dat2[,2,], dat2[,3,], dat2[,4,]), dim=dim(tmp)),
		"apply.gdsn, 3d array test 2")

	tmp <- simplify2array(apply.gdsn(node, margin=3, FUN=function(x) x))
	checkEquals(tmp, dat2, "apply.gdsn, 3d array test 3")


	####  apply functions over dimensions with selection  ####

	s1 <- c(TRUE, FALSE, TRUE)
	s2 <- c(FALSE, TRUE, FALSE, TRUE)
	s3 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)
	d <- dat2[s1,s2,s3]

	tmp <- simplify2array(apply.gdsn(node, margin=1, selection = list(s1, s2, s3),
		FUN=function(x) x))
	checkEquals(tmp, array(c(d[1,,], d[2,,]), dim=dim(tmp)),
		"apply.gdsn, 3d array test 4")

	tmp <- simplify2array(apply.gdsn(node, margin=2, selection = list(s1, s2, s3),
		FUN=function(x) x))
	checkEquals(tmp, array(c(d[,1,], d[,2,]), dim=dim(tmp)),
		"apply.gdsn, 3d array test 5")

	tmp <- simplify2array(apply.gdsn(node, margin=3, selection = list(s1, s2, s3),
		FUN=function(x) x))
	checkEquals(tmp, d, "apply.gdsn, 3d array test 6")


	# close
	closefn.gds(f)

	# delete the GDS file
	unlink("tmp.gds")
}
