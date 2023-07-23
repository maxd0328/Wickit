
# Wickit Modules

In Wickit, source files are organized into modules. Modules are intended to bring simplicity and fluidity to Wickit projects, and serve many benefits such as organization, isolation, and access control.

The following outlines the primary features of modules:

 * **Absolute imports**: All resources within a module can be accessed relative to the root of the module.
 * **Access control**: Certain access modifiers of Wickit, namely `restricted` for read and `confined` for write, limit permissions for any given field to within the module.
 * **Loose structure**: Modules do not have to be tied to the filesystem, and all that is needed to define them is a modulefile (`module.xml`).
 * **Package system**: Code within modules can be organized into non-filesystem bound packages, allowing for the organization of code.
 * **Build pipeline**: Modules can have unique build instructions, including where to build, what to build, and any additional checks or procedures that should be performed.
 * **All local**: Unlike other module/package systems, modules do not need to be published to a package manager, and can all be handled locally using modulefiles. (No symbolic links necessary!)

## 1. Modulefile Structure

The following gives the structure of a modulefile:

```
<module>
    <dependencies>
        <dependency src="wickitstd">
        <dependency src="file://path/to/exampleModule/">
    </dependencies>
    <packages>
        <package name="org">
            <package name="example">
                <asset src="file://path/to/example.wckt">
            </package>
        </package>
    </packages>
    <build when="changed">
        <mount pckg="org.example" dst="file://buildFolder/">
        <pipeline when="before-each">
            <stage cmd="cat {{asset-src}}">
        </pipeline>
    </build>
    <entry symbol="org.example.main">
</module>
```

Within the `<module>` tag, there are four primary components:

 * **Dependencies**: Specifies other modules which this module is dependent on, and optionally how they should be imported.
 * **Packages**: Defines the package structure for this module, which is used for imports both within the module and by dependent modules.
 * **Build**: Specifies the build options, including where to mount packages (customization of the build directory), and build pipelines that can run at arbitrary times during the build process.
 * **Entry**: An optional component which specifies the entry point of the module when executed, if applicable.

## 2. Modules with CLI

The Wickit CLI can be used to interface with modules, including building and running them with certain customizations. The command variants and options are shown below:

### 2.1 Building

`wickit build [module-path] [options]`

| Option | Long Option | Arguments | Description |
| ------ | ----------- | --------- | ----------- |
| `-p` | `--package` | `[package]` | Build only within a specific package
| `-s` | `--asset` | `[source]` | Build only a specific asset |
| `-a` | `--all` | None | Rebuild all assets |
| `-c` | `--changed` | None | Build assets only if changed |
| N/A | `--no-recurse` | None | Do not build dependencies |
| N/A | `--no-pipeline` | None | Ignore module build pipeline |
| `-d` | `--debug` | None | Include debug source tables |

### 2.2 Running

`wickit run [module-path] [options]`

| Option | Long Option | Arguments | Description |
| ------ | ----------- | --------- | ----------- |
| `-e` | `--entry` | `[symbol]` | Specify a custom entry point |
| `-d` | `--debug` | None | Run module in debug mode |
| `-l` | `--live` | None | Build assets in memory only |
| N/A | `--no-build` | None | Do not build assets during execution |
| N/A | `--no-recurse` | None | Do not build dependencies |
| N/A | `--no-pipeline` | None | Ignore module build pipeline |

### 2.3 Exporting

`wickit export [module-path] [options]`

| Option | Long Option | Arguments | Description |
| ------ | ----------- | --------- | ----------- |
| `-o` | `--output` | `[path]` | Specify output file/directory
| `-d` | `--debug` | None | Include debug source tables |
| N/A | `--no-build` | None | Do not build assets during execution |
| N/A | `--no-recurse` | None | Do not build dependencies |
| N/A | `--no-pipeline` | None | Ignore module build pipeline |

Note that exporting will create a file similar to the `OPP` format (see bytecode documentation), but this exact format hasn't been realized yet.

## 3. Modulefile Specifications

### 3.1 Module Tag `<module>`

Attributes:
 * None

Children:
 * `<dependencies>`
 * `<packages>`
 * `<build>`
 * `<entry>`

### 3.2 Dependencies Tag `<dependencies>`

Attributes:
 * None

Children:
 * `<dependency>`

### 3.3 Dependency Tag `<dependency ...>`

Attributes:
 * `src`: URL of module (modulefile or containing folder, or path to exported module)
 * `bundle`: Whether or not the dependency should be bundled with the current module when exported (must be `"true"` or `"false"`). Note that this is an optional attribute that is false by default.
 * `pckg`: The package that the current module should be dependent on (such as `"org.example"`). This package is treated as the root package, and so all child symbols are directly imported without being contained in `org.example`. This is an optional attribute, taking the root package by default (hence including all packages).
 * `into`: Defines a package which all imported symbols should be placed into. This is also an optional attribute, taking the root package by default.

Children:
 * None

### 3.4 Packages Tag `<packages>`

Attributes:
 * None

Children:
 * `<package>`
 * `<asset>`

### 3.5 Package Tag `<package ...>`

Attributes:
 * `name`: The name of the package. This package is then accessed as a child of its parent package (or root package if it has no parent). In the example from section 1, the `example` package would be accessed as `org.example`.
 * `visibility`: The visibility of the package, which may have a visibility no more permissive than its parent. The acceptable values are `"public"`: accessible from any module, `"restricted"`: accessible only within the module, and `"private"`: accessible only within the parent package. These definitions align with those found in the language semantics documentation. This is an optional attribute, with a default value of `"public"`.

Children:
 * `<package>`
 * `<asset>`

### 3.6 Asset Tag `<asset ...>`

Attributes:
 * `src`: The URL of the source file associated with this asset.

Children:
 * None

### 3.7 Build Tag `<build ...>`

Attributes:
 * `when`: Specifies when assets should be rebuilt. Acceptable values are `"always"`, `"changed"`, and `"never"`. This attribute is optional with a default value of `"changed"`.
 * `dst`: A URL specifying the directory where build outputs should be located. This is optional with a default value of `%modulepath%/build/`.

Children:
 * `<mount>`
 * `<pipeline>`

### 3.8 Mount Tag `<mount ...>`

Attributes:
 * `pckg`: Specifies the package to be mounted
 * `dst`: A URL specifying the mount directory, that is, where the build output of all assets within that package will be located.

Children:
 * None

### 3.9 Pipeline Tag `<pipeline ...>`

Attributes:
 * `when`: Specifies when the pipeline should be run. Acceptable values are `"before-each"`, `"before-all"`, `"after-each"`, and `"after-all"`.
 * `on`: A string specifying a filter for what must be built in order for the pipeline to run. For "all" pipelines, at least one asset matching the filter must be built, and for "each" pipelines, the pipeline will be run for every asset matching the filter. Filters are disjunctions of conditions that are comma separated, where each condition can be a package: `"org.example"`, or an asset: `"&file:///path/to/asset"`. This field is optional, and is set to blank by default.

Children:
 * `<stage>`

### 3.10 Stage Tag `<stage ...>`

Attributes:
 * `cmd`: The command to be run for this stage. The commands are expressed in terms of the native command line, but special compiler variables may be used in between double curly braces like so: `{{variable}}`. Some variables which can be used are the following:
   * `asseturl`: The URL of the asset(s) being compiled
   * `buildurl`: The URL of the build output(s)
   * `package`: The package(s) containing the asset(s) being compiled
   * `modulepath`: The path to the current module

Children:
 * None

### 3.11 Entry Tag `<entry ...>`

Attributes:
 * `symbol`: The function, referenced from the root package, at which to start execution of this module.

Children:
 * None
