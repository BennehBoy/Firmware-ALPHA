# List of all the board related files.
BOARDSRC = $(CHIBIOS)/hw/FK407M1/board.c

# Required include directories
BOARDINC = $(CHIBIOS)/hw/FK407M1

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)