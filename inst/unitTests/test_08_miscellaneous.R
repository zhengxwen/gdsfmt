#############################################################
#
# DESCRIPTION: miscellaneous
#

library(RUnit)
library(digest)
library(tools)
library(gdsfmt)


#############################################################
#
# test functions
#

test.digest <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.digest <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{	 
		len <- sample.int(100000, 1)
		val <- as.raw(sample.int(256, len, replace=TRUE) - 1L)
		writeBin(val, con="test.bin")

		# cteate a GDS file
		f <- createfn.gds("test.gds")
		add.gdsn(f, "raw", val)
		closefn.gds(f)

		f <- openfn.gds("test.gds")
		hash1 <- unname(digest.gdsn(index.gdsn(f, "raw")))
		closefn.gds(f)

		# check with other program
		hash2 <- unname(md5sum("test.bin"))

		checkEquals(hash1, hash2, paste("md5 digest", i))
	}

	# delete the temporary files
	unlink(c("test.gds", "test.bin"), force=TRUE)
}


test.digest.factor <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.digest.factor <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{	 
		f <- createfn.gds("test.gds")

		v <- sample.int(26, 10000, TRUE)
		v[sample.int(length(v), 10)] <- NA
		vv <- factor(v, labels=letters)

		n1 <- add.gdsn(f, "i1", vv)
		n2 <- add.gdsn(f, "i2", as.character(vv), check=FALSE)

		checkEquals(digest.gdsn(n1, action="Robject"),
			digest.gdsn(n2, action="Robject"), "md5 R object digest")

		closefn.gds(f)
	}

	# delete the temporary file
	unlink("test.gds", force=TRUE)
}


test.zip_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.zip_block.concatenate <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{
		val <- sample.int(2^28, 500000, replace=TRUE)
		val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="ZIP_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="ZIP_RA:16K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}


test.zip_block_real16.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.zip_block_real16.concatenate <<<<\n")

	set.seed(1000)

	for (i in 1:10)
	{
		val <- round(runif(500000), 4)
		val2 <- round(runif(sample.int(256, 1)), 4)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "float", val, storage="packedreal16",
			compress="ZIP_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "float2", storage="packedreal16", compress="ZIP_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "float3", storage="packedreal16", compress="ZIP_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "float4", storage="packedreal16", compress="ZIP_RA:16K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}


test.lz4_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.lz4_block.concatenate <<<<\n")

	set.seed(100)

	for (i in 1:10)
	{
		val <- sample.int(2^28, 500000, replace=TRUE)
		val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="LZ4_RA:16K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="LZ4_RA:16K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}


test.lzma_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.lzma_block.concatenate <<<<\n")

	set.seed(100)

	for (i in 1:10)
	{
		val <- sample.int(2^28, 500000, replace=TRUE)
		val2 <- sample.int(2^28, sample.int(256, 1), replace=TRUE)

		# cteate a GDS file
		f <- createfn.gds("test.gds")

		n1 <- add.gdsn(f, "int", val, compress="LZMA_RA:32K", closezip=TRUE)

		n2 <- add.gdsn(f, "int2", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkEquals(val, read.gdsn(n2), "concatenating compressed block (1)")

		n3 <- add.gdsn(f, "int3", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n3, val2)
		append.gdsn(n3, n1)
		readmode.gdsn(n3)
		checkEquals(c(val2, val), read.gdsn(n3), "concatenating compressed block (2)")

		n4 <- add.gdsn(f, "int4", storage="int", compress="LZMA_RA:32K")
		append.gdsn(n4, n1)
		append.gdsn(n4, val2)
		append.gdsn(n4, n1)
		readmode.gdsn(n4)
		checkEquals(c(val, val2, val), read.gdsn(n4), "concatenating compressed block (3)")

		closefn.gds(f)
	}
}


test.string_block.concatenate <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.string_block.concatenate <<<<\n")

	on.exit({
		try(closefn.gds(f), silent=TRUE)
		try(closefn.gds(f2), silent=TRUE)
		unlink(c("test.gds", "test2.gds"), force=TRUE)
	})
	set.seed(1000)

	newval <- function(n)
	{
		sprintf("gene%05d|%s", sample.int(20000L, n, replace=TRUE),
			strrep("éx", sample.int(8L, n, replace=TRUE) - 1L))
	}
	sz <- function(node) objdesp.gdsn(node)$size

	# read all, then go backwards so that the offset index is consulted
	check <- function(node, val, msg)
	{
		checkIdentical(val, read.gdsn(node), msg)
		n <- length(val)
		i <- c(n - 2L, 131073L, 65537L, 65535L, sample.int(n - 2L, 5L), 1L)
		for (i in sort(unique(i[i <= n - 2L]), decreasing=TRUE))
		{
			checkIdentical(val[i + 0:2], read.gdsn(node, start=i, count=3L),
				paste(msg, "at", i))
		}
		sel <- rep(c(FALSE, TRUE, FALSE), length.out=n)
		checkIdentical(val[sel], readex.gdsn(node, sel), paste(msg, "readex"))
	}

	types <- c("string", "string16", "string32",
		"cstring", "cstring16", "cstring32")
	compress <- list(c("ZIP_RA:16K", "ZIP_RA:16K"), c("LZ4_RA:16K", "LZ4_RA:16K"),
		c("ZIP_RA:16K", "ZIP_RA:64K"), c("ZIP_RA:16K", ""),
		c("", "LZ4_RA:16K"), c("", ""), c("ZIP", "ZIP"))

	f <- createfn.gds("test.gds")
	f2 <- createfn.gds("test2.gds")
	ans <- list()
	for (st in types)
	{
		for (cp in compress)
		{
			nm <- paste(st, cp[1L], cp[2L])
			val <- newval(70000L)
			v1 <- newval(1000L); v2 <- newval(777L)
			src <- add.gdsn(f, paste(nm, "src"), val, storage=st,
				compress=cp[1L], closezip=TRUE)

			# the target holds data already, and is read before appending
			n1 <- add.gdsn(f, nm, v1, storage=st, compress=cp[2L])
			if (cp[2L] == "")
				checkIdentical(v1[2:4], read.gdsn(n1, start=2L, count=3L), nm)
			append.gdsn(n1, src)
			append.gdsn(n1, v2)
			append.gdsn(n1, src)
			readmode.gdsn(n1)
			ans[[nm]] <- c(v1, val, v2, val)
			check(n1, ans[[nm]], paste("append.gdsn()", nm))

			# to another file
			copyto.gdsn(f2, src, nm)
			check(index.gdsn(f2, nm), val, paste("copyto.gdsn()", nm))
			check(src, val, paste("source", nm))
		}

		# whole compressed blocks are carried over: a node compressed by
		# "ZIP_RA.fast" keeps its size in a "ZIP_RA.max" node, unless it is
		# small or allow.block=FALSE
		for (n in c(70000L, 30000L))
		{
			val <- newval(n)
			n1 <- add.gdsn(f, paste(st, n, "fast"), val, storage=st,
				compress="ZIP_RA.fast:16K", closezip=TRUE)
			n2 <- add.gdsn(f, paste(st, n, "max"), val, storage=st,
				compress="ZIP_RA.max:16K", closezip=TRUE)
			for (flag in c(TRUE, FALSE))
			{
				n3 <- add.gdsn(f, paste(st, n, flag), storage=st,
					compress="ZIP_RA.max:16K")
				append.gdsn(n3, n1, allow.block=flag)
				readmode.gdsn(n3)
				s <- paste0("append.gdsn(, allow.block=", flag, ") ", st, " ", n)
				checkIdentical(val, read.gdsn(n3), s)
				checkEquals(sz(if (flag && n >= 65536L) n1 else n2), sz(n3),
					paste(s, "size"))
			}
		}

		# a source still being written
		val <- newval(70000L)
		n1 <- add.gdsn(f, paste(st, "writing"), storage=st)
		append.gdsn(n1, val)
		n2 <- add.gdsn(f, paste(st, "from writing"), storage=st,
			compress="ZIP_RA:16K")
		append.gdsn(n2, n1)
		readmode.gdsn(n2)
		checkIdentical(val, read.gdsn(n2), paste(st, "source being written"))
		append.gdsn(n1, val[1:5])
		checkIdentical(c(val, val[1:5]), read.gdsn(n1), paste(st, "source being written"))
	}
	closefn.gds(f)
	closefn.gds(f2)

	# reopen
	f <- openfn.gds("test.gds")
	f2 <- openfn.gds("test2.gds")
	for (nm in names(ans))
		check(index.gdsn(f, nm), ans[[nm]], paste("reopen", nm))
	for (nm in names(ans))
		check(index.gdsn(f2, nm), read.gdsn(index.gdsn(f, paste(nm, "src"))),
			paste("reopen copyto.gdsn()", nm))
	closefn.gds(f)
	closefn.gds(f2)
}


test.block_append.position <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.block_append.position <<<<\n")

	on.exit({
		try(closefn.gds(f), silent=TRUE)
		unlink("test.gds", force=TRUE)
	})
	set.seed(1000)

	newval <- function(st, n)
	{
		switch(st,
			float64 = runif(n),
			bit2 = sample(0:3, n, replace=TRUE),
			packedreal16 = round(runif(n), 3),
			vl_int = sample(-1e6:1e6, n, replace=TRUE),
			sample.int(1e6, n, replace=TRUE))
	}

	f <- createfn.gds("test.gds")
	ans <- list()
	for (st in c("int32", "float64", "bit2", "packedreal16", "vl_int", "vl_uint"))
	{
		n1 <- add.gdsn(f, paste(st, "src"), newval(st, 70003L), storage=st)
		val <- read.gdsn(n1)

		# the target is read before a node is appended to it
		n2 <- add.gdsn(f, st, newval(st, 100001L), storage=st)
		val0 <- read.gdsn(n2)
		read.gdsn(n2, start=5L, count=10L)
		append.gdsn(n2, n1)
		append.gdsn(n2, val0[1:7])
		ans[[st]] <- c(val0, val, val0[1:7])
		checkIdentical(ans[[st]], read.gdsn(n2), paste(st, "read before appending"))
	}
	closefn.gds(f)

	f <- openfn.gds("test.gds")
	for (nm in names(ans))
		checkIdentical(ans[[nm]], read.gdsn(index.gdsn(f, nm)), paste("reopen", nm))
	closefn.gds(f)
}


test.self_reference <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.self_reference <<<<\n")

	on.exit({
		try(closefn.gds(f), silent=TRUE)
		unlink("test.gds", force=TRUE)
	})
	set.seed(1000)

	newval <- function(st, n)
	{
		switch(st,
			float64 = runif(n),
			bit2 = sample(0:3, n, replace=TRUE),
			string = as.character(sample.int(1e6, n, replace=TRUE)),
			sp.int32 = { v <- integer(n); v[seq(1L, n, by=7L)] <- 1L; v },
			sample.int(1e6, n, replace=TRUE))
	}

	f <- createfn.gds("test.gds")
	ans <- list()
	for (st in c("int32", "float64", "bit2", "vl_int", "string", "sp.int32"))
	{
		for (n in c(70003L, 10L))
		{
			nm <- paste(st, n)
			# append a node to itself
			n1 <- add.gdsn(f, paste(nm, "append"), newval(st, n), storage=st)
			val <- read.gdsn(n1, .sparse=FALSE)
			append.gdsn(n1, n1)
			ans[[paste(nm, "append")]] <- c(val, val)
			checkIdentical(c(val, val), read.gdsn(n1, .sparse=FALSE),
				paste("append.gdsn(x, x)", nm))

			# assign a node to itself
			n2 <- add.gdsn(f, paste(nm, "assign"), val, storage=st)
			assign.gdsn(n2, n2)
			ans[[paste(nm, "assign")]] <- val
			checkIdentical(val, read.gdsn(n2, .sparse=FALSE),
				paste("assign.gdsn(x, x)", nm))
		}
	}

	# copy a folder into itself or into one of its subfolders
	d <- addfolder.gdsn(f, "dir")
	add.gdsn(d, "x", 1:10)
	s <- addfolder.gdsn(d, "sub")
	checkException(copyto.gdsn(d, d, "copy"), silent=TRUE)
	checkException(copyto.gdsn(s, d, "copy"), silent=TRUE)
	checkEquals(c("x", "sub"), ls.gdsn(d), "copyto.gdsn() into itself")
	checkEquals(character(), ls.gdsn(s), "copyto.gdsn() into itself")
	closefn.gds(f)

	f <- openfn.gds("test.gds")
	for (nm in names(ans))
	{
		checkIdentical(ans[[nm]], read.gdsn(index.gdsn(f, nm), .sparse=FALSE),
			paste("reopen", nm))
	}
	closefn.gds(f)
}


test.moveto.into <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.moveto.into <<<<\n")

	on.exit(unlink("test.gds", force=TRUE))

	# create a GDS file with a node and a sub-folder
	f <- createfn.gds("test.gds")
	add.gdsn(f, "x", 1:10)
	addfolder.gdsn(f, "sub")
	addfolder.gdsn(f, "other")

	# successful cross-folder move
	moveto.gdsn(index.gdsn(f, "x"), index.gdsn(f, "sub"), relpos="into")
	checkTrue(!is.null(index.gdsn(f, "sub/x", silent=TRUE)),
		"moveto.gdsn(into): node should be at new location")
	checkTrue(is.null(index.gdsn(f, "x", silent=TRUE)),
		"moveto.gdsn(into): node should not be at old location")
	checkEquals(1:10, read.gdsn(index.gdsn(f, "sub/x")),
		"moveto.gdsn(into): data preserved")

	# loc.node must be a folder
	add.gdsn(f, "y", 11:20)
	checkException(
		moveto.gdsn(index.gdsn(f, "y"), index.gdsn(f, "sub/x"), relpos="into"),
		"moveto.gdsn(into): loc.node must be a folder",
		silent=TRUE)

	# duplicate name in destination should error (engine throws)
	add.gdsn(f, "x", 100:110)  # /x exists again
	checkException(
		moveto.gdsn(index.gdsn(f, "x"), index.gdsn(f, "sub"), relpos="into"),
		"moveto.gdsn(into): duplicate name should error",
		silent=TRUE)

	# moving folder into its own descendant should error
	addfolder.gdsn(index.gdsn(f, "sub"), "deep")
	checkException(
		moveto.gdsn(index.gdsn(f, "sub"), index.gdsn(f, "sub/deep"), relpos="into"),
		"moveto.gdsn(into): cannot move into descendant",
		silent=TRUE)

	# no-op when already a direct child of destination
	moveto.gdsn(index.gdsn(f, "sub/x"), index.gdsn(f, "sub"), relpos="into")
	checkTrue(!is.null(index.gdsn(f, "sub/x", silent=TRUE)),
		"moveto.gdsn(into): no-op when already in destination")

	closefn.gds(f)
}
