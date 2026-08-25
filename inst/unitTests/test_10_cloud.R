#############################################################
#
# DESCRIPTION: the registry of cloud URL scheme handlers
#

library(RUnit)
library(gdsfmt)


#############################################################
#
# test functions
#

test.cloud.register <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.cloud.register <<<<\n")

	on.exit(gdsUnregisterCloudHandler(c("unittestfs", "unittestfs2")))
	gdsUnregisterCloudHandler(c("unittestfs", "unittestfs2"))

	h <- function(filename, ...) filename

	# register, and get NULL back since nothing was registered before
	checkTrue(is.null(gdsRegisterCloudHandler("unittestfs", h, "gdsfmt")))

	# it shows up in the listing
	d <- gdsCloudHandlers()
	checkTrue(is.data.frame(d))
	checkEquals(c("scheme", "package"), names(d))
	checkTrue("unittestfs" %in% d$scheme)
	checkEquals("gdsfmt", d$package[match("unittestfs", d$scheme)])

	# the scheme is normalized to lower case
	gdsRegisterCloudHandler("UnitTestFS2", h)
	d <- gdsCloudHandlers()
	checkTrue("unittestfs2" %in% d$scheme)
	checkTrue(is.na(d$package[match("unittestfs2", d$scheme)]))

	# re-registering replaces, and returns the previous handler
	h2 <- function(filename, ...) "second"
	old <- gdsRegisterCloudHandler("unittestfs", h2, "gdsfmt")
	checkTrue(identical(old, h))
	checkEquals(1L, sum(gdsCloudHandlers()$scheme == "unittestfs"))
}


test.cloud.unregister <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.cloud.unregister <<<<\n")

	on.exit(gdsUnregisterCloudHandler("unittestfs"))

	gdsRegisterCloudHandler("unittestfs", function(filename, ...) filename)
	checkEquals(c(unittestfs=TRUE), gdsUnregisterCloudHandler("unittestfs"))
	checkTrue(!("unittestfs" %in% gdsCloudHandlers()$scheme))

	# unregistering an unknown scheme is not an error
	checkEquals(c(nosuchfs=FALSE), gdsUnregisterCloudHandler("nosuchfs"))
}


test.cloud.dispatch <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.cloud.dispatch <<<<\n")

	on.exit(gdsUnregisterCloudHandler("unittestfs"))

	# an unregistered scheme is an error
	checkException(openfn.gds("unittestfs://bucket/f.gds"), silent=TRUE)

	# openfn.gds() forwards the whole URL to the handler
	gdsRegisterCloudHandler("unittestfs",
		function(filename, ...) list(url=filename, args=list(...)), "gdsfmt")
	ans <- openfn.gds("unittestfs://bucket/f.gds")
	checkEquals("unittestfs://bucket/f.gds", ans$url)
	checkTrue("allow.error" %in% names(ans$args))

	# cloud URLs are read-only
	checkException(openfn.gds("unittestfs://bucket/f.gds", readonly=FALSE),
		silent=TRUE)
}


test.cloud.invalid <- function()
{
	verbose <- options("test.verbose")$test.verbose
	if (verbose) cat("\n>>>> test.cloud.invalid <<<<\n")

	# 'scheme' should be a single valid scheme name without "://"
	checkException(gdsRegisterCloudHandler("s3://",
		function(filename, ...) NULL), silent=TRUE)
	checkException(gdsRegisterCloudHandler(c("a", "b"),
		function(filename, ...) NULL), silent=TRUE)

	# 'handler' should be a function with a '...' argument
	checkException(gdsRegisterCloudHandler("unittestfs", "not a function"),
		silent=TRUE)
	checkException(gdsRegisterCloudHandler("unittestfs",
		function(filename) NULL), silent=TRUE)
}
