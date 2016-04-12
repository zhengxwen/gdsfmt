# ===========================================================================
#
# gdsfmt-main.r: R Interface to CoreArray Genomic Data Structure (GDS) Files
#
# Copyright (C) 2011-2016    Xiuwen Zheng
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
##############################################################################

#############################################################
# Create a new CoreArray Genomic Data Structure (GDS) file
#
createfn.gds <- function(filename, allow.duplicate=FALSE)
{
    stopifnot(is.character(filename), length(filename)==1L)
    stopifnot(is.logical(allow.duplicate))

    # 'normalizePath' does not work if the file does not exist
    tmpf <- file(filename, "wb")
    close(tmpf)

    filename <- normalizePath(filename, mustWork=FALSE)
    ans <- .Call(gdsCreateGDS, filename, allow.duplicate)
    names(ans) <- c("filename", "id", "root", "readonly")
    ans$filename <- filename
    class(ans) <- "gds.class"
    ans
}


#############################################################
# Open an existing file
#
openfn.gds <- function(filename, readonly=TRUE, allow.duplicate=FALSE,
    allow.fork=FALSE)
{
    stopifnot(is.character(filename), length(filename)==1L)

    filename <- normalizePath(filename, mustWork=FALSE)
    ans <- .Call(gdsOpenGDS, filename, readonly, allow.duplicate,
        allow.fork)
    names(ans) <- c("filename", "id", "root", "readonly")
    ans$filename <- filename
    class(ans) <- "gds.class"
    ans
}


#############################################################
# Close an open CoreArray Genomic Data Structure (GDS) file
#
closefn.gds <- function(gdsfile)
{
    stopifnot(inherits(gdsfile, "gds.class"))
    .Call(gdsCloseGDS, gdsfile)
    invisible()
}


#############################################################
# Write the data cached in memory to disk
#
sync.gds <- function(gdsfile)
{
    stopifnot(inherits(gdsfile, "gds.class"))
    .Call(gdsSyncGDS, gdsfile)
    invisible()
}


#############################################################
# Clean up fragments of a GDS file
#
cleanup.gds <- function(filename, verbose=TRUE)
{
    stopifnot(is.character(filename), length(filename)==1L)
    .Call(gdsTidyUp, filename, verbose)
    invisible()
}


#############################################################
# enumerate all opened GDS files
#
showfile.gds <- function(closeall=FALSE, verbose=TRUE)
{
    stopifnot(is.logical(closeall))
    stopifnot(is.logical(verbose))

    rv <- .Call(gdsGetConnection)

    if (length(rv) > 0L)
    {
        nm <- NULL; rd <- NULL
        for (i in seq_along(rv))
        {
            names(rv[[i]]) <- c("filename", "id", "root", "readonly")
            class(rv[[i]]) <- "gds.class"
            nm <- c(nm, rv[[i]]$filename)
            rd <- c(rd, rv[[i]]$readonly)
        }
        if (verbose & !closeall)
        {
            print(data.frame(FileName=nm, ReadOnly=rd,
                State=rep("open", length(rd))))
        }
    } else
        rv <- NULL

    # close all opened GDS files
    if (closeall & !is.null(rv))
    {
        if (verbose)
        {
            print(data.frame(FileName=nm, ReadOnly=rd,
                State=rep("closed", length(rd))))
        }
        for (i in seq_along(rv))
            closefn.gds(rv[[i]])
        rv <- NULL
    }

    invisible(rv)
}


#############################################################
# Diagnose the GDS file
#
diagnosis.gds <- function(gds)
{
    stopifnot(inherits(gds, "gds.class") | inherits(gds, "gdsn.class"))

    if (inherits(gds, "gds.class"))
    {
        # call C function
        rv <- .Call(gdsDiagInfo, gds)

        names(rv) <- c("stream", "log")
        rv$stream <- as.data.frame(rv$stream, stringsAsFactors=FALSE)
        colnames(rv$stream) <- c("id", "size", "capacity", "num_chunk", "path")

    } else {
        # call C function
        rv <- .Call(gdsDiagInfo2, gds)
    }

    rv
}





##############################################################################
# File Structure Operations
##############################################################################

#############################################################
# Get the number of variables in a specified folder
#
cnt.gdsn <- function(node, include.hidden=FALSE)
{
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    .Call(gdsNodeChildCnt, node, include.hidden)
}


#############################################################
# Get the variable name of a node
#
name.gdsn <- function(node, fullname=FALSE)
{
    stopifnot(inherits(node, "gdsn.class"))
    .Call(gdsNodeName, node, fullname)
}


#############################################################
# Rename a GDS node
#
rename.gdsn <- function(node, newname)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.character(newname), length(newname)==1L)

    .Call(gdsRenameNode, node, newname)
    invisible()
}


#############################################################
# Get a list of names for the child nodes
#
ls.gdsn <- function(node, include.hidden=FALSE)
{
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    .Call(gdsNodeEnumName, node, include.hidden)
}


#############################################################
# Get a specified node
#
index.gdsn <- function(node, path=NULL, index=NULL, silent=FALSE)
{
    # check
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.logical(silent))

    .Call(gdsNodeIndex, node, path, index, silent)
}


#############################################################
# Get the folder node which contains the specified node
#
getfolder.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))
    .Call(gdsGetFolder, node)
}


#############################################################
# Get the descritpion of a specified node
#
objdesp.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))

    ans <- .Call(gdsNodeObjDesp, node)
    names(ans) <- c("name", "fullname", "storage", "trait", "type",
        "is.array", "dim", "encoder", "compress", "cpratio", "size",
        "good", "hidden", "message", "param")
    attr(ans$type, "levels") <- c("Label", "Folder", "VFolder", "Raw",
        "Integer", "Factor", "Logical", "Real", "String", "Unknown")
    attr(ans$type, "class") <- "factor"
    ans
}


#############################################################
# Add a GDS node
#
add.gdsn <- function(node, name, val=NULL, storage=storage.mode(val),
    valdim=NULL, compress=c("", "ZIP", "ZIP_RA", "LZ4", "LZ4_RA"),
    closezip=FALSE, check=TRUE, replace=FALSE, visible=TRUE, ...)
{
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    if (missing(name))
    {
        nmlist <- ls.gdsn(node)
        repeat {
            name <- sprintf("tmp_%06x", round(runif(1, 0, 2^24-1)))
            if (!is.element(name, nmlist))
                break
        }
    }
    stopifnot(is.character(name), length(name)==1L)

    dots <- list(...)
    if (inherits(storage, "gdsn.class"))
    {
        dp <- objdesp.gdsn(storage)
        storage <- dp$storage
        if (is.null(valdim))
        {
            valdim <- dp$dim
            if (is.numeric(valdim))
                valdim[length(valdim)] <- 0L
        }
        if (identical(compress, c("", "ZIP", "ZIP_RA", "LZ4", "LZ4_RA")))
        {
            compress <- dp$compress
        }
    }

    stopifnot(is.character(storage), length(storage)==1L)
    stopifnot(is.character(compress), length(compress)>0L)
    compress <- compress[1L]

    stopifnot(is.logical(closezip))
    stopifnot(is.logical(check))
    stopifnot(is.logical(replace))
    stopifnot(is.logical(visible))

    # call C function
    ans <- .Call(gdsAddNode, node, name, val, storage, valdim, compress,
        closezip, check, replace, visible, dots)

    if (storage == "list")
    {
        nm <- class(val)
        if (!identical(nm, "data.frame") & !("list" %in% nm))
            nm <- c(nm, "list")
        put.attr.gdsn(ans, "R.class", nm)

        nm <- names(val)
        for (i in seq_along(nm))
        {
            add.gdsn(ans, nm[i], val[[i]], compress=compress,
                closezip=closezip, check=check)
        }
    } else if (storage == "logical")
    {
        put.attr.gdsn(ans, "R.logical")
    } else if (is.factor(val))
    {
        put.attr.gdsn(ans, "R.class", "factor")
        put.attr.gdsn(ans, "R.levels", levels(val))
    }

    invisible(ans)
}


#############################################################
# Add a (virtual) folder
#
addfolder.gdsn <- function(node, name, type=c("directory", "virtual"),
    gds.fn="", replace=FALSE, visible=TRUE)
{
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    if (missing(name))
    {
        nmlist <- ls.gdsn(node)
        repeat {
            name <- sprintf("tmp_%06x", round(runif(1, 0, 2^24-1)))
            if (!is.element(name, nmlist))
                break
        }
    }
    stopifnot(is.character(name), length(name)==1L)

    type <- match.arg(type)
    if (type == "virtual")
    {
        stopifnot(is.character(gds.fn), length(gds.fn)==1L)
        stopifnot(!is.na(gds.fn))
        stopifnot(gds.fn != "")
    }

    stopifnot(is.logical(replace))
    stopifnot(is.logical(visible))

    # call C function
    ans <- .Call(gdsAddFolder, node, name, type, gds.fn, replace, visible)

    invisible(ans)
}


#############################################################
# Add a GDS node with a file
#
addfile.gdsn <- function(node, name, filename,
    compress=c("ZIP", "ZIP_RA", "LZ4", "LZ4_RA"), replace=FALSE, visible=TRUE)
{
    if (inherits(node, "gds.class"))
        node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    if (missing(name))
    {
        nmlist <- ls.gdsn(node)
        repeat {
            name <- sprintf("tmp_%06x", round(runif(1, 0, 2^24-1)))
            if (!is.element(name, nmlist))
                break
        }
    }
    stopifnot(is.character(name), length(name)==1L)
    stopifnot(is.character(filename), length(filename)==1L)

    stopifnot(is.character(compress), length(compress)>0L)
    compress <- compress[1L]

    stopifnot(is.logical(replace))
    stopifnot(is.logical(visible))

    # call C function
    ans <- .Call(gdsAddFile, node, name, filename, compress, replace, visible)

    invisible(ans)
}


#############################################################
# Get a file from a stream container
#
getfile.gdsn <- function(node, out.filename)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.character(out.filename), length(out.filename)==1L)

    .Call(gdsGetFile, node, out.filename)
    invisible()
}


#############################################################
# Delete a specified node
#
delete.gdsn <- function(node, force=FALSE)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.logical(force))

    .Call(gdsDeleteNode, node, force)
    invisible()
}




##############################################################################
# Attribute
##############################################################################

#############################################################
# Add an attribute to a GDS node
#
put.attr.gdsn <- function(node, name, val=NULL)
{
    stopifnot(inherits(node, "gdsn.class"))

    if (inherits(val, "gdsn.class"))
    {
        .Call(gdsPutAttr2, node, val)
    } else {
        stopifnot(is.character(name), length(name)==1L)
        .Call(gdsPutAttr, node, name, val)
    }
    invisible()
}


#############################################################
# Get the attributes of a GDS node
#
get.attr.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))
    .Call(gdsGetAttr, node)
}


#############################################################
# Remove an attribute from a GDS node
#
delete.attr.gdsn <- function(node, name)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.character(name), length(name)>0L)
    .Call(gdsDeleteAttr, node, name)
    invisible()
}





##############################################################################
# Data Operations
##############################################################################

#############################################################
# Modify the data compression mode of data field
#
compression.gdsn <- function(node,
    compress=c("", "ZIP", "ZIP_RA", "LZ4", "LZ4_RA"))
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.character(compress), length(compress)>0L)
    compress <- compress[1L]

    .Call(gdsObjCompress, node, compress)
    return(node)
}


#############################################################
# Get into read mode of compression
#
readmode.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))
    .Call(gdsObjCompressClose, node)
    return(node)
}


#############################################################
# Set the new dimension of the data field for a GDS node
#
setdim.gdsn <- function(node, valdim, permute=FALSE)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.numeric(valdim) & is.vector(valdim))
    stopifnot(is.logical(permute))

    .Call(gdsObjSetDim, node, valdim, permute)
    return(node)
}


#############################################################
# Transpose an array by permuting its dimensions
#
permdim.gdsn <- function(node, dimidx, target=NULL)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.numeric(dimidx) & is.vector(dimidx))
    stopifnot(is.null(target) | inherits(target, "gdsn.class"))

    # check dimidx
    dm <- objdesp.gdsn(node)$dim
    if (length(dm) != length(dimidx))
        stop("'dimidx' should have ", length(dm), " element(s).")
    flag <- rep(TRUE, length(dm))
    flag[dimidx] <- FALSE
    if (sum(flag) > 0L)
        stop("'dimidx' should be a permutation of 1..", length(dm), ".")

    if (all(dimidx == seq_along(dm)))
    {
        if (!is.null(target))
        {
            assign.gdsn(target, node, append=FALSE)
            target.node <- target
        } else
            target.node <- node
    } else {
        if (is.null(target))
        {
            target.node <- add.gdsn(getfolder.gdsn(node), storage=node)
            put.attr.gdsn(target.node, val=node)
        } else
            target.node <- target

        vdim <- dm[dimidx]
        vdim[length(vdim)] <- 0L
        setdim.gdsn(target.node, vdim, permute=FALSE)

        if (length(dm) == 2L)
        {
            # tranpose a matrix
            apply.gdsn(node, margin=1L, FUN=`c`,
                as.is="gdsnode", target.node=target.node, .useraw=TRUE)
            readmode.gdsn(target.node)
        } else {
            i <- dimidx[length(dimidx)]
            dimidx <- dimidx[-length(dimidx)]
            dimidx[dimidx > i] <- dimidx[dimidx > i] - 1L

            apply.gdsn(node, margin=i, FUN=aperm,
                as.is="gdsnode", target.node=target.node, .useraw=TRUE,
                perm = dimidx)
            readmode.gdsn(target.node)
        }

        if (is.null(target))
            moveto.gdsn(target.node, node, relpos="replace+rename")
    }

    invisible()
}


#############################################################
# Append data to a specified variable
#
append.gdsn <- function(node, val, check=TRUE)
{
    stopifnot(inherits(node, "gdsn.class"))

    if (inherits(val, "gdsn.class"))
        .Call(gdsObjAppend2, node, val)
    else
        .Call(gdsObjAppend, node, val, check)

    invisible()
}


#############################################################
# Read data field of a GDS node
#
read.gdsn <- function(node, start=NULL, count=NULL,
    simplify=c("auto", "none", "force"), .useraw=FALSE, .value=NULL,
    .substitute=NULL)
{
    stopifnot(inherits(node, "gdsn.class"))
    simplify <- match.arg(simplify)

    if (is.null(start) & is.null(count))
    {
        rvattr <- get.attr.gdsn(node)
        rvclass <- rvattr$R.class
        if (!is.null(rvclass))
        {
            if (identical(rvclass, "data.frame") | ("list" %in% rvclass))
            {
                nm <- ls.gdsn(node, include.hidden=FALSE)
                r <- vector("list", length(nm))
                names(r) <- nm
                for (i in seq_along(nm))
                {
                    n <- index.gdsn(node, nm[i])
                    r[[i]] <- read.gdsn(n, .useraw=.useraw,
                        .value=.value, .substitute=.substitute)
                }

                if (identical(rvclass, "data.frame"))
                {
                    r <- as.data.frame(r, stringsAsFactors=FALSE)
                } else {
                    rvclass <- setdiff(rvclass, "list")
                    if (length(rvclass) > 0L)
                        class(r) <- rvclass
                }

                return(r)
            }
        }
    }

    .Call(gdsObjReadData, node, start, count, simplify, .useraw,
        list(.value, .substitute))
}


#############################################################
# Read data field of a GDS node
#
readex.gdsn <- function(node, sel=NULL, simplify=c("auto", "none", "force"),
    .useraw=FALSE, .value=NULL, .substitute=NULL)
{
    stopifnot(inherits(node, "gdsn.class"))
    simplify <- match.arg(simplify)

    if (!is.null(sel))
    {
        stopifnot(is.logical(sel) | is.numeric(sel) | is.list(sel))
        if (is.logical(sel)) sel <- list(sel)
        if (is.numeric(sel)) sel <- list(sel)

        # read
        idx <- list(NULL)
        dat <- .Call(gdsObjReadExData, node, sel, .useraw, idx)
        if (!is.null(idx[[1L]]))
            dat <- do.call(`[`, idx[[1L]])
        .Call(gdsDataFmt, dat, simplify, list(.value, .substitute))
    } else {
        # output
        read.gdsn(node)
    }
}


#############################################################
# Apply functions over array margins of a GDS node
#
apply.gdsn <- function(node, margin, FUN, selection=NULL,
    as.is=c("list", "none", "integer", "double", "character", "logical",
    "raw", "gdsnode"), var.index=c("none", "relative", "absolute"),
    target.node=NULL, .useraw=FALSE, .value=NULL, .substitute=NULL, ...)
{
    # check
    if (inherits(node, "gdsn.class"))
    {
        stopifnot(is.numeric(margin), length(margin)==1L)
        stopifnot(is.null(selection) | is.list(selection))

        node <- list(node)
        if (!is.null(selection))
            selection <- list(selection)
    } else {
        if (!is.list(node))
        {
            stop(
            "'node' should be 'gdsn.class' or a list of 'gdsn.class' objects.")
        }
        for (i in seq_along(node))
        {
            if (!inherits(node[[i]], "gdsn.class"))
            {
                stop(sprintf(
                    "node[[%d]] should be an object of 'gdsn' class.", i))
            }
        }
    
        stopifnot(is.numeric(margin))
        stopifnot(length(margin) == length(node))
    
        stopifnot(is.null(selection) | is.list(selection))
        if (!is.null(selection))
            stopifnot(length(selection) == length(node))
    }

    FUN <- match.fun(FUN)
    as.is <- match.arg(as.is)
    var.index <- match.arg(var.index)

    if (as.is == "gdsnode")
    {
        if (inherits(target.node, "gdsn.class"))
            target.node <- list(target.node)
        if (is.list(target.node))
        {
            for (i in seq_along(target.node))
                stopifnot(inherits(target.node[[i]], "gdsn.class"))
        } else {
            stop(
            "'target.node' should be 'gdsn.class' or a list of 'gdsn.class'.")
        }
    } else {
        if (!is.null(target.node))
            stop("'target.node' should be NULL.")
    }

    # call C function -- set starting index
    .Call(gdsApplySetStart, 1L)

    # call C function -- apply calling
    ans <- .Call(gdsApplyCall, node, as.integer(margin), FUN,
        selection, as.is, var.index, target.node, new.env(),
        .useraw, list(.value, .substitute))

    if (is.null(ans))
        invisible()
    else
        ans
}


#############################################################
# Apply functions over array margins of a list of GDS nodes in parallel
#
clusterApply.gdsn <- function(cl, gds.fn, node.name, margin,
    FUN, selection=NULL, as.is=c("list", "none", "integer", "double",
    "character", "logical", "raw"),
    var.index=c("none", "relative", "absolute"), .useraw=FALSE,
    .value=NULL, .substitute=NULL, ...)
{
    #########################################################
    # library
    #
    if (!requireNamespace("parallel"))
        stop("The 'parallel' package should be installed.")


    #########################################################
    # check
    #
    stopifnot(inherits(cl, "cluster"))
    stopifnot(is.character(gds.fn), length(gds.fn)==1L)
    stopifnot(is.character(node.name))
    stopifnot(is.numeric(margin), length(margin)==length(node.name))
    margin <- as.integer(margin)

    if (!is.null(selection))
    {
        if (!is.list(selection[[1L]]))
            selection <- list(selection)
    }


    FUN <- match.fun(FUN)
    as.is <- match.arg(as.is)
    var.index <- match.arg(var.index)


    #########################################################
    # new selection
    #

    gfile <- openfn.gds(gds.fn, allow.duplicate=TRUE)
    on.exit({ closefn.gds(gfile) })

    nd_nodes <- vector("list", length(node.name))
    names(nd_nodes) <- names(node.name)
    for (i in seq_along(nd_nodes))
    {
        v <- index.gdsn(gfile, path=node.name[i], silent=TRUE)
        nd_nodes[[i]] <- v
        if (is.null(v))
        {
            stop(sprintf("There is no node \"%s\" in the specified gds file.",
                node.name[i]))
        }
    }

    new.selection <- .Call(gdsApplyCreateSelection, nd_nodes,
        margin, selection)

    # the count of elements
    MarginCount <- sum(new.selection[[1L]][[ margin[1L] ]], na.rm=TRUE)
    if (MarginCount <= 0L)
        return(invisible())


    #########################################################
    # run
    #

    if (length(cl) > 1L)
    {
        # close the GDS file
        closefn.gds(gfile)
        on.exit()

        clseq <- parallel::splitIndices(MarginCount, length(cl))
        sel.list <- vector("list", length(cl))
        start <- 1L

        # for - loop: multi processes
        for (i in seq_along(cl))
        {
            n <- length(clseq[[i]])
            if (n > 0L)
            {
                tmp <- new.selection

                # for - loop: multiple variables
                for (j in seq_along(tmp))
                {
                    sel <- tmp[[j]]
                    idx <- which(sel[[ margin[j] ]])
                    flag <- rep(FALSE, length(sel[[ margin[j] ]]))
                    flag[ idx[ clseq[[i]] ] ] <- TRUE
                    sel[[ margin[j] ]] <- flag
                    tmp[[j]] <- sel
                }

                sel.list[[i]] <- list(start=start, n=n, sel=tmp)
                start <- start + n
            } else {
                sel.list[[i]] <- list(start=start, n=0L, sel=NULL)
            }
        }

        # enumerate
        ans <- parallel::clusterApply(cl, sel.list, fun =
                function(item, gds.fn, node.name, margin, FUN,
                    as.is, var.index, ...)
            {
                if (item$n <= 0L) return(NULL)

                # load the package
                library(gdsfmt)

                # open the file
                gfile <- openfn.gds(gds.fn, allow.duplicate=TRUE)
                on.exit({ closefn.gds(gfile) })

                nd_nodes <- vector("list", length(node.name))
                names(nd_nodes) <- names(node.name)
                for (i in seq_along(nd_nodes))
                    nd_nodes[[i]] <- index.gdsn(gfile, path=node.name[i])

                # call C function -- set starting index
                .Call(gdsApplySetStart, item$start)

                # call C function -- apply calling
                .Call(gdsApplyCall, nd_nodes, margin, FUN, item$sel, as.is,
                    var.index, NULL, new.env(), .useraw,
                    list(.value, .substitute))

            }, gds.fn=gds.fn, node.name=node.name, margin=margin,
                FUN=FUN, as.is=as.is, var.index=var.index, ...
        )

        if (as.is != "none")
            unlist(ans, recursive=FALSE)
        else 
            invisible()

    } else {
        apply.gdsn(nd_nodes, margin, FUN, selection, as.is,
            var.index, .useraw, ...)
    }
}


#############################################################
# Write data to a GDS node
#
write.gdsn <- function(node, val, start=NULL, count=NULL, check=TRUE)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(!missing(val))

    if (is.null(start) & is.null(count))
    {
        .Call(gdsObjWriteAll, node, val, check)
    } else {
        .Call(gdsObjWriteData, node, val, start, count, check)
    }
    invisible()
}


#############################################################
# Assign a GDS variable from another variable
#
assign.gdsn <- function(node, src.node=NULL, resize=TRUE, seldim=NULL,
    append=FALSE, .value=NULL, .substitute=NULL)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.null(src.node) | inherits(src.node, "gdsn.class"))
    stopifnot(is.logical(resize), length(resize)==1L)
    stopifnot(is.logical(append), length(append)==1L)

    if (is.null(seldim))
    {
        if (is.null(src.node))
        {
            # no need to resize
            if (!is.null(.value) | !is.null(.substitute))
            {
                src.node <- add.gdsn(getfolder.gdsn(node), storage=node)
                put.attr.gdsn(src.node, val=node)

                dm <- objdesp.gdsn(node)$dim
                dm[length(dm)] <- 0L
                setdim.gdsn(src.node, dm, permute=FALSE)

                apply.gdsn(node, margin=length(dm), FUN=`c`, as.is="gdsnode",
                    target.node=src.node,
                    .value=.value, .substitute=.substitute)
                if (!append) readmode.gdsn(src.node)

                moveto.gdsn(src.node, node, relpos="replace+rename")
            } else {
                if (!append) readmode.gdsn(src.node)
            }
        } else {
            if (resize)
            {
                if (is.null(.value) & is.null(.substitute))
                {
                    # call C function
                    .Call(gdsAssign, node, src.node)
                    return(invisible())
                } else {
                    dm <- objdesp.gdsn(src.node)$dim
                    dm[length(dm)] <- 0L
                    setdim.gdsn(node, dm, permute=FALSE)
                }
            } else {
                if (is.null(.value) & is.null(.substitute))
                {
                    append.gdsn(node, src.node)
                    return(invisible())
                }
            }

            apply.gdsn(src.node, margin=length(dm), FUN=`c`,
                as.is="gdsnode", target.node=node, .value, .substitute)
            if (!append) readmode.gdsn(node)
        }

    } else {
        if (!is.list(seldim))
            seldim <- list(seldim)
        if (is.null(src.node))
        {
            src <- node
            dst <- add.gdsn(getfolder.gdsn(node), storage=node)
            put.attr.gdsn(dst, val=src)
        } else {
            src <- src.node
            dst <- node
        }

        if (any(sapply(seldim, FUN=is.numeric)))
        {
            dm <- objdesp.gdsn(src)$dim
            if (length(dm) != length(seldim))
                stop("Invalid 'seldim': incorrect number of dimensions.")

            newdm <- integer(length(dm))
            for (i in seq_along(dm))
            {
                if (is.null(seldim[[i]]))
                    newdm[i] <- dm[i]
                else if (is.numeric(seldim[[i]]))
                    newdm[i] <- length(seldim[[i]])
                else if (is.logical(seldim[[i]]))
                    newdm[i] <- sum(seldim[[i]], na.rm=TRUE)
                else if (is.raw(seldim[[i]]))
                    newdm[i] <- sum(seldim[[i]] != 0L)
                else
                    stop("Invalid 'seldim[[", i, "]]'.")
            }

            sel1 <- seldim[[length(seldim)]]
            if (is.null(sel1))
                sel1 <- seq_len(dm[length(dm)])
            else if (is.logical(sel1))
                sel1 <- which(sel1)
            else if (is.raw(sel1))
                sel1 <- which(sel1 != 0L)

            MB_use <- 1024*1024*500    # 500MB
            if (.Platform$OS.type == "windows")
            {
                m <- memory.size(TRUE) %/% 2
                if (MB_use > m) MB_use <- m
            }

            subcnt <- prod(newdm[-length(newdm)])
            inccnt <- MB_use %/% subcnt
            if (inccnt <= 1L) inccnt <- 1L

            if (resize)
            {
                newdm[length(newdm)] <- 0L
                setdim.gdsn(dst, newdm, permute=FALSE)
            }

            # for-loop
            st <- 1L
            n <- length(sel1); n1 <- n + 1L
            while (st <= n)
            {
                if ((st + inccnt) <= n1)
                    cnt <- inccnt
                else
                    cnt <- n1 - st
                seldim[[length(seldim)]] <- sel1[seq.int(st, length.out=cnt)]
                dat <- readex.gdsn(src, sel=seldim, simplify="none",
                    .value=.value, .substitute=.substitute)
                append.gdsn(dst, dat, check=!append)
                st <- st + inccnt
            }
            if (!append) readmode.gdsn(dst)

        } else {
            dm <- objdesp.gdsn(src)$dim
            if (resize)
            {
                for (i in seq_along(dm))
                {
                    if (is.logical(seldim[[i]]))
                    {
                        dm[i] <- sum(seldim[[i]], na.rm=TRUE)
                    } else if (is.raw(seldim[[i]]))
                    {
                        dm[i] <- sum(seldim[[i]]!=0L, na.rm=TRUE)
                    } else if (!is.null(seldim[[i]]))
                        stop("Invalid 'seldim'.")
                }
                dm[length(dm)] <- 0L
                setdim.gdsn(dst, dm, permute=FALSE)
            }

            .useraw <- is.null(.value) & is.null(.substitute)
            apply.gdsn(src, margin=length(dm), FUN=`c`, selection=seldim,
                as.is="gdsnode", target.node=dst, .useraw=.useraw,
                .value=.value, .substitute=.substitute)
            if (!append) readmode.gdsn(dst)
        }

        if (is.null(src.node))
            moveto.gdsn(dst, node, relpos="replace+rename")
    }

    invisible()
}


#############################################################
# Cache the data associated with a GDS variable
#
cache.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))

    .Call(gdsCache, node)
    invisible()
}


#############################################################
# Move to a new location
#
moveto.gdsn <- function(node, loc.node,
    relpos = c("after", "before", "replace", "replace+rename"))
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(inherits(loc.node, "gdsn.class"))
    relpos <- match.arg(relpos)

    .Call(gdsMoveTo, node, loc.node, relpos)
    invisible()
}


#############################################################
# Copy to a new GDS node
#
copyto.gdsn <- function(node, source, name=NULL)
{
    if (inherits(node, "gds.class")) node <- node$root
    stopifnot(inherits(node, "gdsn.class"))

    if (inherits(source, "gds.class")) source <- source$root
    stopifnot(inherits(source, "gdsn.class"))

    if (is.null(name))
        name <- name.gdsn(source, fullname=FALSE)
    stopifnot(is.character(name), length(name)==1L)

    .Call(gdsCopyTo, node, name, source)
    invisible()
}


#############################################################
# Get whether elements in node
#
is.element.gdsn <- function(node, set)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.numeric(set) | is.character(set))
    .Call(gdsIsElement, node, set)
}


#############################################################
# Create hash function digests
#
digest.gdsn <- function(node, algo=c("md5", "sha1", "sha256", "sha384",
    "sha512"), action=c("none", "Robject", "add", "add.Robj", "clear",
    "verify", "return"))
{
    stopifnot(inherits(node, "gdsn.class"))
    algo <- match.arg(algo)
    action <- match.arg(action)

    algolist <- c("md5", "sha1", "sha256", "sha384", "sha512")
    algoname <- c(algolist, paste0(algolist, "_r"))
    algolist <- c(algolist, algolist)

    if (action == "clear")
    {
        at <- get.attr.gdsn(node)
        nm <- intersect(names(at), algoname)
        if (length(nm) > 0L)
            delete.attr.gdsn(node, nm)
        invisible()
    } else if (action %in% c("verify", "return"))
    {
        at <- get.attr.gdsn(node)
        ans <- rep(NA, length(algoname))
        names(ans) <- algoname
        for (i in seq_along(ans))
        {
            h1 <- at[[algoname[i]]]
            if (is.character(h1) & !anyNA(h1))
            {
                h2 <- unname(digest.gdsn(node, algo=algolist[i],
                    action=ifelse(i<6, "none", "Robject")))
                ans[i] <- identical(h1, h2)
            }
        }
        if (action == "verify")
        {
            v <- !ans
            v[is.na(v)] <- FALSE
            if (sum(v) > 0L)
            {
                s <- algoname[v]
                if (length(s) > 1L)
                    stop(paste(s, collapse=", "), " verification fail.")
                else
                    stop(s, " verification fails.")
            }
        }
        ans
    } else {
        if (requireNamespace("digest", quietly=TRUE))
        {
            flag <- action %in% c("Robject", "add.Robj")
            ans <- .Call(gdsDigest, node, algo, flag)
            if (flag) algo <- paste0(algo, "_r")
            if (action %in% c("add", "add.Robj"))
            {
                if (is.na(ans))
                    warning("No valid hash code to add.")
                put.attr.gdsn(node, algo, ans)
            }
            names(ans) <- algo
            ans
        } else
            NA_character_
    }
}


#############################################################
# Summary of GDS data
#
summarize.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))
    .Call(gdsSummary, node)
}



##############################################################################
# Error function
##############################################################################

#############################################################
# Return the last error
#
lasterr.gds <- function()
{
    .Call(gdsLastErrGDS)
}

#############################################################
# Return the parameters in the GDS system
#
system.gds <- function()
{
    rv <- .Call(gdsSystem)

    s <- rv$compression.encoder
    rv$compression.encoder <- data.frame(
        encoder = rv$compression.encoder[seq.int(1L, length(s), 2L)],
        description = rv$compression.encoder[seq.int(2L, length(s), 2L)],
        stringsAsFactors = FALSE)
    rv$class.list <- data.frame(rv$class.list, stringsAsFactors=FALSE)
    colnames(rv$class.list) <- c("name", "description")
    rv$class.list <- rv$class.list[order(rv$class.list$description), ]
    rownames(rv$class.list) <- NULL

    rv$options <- list(
        gds.crayon = getOption("gds.crayon", NULL),
        gds.parallel = getOption("gds.parallel", NULL)
    )

    rv
}



##############################################################################
# R Generic functions
##############################################################################

.crayon <- function()
{
    crayon.flag <- getOption("gds.crayon", TRUE)
    if (!is.logical(crayon.flag))
        crayon.flag <- TRUE
    crayon.flag <- crayon.flag[1L]
    if (is.na(crayon.flag))
        crayon.flag <- FALSE
    crayon.flag && requireNamespace("crayon", quietly=TRUE)
}

print.gds.class <- function(x, ...)
{
    # check
    stopifnot(inherits(x, "gds.class"))

    size <- .Call(gdsFileSize, x)
    if (.crayon())
    {
        s <- paste0(crayon::inverse("File:"), " ", x$filename, " ",
            crayon::blurred(paste0("(", .Call(gdsFmtSize, size), ")")), "\n")
    } else {
        s <- paste0("File: ", x$filename, " (", .Call(gdsFmtSize, size), ")\n")
    }
    cat(s)
    print(x$root, ...)
}

print.gdsn.class <- function(x, expand=TRUE, all=FALSE, attribute=FALSE,
    attribute.trim=FALSE, ...)
{
    if (.crayon())
    {
        BOLD <- `c`
        UNDERLINE <- crayon::silver
        BLURRED <- crayon::blurred
        WARN <- crayon::magenta
    } else {
        BOLD <- UNDERLINE <- BLURRED <- WARN <- `c`
    }

    enum <- function(node, prefix, fullname, last)
    {
        n <- objdesp.gdsn(node)

        if (n$type == "Label")
        {
            lText <- " "; rText <- " "
        } else if (n$type == "VFolder")
        {
            if (n$good)
            {
                lText <- "[ -->"; rText <- "]"
            } else {
                lText <- WARN("[ -X-"); rText <- WARN("]")
                expand <- FALSE
            }
        } else if (n$type == "Folder")
        {
            lText <- "["; rText <- "]"
        } else if (n$type == "Unknown")
        {
            lText <- WARN("  -X-"); rText <- WARN("")
            expand <- FALSE
        } else {
            lText <- BLURRED("{"); rText <- BLURRED("}")
        }
        s <- paste0(prefix, "+ ", BOLD(name.gdsn(node, fullname)), "   ",
            lText, " ", UNDERLINE(n$trait))

        # if logical, factor, list, or data.frame
        at <- get.attr.gdsn(node)
        if (n$type == "Logical")
        {
            s <- paste0(s, BLURRED(",logical"))
        } else if (n$type == "Factor")
        {
            s <- paste0(s, BLURRED(",factor"))
        } else if ("R.class" %in% names(at))
        {
            if (n$trait != "")
                s <- paste0(s, BLURRED(","))
            if (!is.null(at$R.class))
                s <- paste0(s, BLURRED(paste(at$R.class, collapse=",")))
        }

        # show the dimension
        if (!is.null(n$dim))
        {
            s <- paste0(s, " ")
            s <- paste0(s, UNDERLINE(paste(n$dim, collapse="x")))
        }

        # show compression
        if (is.character(n$encoder))
        {
            if (n$encoder != "")
            {
                if (attribute)
                    s <- paste(s, BLURRED(n$compress))
                else
                    s <- paste(s, BLURRED(n$encoder))
            }
        }
        if (is.numeric(n$cpratio))
        {
            if (is.finite(n$cpratio))
            {
                if (n$cpratio >= 0.10)
                    s <- paste0(s, BLURRED(sprintf("(%0.1f%%)", 100*n$cpratio)))
                else
                    s <- paste0(s, BLURRED(sprintf("(%0.2f%%)", 100*n$cpratio)))
            }
        }

        if (is.finite(n$size))
            s <- paste0(s, BLURRED(", "), BLURRED(.Call(gdsFmtSize, n$size)))

        if (length(at) > 0L)
            s <- paste(s, rText, "*")
        else
            s <- paste0(s, " ", rText)

        if (attribute & ((length(at)>0L) | !is.null(n$param)))
        {
            if (!is.null(n$param))
            {
                b <- paste(names(n$param), format(n$param, justify="none"),
                    sep=": ")
                b <- paste(b, collapse="; ")
                b <- paste0("[", b, "] ")
            } else
                b <- ""

            if (length(at) > 0L)
            {
                a <- paste(names(at), format(at, justify="none"), sep=": ")
                if (attribute.trim)
                {
                    for (i in which(nchar(a) > 32L))
                        a[i] <- paste(substr(a[i], 1L, 32L), "...")
                }
                a <- paste(a, collapse="; ")
                if (attribute.trim)
                {
                    if (nchar(a) > 64L)
                        a <- paste(substr(a, 1L, 64L), "...")
                }
            } else
                a <- ""

            s <- paste0(s, "< ", BLURRED(paste0(b, a)))
        }

        s <- paste0(s, "\n")
        cat(s)

        if (expand)
        {
            nm <- ls.gdsn(node, include.hidden=all)
            for (i in seq_along(nm))
            {
                n <- nchar(prefix)
                if (i < length(nm))
                {
                    if (n >= 3L)
                    {
                        if (last)
                            s <- paste0(substr(prefix, 1L, n-3L), "   |--")
                        else
                            s <- paste0(substr(prefix, 1L, n-3L), "|  |--")
                    } else
                        s <- "|--"
                } else {
                    if (n >= 3L)
                    {
                        if (last)
                            s <- paste0(substr(prefix, 1L, n-3L), "   \\--")
                        else
                            s <- paste0(substr(prefix, 1L, n-3L), "|  \\--")
                    } else
                        s <- "\\--"
                }
                enum(index.gdsn(node, nm[i]), s, FALSE, i>=length(nm))
            }
        }
    }

    # check
    stopifnot(inherits(x, "gdsn.class"))
    stopifnot(is.logical(all), length(all)==1L)
    stopifnot(is.logical(expand), length(expand)==1L)

    .Call(gdsNodeValid, x)
    enum(x, "", TRUE, TRUE)

    invisible()
}



##############################################################################
# Unit testing
##############################################################################

#############################################################
# Run all unit tests
#
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
