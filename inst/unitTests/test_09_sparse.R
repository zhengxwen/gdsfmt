#############################################################
#
# DESCRIPTION: test sparse array storage (1-D and 2-D)
#

source(system.file("unitTests", "include.r", package="gdsfmt"))



#############################################################
#
# test functions
#

test.sparse.array <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.sparse.array <<<<\n")

	f <- createfn.gds("tmp.gds")
	on.exit(closefn.gds(f), add=TRUE, after=FALSE)

	# ---- 1-D sparse integer (regression: used to return only the 1st nonzero) ----
	v1 <- integer(500L); v1[c(10L, 200L, 450L)] <- c(11L, 22L, 33L)
	add.gdsn(f, "sp1", v1, storage="sp.int32")
	# ---- larger 1-D sparse ----
	v2 <- integer(100000L); p2 <- seq(1L, 100000L, 100L); v2[p2] <- as.integer(p2)
	add.gdsn(f, "sp2", v2, storage="sp.int32")
	# ---- 1-D sparse double ----
	v3 <- numeric(20L); v3[c(3L, 7L, 15L)] <- c(1.5, 2.25, -4.0)
	add.gdsn(f, "sp3", v3, storage="sp.real64")
	# ---- 2-D sparse integer ----
	m <- matrix(0L, 4L, 5L); m[2,1] <- 7L; m[1,3] <- 3L; m[4,2] <- 9L; m[3,5] <- 2L
	add.gdsn(f, "spm", m, storage="sp.int32")
	sync.gds(f)

	r1 <- as.integer(as.matrix(read.gdsn(index.gdsn(f, "sp1"))))
	r2 <- as.integer(as.matrix(read.gdsn(index.gdsn(f, "sp2"))))
	r3 <- as.numeric(as.matrix(read.gdsn(index.gdsn(f, "sp3"))))
	rm <- as.matrix(read.gdsn(index.gdsn(f, "spm")))

	checkEquals(r1, v1, "1-D sparse int (3 nonzeros)")
	checkEquals(r2, v2, "1-D sparse int (1000 nonzeros)")
	checkEquals(r3, v3, "1-D sparse double")
	checkEquals(unname(rm), m, "2-D sparse int")
	# explicit nnz check for the regression
	checkEquals(sum(r1 != 0L), 3L, "1-D sparse must keep all nonzeros")
}


test.sparse.packedreal <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.sparse.packedreal <<<<\n")

	f <- createfn.gds("tmp.gds")
	on.exit(closefn.gds(f), add=TRUE, after=FALSE)

	# ---- values on the default scale, with NaN, negatives and a value that
	#      rounds to zero ----
	v <- numeric(300L)
	v[c(5L, 20L, 21L, 100L, 250L, 299L)] <- c(0.5, -1.27, NaN, 1.27, 0.001, -0.02)
	ex <- v; ex[250L] <- 0    # 0.001 rounds to the integer 0 on scale 0.01
	n1 <- add.gdsn(f, "sp8", v, storage="sp.real8")
	# unsigned: negatives cannot be represented, they read back as NaN
	vu <- abs(v); vu[299L] <- 2.55    # 255 is the missing value of 8 bits
	exu <- vu; exu[250L] <- 0; exu[299L] <- NaN
	n2 <- add.gdsn(f, "sp8u", vu, storage="sp.real8u")
	# 16 bits, default scale 0.0001
	v16 <- numeric(300L)
	v16[c(3L, 150L, 151L, 300L)] <- c(3.2767, -3.2767, NaN, 0.00004)
	ex16 <- v16; ex16[300L] <- 0
	n3 <- add.gdsn(f, "sp16", v16, storage="sp.real16")
	n4 <- add.gdsn(f, "sp16u", abs(v16), storage="sp.real16u")
	ex16u <- abs(ex16)

	# ---- a user-defined scale, checked through objdesp; the values are
	#      multiples of the scale so that they come back exactly ----
	m <- matrix(0, 6L, 5L)
	m[2L,1L] <- 32/127; m[1L,3L] <- -95/127; m[4L,2L] <- 1; m[3L,5L] <- NaN
	m[6L,5L] <- -1
	n5 <- add.gdsn(f, "spm", m, storage="sp.real8", scale=1/127)
	p5 <- objdesp.gdsn(n5)$param
	checkEquals(p5$offset, 0, "sp.real8 offset is zero")
	checkEquals(p5$scale, 1/127, "sp.real8 keeps the scale it was given")
	checkTrue(is.sparse.gdsn(n5), "sp.real8 is a sparse array")
	checkEquals(objdesp.gdsn(n1)$param$scale, 0.01, "sp.real8 default scale")
	checkEquals(objdesp.gdsn(n3)$param$scale, 0.0001, "sp.real16 default scale")

	# a nonzero offset is refused
	checkException(add.gdsn(f, "bad", storage="sp.real8", offset=1),
		"sp.real8 refuses a nonzero offset")
	# storage=node copies the scale
	n6 <- add.gdsn(f, "spm2", storage=n5)
	checkEquals(objdesp.gdsn(n6)$param$scale, 1/127, "storage=node copies the scale")
	append.gdsn(n6, n5)

	# ---- append across scales: the values are re-rounded on the new scale ----
	n7 <- add.gdsn(f, "sp8b", storage="sp.real8", scale=0.02)
	append.gdsn(n7, n1)
	ex7 <- ex; ex7[c(20L, 100L)] <- c(-1.28, 1.28)    # 63.5 rounds to 64
	# ---- append with the same scale, large enough for the raw-record path ----
	big <- numeric(200000L); pb <- seq(1L, 200000L, 7L)
	big[pb] <- round(sin(pb), 2)
	n8 <- add.gdsn(f, "big", big, storage="sp.real8")
	n9 <- add.gdsn(f, "big2", storage="sp.real8")
	append.gdsn(n9, n8); append.gdsn(n9, n8)
	sync.gds(f)

	# ---- read back, dense and sparse ----
	r1 <- read.gdsn(n1, .sparse=FALSE)
	checkEquals(r1, ex, "sp.real8 round trip")
	checkEquals(as.numeric(as.matrix(read.gdsn(n1, .sparse=TRUE))), ex,
		"sp.real8 as a sparse matrix")
	checkEquals(read.gdsn(n2, .sparse=FALSE), exu, "sp.real8u round trip")
	checkEquals(read.gdsn(n3, .sparse=FALSE), ex16, "sp.real16 round trip")
	checkEquals(read.gdsn(n4, .sparse=FALSE), ex16u, "sp.real16u round trip")
	checkEquals(read.gdsn(n5, .sparse=FALSE), m, "sp.real8 matrix, scale 1/127")
	checkEquals(as.matrix(read.gdsn(n5, .sparse=TRUE)), m, check.attributes=FALSE,
		"sp.real8 matrix as a sparse matrix")
	checkEquals(read.gdsn(n6, .sparse=FALSE), m, "append with the same scale")
	checkEquals(read.gdsn(n7, .sparse=FALSE), ex7, "append across scales")
	checkEquals(read.gdsn(n9, .sparse=FALSE), c(big, big), "large append")
	# random access and selection
	checkEquals(read.gdsn(n1, start=20L, count=2L, .sparse=FALSE), ex[20:21],
		"sp.real8 random access")
	sel <- rep(c(TRUE, FALSE), 150L)
	checkEquals(readex.gdsn(n1, sel, .sparse=FALSE), ex[sel], "sp.real8 selection")
	checkEquals(read.gdsn(n5, start=c(1L,3L), count=c(6L,2L), .sparse=FALSE), m[,3:4],
		"sp.real8 matrix block")

	# ---- reopen: the scale is stored in the file ----
	closefn.gds(f)
	f <- openfn.gds("tmp.gds")
	n5 <- index.gdsn(f, "spm")
	checkEquals(objdesp.gdsn(n5)$param$scale, 1/127, "scale survives reopening")
	checkEquals(read.gdsn(n5, .sparse=FALSE), m, "values survive reopening")
	checkEquals(read.gdsn(index.gdsn(f, "sp16"), .sparse=FALSE), ex16,
		"sp.real16 survives reopening")
	checkEquals(read.gdsn(index.gdsn(f, "big2"), start=100000L, count=10L,
		.sparse=FALSE),
		big[100000:100009], "large append random access after reopening")
}
