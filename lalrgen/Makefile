# ---------------------------------------------------------------------------
# Wickit LALR Parse Table Generator v1.0.0
# 
# Build output = ./build/lalrgen
# Max Derbenwick 2023
# ---------------------------------------------------------------------------

# Define compiler commands
JAVAC		= javac
JAVA		= java
JAVACFLAGS	= -g

# Specify source and build directories
SRCDIR = src
BINDIR = ../build/lalrgen

# Specify classpath and sources
CLASSPATH = $(BINDIR)
SOURCES = $(SRCDIR)/*.java $(SRCDIR)/io/*.java $(SRCDIR)/lalr/*.java
CLASSES = $(BINDIR)/classes.stamp

# ALL = build classes
all: $(CLASSES)

# Create the build directory
$(BINDIR):
	@mkdir -p $(BINDIR)

# Build each class from source
$(CLASSES): $(SOURCES) | $(BINDIR)
	$(JAVAC) $(JAVACFLAGS) -cp $(CLASSPATH) -d $(BINDIR) $(SOURCES)
	@touch $(CLASSES)

# Delete build directory
clean:
	@rm -rf $(BINDIR)

.PHONY: all run clean

# ---------------------------------------------------------------------------
# Max Derbenwick 2023
# ---------------------------------------------------------------------------
