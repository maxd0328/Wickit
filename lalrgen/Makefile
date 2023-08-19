# ---------------------------------------------------------------------------
# Wickit LALR Parse Table Generator v1.0.0
# 
# Build output = ./build/lalrgen
# Max Derbenwick 2023
# ---------------------------------------------------------------------------

# Define compiler commands
JAVAC = javac
JAVA = java

# Specify source and build directories
SRCDIR = src
BINDIR = ../build/lalrgen

# Specify classpath and sources
CLASSPATH = $(BINDIR)
SOURCES = $(SRCDIR)/*.java $(SRCDIR)/io/*.java $(SRCDIR)/lalr/*.java
CLASSES = $(patsubst $(SRCDIR)/%.java,$(BINDIR)/%.class,$(SOURCES))

# ALL = build classes
all: $(CLASSES)

# Create the build directory
$(BINDIR):
	@mkdir -p $(BINDIR)

# Build each class from source
$(CLASSES): $(SOURCES) | $(BINDIR)
	$(JAVAC) -cp $(CLASSPATH) -d $(BINDIR) $(SOURCES)

# Delete build directory
clean:
	@rm -rf $(BINDIR)

.PHONY: all run clean

# ---------------------------------------------------------------------------
# Max Derbenwick 2023
# ---------------------------------------------------------------------------
