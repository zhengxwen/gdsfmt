# ===========================================================================
#
# methods.r: generic methods for GDS object
#
# Copyright (C) 2015    Xiuwen Zheng
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
# Class Register
##############################################################################

# register old-style (S3) classes and inheritance
setOldClass("gds.class")
setOldClass("gdsn.class")



#############################################################
# show methods
#

# read and drop upper dimensions
.get <- function(node, start, count)
{
    v <- read.gdsn(node, start=start, count=count, simplify="none")
    if (!is.null(dm <- dim(v)))
    {
        if (length(dm) > 2L)
            dim(v) <- dm[c(1L,2L)]
    }
    if (is.factor(v)) v <- as.character(v)
    v
}

# view 2-dim array
.view_dim2 <- function(dm, node, st, nprev, BLURRED)
{
    if (any(dm[1L] <= 0L, dm[2L] <= 0L))
    {
        cat("\n")
        return(invisible())
    }

    cn <- rep(1L, length(st))
    if (dm[1L] <= nprev*2L)
    {
        if (dm[2L] <= nprev*2L)
        {
            v <- .get(node, c(1L,1L,st), c(-1L,-1L,cn))
        } else {
            v <- cbind(
                .get(node, c(1L,1L,st), c(-1L,nprev,cn)),
                .get(node, c(1L,dm[2L]-nprev+1L,st), c(-1L,nprev,cn))
            )
        }
    } else {
        if (dm[2L] <= nprev*2L)
        {
            v <- rbind(
                .get(node, c(1L,1L,st), c(nprev,-1L,cn)),
                .get(node, c(dm[1L]-nprev+1L,1L,st), c(nprev,-1L,cn))
            )
        } else {
            v1 <- cbind(
                .get(node, c(1L,1L,st), c(nprev,nprev,cn)),
                .get(node, c(1L,dm[2L]-nprev+1L,st), c(nprev,nprev,cn))
            )
            v2 <- cbind(
                .get(node, c(dm[1L]-nprev+1L,1L,st), c(nprev,nprev,cn)),
                .get(node, c(dm[1L]-nprev+1L,dm[2L]-nprev+1L,st), c(nprev,nprev,cn))
            )
            v <- rbind(v1, v2)
        }
    }

    s <- format(v)
    if (dm[2L] > nprev*2L)
    {
        s <- s[, c(1L:nprev, NA, seq(nprev+1L,ncol(s))), drop=FALSE]
        s[, nprev+1L] <- BLURRED("..")
    }
    if (dm[1L] > nprev*2L)
    {
        s <- s[c(1L:nprev, NA, seq(nprev+1L,nrow(s))), , drop=FALSE]
        s[nprev+1L, ] <- ""
        s[nprev+1L, 1L] <- BLURRED("......")
    }

    write.table(s, col.names=FALSE, row.names=FALSE, quote=FALSE)
    invisible()
}

# view >2-dim array
.view_dim <- function(i, st, dm, node, nprev, BLURRED, UNDERLINE)
{
    if (i > length(dm))
    {
        cat(UNDERLINE(sprintf("[,,%s]:\n", paste(st, collapse=","))))
        .view_dim2(dm, node, st, nprev, BLURRED)
    } else {
        for (j in seq_len(min(dm[i], nprev)))
        {
            st2 <- c(st, j)
            .view_dim(i + 1L, st2, dm, node, nprev, BLURRED, UNDERLINE)
        }
    }
    invisible()
}


setMethod("show", signature(object="gdsn.class"),
    function(object)
    {
        print(object, attribute=TRUE)

        n <- getOption("gds.preview.num", 6L)
        if (.crayon())
        {
            INVERSE <- crayon::inverse
            UNDERLINE <- crayon::silver
            BLURRED <- crayon::blurred
        } else {
            INVERSE <- UNDERLINE <- BLURRED <- `c`
        }

        dp <- objdesp.gdsn(object)
        dm <- dp$dim
        if (dp$is.array & !is.null(dm))
        {
            cat(INVERSE("Preview:\n"))
            if (length(dm) == 1L)
            {
                if (dm <= 0L)
                {
                    s <- ""
                } else if (dm <= n*2L)
                {
                    s <- format(read.gdsn(object))
                } else {
                    s <- format(c(.get(object,1L,n), .get(object, dm-n+1L, n)))
                    s <- s[c(1L:n, NA, seq(n+1L, length(s)))]
                    s[n+1L] <- BLURRED("...")
                }
                cat(s, sep="\n")
            } else if (length(dp$dim) == 2L)
            {
                .view_dim2(dm, object, NULL, n, BLURRED)
            } else {
                .view_dim(3L, NULL, dm, object, n, BLURRED, UNDERLINE)
            }
        }
        invisible()
    }
)
