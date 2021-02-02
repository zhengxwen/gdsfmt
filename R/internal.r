# ===========================================================================
#
# internal.r: internal functions for GDS objects
#
# Copyright (C) 2020-2021    Xiuwen Zheng
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
# global options

options(gds.verbose=TRUE)



##############################################################################
# File Operations

# return TRUE if file reopen occurs
.reopen <- function(gdsfile)
{
    stopifnot(inherits(gdsfile, "gds.class"))
    .Call(gdsReopenGDS, gdsfile)
}


# return file size(s) in T/G/M/K/B
.pretty_dsize <- function(sz) .Call(gdsFmtSize, sz)



##############################################################################
# Parallel functions

# Dynamic call using the cluster object in the parallel package
#   cl -- a cluster object
#   .num -- the total number of segments
#   .fun -- a user-defined function
#   .combinefun -- a user-defined function for combining the returned values
#   .updatefun -- a user-defined function for updating progress (could be NULL)
#   ... -- other parameters passed to .fun
#

.parse_send_data <- quote(
    parallel:::sendData(con, list(type=type,data=value,tag=tag)))
.postNode <- function(con, type, value=NULL, tag=NULL) eval(.parse_send_data)

.sendCall <- function(con, fun, args, return=TRUE, tag=NULL)
{
    .postNode(con, "EXEC", list(fun=fun, args=args, return=return, tag=tag))
    NULL
}

.parse_recv_one_data <- quote(parallel:::recvOneData(cl))
.recvOneResult <- function(cl)
{
    v <- eval(.parse_recv_one_data)
    list(value=v$value$value, node=v$node, tag=v$value$tag)
}

.DynamicClusterCall <- function(cl, .num, .fun, .combinefun,
    .updatefun=NULL, ...)
{
    # in order to use the internal functions accessed by ':::'
    # the functions are all defined in 'parallel/R/snow.R'

    # check
    stopifnot(is.null(cl) | inherits(cl, "cluster"))
    stopifnot(is.numeric(.num))
    stopifnot(is.function(.fun))
    stopifnot(is.character(.combinefun) | is.function(.combinefun))
    stopifnot(is.null(.updatefun) | is.function(.updatefun))

    if (!is.null(cl))
    {
        ## parallel implementation

        if (identical(.combinefun, "unlist") | identical(.combinefun, "list"))
            ans <- vector("list", .num)
        else
            ans <- NULL

        p <- length(cl)
        if ((.num > 0L) && p)
        {
            ####  this closure is sending to all nodes

            argfun <- function(i) c(i, list(...))
            submit <- function(node, job)
                .sendCall(cl[[node]], .fun, argfun(job), tag = job)

            for (i in 1:min(.num, p)) submit(i, i)
            for (i in seq_len(.num))
            {
                d <- .recvOneResult(cl)
                j <- i + min(.num, p)
                if (j <= .num) submit(d$node, j)

                dv <- d$value
                if (inherits(dv, "try-error"))
                {
                    stop("One of the nodes produced an error: ",
                        as.character(dv))
                }

                if (is.function(.combinefun))
                {
                    if (is.null(ans))
                        ans <- dv
                    else
                        ans <- .combinefun(ans, dv)
                } else if (.combinefun %in% c("unlist", "list"))
                {
                    # assignment NULL would remove it from the list
                    if (!is.null(dv)) ans[[d$tag]] <- dv
                }

                if (!is.null(.updatefun)) .updatefun(i)
            }
        }
    } else {
        ####  serial implementation
        if (is.function(.combinefun))
        {
            ans <- NULL
            for (i in seq_len(.num))
            {
                dv <- .fun(i, ...)
                if (is.null(ans))
                    ans <- dv
                else
                    ans <- .combinefun(ans, dv)
            }
        } else if (identical(.combinefun, "none"))
        {
            for (i in seq_len(.num)) .fun(i, ...)
            ans <- NULL
        } else if (.combinefun %in% c("unlist", "list"))
        {
            ans <- vector("list", .num)
            for (i in seq_len(.num))
            {
                v <- .fun(i, ...)
                # assignment NULL would remove it from the list
                if (!is.null(v)) ans[[i]] <- v
            }
        }
    }

    # output
    ans
}



# Dynamic call using forking
#   ncore -- the number of cores
#   .num -- the total number of segments
#   .fun -- a user-defined function
#   .combinefun -- a user-defined function for combining the returned values
#   .updatefun -- a user-defined function for updating progress (could be NULL)
#   ... -- other parameters passed to .fun
#

.parse_prepare_cleanup <- quote(parallel:::prepareCleanup())
.mc_prepCleanup <- function() eval(.parse_prepare_cleanup)

.parse_cleanup <- quote(parallel:::cleanup(TRUE))
.mc_cleanup <- function() eval(.parse_cleanup)

.parse_process_id <- quote(parallel:::processID(jobs))
.mc_processID <- function(jobs) eval(.parse_process_id)

.parse_sel_child <- quote(parallel:::selectChildren(children, timeout))
.mc_selectChildren <- function(children, timeout) eval(.parse_sel_child)

.parse_read_child <- quote(parallel:::readChild(child))
.mc_readChild <- function(child) eval(.parse_read_child)

.DynamicForkCall <- function(ncore, .num, .fun, .combinefun, .updatefun, ...)
{
    # in order to use the internal functions accessed by ':::'
    # the functions are all defined in 'parallel/R/unix/mclapply.R'

    # check
    stopifnot(is.numeric(ncore), length(ncore)==1L)
    stopifnot(is.numeric(.num), length(.num)==1L)
    stopifnot(is.function(.fun))
    stopifnot(is.character(.combinefun) | is.function(.combinefun))
    stopifnot(is.null(.updatefun) | is.function(.updatefun))

    # all processes created from now on will be terminated by cleanup
    parallel::mc.reset.stream()
    .mc_prepCleanup()
    on.exit(.mc_cleanup())

    # initialize
    if (identical(.combinefun, "unlist") | identical(.combinefun, "list"))
        ans <- vector("list", .num)
    else
        ans <- NULL

    jobs <- lapply(seq_len(min(.num, ncore)), function(i)
        parallel::mcparallel(.fun(i, ...), name=NULL, mc.set.seed=TRUE, silent=FALSE))
    jobsp <- .mc_processID(jobs)
    jobid <- seq_along(jobsp)
    has.errors <- 0L

    finish <- rep(FALSE, .num)
    nexti <- length(jobid) + 1L
    while (!all(finish))
    {
        s <- .mc_selectChildren(jobs[!is.na(jobsp)], -1)
        if (is.null(s)) break  # no children, should not happen
        if (is.integer(s))
        {
            for (ch in s)
            {
                ji <- match(TRUE, jobsp==ch)
                ci <- jobid[ji]
                r <- .mc_readChild(ch)
                if (is.raw(r))
                {
                    child.res <- unserialize(r)
                    if (inherits(child.res, "try-error"))
                    {
                        has.errors <- has.errors + 1L
                        stop(child.res)
                    }
                    if (is.function(.combinefun))
                    {
                        if (inherits(child.res, "try-error"))
                            stop(child.res)
                        if (is.null(ans))
                            ans <- child.res
                        else
                            ans <- .combinefun(ans, child.res)
                    } else if (.combinefun %in% c("unlist", "list"))
                    {
                        # assignment NULL would remove it from the list
                        if (!is.null(child.res)) ans[[ci]] <- child.res
                    }
                    if (!is.null(.updatefun)) .updatefun(ci)
                } else {
                    # the job has finished
                    finish[ci] <- TRUE
                    jobsp[ji] <- jobid[ji] <- NA_integer_
                    # still something to do
                    if (nexti <= .num)
                    {
                        jobid[ji] <- nexti
                        jobs[[ji]] <- parallel::mcparallel(.fun(nexti, ...),
                            name=NULL, mc.set.seed=TRUE, silent=FALSE)
                        jobsp[ji] <- .mc_processID(jobs[[ji]])
                        nexti <- nexti + 1L
                    }
                }
            }
        }
    }

    if (has.errors)
    {
        warning(sprintf("%d function calls resulted in an error", has.errors),
            immediate.=TRUE, domain=NA)
    }

    # output
    ans
}



##############################################################################
# Unit testing

# Run all unit tests
UnitTest <- function()
{
    # load R packages
    if (!requireNamespace("RUnit"))
        stop("Please install RUnit package!")

    options(test.verbose=TRUE)

    # define a test suite
    myTestSuite <- RUnit::defineTestSuite("gdsfmt examples",
        system.file("unitTests", package = "gdsfmt"),
        testFileRegexp = "^test_.*\\.R$")

    # run the test suite
    testResult <- RUnit::runTestSuite(myTestSuite)

    # print detailed text protocol to standard out:
    RUnit::printTextProtocol(testResult)

    # return
    invisible()
}
