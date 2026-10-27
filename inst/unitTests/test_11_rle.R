#############################################################
#
# DESCRIPTION: test run-length encoded arrays ("rle.*" storage)
#

source(system.file("unitTests", "include.r", package="gdsfmt"))



#############################################################
#
# helpers
#

# the bytes of the data stream of a node
.rle_raw <- function(node, fn)
{
	d <- diagnosis.gds(node)$data
	con <- file(fn, "rb")
	on.exit(close(con))
	out <- raw(0L)
	for (i in seq_along(d$chunk_offset))
	{
		seek(con, d$chunk_offset[i])
		out <- c(out, readBin(con, "raw", d$chunk_size[i]))
	}
	out[seq_len(d$total_size)]
}

# the size of the data stream of a node
.rle_size <- function(node) diagnosis.gds(node)$data$total_size

# a mixture of runs, arithmetic progressions and random values
.rle_mix <- function(n, type)
{
	out <- NULL
	while (length(out) < n)
	{
		L <- sample(c(1:5, 31:33, 127:129, 300, 5000), 1L)
		k <- sample(4L, 1L)
		v <- switch(type,
			int = switch(k, rep(sample(-5:5, 1L), L),
				seq_len(L)*sample(-3:3, 1L) + sample(-100:100, 1L),
				sample(-1000:1000, L, replace=TRUE),
				sample(c(NA, 1L, 2L), L, replace=TRUE)),
			uint8 = switch(k, rep(sample(0:255, 1L), L),
				(seq_len(L) + sample(0:255, 1L)) %% 256L,
				sample(0:255, L, replace=TRUE), sample(0:3, L, replace=TRUE)),
			real = switch(k, rep(sample(c(0, -0, 1.5, NA, NaN, Inf), 1L), L),
				sample(c(0, 3.7, -5), 1L) + (0:(L-1))*sample(c(0.1, 0.25, 0.01), 1L),
				rnorm(L), round(rnorm(L), 1L)),
			string = switch(k, rep(sample(c("", "a", "été", strrep("x", 200)), 1L), L),
				sample(c("A", "B", "C"), L, replace=TRUE), as.character(sample(1e6, L)),
				rep("", L)))
		out <- c(out, v)
	}
	out[seq_len(n)]
}



#############################################################
#
# test functions
#

test.rle.format <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.format <<<<\n")

	f <- createfn.gds("tmp_rle.gds")
	a <- add.gdsn(f, "a", rep(5L, 32L), storage="rle.int32")       # REPEAT, L=32
	b <- add.gdsn(f, "b", rep(5L, 33L), storage="rle.int32")       # REPEAT, L=33
	s <- add.gdsn(f, "s", 1:4097, storage="rle.int32")             # STEP, L=4097
	l <- add.gdsn(f, "l", c(1L, 5L, 2L), storage="rle.int32")      # LITERAL, L=3
	u <- add.gdsn(f, "u", c(254L, 255L, 0L, 1L), storage="rle.uint8")  # STEP with wrap-around
	t1 <- add.gdsn(f, "t1", rep("a", 64L), storage="rle.string")   # REPEAT, L=64
	t2 <- add.gdsn(f, "t2", rep("a", 65L), storage="rle.string")   # REPEAT, L=65
	t3 <- add.gdsn(f, "t3", c("a", "bc"), storage="rle.string")    # LITERAL, L=2
	d <- add.gdsn(f, "d", 0.1 + (0:2)*0.3, storage="rle.float64")  # STEP of doubles
	sync.gds(f)

	fn <- "tmp_rle.gds"
	checkIdentical(.rle_raw(a, fn), as.raw(c(0x7D, 5, 0, 0, 0)), "REPEAT, 1-byte header")
	checkIdentical(.rle_raw(b, fn), as.raw(c(0x81, 0x01, 5, 0, 0, 0)), "REPEAT, 2-byte header")
	checkIdentical(.rle_raw(s, fn), as.raw(c(0x83, 0x80, 0x01, 1, 0, 0, 0, 1, 0, 0, 0)),
		"STEP, 3-byte header")
	checkIdentical(.rle_raw(l, fn), as.raw(c(0x04, 1, 0, 0, 0, 5, 0, 0, 0, 2, 0, 0, 0)),
		"LITERAL")
	checkIdentical(.rle_raw(u, fn), as.raw(c(0x0F, 0xFE, 0x01)), "uint8 STEP")
	checkIdentical(.rle_raw(t1, fn), as.raw(c(0x7F, 0x01, 0x61)), "string REPEAT, 1-byte header")
	checkIdentical(.rle_raw(t2, fn), as.raw(c(0x81, 0x01, 0x01, 0x61)),
		"string REPEAT, 2-byte header")
	checkIdentical(.rle_raw(t3, fn), as.raw(c(0x02, 0x01, 0x61, 0x02, 0x62, 0x63)),
		"string LITERAL")
	checkIdentical(.rle_raw(d, fn)[1L], as.raw(0x0B), "double STEP header")
	checkEquals(.rle_size(d), 17, "double STEP size")

	# header lengths at their limits
	for (L in c(32, 33, 4096, 4097, 524288, 524289))
	{
		n <- add.gdsn(f, "n", rep(7L, L), storage="rle.int32", replace=TRUE)
		sync.gds(f)
		checkEquals(.rle_size(n),
			4 + (if (L <= 32) 1 else if (L <= 4096) 2 else if (L <= 524288) 3 else 4),
			paste("int32 REPEAT header, L =", L))
	}
	for (L in c(64, 65, 8192, 8193))
	{
		n <- add.gdsn(f, "n", rep("z", L), storage="rle.string", replace=TRUE)
		sync.gds(f)
		checkEquals(.rle_size(n), 2 + (if (L <= 64) 1 else if (L <= 8192) 2 else 3),
			paste("string REPEAT header, L =", L))
	}
	# 128 values without any run fill one literal record, 129 need two
	v <- (0:128)^2 %% 251
	n <- add.gdsn(f, "n", v[1:128], storage="rle.uint8", replace=TRUE)
	sync.gds(f)
	checkEquals(.rle_size(n), 129, "one literal record")
	n <- add.gdsn(f, "n", v, storage="rle.uint8", replace=TRUE)
	sync.gds(f)
	checkEquals(.rle_size(n), 131, "two literal records")
	checkIdentical(read.gdsn(n), as.integer(v), "two literal records, values")

	closefn.gds(f)
}


test.rle.roundtrip <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.roundtrip <<<<\n")

	set.seed(100)
	st <- c(int="rle.int32", uint8="rle.uint8", real="rle.float64",
		string="rle.string")
	for (cp in c("", "LZ4_RA"))
	{
		f <- createfn.gds("tmp_rle.gds")
		val <- list()
		for (tp in names(st))
		{
			x <- .rle_mix(80000L, tp)
			val[[tp]] <- x
			add.gdsn(f, tp, x, storage=st[[tp]], compress=cp, closezip=TRUE)
			# the same values appended in chunks
			n <- add.gdsn(f, paste0(tp, "2"), storage=st[[tp]], compress=cp)
			i <- 0L
			while (i < length(x))
			{
				m <- min(length(x) - i, sample(c(1L, 3L, 1000L, 20000L), 1L))
				append.gdsn(n, x[i + seq_len(m)])
				i <- i + m
			}
			readmode.gdsn(n)
		}
		closefn.gds(f)

		f <- openfn.gds("tmp_rle.gds")
		for (tp in names(st))
		{
			x <- val[[tp]]
			for (nm in c(tp, paste0(tp, "2")))
			{
				n <- index.gdsn(f, nm)
				checkIdentical(read.gdsn(n), x, paste(cp, nm, "read"))
				# probe backwards, so that the checkpoints are used
				for (k in sort(sample(length(x), 25L), decreasing=TRUE))
				{
					cnt <- min(length(x) - k + 1L, sample(c(1L, 7L, 300L), 1L))
					checkIdentical(read.gdsn(n, start=k, count=cnt),
						x[k - 1L + seq_len(cnt)], paste(cp, nm, "random access"))
				}
				sel <- runif(length(x)) < 0.05
				checkIdentical(readex.gdsn(n, sel), x[sel], paste(cp, nm, "selection"))
			}
		}
		closefn.gds(f)
	}
}


test.rle.real <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.real <<<<\n")

	f <- createfn.gds("tmp_rle.gds")

	# each sequence is a single STEP record: a header and two doubles
	sq <- list(seq(0, 1, by=0.01), seq(1, 2, by=0.1), seq(3.7, 100, by=0.3),
		seq(-5, 5, by=0.25), seq(1000.1, 2000, by=0.01), 0.1 + (0:9999)*0.3)
	for (x in sq)
	{
		n <- add.gdsn(f, "x", x, storage="rle.float64", replace=TRUE)
		sync.gds(f)
		checkIdentical(read.gdsn(n), x, "sequence")
		# v0 + i*d is rounded twice, in the product and in the sum: a fused
		# multiply-add would not reproduce 0.1 + (0:9999)*0.3
		checkTrue(.rle_size(n) <= 19, "sequence in one record")
	}

	# NA, NaN, -0 and 0 keep their bits
	x <- c(NA, NA, NaN, NaN, -0, -0, 0, 0, Inf, -Inf, NA, NaN, -0, 0, 1.5)
	n <- add.gdsn(f, "x", x, storage="rle.float64", replace=TRUE)
	checkIdentical(read.gdsn(n), x, "special values")
	checkTrue(identical(read.gdsn(n), x, num.eq=FALSE, single.NA=FALSE),
		"special values, bitwise")

	# 32-bit reals read like "float32"
	x <- c(seq(-5, 5, by=0.25), rnorm(100), rep(0.1, 50))
	n1 <- add.gdsn(f, "a", x, storage="rle.float32", replace=TRUE)
	n2 <- add.gdsn(f, "b", x, storage="float32", replace=TRUE)
	checkIdentical(read.gdsn(n1), read.gdsn(n2), "rle.float32")

	closefn.gds(f)
}


test.rle.append <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.append <<<<\n")

	set.seed(200)
	fn <- "tmp_rle.gds"
	for (tp in c("rle.int32", "rle.string"))
	{
		x <- c(rep(3L, 70000L), 1:70000, sample(1000L, 5000L, TRUE), 7:1)
		if (tp == "rle.string") x <- as.character(x %/% 7L)

		f <- createfn.gds(fn)
		a <- add.gdsn(f, "a", x, storage=tp)
		# appended in chunks, synchronized in between: the last record is
		# continued, so that the bytes are those of a single write
		b <- add.gdsn(f, "b", storage=tp)
		i <- 0L
		while (i < length(x))
		{
			m <- min(length(x) - i, sample(c(1L, 5L, 777L, 33333L), 1L))
			append.gdsn(b, x[i + seq_len(m)])
			i <- i + m
			if (runif(1) < 0.3) sync.gds(f)
			# a read in between does not cut the run short
			if (runif(1) < 0.3)
				checkIdentical(read.gdsn(b, start=i, count=1L), x[i], "read the last value")
		}
		# from another node: its records are copied, and the values it still
		# holds in memory follow
		src <- add.gdsn(f, "src", x[1:100000], storage=tp)
		append.gdsn(src, x[100001:length(x)])
		d <- add.gdsn(f, "d", storage=tp)
		append.gdsn(d, src)
		e <- add.gdsn(f, "e", x[1:10], storage=tp)
		append.gdsn(e, a)
		sync.gds(f)

		checkIdentical(read.gdsn(b), x, paste(tp, "chunks"))
		checkIdentical(.rle_raw(a, fn), .rle_raw(b, fn), paste(tp, "chunks, bytes"))
		checkIdentical(read.gdsn(d), x, paste(tp, "append a node"))
		checkIdentical(.rle_raw(a, fn), .rle_raw(d, fn), paste(tp, "append a node, bytes"))
		checkIdentical(read.gdsn(e), c(x[1:10], x), paste(tp, "append a node after data"))
		closefn.gds(f)

		# reopen and append
		f <- openfn.gds(fn, readonly=FALSE)
		a <- index.gdsn(f, "a"); b <- index.gdsn(f, "b")
		append.gdsn(a, rev(x[1:10])); append.gdsn(b, rev(x[1:10]))
		closefn.gds(f)
		f <- openfn.gds(fn)
		a <- index.gdsn(f, "a"); b <- index.gdsn(f, "b")
		checkIdentical(read.gdsn(b), c(x, rev(x[1:10])), paste(tp, "append after reopening"))
		checkIdentical(.rle_raw(a, fn), .rle_raw(b, fn), paste(tp, "append after reopening, bytes"))
		closefn.gds(f)
	}
}


test.rle.compress <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.compress <<<<\n")

	set.seed(300)
	x <- c(rep(1L, 100000L), 1:100000, sample(100L, 100000L, TRUE))
	f <- createfn.gds("tmp_rle.gds")
	for (cp in c("ZIP", "ZIP_RA", "LZ4", "LZ4_RA", "LZMA", "LZMA_RA"))
	{
		n <- add.gdsn(f, "x", x, storage="rle.int32", compress=cp, closezip=TRUE,
			replace=TRUE)
		checkIdentical(read.gdsn(n), x, cp)
		k <- 250000L
		checkIdentical(read.gdsn(n, start=k, count=100L), x[k - 1L + 1:100], cp)
	}

	# switching the compression
	n <- add.gdsn(f, "x", x, storage="rle.int32", replace=TRUE)
	for (cp in c("ZIP_RA", "", "LZ4_RA", "", "LZMA"))
	{
		compression.gdsn(n, cp)
		checkIdentical(read.gdsn(n), x, paste("compression.gdsn", cp))
	}
	# a compressed node in read mode takes no more data ...
	checkException(append.gdsn(n, 1:3), "append in read mode", silent=TRUE)
	# ... until the compression is removed
	compression.gdsn(n, "")
	append.gdsn(n, 1:3)
	checkIdentical(read.gdsn(n), c(x, 1:3), "append after decompressing")
	closefn.gds(f)
}


test.rle.dim <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink("tmp_rle.gds", force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.dim <<<<\n")

	f <- createfn.gds("tmp_rle.gds")

	# matrix
	m <- matrix(rep(1:50, each=40L), 40L, 50L)
	n <- add.gdsn(f, "m", m, storage="rle.int32")
	checkIdentical(read.gdsn(n), m, "matrix")
	checkIdentical(read.gdsn(n, start=c(3, 5), count=c(10, 20)), m[3:12, 5:24],
		"sub-matrix")
	sel <- list(1:40 %% 3L == 0L, 1:50 %% 2L == 0L)
	checkIdentical(readex.gdsn(n, sel), m[sel[[1L]], sel[[2L]]], "selection")
	checkIdentical(apply.gdsn(n, 2L, sum, as.is="integer"),
		as.integer(colSums(m)), "apply.gdsn")

	# reshaping keeps the order of the elements
	setdim.gdsn(n, c(20L, 100L))
	checkIdentical(read.gdsn(n), matrix(m, 20L, 100L), "reshape")
	setdim.gdsn(n, c(40L, 50L))
	# resizing an inner dimension would move the data, only the last one can
	checkException(setdim.gdsn(n, c(20L, 50L), permute=TRUE), "inner dimension",
		silent=TRUE)
	setdim.gdsn(n, c(40L, 60L))
	checkIdentical(read.gdsn(n), cbind(m, matrix(0L, 40L, 10L)), "grow")
	setdim.gdsn(n, c(40L, 25L))
	checkIdentical(read.gdsn(n), m[, 1:25], "shrink")

	# no writing in place
	checkException(write.gdsn(n, 1L, start=c(1, 1), count=c(1, 1)), "write in place",
		silent=TRUE)

	# grow and shrink a vector
	x <- c(rep(3L, 100000L), 1:70000, 7:1)
	n <- add.gdsn(f, "x", x, storage="rle.int32")
	setdim.gdsn(n, length(x) + 1000L)
	checkIdentical(read.gdsn(n), c(x, integer(1000L)), "grow a vector")
	for (k in c(170010L, 170007L, 170000L, 100050L, 100000L, 5L, 0L))
	{
		setdim.gdsn(n, k)
		checkIdentical(read.gdsn(n), c(x, integer(1000L))[seq_len(k)],
			paste("shrink to", k))
	}
	append.gdsn(n, x)
	closefn.gds(f)

	f <- openfn.gds("tmp_rle.gds")
	checkIdentical(read.gdsn(index.gdsn(f, "x")), x, "shrink, then append")
	closefn.gds(f)
}


test.rle.file <- function()
{
	on.exit({
		showfile.gds(closeall=TRUE, verbose=FALSE)
		unlink(c("tmp_rle.gds", "tmp_rle2.gds"), force=TRUE)
	})

	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n\n>>>> test.rle.file <<<<\n")

	set.seed(400)
	x <- c(rep(1L, 200000L), 1:100000, sample(5L, 100000L, TRUE))
	s <- rep(c("ab", "cd", "ab", "中文"), each=50000L)

	f <- createfn.gds("tmp_rle.gds")
	add.gdsn(f, "x", x, storage="rle.int32")
	add.gdsn(f, "gone", x, storage="rle.int32")
	add.gdsn(f, "s", s, storage="rle.string")
	add.gdsn(f, "plain", x, storage="int32")
	add.gdsn(f, "fac", factor(c("u", "v", "u", "u")), storage="rle.int32")
	add.gdsn(f, "raw", as.raw(c(1, 1, 1, 2, 255)), storage="rle.uint8")
	add.gdsn(f, "i64", c(rep(3L, 10L), 1:10), storage="rle.int64")
	sync.gds(f)

	n <- index.gdsn(f, "x")
	d <- objdesp.gdsn(n)
	checkIdentical(d$storage, "dRLEInt32", "objdesp storage")
	checkIdentical(d$trait, "RLEInt32", "objdesp trait")
	checkTrue(!is.null(diagnosis.gds(n)$index), "the checkpoint table is reported")
	checkIdentical(read.gdsn(index.gdsn(f, "fac")), factor(c("u", "v", "u", "u")),
		"factor")
	checkIdentical(read.gdsn(index.gdsn(f, "raw"), .useraw=TRUE),
		as.raw(c(1, 1, 1, 2, 255)), "raw")
	checkIdentical(read.gdsn(index.gdsn(f, "i64")), c(rep(3L, 10L), 1:10), "int64")
	checkIdentical(read.gdsn(index.gdsn(f, "s")), s, "UTF-8 strings")
	if (requireNamespace("digest", quietly=TRUE))
	{
		checkIdentical(digest.gdsn(n, action="Robject"),
			digest.gdsn(index.gdsn(f, "plain"), action="Robject"), "digest")
	}

	delete.gdsn(index.gdsn(f, "gone"))
	copyto.gdsn(f, n, "copy")
	checkIdentical(read.gdsn(index.gdsn(f, "copy")), x, "copyto.gdsn")
	closefn.gds(f)

	cleanup.gds("tmp_rle.gds", verbose=FALSE)
	f <- openfn.gds("tmp_rle.gds")
	n <- index.gdsn(f, "x")
	for (k in c(399990L, 300001L, 250000L, 1L))
	{
		checkIdentical(read.gdsn(n, start=k, count=10L), x[k - 1L + 1:10],
			"random access after cleanup.gds")
	}
	checkIdentical(read.gdsn(index.gdsn(f, "s")), s, "strings after cleanup.gds")
	checkIdentical(read.gdsn(index.gdsn(f, "copy")), x, "copy after cleanup.gds")
	closefn.gds(f)
}
