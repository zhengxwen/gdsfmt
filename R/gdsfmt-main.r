# ===========================================================================
#
# gdsfmt-main.r: R Interface to CoreArray Genomic Data Structure (GDS) files
#
# Copyright (C) 2011-2015    Xiuwen Zheng [zhengx@u.washington.edu]
#
# This is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License Version 3 as
# published by the Free Software Foundation.
#
# CoreArray is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with CoreArray.
# If not, see <http://www.gnu.org/licenses/>.



##############################################################################
# File Operations
##############################################################################

#############################################################
# Create a new CoreArray Genomic Data Structure (GDS) file
#
createfn.gds <- function(filename, allow.duplicate=FALSE)
{
    stopifnot(is.character(filename) & is.vector(filename))
    stopifnot(length(filename) == 1)
    stopifnot(is.logical(allow.duplicate))

    # 'normalizePath' does not work if the file does not exist
    tmpf <- file(filename, "wb")
    close(tmpf)

    filename <- normalizePath(filename, mustWork=FALSE)
    ans <- .Call(gdsCreateGDS, filename, allow.duplicate)
    names(ans) <- c("filename", "id", "root", "readonly")
    ans$filename <- filename
    class(ans$root) <- "gdsn.class"
    class(ans) <- "gds.class"
    ans
}


#############################################################
# Open an existing file
#
openfn.gds <- function(filename, readonly=TRUE, allow.duplicate=FALSE,
    allow.fork=FALSE)
{
    stopifnot(is.character(filename) & is.vector(filename))
    stopifnot(length(filename) == 1)

    filename <- normalizePath(filename, mustWork=FALSE)
    ans <- .Call(gdsOpenGDS, filename, readonly, allow.duplicate,
        allow.fork)
    names(ans) <- c("filename", "id", "root", "readonly")
    ans$filename <- filename
    class(ans$root) <- "gdsn.class"
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
    stopifnot(is.character(filename) & is.vector(filename))
    stopifnot(length(filename) == 1)

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

    if (length(rv) > 0)
    {
        nm <- NULL; rd <- NULL
        for (i in 1:length(rv))
        {
            names(rv[[i]]) <- c("filename", "id", "root", "readonly")
            class(rv[[i]]$root) <- "gdsn.class"
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
        for (i in 1:length(rv))
            closefn.gds(rv[[i]])
        rv <- NULL
    }

    invisible(rv)
}


#############################################################
# Diagnose the GDS file
#
diagnosis.gds <- function(gdsfile)
{
    stopifnot(inherits(gdsfile, "gds.class"))

    # call C function
    rv <- .Call(gdsDiagInfo, gdsfile)

    names(rv) <- c("stream", "log")
    rv$stream <- as.data.frame(rv$stream, stringsAsFactors=FALSE)
    colnames(rv$stream) <- c("id", "size", "capacity", "num.chunk", "path")

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
    stopifnot(is.character(newname) & is.vector(newname))
    stopifnot(length(newname) == 1)

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
    stopifnot(is.logical(silent) & is.vector(silent))
    stopifnot(length(silent) == 1)

    ans <- .Call(gdsNodeIndex, node, path, index, silent)
    if (!is.null(ans))
        class(ans) <- "gdsn.class"
    ans
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
        "good", "message", "param")
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
        name <- paste("Item", cnt.gdsn(node)+1, sep="")
    stopifnot(is.character(name) & is.vector(name))
    stopifnot(length(name) == 1)

    stopifnot(is.character(storage) & is.vector(storage))
    stopifnot(length(storage) == 1)

    stopifnot(is.character(compress) & is.vector(compress))
    stopifnot(length(compress) > 0)
    compress <- compress[1L]

    stopifnot(is.logical(closezip) & is.vector(closezip))
    stopifnot(length(closezip) == 1)

    stopifnot(is.logical(check) & is.vector(check))
    stopifnot(length(check) == 1)

    stopifnot(is.logical(replace) & is.vector(replace))
    stopifnot(length(replace) == 1)

    stopifnot(is.logical(visible) & is.vector(visible))
    stopifnot(length(visible) == 1)

    # call C function
    ans <- .Call(gdsAddNode, node, name, val, storage, valdim, compress,
        closezip, check, replace, list(...))
    class(ans) <- "gdsn.class"

    if (storage == "list")
    {
        nm <- class(val)
        if (!identical(nm, "data.frame") & !("list" %in% nm))
            nm <- c(nm, "list")
        put.attr.gdsn(ans, "R.class", nm)

        nm <- names(val)
        for (i in 1:length(nm))
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

    if (!visible)
        put.attr.gdsn(ans, "R.invisible")

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
        name <- paste("Item", cnt.gdsn(node)+1, sep="")
    stopifnot(is.character(name) & is.vector(name))
    stopifnot(length(name) == 1)

    type <- match.arg(type)
    if (type == "virtual")
    {
        stopifnot(is.character(gds.fn) & is.vector(gds.fn))
        stopifnot(length(gds.fn) == 1)
        stopifnot(!is.na(gds.fn))
        stopifnot(gds.fn != "")
    }

    stopifnot(is.logical(replace) & is.vector(replace))
    stopifnot(length(replace) == 1)

    stopifnot(is.logical(visible) & is.vector(visible))
    stopifnot(length(visible) == 1)

    # call C function
    ans <- .Call(gdsAddFolder, node, name, type, gds.fn, replace)
    class(ans) <- "gdsn.class"

    if (!visible)
        put.attr.gdsn(ans, "R.invisible")

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
        name <- paste("Item", cnt.gdsn(node)+1, sep="")
    stopifnot(is.character(name) & is.vector(name))
    stopifnot(length(name) == 1)

    stopifnot(is.character(filename) & is.vector(filename))
    stopifnot(length(filename) == 1)

    stopifnot(is.character(compress) & is.vector(compress))
    stopifnot(length(compress) > 0)
    compress <- compress[1L]

    stopifnot(is.logical(replace) & is.vector(replace))
    stopifnot(length(replace) == 1)

    stopifnot(is.logical(visible) & is.vector(visible))
    stopifnot(length(visible) == 1)

    # call C function
    ans <- .Call(gdsAddFile, node, name, filename, compress, replace)
    class(ans) <- "gdsn.class"

    if (!visible)
        put.attr.gdsn(ans, "R.invisible")

    invisible(ans)
}


#############################################################
# Get a file from a stream container
#
getfile.gdsn <- function(node, out.filename)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.character(out.filename) & is.vector(out.filename))
    stopifnot(length(out.filename) == 1)

    .Call(gdsGetFile, node, out.filename)
    invisible()
}


#############################################################
# Delete a specified node
#
delete.gdsn <- function(node, force=FALSE)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.logical(force) & is.vector(force))
    stopifnot(length(force) == 1)

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
    stopifnot(is.character(name) & is.vector(name))
    stopifnot(length(name) == 1)

    .Call(gdsPutAttr, node, name, val)
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
    stopifnot(is.character(name) & is.vector(name))
    stopifnot(length(name) == 1)

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
    stopifnot(is.character(compress) & is.vector(compress))
    stopifnot(length(compress) > 0)
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
# Append data to a specified variable
#
append.gdsn <- function(node, val, check=TRUE)
{
    stopifnot(inherits(node, "gdsn.class"))

    .Call(gdsObjAppend, node, val, check)
    invisible()
}


#############################################################
# Read data field of a GDS node
#
read.gdsn <- function(node, start=NULL, count=NULL,
    simplify=c("auto", "none", "force"), .useraw=FALSE)
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
                cnt <- cnt.gdsn(node)
                r <- vector("list", cnt)
                if (cnt > 0)
                {
                    for (i in 1:cnt)
                    {
                        n <- index.gdsn(node, index=i)
                        r[[i]] <- read.gdsn(n)
                        names(r)[i] <- name.gdsn(n)
                    }
                }

                if (identical(rvclass, "data.frame"))
                {
                    r <- as.data.frame(r, stringsAsFactors=FALSE)
                } else {
                    rvclass <- setdiff(rvclass, "list")
                    if (length(rvclass) > 0)
                        class(r) <- rvclass
                }

                return(r)
            }
        }
    }

    .Call(gdsObjReadData, node, start, count, simplify, .useraw)
}


#############################################################
# Read data field of a GDS node
#
readex.gdsn <- function(node, sel=NULL, simplify=c("auto", "none", "force"),
    .useraw=FALSE)
{
    stopifnot(inherits(node, "gdsn.class"))
    simplify <- match.arg(simplify)

    if (!is.null(sel))
    {
        stopifnot(is.logical(sel) | is.list(sel))
        if (is.logical(sel)) sel <- list(d1=sel)
        # read
        .Call(gdsObjReadExData, node, sel, simplify, .useraw)
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
    target.node=NULL, .useraw=FALSE, ...)
{
    # check
    if (inherits(node, "gdsn.class"))
    {
        stopifnot(inherits(node, "gdsn.class"))
        stopifnot(is.numeric(margin) & (length(margin)==1))
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
        for (i in 1:length(node))
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
            for (i in seq_len(length(target.node)))
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
        selection, as.is, var.index, .useraw, target.node, new.env())

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
    var.index=c("none", "relative", "absolute"), .useraw=FALSE, ...)
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
    stopifnot(is.character(gds.fn) & (length(gds.fn)==1))
    stopifnot(is.character(node.name))
    stopifnot(is.numeric(margin) & (length(margin)==length(node.name)))
    margin <- as.integer(margin)

    if (!is.null(selection))
    {
        if (!is.list(selection[[1]]))
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
    for (i in 1:length(nd_nodes))
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
    MarginCount <- sum(new.selection[[1]][[ margin[1] ]], na.rm=TRUE)
    if (MarginCount <= 0)
        return(invisible())


    #########################################################
    # run
    #

    if (length(cl) > 1)
    {
        # close the GDS file
        closefn.gds(gfile)
        on.exit()

        clseq <- parallel::splitIndices(MarginCount, length(cl))
        sel.list <- vector("list", length(cl))
        start <- 1L

        # for - loop: multi processes
        for (i in 1:length(cl))
        {
            n <- length(clseq[[i]])
            if (n > 0L)
            {
                tmp <- new.selection

                # for - loop: multiple variables
                for (j in 1:length(tmp))
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
                for (i in 1:length(nd_nodes))
                    nd_nodes[[i]] <- index.gdsn(gfile, path=node.name[i])

                # call C function -- set starting index
                .Call(gdsApplySetStart, item$start)
                # call C function -- apply calling
                .Call(gdsApplyCall, nd_nodes, margin, FUN, item$sel, as.is,
                    var.index, .useraw, NULL, new.env())

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
assign.gdsn <- function(dest.obj, src.obj, append=TRUE)
{
    stopifnot(inherits(dest.obj, "gdsn.class"))
    stopifnot(inherits(src.obj, "gdsn.class"))
    stopifnot(is.logical(append) & is.vector(append))
    stopifnot(length(append) == 1)

    # call C function
    .Call(gdsAssign, dest.obj, src.obj, append)

    invisible()
}


#############################################################
# Caching the data associated with a GDS variable
#
cache.gdsn <- function(node)
{
    stopifnot(inherits(node, "gdsn.class"))

    # call C function
    .Call(gdsCache, node)
    invisible()
}


#############################################################
# move to a new location
#
moveto.gdsn <- function(node, loc.node,
    relpos = c("after", "before", "replace"))
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(inherits(loc.node, "gdsn.class"))
    relpos <- match.arg(relpos)
    relpos <- match(relpos, c("after", "before", "replace"))

    # call C function
    .Call(gdsMoveTo, node, loc.node, relpos)
    if (relpos == 3L)
    {
        nm <- name.gdsn(loc.node)
        delete.gdsn(loc.node)
        rename.gdsn(node, nm)
    }

    invisible()
}


#############################################################
# whether elements in node
#
is.element.gdsn <- function(node, set)
{
    stopifnot(inherits(node, "gdsn.class"))
    stopifnot(is.vector(set))
    stopifnot(is.numeric(set) | is.character(set))

    # call C function
    .Call(gdsIsElement, node, set)
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
        encoder = rv$compression.encoder[seq(1, length(s), 2)],
        description = rv$compression.encoder[seq(2, length(s), 2)],
        stringsAsFactors = FALSE)
    rv$class.list <- data.frame(rv$class.list, stringsAsFactors=FALSE)
    colnames(rv$class.list) <- c("name", "description")
    rv$class.list <- rv$class.list[order(rv$class.list$description), ]
    rownames(rv$class.list) <- NULL
    rv
}



##############################################################################
# R Generic functions
##############################################################################

print.gds.class <- function(x, all=FALSE, ...)
{
    # check
    stopifnot(inherits(x, "gds.class"))
    stopifnot(is.logical(all) & is.vector(all))
    stopifnot(length(all) == 1L)

    .Call(gdsFileValid, x)
    cat("File: ", x$filename, "\n", sep="");
    print(x$root, all=all, ...)
}

print.gdsn.class <- function(x, expand=TRUE, all=FALSE, ...)
{
    enum <- function(node, space, level, expand, fullname)
    {
        at <- get.attr.gdsn(node)
        if (!all)
        {
            if ("R.invisible" %in% names(at))
                return(invisible())
        }

        n <- objdesp.gdsn(node)
        if (n$type == "Label")
        {
            lText <- " "; rText <- " "
        } else if (n$type == "VFolder")
        {
            lText <- if (n$good) "[ -->" else "[ -X-"
            rText <- "]"
        } else if (n$type == "Folder")
        {
            lText <- "["; rText <- "]"
        } else if (n$type == "Unknown")
        {
            lText <- "  -X-"; rText <- ""
        } else {
            lText <- "{"; rText <- "}"
        }
        cat(space, "+ ", name.gdsn(node, fullname), "   ",
            lText, " ", n$trait, sep="")

        # if logical, factor, list, or data.frame
        if (n$type == "Logical")
        {
            cat(",logical")
        } else if (n$type == "Factor")
        {
            cat(",factor")
        } else if ("R.class" %in% names(at))
        {
            if (n$trait != "")
                cat(",")
            if (!is.null(at$R.class))
                cat(paste(at$R.class, sep="", collapse=","))
        }

        # show the dimension
        if (!is.null(n$dim))
        {
            cat(" ")
            cat(n$dim, sep="x")
        }

        # show compression
        if (is.character(n$encoder))
        {
            if (n$encoder != "") cat("", n$encoder)
        }
        if (is.numeric(n$cpratio))
        {
            if (is.finite(n$cpratio))
                cat(sprintf("(%0.2f%%)", 100*n$cpratio))
        }

        if (length(at) > 0)
            cat(" ", rText, " *\n", sep="")
        else
            cat(" ", rText, "\n", sep="")

        if (expand)
        {
            cnt <- cnt.gdsn(node)
            if (cnt > 0)
            {
                for (i in seq_len(cnt))
                {
                    m <- index.gdsn(node, index=i)
                    if (level==1L)
                        s <- paste("|--", space, sep="")
                    else
                        s <- paste("|  ", space, sep="")
                    enum(m, s, level+1L, TRUE, FALSE)
                }
            }
        }
    }

    # check
    stopifnot(inherits(x, "gdsn.class"))
    stopifnot(is.logical(all) & is.vector(all))
    stopifnot(length(all) == 1L)
    stopifnot(is.logical(expand) & is.vector(expand))
    stopifnot(length(expand) == 1L)

    .Call(gdsNodeValid, x)
    enum(x, "", 1L, expand, TRUE)

    invisible()
}





##############################################################################
# Unit testing
##############################################################################

#############################################################
# Run all unit tests
#
gdsUnitTest <- function()
{
    # load R packages
    if (!requireNamespace("RUnit"))
        stop("Please install RUnit package!")

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
