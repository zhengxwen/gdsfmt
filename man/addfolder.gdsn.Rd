\name{addfolder.gdsn}
\alias{addfolder.gdsn}
\title{Add a folder to the GDS node}
\description{
	Add a directory or a virtual folder to the GDS node.
}

\usage{
addfolder.gdsn(node, name, type=c("directory", "virtual"), gds.fn="",
	replace=FALSE)
}

\arguments{
	\item{node}{an object of class \code{\link{gdsn.class}} or
		\code{\link{gds.class}}}
	\item{name}{the variable name of the added node; if it is not specified,
		``\code{Item} \code{N}'' is assigned to \code{name}, where \code{N}
		is the number of child nodes plus one}
	\item{type}{"directory" (default) -- create a directory of GDS node;
		"virtual" -- create a virtual folder linking another GDS file by
		mapping all of the content to this virtual folder}
	\item{gds.fn}{the name of another GDS file; it is applicable only if
		\code{type="virtual"}}
	\item{replace}{if \code{TRUE}, replace the existing variable silently
		if possible}
}

\value{
	An object of class \code{\link{gdsn.class}}.
}

\references{\url{http://github.com/zhengxwen/gdsfmt}}
\author{Xiuwen Zheng}
\seealso{
	\code{\link{add.gdsn}}, \code{\link{addfile.gdsn}}
}

\examples{
# create the first GDS file
f1 <- createfn.gds("test1.gds")

add.gdsn(f1, "NULL")
addfolder.gdsn(f1, "dir")
add.gdsn(f1, "int", 1:100)
f1

# open the GDS file
closefn.gds(f1)

##############################################

# create the second GDS file
f2 <- createfn.gds("test2.gds")

add.gdsn(f2, "int", 101:200)

# link to the first file
addfolder.gdsn(f2, "virtual_folder", type="virtual", gds.fn="test1.gds")

f2

# open the GDS file
closefn.gds(f2)


##############################################

# open the second file (writable)
(f <- openfn.gds("test2.gds", FALSE))
# + 	[  ]
# |--+ int	{ Int32 100 }
# |--+ virtual_folder	[ --> test1.gds ]
# |  |--+ NULL	    
# |  |--+ dir	[  ]
# |  |--+ int	{ Int32 100 }

read.gdsn(index.gdsn(f, "int"))
read.gdsn(index.gdsn(f, "virtual_folder/int"))
add.gdsn(index.gdsn(f, "virtual_folder/dir"), "nm", 1:10)

f

# open the GDS file
closefn.gds(f)


##############################################
# open 'test1.gds', there is a new variable "dir/nm"

(f <- openfn.gds("test1.gds"))
closefn.gds(f)


##############################################
# remove 'test1.gds'

file.remove("test1.gds")

\dontrun{(f <- openfn.gds("test2.gds"))}
# + 	[  ]
# |--+ int	{ Int32 100 }
# |--+ virtual_folder	[ -X- test1.gds ]

\dontrun{closefn.gds(f)}


# delete the temporary file
unlink("test.gds", force=TRUE)
}

\keyword{GDS}
\keyword{utilities}
