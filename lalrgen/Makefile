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
SOURCES = $(wildcard $(SRCDIR)/*.java)
CLASSES = $(SOURCES:$(SRCDIR)/%.java=$(BINDIR)/%.class)

# ALL = build classes
all: $(CLASSES)

# Build each class from source
$(CLASSES): $(BINDIR)/%.class : $(SRCDIR)/%.java
	@mkdir -p $(BINDIR)
	$(JAVAC) -cp $(CLASSPATH) -d $(BINDIR) $<

# Run LALR generator
run: $(CLASSES)
	$(JAVA) -cp $(CLASSPATH) Main

# Delete build directory
clean:
	@rm -rf $(BINDIR)

.PHONY: all run clean

# ---------------------------------------------------------------------------
# Max Derbenwick 2023
# ---------------------------------------------------------------------------
