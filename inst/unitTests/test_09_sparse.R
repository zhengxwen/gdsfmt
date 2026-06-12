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
