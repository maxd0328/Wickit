
# Wickit Bytecode

## 1. Bytecode File Structure

The following gives the structure of Wickit bytecode (opp) files, which are compiled asset files. The table indicates what each range of bytes in the file is used for.

Note that every number in this document is assumed to be **hexadecimal**, unless otherwise specified. Furthermore, note that all static symbol references are constructed from the root of the module, not the root of the asset file.

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 01 | `OPP_SIGNATURE` | Used to validate that the file is an OPP file, always equal to `0xef01` |
| 02 | 03 | `VER_MAJ` | Major version of the compiler used to compile this file |
| 03 | 04 | `VER_MIN` | Minor version of the compiler used to compile this file |
| 05 | 0c | `COM_POSIX` | Unix timestamp of when this file was compiled |
| 0d | 10 | `SRC_CHKSUM` | CRC32 checksum of the original source file used to compile this file |
| 11 | 12 | `INIT_PTR` | Pointer to static property initializer function in constant table (`CFNLIT`) |
| 13 | 16 | `LEN_DECLTBL` (`LD`) | Length in bytes of the declaration table |
| 17 | 1a | `LEN_CONSTTBL` (`LC`) | Length in bytes of the constant table |
| 1b | `1a + LD` | `DECLTBL` | The declaration table for this OPP file |
| `1b + LD` | `1a + LD + LC` | `CONSTTBL` | The constant table for this OPP file |
| `1b + LD + LC` | ... | `BYTECODE_POOL` | Pool of bytecode instructions, referenced by the declaration table` |

Bytes 00 to 1a are considered the *OPP Header*.

## 2. Declaration Table

The declaration table is responsible for specifying static symbols declared in the asset file. Every entry has an entry type which is determined by an entry signature in the first byte, as well as an arbitrary number of fields determined by the entry type and any other auxilary fields.

### 2.1 Access Level Byte

Access levels in OPP files are specified with single bytes, and the following gives a table of possible values for this byte and their corresponding interpretations.

| Access Byte Value (Binary) | Interpretation |
| ----------------- | -------------- |
| 0000 0000 | `public` |
| 0000 0001 | `restricted` |
| 0000 0010 | `private` |

Note that while all other values of the access level byte are illegal, it is especially illegal to have a value of `fe` (1111 1110), as this could cause misinterpretations in the template property sub-table.

### 2.2 Generic Argument Sub-Table

Many declarations use a generic argument sub-table, which contains 2-byte entries specified by the following table.

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| ----> 00 | 01 | `GX_SATISFIES` | Pointer to satisfaction requirement type in constant table (`CTYPE`), or `0` if none |

### 2.3 Type Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Type declaration signature = `00` |
| 01 | 01 | `ACCESS_LVL` | The access level of the type declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the type declaration in constant table (`CUTF8`) |
| 04 | 05 | `DECL_TYPE` | Pointer to the declared type in constant table (`CTYPE`) |
| 06 | 09 | `LEN_GXTBL` (`LG`) | Length of the generic argument sub-table |
| 0a | `09 + LG` | `GXTBL` | Generic argument sub-table (see section 2.2) |

### 2.4 Namespace Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Namespace declaration signature = `01` |
| 01 | 01 | `ACCESS_LVL` | The access level of the namespace declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the namespace in constant table (`CUTF8`) |
| 04 | 07 | `LEN_DECLTBL` (`LD`) | Length in bytes of declaration sub-table |
| 08 | `07 + LD` | `DECLTBL` | Declaration sub-table for static symbols declared in the namespace |

### 2.5 Contract Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Contract declaration signature = `02` |
| 01 | 01 | `ACCESS_LVL` | The access level of the contract declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the contract in constant table (`CUTF8`) |
| 04 | 05 | `EXT_TYPE` | Pointer to the type this contract extends in constant table (`CTYPE`), or `0` if none |
| 06 | 09 | `LEN_GXTBL` (`LG`) | Length of the generic argument sub-table |
| 0a | 0d | `LEN_PROPTBL` (`LP`) | Length in bytes of property sub-table |
| 0e | 12 | `LEN_DECLTBL` (`LD`) | Length in bytes of declaration sub-table |
| 13 | `12 + LG` | `GXTBL` | Generic argument sub-table (see section 2.2) |
| `13 + LG` | `12 + LG + LP` | `PROPTBL` | Property sub-table for contract properties (details below) |
| `13 + LG + LP` | `12 + LG + LP + LD` | `DECLTBL` | Declaration sub-table for inner static symbols declared in this contract |

#### Property Sub-Table

The property sub-table is made up of 5-byte entries, and a table for each entry is given below.

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ACCESS_LVL` | The access level of the contract declaration |
| 01 | 02 | `DECL_NAME` | Pointer to the name of the property in constant table (`CUTF8`) |
| 03 | 04 | `DECL_TYPE` | Pointer to the type of the property in constant table (`CTYPE`) |

### 2.6 Constructor Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Constructor declaration signature = `03` |
| 01 | 01 | `ACCESS_LVL` | The access level of the constructor declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the constructor in constant table (`CUTF8`), or `0` if template constructor |
| 04 | 05 | `DECL_TYPE` | Pointer to the type of this constructor (`CTYPE`, must be a function type) |
| 06 | 07 | `DECL_IMPL` | Pointer to the bytecode implementation of this constructor in constant table (`CFNLIT`) |
| 08 | 0b | `LEN_GXTBL` (`LG`) | Length of the generic argument sub-table |
| 0c | `0b + LG` | `GXTBL` | Generic argument sub-table (see section 2.2) |

### 2.7 Switch Constructor Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Switch constructor declaration signature = `04` |
| 01 | 01 | `ACCESS_LVL` | The access level of the switch constructor declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the constructor in constant table (`CUTF8`), or `0` if template constructor |
| 04 | 07 | `LEN_CASETBL` (`LC`) | Length in bytes of the case sub-table |
| 08 | `07 + LC` | Case sub-table for each constructor case (details below) |

The case sub-table is made up of entries which are identical to bytes 01-0b+ of a constructor declaration (see section 2.6).

### 2.8 Template Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Template declaration signature = `05`, or `06` for partial templates |
| 01 | 01 | `ACCESS_LVL` | The access level of the template declaration |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the template in constant table (`CUTF8`) |
| 04 | 05 | `EXT_TYPE` | Pointer to the type this template extends in constant table (`CTYPE`), or `0` if none |
| 06 | 09 | `LEN_GXTBL` (`LG`) | Length of the generic argument sub-table |
| 0a | 0d | `LEN_PROPTBL` (`LP`) | Length in bytes of property sub-table |
| 0e | 12 | `LEN_DECLTBL` (`LD`) | Length in bytes of declaration sub-table |
| 13 | `12 + LG` | `GXTBL` | Generic argument sub-table (see section 2.2) |
| `13 + LG` | `12 + LG + LP` | `PROPTBL` | Property sub-table for template properties (details below) |
| `13 + LG + LP` | `12 + LG + LP + LD` | `DECLTBL` | Declaration sub-table for inner static symbols declared in this template |

Note that the property sub-table for non-partial templates is identical to the property sub-table for contracts. However, for partial templates, they are 5 or 6-byte entries, with the 6th being an optional prefix byte with a value of `ef` to indicate a partial property.

Furthermore, note that the template constructor is declared as a static symbol in the declaration sub-table as a constructor or switch constructor declaration with a `0` in its `DECL_NAME` field.

### 2.9 Static Property Declaration

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Static property declaration signature = `07` |
| 01 | 01 | `ACCESS_LVL` | The access level of the static property |
| 02 | 03 | `DECL_NAME` | Pointer to the name of the static property in constant table (`CUTF8`) |
| 04 | 05 | `DECL_TYPE` | Pointer to the type of the static property in constant table (`CTYPE`) |

## 3. Constant Table

The constant table is responsible for defining any constants used either in the bytecode pool or in the declaration table. Constants include literals, strings (also technically literals in some cases), and references. Like the declaration table, every entry starts with a single byte signature to indicate the type of constant being defined, with an arbitrary number of bytes per entry decided by the entry type signature.

The constant table is referenced throughout the OPP file using 16-bit indices. Note that the constant table is indexed starting at `1`, as `0` often indicates *none*. Hence, the constant table can have a maximum of 2^16 - 1 entries.

### 3.1 UTF-8 String `CUTF8`

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | UTF-8 string signature = `00` |
| 01 | 02 | `CONST_LEN` | Length in bytes of the character sequence |
| 03 | `02 + CONST_LEN` | UTF-8 character sequence |

### 3.2 Type Expression `CTYPE`

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Type expression signature = `01`, or `02` for optional type |
| 01 | 04 | `LEN_DISJTBL` | Length in bytes of the disjunction sub-table |
| 05 | `04 + LEN_DISJTBL` | `DISJTBL` | The disjunction sub-table (details below) |

#### Disjunction Sub-Table

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 03 | `LEN_CONJTBL` | Length in bytes of the conjunction sub-table |
| 04 | `03 + LEN_CONJTBL` | `CONJTBL` | The conjunction sub-table (details below) |

#### Conjunction Sub-Table

The conjunction sub-table has a unique set of entry signatures for different unit types. The unit types are inline contracts, functions, switch functions, and type references. A table is given for each:

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `UNIT_SIG` | Contract type unit signature = `00` |
| 01 | 04 | `LEN_PROPTBL` (`LP`) | Length in bytes of the property sub-table |
| 05 | `04 + LP` | `PROPTBL` | Property sub-table (immediately below) |
| ----> 00 | 01 | `PROP_NAME` | Pointer to the name of the property in constant table (`CUTF8`) |
| ----> 02 | 03 | `PROP_TYPE` | Pointer to the type of the property in constant table (`CTYPE`) |

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `UNIT_SIG` | Function type unit signature = `01` |
| 01 | 02 | `TYPE_RET` | Pointer to the return type of the function in constant table (`CTYPE`) |
| 03 | 06 | `LEN_ARGTBL` (`LA`) | Length in bytes of the argument sub-table |
| 07 | 0a | `LEN_GXTBL` (`LG`) | Length in bytes of the generic argument sub-table |
| 07 | `06 + LA` | `ARGTBL` | Argument sub-table (immediately below) |
| ----> 00 | 01 | `ARG_TYPE` | Pointer to the type of the argument in constant table (`CTYPE`) |
| `07 + LA` | `06 + LA + LG` | `GXTBL` | Generic argument sub-table (immediately below) |
| ----> 00 | 01 | `GX_SATISFIES` | Pointer to satisfaction requirement type in constant table (`CTYPE`), or `0` if none |

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `UNIT_SIG` | Switch function type unit signature = `02` |
| 01 | 04 | `LEN_CASETBL` (`LC`) | Length in bytes of the case sub-table |
| 05 | `04 + LC` | `CASETBL` | Case sub-table (immediately below) |
| ----> 00 | 01 | `TYPE_RET` | Pointer to the return type of the case function in constant table (`CTYPE`) |
| ----> 02 | 05 | `LEN_ARGTBL` (`LA`) | Length in bytes of the argument sub-table |
| ----> 06 | 09 | `LEN_GXTBL` (`LG`) | Length in bytes of the generic argument sub-table |
| ----> 0a | `09 + LA` | `ARGTBL` | Argument sub-table (immediately below) |
| ----> ----> 00 | 01 | `ARG_TYPE` | Pointer to the type of the argument in constant table (`CTYPE`) |
| ----> `0a + LA` | `09 + LA + LG` | `GXTBL` | Generic argument sub-table (immediately below) |
| ----> ----> 00 | 01 | `GX_SATISFIES` | Pointer to satisfaction requirement type in constant table (`CTYPE`), or `0` if none |

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `UNIT_SIG` | Type reference unit signature = `03` |
| 01 | 02 | `SYM_REF` | Pointer to symbol reference in constant table (`CUTF8`) |
| 03 | 06 | `LEN_GXTBL` (`LG`) | Length in bytes of generic type sub-table |
| 07 | `06 + LG` | `GXTBL` | Generic type sub-table (immediately below) |
| ----> 00 | 01 | `TYPE_GX` | Pointer to generic type in constant table (`CTYPE`) |

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `UNIT_SIG` | Generic type reference unit signature = `04` |
| 01 | 04 | `INDEX_GX` | Index of referenced generic type starting from furthest scope |

### 3.3 Function Literal `CFNLIT`

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Function literal signature = `03` |
| 01 | 04 | `INSTR_OFF` | Offset in bytes at which the function begins in the bytecode pool |
| 05 | 06 | `INSTR_LEN` | Length in bytes of the function in the bytecode pool |

### 3.4 All Numeric Literals

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | Literal signature (detailed below) |
| 01 | \*X | `LIT_VAL` | Value of the literal |

\*X refers to the size of the literal value, see below.

The following table gives the entry signatures for each type of numeric literal, as well as the size in bytes of the literal value:

| Entry Type | Entry Signature | Literal Size | Description |
| ---------- | --------------- | ------------ | ----------- |
| `CUINTLIT` | `04` | 4 | Unsigned Integer Literal |
| `CINTLIT` | `05` | 4 | Signed Integer Literal |
| `CULNGLIT` | `06` | 8 | Unsigned Long Literal |
| `CLNGLIT` | `07` | 8 | Signed Long Literal |
| `CFLTLIT` | `08` | 4 | Float Literal |
| `CDBLLIT` | `09` | 8 | Double Literal |

### 3.5 String Literal `CSTRLIT`

| First Byte | Last Byte | Field Name | Description |
| ---------- | --------- | ---------- | ----------- |
| 00 | 00 | `ENTRY_SIG` | String literal signature = `0a` |
| 01 | 02 | `LIT_LEN` | Length of the string literal in bytes (number of characters * 2) |
| 03 | `02 + LIT_LEN` | Value of the string literal as a UTF-16 string |

## 4. Bytecode Instruction Listing

| Opcode | Pneumonic | Arguments | Stack | Description |
| ------ | --------- | --------- | ----- | ----------- |
| 00 | nop | | | Performs no operation. |
| 01 | null | | -> `null` | Pushes `null` onto the stack |
| 02 | new | | -> `{}` | Pushes a new object onto the stack |
| 03 | store | 1: index | *ref* -> | Stores value in local variable #index |
| 04 | storew | 2: indexL, indexH | *value* -> | Stores value in local variable #indexH:#indexL |
| 05 | load | 1: index | -> *value* | Loads value from local variable #index |
| 06 | loadw | 2: indexL, indexH | -> *value* | Loads value from local variable #indexH:#indexL |
| 07 | const | 1: index | -> *value* | Loads value from constant table at #index |
| 08 | constw | 2: indexL, indexH | -> *value* | Loads value from constant table at #index |
| 09 | invoke | | *ref*, *...args* -> *result* | Invokes a function by reference with arguments and pushes return value if not void |
| 0a | getprop | 1: index | *ref* -> *value* | Gets property from reference identified by constant pool at #index |
| 0b | getpropw | 2: indexL, indexH | *ref* -> *value* | Gets property from reference identified by constant pool at #indexH:#indexL |
| 0c | setprop | 1: index | *ref*, *value* -> | Sets property from reference identified by constant pool at #index |
| 0d | setpropw | 2: indexL, indexH | *ref*, *value* -> | Sets property from reference identified by constant pool at #indexH:#indexL |
| 0e | invokecon | 1: index | *ref*, *...args* -> *ref* | Invokes constructor identified by constant table at #index on reference with arguments |
| 0f | invokeconw | 2: indexL, indexH | *ref* -> *ref* | Invokes constructor identified by constant table at #indexH:#indexL on reference with arguments |
| 10 | this | | -> *ref* | Pushes a reference to `this` onto the stack, if applicable |
| 11 | goto | 1: index | | Jumps to the instruction at branch index #index of the current function |
| 12 | gotow | 2: indexL, indexH | | Jumps to the instruction at branch index #indexH:#indexL of the current function |
| 13 | satisfies | 1: index | *ref* -> *value* | Pushes `true` if reference satisfies type in constant table at #index, otherwise `false` |
| 14 | satisfiesw | 2: indexL, indexH | *ref* -> *value* | Pushes `true` if reference satisfies type in constant table at #indexH:#indexL, otherwise `false` |
| 15 | checktype | 1: index | *ref* -> *ref* | Throws an error if reference does not satisfy type in constant table at #index |
| 16 | checktypew | 2: indexL, indexH | *ref* -> *ref* | Throws an error if reference does not satisfy type in constant table at #indexH:#indexL |
| 17-37 | \*operator-invocations | | *ref*, *...args* -> *result* | Fetches and invokes an `operator` property from reference with given extra arguments |
| 38 | refequ | | *ref0*, *ref1* -> *result* | Pushes `true` if two references are equal, otherwise `false` |
| 39 | refneq | | *ref0*, *ref1* -> *result* | Pushes `true` if two references are not equal, otherwise `false` |
| 3a | gotoif | 1: index | *ref* -> | Jumps to instruction at branch index #index if reference, assumed to satisfy `Bool`, is true |
| 3b | gotoifw | 2: indexL, indexH | *ref* -> | Jumps to instruction at branch index #indexH:#indexL if reference, assumed to satisfy `Bool`, is true |
| 3c | gotoiftruthy | 1: index | *ref* -> | Jumps to instruction at branch index #index if reference is truthy* |
| 3d | gotoiftruthyw | 2: indexL, indexH | *ref* -> | Jumps to instruction at branch index #indexH:#indexL if reference is truthy* |
| 3e | gotoifnull | 1: index | *ref* -> | Jumps to instruction at branch index #index if reference is null |
| 3f | gotoifnullw | 2: indexL, indexH | *ref* -> | Jumps to instruction at branch index #indexH:#indexL if reference is null |
| 40 | gotoifnonnull | 1: index | *ref* -> | Jumps to instruction at branch index #index if reference is non-null |
| 41 | gotoifnonnullw | 2: indexL, indexH | *ref* -> | Jumps to instruction at branch index #indexH:#indexL if reference is non-null |
| 42 | ubconst | 1: value | -> *ref* | Pushes a reference to an unsigned byte with a given value |
| 43 | bconst | 1: value | -> *ref* | Pushes a reference to a signed byte with a given value |
| 44 | usconst | 1: value | -> *ref* | Pushes a reference to an unsigned short with a given value |
| 45 | usconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to an unsigned short with value #valueH:#valueL |
| 46 | sconst | 1: value | -> *ref* | Pushes a reference to a signed short with a given value |
| 47 | sconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to a signed short with value #valueH:#valueL |
| 48 | uconst | 1: value | -> *ref* | Pushes a reference to an unsigned integer with a given value |
| 49 | uconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to an unsigned integer with value #valueH:#valueL |
| 4a | iconst | 1: value | -> *ref* | Pushes a reference to a signed integer with a given sign-extended value |
| 4b | iconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to a signed integer with sign-extendev value #valueH:#valueL |
| 4c | ulconst | 1: value | -> *ref* | Pushes a reference to an unsigned long with a given value |
| 4d | ulconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to an unsigned long with value #valueH:#valueL |
| 4e | lconst | 1: value | -> *ref* | Pushes a reference to a signed long with a given sign-extended value |
| 4f | lconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to a signed long with sign-extended value #valueH:#valueL |
| 50 | blconst_true | | -> *ref* | Pushes a reference to `true` onto the stack |
| 51 | blconst_false | | -> *ref* | Pushes a reference to `false` onto the stack |
| 52 | chconst | 1: value | -> *ref* | Pushes a reference to a character with a given value |
| 53 | chconstw | 2: valueL, valueH | -> *ref* | Pushes a reference to a character with value #valueH:#valueL |
| 54 | dup | | *value* -> *value*, *value* | Duplicates a value on the stack |
| 55 | return | | | Returns from the function with no return value |
| 56 | vreturn | | *value* -> | Returns from the function with the top of the stack as the return value |

### 4.1 Operator Invocation Instructions

| Opcode | Pneumonic | Argument Count | Operator Property Name |
| ------ | --------- | -------------- | ---------------------- |
| 17 | add | 1 | `operator+` |
| 18 | sub | 1 | `operator-` |
| 19 | mul | 1 | `operator*` |
| 1a | div | 1 | `operator/` |
| 1b | mod | 1 | `operator%` |
| 1c | and | 1 | `operator&` |
| 1d | or | 1 | `operator\|` |
| 1e | xor | 1 | `operator^` |
| 1f | shl | 1 | `operator<<` |
| 20 | shr | 1 | `operator>>` |
| 21 | lnot | 0 | `operator!` |
| 22 | not | 0 | `operator~` |
| 23 | pos | 0 | `operator\+` |
| 24 | neg | 0 | `operator\-` |
| 25 | equ | 1 | `operator==` |
| 26 | neq | 1 | `operator!=` |
| 27 | grt | 1 | `operator>` |
| 28 | lst | 1 | `operator<` |
| 29 | gte | 1 | `operator>=` |
| 2a | lte | 1 | `operator<=` |
| 2b | addeq | 1 | `operator+=` |
| 2c | subeq | 1 | `operator-=` |
| 2d | muleq | 1 | `operator*=` |
| 2e | diveq | 1 | `operator/=` |
| 2f | modeq | 1 | `operator%=` |
| 30 | andeq | 1 | `operator&=` |
| 31 | oreq | 1 | `operator\|=` |
| 32 | xoreq | 1 | `operator^=` |
| 33 | shleq | 1 | `operator<<=` |
| 34 | shreq | 1 | `operator>>=` |
| 35 | inc | 0 | `operator++` |
| 36 | dec | 0 | `operator--` |
| 37 | index | 1 | `operator[]` |

## 5. Limitations

This format creates a number of limitations on what can be compiled, and such a list is given below:

 * The size of the declaration table may not exceed 2^32 bytes (~4GB).
 * The size of the constant table may not exceed 2^32 bytes (~4GB).
 * The size of the bytecode pool may not exceed 2^32 bytes (~4GB).
 * The number of entries in the constant table may not exceed 2^16 - 1 (65535).
 * The size of any given function may not exceed 2^16 bytes (65536).
 * The size of a string may not exceed may not exceed 2^16 bytes (65536). Note that for UTF-16 string literals this equates to 32768 characters, and for UTF-8 identifier strings it could be anywhere between 16384 and 65536 characters.
