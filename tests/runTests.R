# ===========================================================================
#
# runTests.R: unit testing
#

library(BiocGenerics)

BiocGenerics:::testPackage("gdsfmt")

# quit
q("no")
