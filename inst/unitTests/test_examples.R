#############################################################
#
# DESCRIPTION: run all examples in gdsfmt
#

library(gdsfmt)


#############################################################
#
# test functions
#

function.list <- c(
	"add.gdsn",
	"addfile.gdsn",
	"addfolder.gdsn",
	"append.gdsn",
	"apply.gdsn",
	"assign.gdsn",
	"cache.gdsn",
	"cleanup.gds",
	"closefn.gds",
	"clusterApply.gdsn",
	"cnt.gdsn",
	"compression.gdsn",
	"copyto.gdsn",
	"createfn.gds",
	"delete.attr.gdsn",
	"delete.gdsn",
	"diagnosis.gds",
	"getfolder.gdsn",
	"gds.class",
	"gdsfmt-package",
	"gdsn.class",
	"get.attr.gdsn",
	"getfile.gdsn",
	"index.gdsn",
	"is.element.gdsn",
	"lasterr.gds",
	"ls.gdsn",
	"moveto.gdsn",
	"name.gdsn",
	"objdesp.gdsn",
	"openfn.gds",
	"permdim.gdsn",
	"print.gds.class",
	"print.gdsn.class",
	"put.attr.gdsn",
	"read.gdsn",
	"readex.gdsn",
	"readmode.gdsn",
	"rename.gdsn",
	"setdim.gdsn",
	"showfile.gds",
	"sync.gds",
	"system.gds",
	"write.gdsn"
)


test.examples <- function()
{
	sapply(function.list, FUN = function(func.name)
		{
			args <- list(
				topic=func.name,
				package="gdsfmt",
				echo=FALSE, verbose=FALSE, ask=FALSE
			)
			suppressWarnings(do.call(example, args))
			NULL
		})
	invisible()
}
