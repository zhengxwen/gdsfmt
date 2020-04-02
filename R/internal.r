# ===========================================================================
#
# internal.r: internal functions for GDS objects
#
# Copyright (C) 2020    Xiuwen Zheng
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

# dynamic call
#   cl -- a cluster object
#   .num -- the total number of segments
#   .fun -- a user-defined function
#   .combinefun -- a user-defined function for combining the returned values
#   .stopcluster -- TRUE/FALSE, if TRUE stop cluster nodes after running the jobs
#   .updatefun -- a user-defined function for updating progress (could be NULL)
.DynamicClusterCall <- function(cl, .num, .fun, .combinefun, .updatefun=NULL,
    .stopcluster=FALSE, ...)
{
    # in order to use the internal functions accessed by ':::'
    # the functions are all defined in 'parallel/R/snow.R'

    # check
    stopifnot(is.null(cl) | inherits(cl, "cluster"))
    stopifnot(is.numeric(.num))
    stopifnot(is.function(.fun))
    stopifnot(is.character(.combinefun) | is.function(.combinefun))
    stopifnot(is.logical(.stopcluster))
    stopifnot(is.null(.updatefun) | is.function(.updatefun))

    .SendData <- parse(text=
        "parallel:::sendData(con, list(type=type,data=value,tag=tag))")
    .RecvOneData <- parse(text="parallel:::recvOneData(cl)")

    postNode <- function(con, type, value = NULL, tag = NULL)
    {
        eval(.SendData)
    }
    sendCall <- function(con, fun, args, return = TRUE, tag = NULL)
    {
        postNode(con, "EXEC",
            list(fun = fun, args = args, return = return, tag = tag))
        NULL
    }
    recvOneResult <- function(cl)
    {
        v <- eval(.RecvOneData)
        list(value = v$value$value, node = v$node, tag = v$value$tag)
    }


    #################################################################

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
                sendCall(cl[[node]], .fun, argfun(job), tag = job)

            for (i in 1:min(.num, p)) submit(i, i)
            for (i in seq_len(.num))
            {
                d <- recvOneResult(cl)
                j <- i + min(.num, p)

                stopflag <- FALSE
                if (j <= .num)
                {
                    submit(d$node, j)
                } else {
                    if (.stopcluster)
                    {
                        parallel::stopCluster(cl[d$node])
                        cl <- cl[-d$node]
                        stopflag <- TRUE
                    }
                }

                dv <- d$value
                if (inherits(dv, "try-error"))
                {
                    if (.stopcluster)
                        parallel::stopCluster(cl)
                    stop("One of the nodes produced an error: ", as.character(dv))
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

                if (stopflag)
                    message(sprintf("Stop \"job %d\".", d$node))
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
