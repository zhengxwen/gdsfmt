# ===========================================================================
#
# cloud.r: registry of cloud URL scheme handlers
#
# Copyright (C) 2026    Xiuwen Zheng
#
# This is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License Version 3 as
# published by the Free Software Foundation.
#
# gdsfmt is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with gdsfmt.
# If not, see <http://www.gnu.org/licenses/>.

##############################################################################
#
# gdsfmt itself has no knowledge of remote storage. Instead, a companion
# package (e.g. gdscloud) registers a handler function for each URL scheme
# it supports, and `openfn.gds()` dispatches to that handler when the file
# name looks like a URL. The functions below are the public interface to
# that registry, so a companion package does not need to reach into the
# gdsfmt namespace with `:::`.
#

# Internal environment holding the registry
.gds_cloud_env <- new.env(parent=emptyenv())
.gds_cloud_env$handlers <- list()   # scheme -> handler function
.gds_cloud_env$pkgs <- list()       # scheme -> name of registering package


#############################################################
# Register a handler for a cloud URL scheme
#
gdsRegisterCloudHandler <- function(scheme, handler, pkgname=NULL)
{
    stopifnot(is.character(scheme), length(scheme)==1L, !is.na(scheme))
    stopifnot(is.function(handler))
    stopifnot(is.null(pkgname) ||
        (is.character(pkgname) && length(pkgname)==1L))

    scheme <- tolower(scheme)
    if (!grepl("^[a-z][a-z0-9+.-]*$", scheme))
    {
        stop("'scheme' should be a valid URL scheme name without \"://\", ",
            "e.g., \"s3\".")
    }
    if (!("..." %in% names(formals(handler))))
        stop("'handler' should have a '...' argument.")

    old <- .gds_cloud_env$handlers[[scheme]]
    .gds_cloud_env$handlers[[scheme]] <- handler
    .gds_cloud_env$pkgs[[scheme]] <-
        if (is.null(pkgname)) NA_character_ else pkgname
    invisible(old)
}


#############################################################
# Unregister handlers for cloud URL schemes
#
gdsUnregisterCloudHandler <- function(scheme)
{
    stopifnot(is.character(scheme), length(scheme)>0L, !anyNA(scheme))
    scheme <- tolower(scheme)
    ans <- logical(length(scheme))
    for (i in seq_along(scheme))
    {
        s <- scheme[i]
        ans[i] <- !is.null(.gds_cloud_env$handlers[[s]])
        .gds_cloud_env$handlers[[s]] <- NULL
        .gds_cloud_env$pkgs[[s]] <- NULL
    }
    names(ans) <- scheme
    invisible(ans)
}


#############################################################
# List the registered cloud URL schemes
#
gdsCloudHandlers <- function()
{
    nm <- names(.gds_cloud_env$handlers)
    if (is.null(nm)) nm <- character()
    pkg <- vapply(nm, function(s)
        {
            v <- .gds_cloud_env$pkgs[[s]]
            if (is.null(v)) NA_character_ else as.character(v)
        }, "", USE.NAMES=FALSE)
    data.frame(scheme=nm, package=pkg, stringsAsFactors=FALSE)
}



##############################################################################
# Internal helpers used by openfn.gds()

# Get a registered cloud handler for a URL scheme, or NULL
.gds_get_cloud_handler <- function(scheme)
{
    .gds_cloud_env$handlers[[scheme]]
}

# Check if a file name is a cloud URL and return the scheme, or NULL
.gds_parse_cloud_scheme <- function(filename)
{
    m <- regmatches(filename,
        regexpr("^[a-z][a-z0-9]+(?=://)", filename, perl=TRUE))
    if (length(m) == 1L && nchar(m) > 0L) m else NULL
}


# Deprecated internal aliases, kept for one devel cycle so that a companion
# package built against gdsfmt <= 1.49.6 keeps working. Use the exported
# gdsRegisterCloudHandler() / gdsUnregisterCloudHandler() instead.
.gds_register_cloud_handler <- function(scheme, handler_fn, pkg)
    gdsRegisterCloudHandler(scheme, handler_fn, pkg)

.gds_unregister_cloud_handler <- function(scheme)
    gdsUnregisterCloudHandler(scheme)
