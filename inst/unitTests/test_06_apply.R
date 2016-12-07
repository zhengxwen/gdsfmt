#############################################################
#
# DESCRIPTION: test data dimension
#

source(system.file("unitTests", "include.r", package="gdsfmt"))



#############################################################
#
# test functions
#

test.apply.gdsn <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.apply.gdsn <<<<\n")

	# data
	dat1 <- matrix(1:(10*6), nrow=10)
	dat2 <- array(1:(3*4*5), dim=c(3,4,5))


	###################################################################
	# a matrix
	#

	# cteate the GDS file "test.gds"
	f <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

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
	f <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

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
}



.test.clusterApply.gdsn <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink(c("test1.gds", "test2.gds"), force=TRUE)
	})

	###########################################################
	# prepare a GDS file

	# cteate a GDS file
	f <- createfn.gds("test1.gds", allow.duplicate=TRUE)

	(n <- add.gdsn(f, "matrix", val=matrix(1:(10*6), nrow=10)))
	read.gdsn(index.gdsn(f, "matrix"))

	closefn.gds(f)


	# cteate the GDS file "test2.gds"
	(f <- createfn.gds("test2.gds"))

	X <- matrix(1:50, nrow=10)
	Y <- matrix((1:50)/100, nrow=10)
	Z1 <- factor(c(rep(c("ABC", "DEF", "ETD"), 3), "TTT"))
	Z2 <- c(TRUE, FALSE, TRUE, FALSE, TRUE)

	node.X <- add.gdsn(f, "X", X)
	node.Y <- add.gdsn(f, "Y", Y)
	node.Z1 <- add.gdsn(f, "Z1", Z1)
	node.Z2 <- add.gdsn(f, "Z2", Z2)
	f

	closefn.gds(f)



	###########################################################
	# apply in parallel

	library(parallel)

	# Use option cl.core to choose an appropriate cluster size.
	cl <- makeCluster(getOption("cl.cores", 4))

	# open the GDS file
	f <- openfn.gds("test1.gds", allow.duplicate=TRUE)

	# Apply functions over rows or columns of matrix

	v1 <- clusterApply.gdsn(cl, "test1.gds", "matrix", margin=1,
		FUN=function(x) x)
	v2 <- apply.gdsn(index.gdsn(f, "matrix"), margin=1,
		FUN=function(x) x)
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [1]")

	v1 <- clusterApply.gdsn(cl, "test1.gds", "matrix", margin=2,
		FUN=function(x) x)
	v2 <- apply.gdsn(index.gdsn(f, "matrix"), margin=2,
		FUN=function(x) x)
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [2]")

	v1 <- clusterApply.gdsn(cl, "test1.gds", "matrix", margin=1,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x)
	v2 <- apply.gdsn(index.gdsn(f, "matrix"), margin=1,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x)
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [3]")

	v1 <- clusterApply.gdsn(cl, "test1.gds", "matrix", margin=2,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x)
	v2 <- apply.gdsn(index.gdsn(f, "matrix"), margin=2,
		selection = list(rep(c(TRUE, FALSE), 5), rep(c(TRUE, FALSE), 3)),
		FUN=function(x) x)
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [4]")

	closefn.gds(f)


	# Apply functions over rows or columns of multiple data sets

	# open the GDS file
	f <- openfn.gds("test2.gds", allow.duplicate=TRUE)

	v1 <- clusterApply.gdsn(cl, "test2.gds", c("X", "Y", "Z1"),
		margin=c(1, 1, 1), FUN=function(x) x)
	v2 <- apply.gdsn(
		list(index.gdsn(f, "X"), index.gdsn(f, "Y"), index.gdsn(f, "Z1")),
		margin=c(1, 1, 1), FUN=function(x) x)
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [5]")

	# with an index
	v1 <- clusterApply.gdsn(cl, "test2.gds", c(X="X", Y="Y", Z1="Z1"),
		margin=c(1, 1, 1), var.index="relative",
		FUN=function(i, x) list(index=i, value=x))
	v2 <- apply.gdsn(
		list(X=index.gdsn(f, "X"), Y=index.gdsn(f, "Y"), Z1=index.gdsn(f, "Z1")),
		margin=c(1, 1, 1), var.index="relative",
		FUN=function(i, x) list(index=i, value=x))
	checkEquals(v1, v2, "clusterApply.gdsn == apply.gdsn [6]")

	closefn.gds(f)

	# stop clusters
	stopCluster(cl)
}



test.apply.transpose <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.apply.transpose <<<<\n")

	valid.dta <- get(load(sprintf("%s/valid/standard.RData", base.path)))

	for (cp in c("", compress.list))
	{
		if (verbose)
			cat("Compression: ", cp, "\n", sep="")
		for (n in type.list)
		{
			if (verbose) cat(n, "\t", sep="")

			dta <- matrix(rep(valid.dta[[sprintf("valid1.%s", n)]], 60),
				nrow=400, ncol=300)

			# create a new gds file
			gfile <- createfn.gds("tmp.gds", allow.duplicate=TRUE)

			# append data
			node <- add.gdsn(gfile, "data", val=dta, storage=n,
				compress=cp, closezip=TRUE)
		
			closefn.gds(gfile)

			gfile <- openfn.gds("tmp.gds", readonly=FALSE, allow.duplicate=TRUE)
			node <- index.gdsn(gfile, "data")
			node.t <- add.gdsn(gfile, "data.t", valdim=c(300,0), storage=n,
				compress=cp)

			apply.gdsn(node, margin=1, FUN=c, as.is="gdsnode",
				target.node=node.t)
			readmode.gdsn(node.t)

			dta.t <- read.gdsn(node.t)

			checkEquals(dta, t(dta.t),
				sprintf("data matrix transpose (%s): %s", cp, n))

			# close the gds file
			closefn.gds(gfile)
		}

		if (verbose) cat("\n")
	}
}
