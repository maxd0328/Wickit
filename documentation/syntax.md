
# Wickit Syntax

The following gives the offical grammar for the Wickit language. Note that the grammar below follows the Backus-Naur Form (BNF), making use of the following annotations:

 * `{x}` - Zero or more occurrences of x
 * `[x]` - Zero or one occurrence of x
 * `(x|y)` - One of x or y (or others if present)

Furthermore, note the following cases of terms:

 * `CamelCase` - Refers to a non-terminal
 * `SCREAMING_SNAKE_CASE` - Refers to a regex terminal (see the language alphabet for details)
 * Other - Refers to a definitive terminal

## 1. General

```
CompilationUnit:
    {ImportDeclaration} {GlobalScopeDeclaration}

ImportDeclaration:
    import StaticSymbol [.*] ;

GlobalScopeDeclaration:
    {Modifier} PropertyDeclaration
    {Modifier} TypeDeclaration
    {Modifier} ContractDeclaration
    {Modifier} FunctionDeclaration
    {Modifier} SwitchFunctionDeclaration
    {Modifier} TemplateDeclaration
    {Modifier} NamespaceDeclaration
    {Modifier} ConstructorDeclaration
    {Modifier} SwitchConstructorDeclaration
```

## 2. Declarations

```
PropertyDeclaration:
    AnyVarDecl ;

AbstractPropertyDeclaration:
    AbstractVarDecl ;

ValuedPropertyDeclaration:
    ValuedVarDecl ;

TypeDeclaration:
    type Identifier [GenericTypeDeclarator] as Type ;

ContractDeclaration:
    contract Identifier [GenericTypeDeclarator] [extends TypeList] { { GlobalScopeDeclaration } }

FunctionDeclaration:
    function Identifier [GenericTypeDeclarator] ([Parameters]): (Type|void) Block

SwitchFunctionDeclaration:
    switch function Identifier { { SwitchFunctionCase } }

SwitchFunctionCase:
    {Modifier} Identifier [GenericTypeDeclarator] ([Parameters]): (Type|void) Block

ConstructorDeclaration:
    constructor [GenericTypeDeclarator] ([Parameters]): (Type|void) Block

SwitchConstructorDeclaration:
    switch constructor Identifier { { SwitchFunctionCase } }

TemplateDeclaration:
    template Identifier [GenericTypeDeclarator] [extends TypeList] { { TemplateScopeDeclaration } }

TemplateScopeDeclaration:
    GlobalScopeDeclaration
    {Modifier} TemplateConstructorDeclaration
    {Modifier} TemplateSwitchConstructorDeclaration

TemplateConstructorDeclaration:
    constructor [GenericTypeDeclarator] ([Parameters]) Block

TemplateSwitchConstructorDeclaration:
    switch constructor Identifier { { TemplateSwitchConstructorCase } }

TemplateSwitchConstructorCase:
    {Modifier} Identifier [GenericTypeDeclarator] ([Parameters]) Block

NamespaceDeclaration:
    namespace Identifier { { GlobalScopeDeclaration } }

Modifier:
    partial
    static
    public
    restricted
    private
```

## 3. Types

```
Type:
    TypeDisjunction

TypeDisjunction:
    TypeConjunction { | TypeConjunction }

TypeConjunction:
    FunctionType { & TypeConjunction }

FunctionType:
	([TypeList]) [GenericTypeDeclarator] -> (PostfixType|void)
	PostfixType -> (PostfixType|void)
	PostfixType

PostfixType:
    UnitType { ([]|?) }

UnitType:
    StaticSymbol [GenericTypeSpecifier]
    ContractType
    SwitchFunctionType
    (Type)

ContractType:
    contract [extends TypeList] { { AbstractPropertyDeclaration } }

SwitchFunctionType:
    switch function { { AbstractSwitchFunctionCase } }

AbstractSwitchFunctionCase:
    {Modifier} Identifier([TypeList]): (Type|void) ;

GenericTypeDeclarator:
    < GenericType { , GenericType } >

GenericType:
    Identifier [ satisfies Type ]

GenericTypeSpecifier:
    <TypeList>
```

## 4. Expressions

```
Expression:
    Expression1 [AssignmentOperator Expression]

AssignmentOperator (',' to delimit):
    =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=,
    :=, :+=, :-=, :*=, :/=, :%=, :&=, :|=, :^=, :<<=, :>>=

Expression1:
    Expression2 [ ? Expression : Expression1 ]

Expression2:
    Expression3 { || Expression3 }

Expression3:
    Expression4 { && Expression4 }

Expression4:
    Expression5 { | Expression5 }

Expression5:
    Expression6 { ^ Expression6 }

Expression6:
    Expression7 { & Expression7 }

Expression7:
    Expression8 { (==|!=) Expression8 }

Expression8:
    Expression9 { Expression8Rest }

Expression8Rest:
    (>|>=|<|<=) Expression9
    satisfies Type

Expression9:
    Expression10 { (<<|>>) Expression10 }

Expression10:
    Expression11 { (+|-) Expression11 }

Expression11:
    Expression12 { (*|/|%) Expression12 }

Expression12:
    (Type) Expression12
    Expression13

Expression13:
    (+|-|!|~|++|--) Expression13
    Expression14

Expression14:
    Expression15 { (++|--) }

Expression15:
    Expression16 { Expression15Rest }

Expression15Rest:
    .PropertyName
    [GenericTypeSpecifier] ([Expressions])
    [Expression] (Actual subscript [], not BNF)

Expression16:
    PropertyName
    PrimitiveLiteral
    ArrayLiteral
    ObjectLiteral
    FunctionLiteral
    this
    new ConstructorInvocation
    (Expression)

ArrayLiteral:
    [ [Expressions] ] (Outer actual brackets [], not BNF)

ObjectLiteral:
    { ObjectScopeDeclaration } (Actual braces {}, not BNF)

ObjectScopeDeclaration:
    ValuedPropertyDeclaration
    FunctionDeclaration
    SwitchFunctionDeclaration

FunctionLiteral:
    function [GenericTypeDeclarator] ([Parameters]): (Type|void) Block
    [GenericTypeDeclarator] ArrowParameters [ : (Type|void) ] -> (Expression|Block)

ArrowParameters:
    ([ InferredAbstractVarDecl { , InferredAbstractVarDecl } ])
    Identifier

ConstructorInvocation:
    StaticSymbol [GenericTypeSpecifier] [ ([Expressions]) ] [ ObjectLiteral ]
```

PropertyName:
    Identifier
    constructor
    conflict

## 5. Statements

```
Block:
    { { BlockStatement } }

BlockStatement:
    Statement
    LocalVariableDeclaration
    TypeDeclaration
    ContractDeclaration
    FunctionDeclaration
    SwitchFunctionDeclaration
    TemplateDeclaration

LocalVariableDeclaration:
    var AnyVarDecl ;

Statement:
    Block
    ;
    Expression ;
    # Identifier Statement
    if(Expression) Statement [else Statement]
    while(Expression) Statement
    do Statement while(Expression) ;
    for(ForCondition) Statement
    switch(Expression) { SwitchCaseGroups }
    break [Identifier] ;
    continue [Identifier] ;
    return [Expression] ;
    throw Expression ;
    try Statement {CatchClause} [finally Statement]
    delegate ConstructorInvocation [ as Identifier ] ;

ForCondition:
    var Identifier [ : Type ] ForConditionRest
    [Expressions] ; [Expression] ; [Expressions]

ForConditionRest:
    [ = Expression ] { , AnyVarDecl } ; [Expression] ; [Expressions]
    in Expression

SwitchCaseGroups:
    { SwitchCaseGroup }

SwitchCaseGroup:
    SwitchLabel { BlockStatement }

SwitchLabel:
    case Expression:
    default:

CatchClause:
    catch [ ( [ Identifier: ] Type) ] Statement
```

## 6. Miscellaneous

```
Identifier:
    IDENTIFIER
    operator OverloadableOperator

PrimitiveLiteral:
    BOOL_LITERAL
    INT_LITERAL
    FLOAT_LITERAL
    CHARACTER_LITERAL
    STRING_LITERAL

StaticSymbol:
    Identifier { . Identifier }

TypeList:
    Type { , Type }

Parameters:
    AbstractVarDecl { , AbstractVarDecl }

Expressions:
    Expression { , Expression }

AnyVarDecl:
    Identifier: Type [ = Expression ]
    Identifier = Expression

AbstractVarDecl:
    Identifier: Type

InferredAbstractVarDecl:
    Identifier [ : Type ]

ValuedVarDecl:
    Identifier [ : Type ] = Expression

OverloadableOperator: (comma separated)
    +, -, *, /, %, &, |, ^, <<, >>, !, ~, \+,
    \-, ==, !=, >, <, >=, <=, +=, -=, *=, /=,
    %=, &=, |=, ^=, <<=, >>=, ++, --, []
```

## 7. Interpretations

The following gives some interpretations and patterns drawn from the above syntax.

### 7.1 Expression Operator Precedence

| Level | Operator | Description         | Associativity |
| ----- | -------- | ------------------- | ------------- |
| 16 | `()`<br>`new` | Parentheses<br>Instantiation | Left-to-right |
| 15 | `.`<br>`[]`<br>`()` | Member access<br>Subscript<br>Function call | Left-to-right
| 14 | `++`<br>`--` | Post-increment<br>Post-decrement | Left-to-right
| 13 | `+`<br>`-`<br>`!`<br>`~`<br>`++`<br>`--` | Unary plus<br>Unary minus<br>Logical not<br>Bitwise not<br>Pre-increment<br>Pre-decrement | Right-to-left
| 12 | `()` | Type cast | Right-to-left |
| 11 | `* / %` | Multiplicative | Left-to-right |
| 10 | `+ -` | Additive | Left-to-right |
| 9 | `<< >>` | Shift | Left-to-right |
| 8 | `> >= < <=`<br>`satisfies` | Relational<br>Satisfaction | Left-to-right |
| 7 | `== !=` | Equality | Left-to-right |
| 6 | `&` | AND | Left-to-right |
| 5 | `^` | XOR | Left-to-right |
| 4 | `\|` | OR | Left-to-right |
| 3 | `&&` | Lazy AND | Left-to-right |
| 2 | `\|\|` | Lazy OR | Left-to-right |
| 1 | `?:` | Ternary | Right-to-left |
| 0 | `= ?= := :?=` | Assignment | Right-to-left |

### 7.2 Type Operator Precedence

| Level | Operator | Description         | Associativity |
| ----- | -------- | ------------------- | ------------- |
| 4 | `.`<br>`()` | Static access<br>Parentheses | Left-to-right |
| 3 | `[]`<br>`?` | Array postfix<br>Optional postfix | Left-to-right |
| 2 | `->` | Function type | N/A |
| 1 | `&` | Conjunction | Left-to-right |
| 0 | `\|` | Disjunction | Left-to-right |
