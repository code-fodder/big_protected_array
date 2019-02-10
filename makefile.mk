#######################################################################
# Include the common makefiles:
#   - Variables:     Sets up the variables with some default values
include make_utils/common_variables.mk
#######################################################################

# Project Name
PROJECT_NAME = big_array

# Location of source files
SOURCE_DIRS = \
	. \

#Includes paths
INC_DIRS = \
	. \

#######################################################################
# Include the common makefiles:
#   - shared_lib:     Sets the shared library options
#   - warnings:       Set the warning flags for various targets
#   - var_autofill:   Generates lists of source, objects etc
#   - rules:          The build rules
include make_utils/common_executable.mk
include make_utils/common_warnings.mk
include make_utils/common_var_autofill.mk
include make_utils/common_rules.mk
#######################################################################
