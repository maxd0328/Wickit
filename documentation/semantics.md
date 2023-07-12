
# Wickit Semantics

This document provides information on all semantic rules and behaviors of the Wickit language, including type checking, symbols, and declarations.

## 1. Types

A type in Wickit boils down to a set of origins and properties with corresponding types. An origin is a constructor that *must* be executed during the creation of an object if it is to satisfy that type. Consider the following example of a Person type:

```
[flags] {
    origin Person,
    name: String,
    age: Int
}
```

In Wickit, origins are associated with templates (discussed in section 2.9). A template when referred to as a type always includes itself as an origin. That is, in order to satisfy a template, the object must originate from that template's constructor.

Flags refer to certain behaviors of the type, and only flag exists so far in Wickit:

 * `switch-behavior`: Indicates that the type is callable via all function properties, determined at compile time based on arguments passed.

All reachable properties of an object with a given type is referred to as the *property space* of that type. Note that property space is recursive in that it includes not only properties included in the type, but also all properties accessible from the properties, and so forth, given each property's type.

The *shallow property space* is defined as non-recursive, so that it contains only the properties directly included in the type.

### 1.1 Type Unions

A type union, denoted as `A|B`, refers to a type which is either A or B (or both). Note that this concept can extend to more than two types as well (left associated). In a type union, the property spaces of the two unioned types are *intersected* to give the resultant property space.

More technically, this means a couple of things. Firstly, the shallow property spaces are intersected, leaving only properties that are common to both of them. Secondly, the types of the common properties are unioned, using the very type unions being discussed currently. And finally, the origins and flags of the types are intersected, leaving only common origins and flags.

For example, consider the following types:

```
{
    origin Person,
    name: String,
    age: Int
}

{
    employeeID: Int,
    age: Double
}
```

The union of these two types would be:

```
{
    age: Int|Double
}
```

### 1.2 Type Intersections

Type intersections, denoted as `A&B`, are much the opposite of type unions. A type intersection refers to a type that is both A and B, and as many other types as desired (again left associated). In a type intersection, the property spaces of the intersected types are *unioned* to give the resultant property space.

As with type unions, this means the following. Firstly, the shallow property space of the intersected types are unioned, hence including properties from both types (only one instance of common properties) in the resultant shallow property space. Secondly, the types of common properties are themselves intersected. And finally, the origins and flags of the types are unioned, therefore requiring all origins and flags in the resultant type.

Considering the two types from the previous example, the intersection of these two types would be:

```
{
    origin Person,
    name: String,
    employeeID: Int,
    age: Int&Double
}
```

### 1.3 Generic Types

Certain contracts (section 2.6) and templates (section 2.9), can have generic type arguments. This means that they accept some kind of generic type potentially used in one of their properties. When using contracts or templates as a type, the generic type must be *filled in* which is done with the following notation: `Type<GenericType>`, or `Type<GenericType1, GenericType2>` in the event of multiple generic type arguments.

### 1.4 Arrays

Arrays can be applied as a suffix to any type, indicating an array of that type, as is the same in many other languages. They are notated as `[]`, and can be nested to create arrays of arrays, and so forth.

Note that arrays are not primitive, and are simply an alias for the `Array` template. For example, the type `Int[]` is treated by the compiler as `Array<Int>`, and `String[][]` is treated by the compiler as `Array<Array<Int>>`.

### 1.5 Optionals

Optional types refer to values that are either of the prescribed type, or null. They are notated with a `?` at the end of the type. The property space of an optional type is always the empty set, and optional types have no origins. Hence, optional types in their primary states are rendered useless, unless *realized* through type widening, which is discussed in section 3 and section 3.1.

Note that optionals can be used at the end of any type except for type declarations and type extension declarations on contracts and templates.

### 1.6 Object Types

Individual object types in Wickit are referred to as contracts, as they represent the smallest unit of requirement. As such, they specify a certain set of properties and their corresponding types that an object must contain in order to satisfy it. They are defined as follows:

```
contract {
    name: String;
    age: Int;
}
```

Furthermore, contracts can be declared to *extend* other contracts, which is done like so:

```
contract extends A, B {
    name: String,
    age: Int
}
```

This defines a set of supertypes of the contract. Then, the contract will be intersected with the intersection of the supertypes. And hence, the contract will also satisfy each supertype. In this case, the contract would be an extension of `A & B`. This is referred to as the complete supertype. Note that cyclic extensions are not allowed.

Contracts are free to redefine properties present in their supertypes, as long as the property space of any redefined property is a superset of the property space of that property's type according to the complete supertype.

Finally, it is worth noting that extensions of contracts allow the avoidance of redeclaring properties, but implicit satisfaction will occur either way. If contract A's property space is a superset of contract B's property space, then A implicitly extends B, and hence also satisfies B.

### 1.7 Function Types

As an exception to standard types, or object types as they will now be referred to, there also exist *function types*. A function type represents a function with a given list of inputs and one given output. Some examples of how they are notated are as follows:

```
() -> void
Int -> Float
(Int, String) -> void
```

Note that a function can return either a type or `void`, indicating that it returns nothing. Additionally, if there is only one argument, the parentheses can be omitted.

Functions have special rules when unioned or intersected with other functions, which are as follows:

 * Union between two function types:
   * Argument types intersected, extra arguments included, return type unioned
 * Intersection between two function types:
   * Argument types unioned, extra arguments made optional, return type intersected

When functions are unioned or intersected with object types, the functions are treated as objects with a single property `call` with a type equal to the function type in question. Type union and intersection then proceeds as normal.

With this in mind, note that a function type `(A...) -> B` is always analagous to the following type when interfacing with objects:

```
{
    call: (A...) -> B;
}
```

Hence, if any type satisfies this contract, it also satisfies `(A...) -> B`. Note that for any primitive function `x`, it is always true that `x == x.call`.

### 1.8 Switch Function Types

In addition to regular functions, switch functions are Wickit's solution to function overloading. A switch function is a function with different cases, where a case is defined by a name, and can have its own argument types and return type. Switch functions are callable, and it will be determined at compile time which case to call based on the arguments passed. If it is ambiguous, the compiler will require that the case name be used.

It is worth noting that behind the scenes, switch functions and objects are no different. The only difference is at compile time, where switch functions are objects with heavy restrictions: they may only have function properties, may not extend anything, and may not be partial or have static properties. Additionally, they are treated as callable objects, where at compile time the call is deferred to one of the cases.

They are defined as such:

```
switch function {
    case1(Int, String): Int;
    case2(String): String;
}
```

When it comes to unions and intersections, switch functions are treated the same as objects. However, they possess the `switch-behavior` flag, which, as discussed in section 1.1, indicates whether or not the type defers calls to its function properties, hence making it callable if not already.

It should be noted that if an object has the `switch-behavior` flag and `callable` flag, the former takes precedence and hence the `call` property is simply treated as another case function.

### 1.9 Combining Type Operators

Type operators can be combined in any order to create new types, with the exception of the *optional* suffix, which can only be placed once at the end of a type. Some examples can be found below:

```
(() -> void) | Int
Int & String ?
(A | B) & C
A | B | C | D
```

## 2. Declarations

Wickit contains a system of static declarations to define types and other structures which hold information and behaviors. Recall from the documentation on modules that all symbols declared in an asset file (Wickit source file) are declared immediately in the file's containing package as defined by the modulefile. Wickit static declarations allow a wide variety of symbols to be declared and also organized within the package.

### 2.1 Namespaces

The simplest static symbol that can be defined in Wickit is a *namespace*. A namespace is simply a container which holds other static symbols, and are treated as local packages within an asset file. The syntax is shown below:

```
namespace X {

}
```

### 2.2 Property Declarations

A property declaration, used both statically and non-statically, declares a certain property of a certain type to exist in the scope it's declared. Properties are not constant, meaning they can be assigned new values at any time.

When declaring a property, a name, type, and optionally value must be provided. If a value is provided and its type is unambiguous, then the property type may be omitted.

Property declarations are written like so:

```
propertyName: Type = SomeValue;
```

### 2.3 Function Declarations

A function declaration is used to declare a function in a static or non-static context. It includes named arguments and return values. It is written as follows:

```
function myFunction(x: Int, y: Int): void {

}
```

Note that function declarations are just a shorthand for declaring functions, and the above example is equivalent to:

```
myFunction = (x: Int, y: Int): void -> {

}
```

### 2.4 Switch Function Declarations

Like function declarations, switch function declarations are used to declare switch functions in a static or non-static context. Recall the definition of a switch function from section 1.8. They are written as follows:

```
switch function mySwitchFunction {
    case1(x: Int, y: Int): void { ... }
    case2(x: String): void { ... }
}
```

As with function declarations, this is just a shorthand for a property declaration:

```
mySwitchFunction: switch function {
    case1(x: Int, y: Int): void;
    case2(x: String): void;
} = { (x, y) -> { ... }, (x) -> { ... } }
```

### 2.5 Type Declarations

Another simple static symbol that can be defined in Wickit is a *type declaration*, also referred to as a type alias. These allow for complex types to be condensed into a single identifier. An example is shown below:

```
type X as SomeTypeExpressionHere;
```

### 2.6 Contract Declarations

Contract declarations are shorthands for declaring a type as a contract type, while also serving as a namespace for other static symbol declaration. Hence, properties (as well as function and switch function declarations) can be given the `static` modifier to indicate that they exist as a part of the contract's namespace. Note that symbol declarations of always-static entities (constructor x, switch constructor x, namespace, contract, template, and type) should not marked with `static`, and for consistency reasons an error is thrown when attempting to do so. Otherwise, they're considered part of the contract type.

It is also important to note that non-static properties in a contract declaration are purely abstract, and hence cannot be given values. An error is thrown if a value is given. By extension, all function and switch function declarations in a contract must be labeled `static`, or else they will be treated as function property declarations and will throw the same error.

Contract declarations are written as shown:

```
contract A {
    x: Int;
    y: Int;
}
```

Contracts can also have generic type parameters, which define arbitrary types that can be used in the contract. For example:

```
contract A<T, J> {
    x: T;
    y: J;
}
```

In the above example, generic type `T` is used as the type of property `x`. When using `A` as a type, the generic type argument must be filled in: `A<..., ...>`.

Generic type arguments can also be defined to have a property space which a superset of a known property space, which can be achieved by writing `<X satisfies SomeType>` as a generic type argument. In this case, `X` is known to satisfy `SomeType` at compile time, and hence `SomeType`'s properties can be accessed.

### 2.7 Constructor Declarations

A constructor is a special kind of function that kind only be declared as a static symbol, and serves to initialize an object to satisfy a given type. Any defined constructor can be invoked to create and return a new object with the expression `new MyConstructor(...)`.

A constructor can be declared in a static context like so:

```
constructor myConstructor(): SomeContractToSatisfy {
    ...
}
```

Constructors always use `this` to refer to the object they initialize.

In addition to regular constructors, switch constructors can be defined similar to switch functions. They themselves are treated as namespaces which can be invoked with the `new` operator, delegating the invocation to the inner constructor with matching arguments. They are defined like so:

```
switch constructor MyConstructor {
    case0(x: Int, y: Int): SomeContractToSatisfy {...}
    case1(x: String): SomeContractToSatisfy {...}
}
```

### 2.8 Constructor Delegation and Conflict Resolution

In addition to provide their own implementation, constructors may also delegate to other constructors. A delegation to another constructor means that said constructor will immediately be called to initialize the state of the same object, and hence results in implicit satisfaction of that constructor's return type. In other words, when creating a constructor that satisfies `A`, delegating to another constructor that satisfies `A` is valid as a complete constructor.

Constructor delegation is performed the same as regular constructor invocation, but with the `delegate` keyword instead of `new`. Optionally, an `as` expression afterwards can be used to name the delegation, which is important for conflict resolution. An example is shown below:

```
constructor myConstructor(): A&B {
    delegate otherConstructor(...) as myDelegation;
    ...
}
```

It is important to note that while multiple delegations are allowed in the same constructor, all constructor delegation must occur *before* any references to `this` in the main constructor.

However, consider the following two cases:

 * Multiple delegated constructors assign different values to the same property.
 * The property space required by the main constructor of a given property is a proper superset of that property's property space assigned by a delegated constructor.

Both of these cases are called *state conflicts*, and require a manual override by the main constructor. This takes the form of an assignment to the conflicting property with the required type.

When resolving conflicts, the main constructor has access to a `conflict` property belonging to `this`, which contains the pseudovalues of conflicting properties according to each delegated constructor that took part in the conflict. The `conflict` property contains properties of its own for each *named* delegation, and those properties are objects containing all conflicting properties according to that delegation. Note that the conflict pseudovalues of a certain delegation are only available if that delegation is named using `as`.

### 2.9 Template Declarations

Templates in Wickit are self-satisfying contracts. They work as contracts with a built-in constructor, declared simply as a constructor without a name or return type. Note that like contracts, templates are also treated as namespaces and hence static symbols can be declared within using the `static` keyword in all cases except when declaring always-static entities. The following shows how a template might be declared:

```
template Person {
    name: String;
    age: Int;

    constructor(...) {
        ...
    }
}
```

The `Person` template itself is treated as a constructor, and so invoking it is done in the same way as with regular constructors: `new Person(...)`.

In addition, template types require their own constructor as an *origin*. Recall the definition of origin as a flag that a certain constructor must be invoked during the creation of an object in order for the type to be satisfied, as discussed in section 1.1. This way, an object would only satisfy type `Person` if it was created by invoking the `Person` constructor, either directly or by delegation. This gives some safety to templates, ensuring that a template's initial state is always controlled by its constructor. Note that templates also allow default property values, although behind the scenes this becomes an invisible assignment within the constructor after the delegations but before any references to `this`.

Like contracts, templates can extend any other type. And like regular constructors, they can delegate to any other constructor as well. Note that switch constructors within templates are also supported.

#### 2.9.1 Partial Templates

Wickit also supports *partial* templates, which are analogous to *abstract* structures in other languages. Partial templates allow for partial satisfaction of the template's contract in the constructor, however every property not to be satisfied must be annotated with the `partial` modifier. In addition, the template itself must be annotated as `partial`.

This in turn means that the template's constructor cannot be invoked standalone, as measures must be taken to complete the partial properties. Methods of doing this are discussed in section 3.x.

### 2.10 Access Modifiers

Wickit supports access modifiers for controlling access to both static symbols and properties of declared contracts/templates. Three static access levels are supported:

 * `public`: Visible anywhere.
 * `restricted`: Visible only within containing module.
 * `private`: Visible only within containing asset (source) file.

For static symbols this is relatively straightforward, however it is slightly more complicated for contract/template properties.

When a visibility other than `public` is chosen for a contract/template property, then the property's ID is annotated with the static location of its declaration. This allows for a type to contain multiple declarations of properties with the same name to be non-conflicting if and only if any reference to such type exists in a static domain where at most one of these properties is accessible. It is also worth noting that a non-public property cannot be initialized outside of its domain, and hence an object satisfying a contract/template containing such a property cannot be externally initialized without an internal constructor or helper function.

Note that the diamond problem with a common constructor is a special case, and could result in an unreconcilable conflict. This is because the constructor, invoked twice, may try to assign two different values to the same non-public property with the exact same annotation. This is a simple state conflict, yet one that is completely unreconcilable if the constructor responsible for conflict resolution is outside the static domain in which that property is accessible.

Given that properties with non-public visibility are only considered within their static domain, type operations only operate on accessible properties. Hence, the following rules are used during type unions and intersections:

 * Unions: properties in the shallow property space intersection will each have their more restrictive visibility chosen.
 * Intersections: properties in the shallow property space intersection will each have their more permissive visibility chosen.

From this it can be derived that any type extending a contract or template may redefine inherited properties with an equal or elevated visibility.

## 3. Using Types

Wickit is very strict in its compile-time type checking, so you can never use a property that isn't guaranteed to exist in the current context. When using an object, all that is accessible is the property space of that object's type given the current static domain. Given this, Wickit uses a series of rules for type casting, which are as follows (here it is assumed that property space also refers to extraneous type information, such as origins and flags):

 * Type narrowing is implicit, i.e. reinterpreting as type with a property space which is a subset of the original type's property space is always allowed.
 * Type widening is explicit, i.e. reinterpreting as a type with a property space which is a superset of the original type's property space requires an explicit cast, which will perform a runtime check that all necessary properties exist for such an operation.
 * Type-casting will always result in a superset property space, i.e. it can never narrow the property space in any way. Successfully casting the type `A & B` into type `C` will result in the type `A & B & C`.
 * The property space of an optional type is always the empty set. This provides complete null safety and also ensures that an optional type cannot be assigned to a non-optional type.

### 3.1 Implicit Type Widening

In many cases, type widening happens implicitly after certain checks are performed. These checks are as follows:

 * `satisfies` check (`x satisfies A`)
 * Not-null check (`x != null`)

If a given scope is only reachable by passing one of the above checks, implicit type widening occurs within that scope, which are as follows for each case:

 * `x satisfies A`: `x` is implicitly casted to `A` within the scope, which as disucussed above means its type is intersected with `A`
 * `x != null`: `x` is considered non-optional within the scope, allowing the property space of its type to be accessed.

Due to advanced control flow analysis, this extends to reverse conditionals as well, as long as a given region of code is *only* reachable if the check passes. Consider the following as an example:

```
var x: Int? = ...;

if(x == null)
    return;

// x is considered 'Int' here
```
### 3.2 Literals

Wickit contains many in-built literals, which are used for creating objects which satisfy fundamental templates. The following gives a list of in-built literals:

| Literal Type | Object Satisfies | Syntax |
| ------------ | ---------------- | ------ |
| Boolean | `Bool` | `true` or `false` |
| Character | `Char` | `'...'` |
| String | `String` | `"..."` |
| Integer | `Int` | `0x...` or `0o...` or `0b...` or `...` |
| Unsigned Byte | `UByte` | `(Integer)ub` |
| Signed Byte | `Byte` | `(Integer)b` |
| Unsigned Short | `UShort` | `(Integer)us` |
| Signed Short | `Short` | `(Integer)s` |
| Unsigned Integer | `UInt` | `(Integer)u` |
| Unsigned Long | `ULong` | `(Integer)U` |
| Signed Long | `Long` | `(Integer)L` |
| Float | `Float` | `...f` |
| Double | `Double` | `...d` or `...` |
| Array | `Array<...>` | `[..., ...]` |
| Object | `contract {...}` | `{..., ...}` |

## 4. Symbol Access

Symbol access is often ambiguous, as there can be symbols of the same name each in a different yet accessible scope. The order of precedence for symbol access is as follows:

1. Local variables, if the access occurs within a function
2. Contract/template properties, if the access occurs within a constructor, member function, or member-level expression. Note that `this` is not required, as due to the type system, properties are known at all times.
3. Symbols declared within the access' containing namespace.
4. Symbols declared in any intermediate containing namespaces, in reverse order of depth.
5. Symbols declared or imported in the asset file's global namespace.

Note that no ambiguities can occur at any one level, but may occur across levels, in which case the one with the highest precedence is chosen.

### 4.1 Imports

Wickit supports importing symbols, including pacakges, from this module or other modules. For how the package and module system works, refer to the modules documentation.

To import a symbol from the global symbol pool of the module (which includes dependencies), the following syntax is used at the beginning of the asset file:

```
import path.to.symbol;
```

Then, `symbol` becomes accessible as a static symbol in the asset file's global scope.

Additionally, all symbols directly contained within a given symbol can be imported by:

```
import path.to.symbol.*;
```

## 5. Additional Semantic Features

### 5.1 Operator Overloading

Wickit supports operator overloading for types, which takes the form of special function properties, each of which being bound to a particular operator. When an operator is used, the corresponding function property of the left most operand is invoked if it exists. An error is thrown at compile time otherwise. Hence, in these operator functions, `this` is considered as the left most operand, and all other operands are passed as arguments. This means that the number of arguments taken by an operator function must be the number of operands minus one, however the type of the arguments is free to choose. Finally, the return value and type (must not be void) of the operator function are taken as the outcome of the operation. Note that operator overloads may also be switch functions as long as every case has the correct number of arguments and is not void.

In order to declare an operator overload, a function property must be declared within a contract/template with a name of `operator` followed by the desired operator to overload. A table is given below:

| Operator | Property Name | Number of Arguments |
| -------- | ------------- | ------------------- |
| `a + b` | `operator+` | 1 |
| `a - b` | `operator-` | 1 |
| `a * b` | `operator*` | 1 |
| `a / b` | `operator/` | 1 |
| `a % b` | `operator%` | 1 |
| `a & b` | `operator&` | 1 |
| `a \| b` | `operator\|` | 1 |
| `a ^ b` | `operator^` | 1 |
| `a << b` | `operator<<` | 1 |
| `a >> b` | `operator>>` | 1 |
| `!a` | `operator!` | 0 |
| `~a` | `operator~` | 0 |
| `+a` | `operator\+` | 0 |
| `-a` | `operator\-` | 0 |
| `a == b` | `operator==` | 1 |
| `a != b` | `operator!=` | 1 |
| `a > b` | `operator>` | 1 |
| `a < b` | `operator<` | 1 |
| `a >= b` | `operator>=` | 1 |
| `a <= b` | `operator<=` | 1 |
| `a += b` or `a :+= b` | `operator+=` | 1 |
| `a -= b` or `a :-= b` | `operator-=` | 1 |
| `a *= b` or `a :*= b` | `operator*=` | 1 |
| `a /= b` or `a :/= b` | `operator/=` | 1 |
| `a %= b` or `a :%= b` | `operator%=` | 1 |
| `a &= b` or `a :&= b` | `operator&=` | 1 |
| `a \|= b` or `a :\|= b` | `operator\|=` | 1 |
| `a ^= b` or `a :^= b` | `operator^=` | 1 |
| `a <<= b` or `a :<<= b` | `operator<<=` | 1 |
| `a >>= b` or `a :>>= b` | `operator>>=` | 1 |
| `a++` or `++a` | `operator++` | 0 |
| `a--` or `--a` | `operator--` | 0 |
| `a[b]` | `operator[]` | 1 |

**A special note on the assignment and increment/decrement operators:** When using these operators, the return value of the overload function is then assigned to the variable on the left-hand side, as per assignment rules. Hence, the assignment operators can simply delegate to their respective non-assignment operator, and the increment/decrement operators can delegate to `operator+` or `operator-` with a right-hand side equal to `1`. However, these operators are still separately overloadable to open up the opportunity for more efficient solutions that don't create a new object and simply return `this`.

Finally, it is important to note that `operator` identifiers can be used **only** as non-static properties in a contract or template, and hence may not be used in a static or local context.

### 5.2 Booleans

Booleans in Wickit always satisfy the `Bool` contract, and they are special in that they are the only type other than function types that interface with the language directly. Hence, they have a special case associated with them.

Any conditional statement in Wickit must ultimately be presented a boolean to decide whether to pass or fail, and so the result of an expression presented in a conditional is treated as follows:

 * If the result satisfies the `Bool` contract, then the internal state of the bool, either `true` or `false`, is used to determine the conditional.
 * Otherwise, the function property `truthy` is invoked, which is defined in all objects to return `true` (but is of course overridable). The outcome of this function, guaranteed to satisfy `Bool`, is then used as normal to determine the conditional.
