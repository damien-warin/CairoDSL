# CairoDSL – Reference Manual

## Chapter 1: Directives, Control Structures and Blocks

### 1.1 Preprocessor directives and prerunning

These directives configure the rendering environment or import external files before drawing execution begins.

- `#include "file.cairo";`: Includes and parses an external source file.
- `#setoutputsize(width, height);`: Sets the output canvas size in pixels (640 × 480 by default).
- `#setpngoutput();` / `#setsvgoutput();`: Enable PNG or SVG export, respectively. SVG is the default format outside the SDL2 environment.
- `#initmessage("Text", expression);`: Displays an initialization message during the prerunning phase.
- `#randinit(seed);` / `#randinit();`: Initializes the pseudo-random number generator. An explicit seed makes a given sequence reproducible. With no argument, the seed is built from the system time and the process PID.

**General rules:**

- All directives beginning with `#` must start at the beginning of a line and end with a semicolon.
- Except for `#include`, which is handled during program parsing, directives are processed during the prerunning phase.
- Multiple `#setoutputsize` directives may be used: only the last value defined before Cairo initialization is retained.

### 1.2 Control structures (conditions and loops)

The language provides several conditional and iterative structures to control code execution:

- `if (cond) { ... } else { ... }`: Conditionally executes blocks. The evaluated expression must return a number (`double`): 0 is false, and any other value is true.
- `for (i = [start:end:step]) { ... }`: Iterates over a numeric range, with an optional step.
- `foreach (item in array) { ... }`: Iterates over the elements of an array.
- `while (cond) { ... }`: Repeats a block of instructions while a condition is true.
- `break;` and `continue;`: Loop control statements used to exit or skip an iteration.

Braces may be omitted when a control structure contains only a single instruction. This is supported in particular by `if`, `for`, `foreach` and `while`.

### 1.3 Understanding blocks (`{ ... }`)

The language distinguishes between two types of instruction blocks:

- **Regular blocks:** groups of standard instructions, used notably in control structures.
- **CanvaBloc blocks:** blocks that automatically save the state of the graphics context (`save()`) before execution and restore it (`restore()`) when they exit.

Some CanvaBloc blocks also provide an auto-draw mechanism. It is enabled in two situations:

- Blocks associated with the `color` instruction.
- The main program, which is itself a global CanvaBloc.

Auto-draw makes it possible, for example, to write:

```cairo
rectangle(10, 10, 100, 100);
```

without an explicit rendering instruction.

This mechanism is triggered only when a path is still available when the block exits.

The `translate`, `rotate`, `scale`, `skewX/Y`, `applymatrix`, `setlinewidth`, `setdash`, `setfont` and `setfontsize` instructions, as well as module calls, also use CanvaBloc blocks, but without auto-draw.

When a CanvaBloc triggers auto-draw, it is performed immediately before the graphics context is restored (`restore()`). Automatic rendering operations therefore do not affect the graphics state retained after the block has closed.

### 1.4 Declaring and calling modules

Modules make it possible to organize code into reusable components.

```cairo
module NomDuModule(arg1, arg2) {
    // Module instructions
    children();
}

// Call without a child block
NomDuModule(val1, val2);

// Call with a child block
NomDuModule(val1, val2) {
    // Instructions passed to children()
}
```

The `children()` function is used inside a module to execute the block of instructions supplied when the module is called.

A module must declare at least one parameter, even if that parameter is not used in its body.

A module's local variables are reset on each execution.

The position of `children()` in the module body determines when the child block is executed.

## Chapter 2: Paths and Geometry

This chapter describes the geometric drawing primitives and the handling of vector paths in the language.

### 2.1 Basic geometric shapes

The language provides several primitives for directly drawing simple geometric shapes.

Some functions accept individual scalar parameters or a numeric array containing those parameters. The `polygon()` and `polyline()` functions accept only an array of coordinates.

- `rectangle(x, y, w, h)` or `rectangle([params])`: Draws a rectangle starting at point `(x, y)`, with width `w` and height `h`.
- `arc(x, y, r, a1, a2)` or `arc([params])`: Draws a circular arc centered at `(x, y)`, with radius `r`, between angles `a1` and `a2`.
- `arcneg(x, y, r, a1, a2)` or `arcneg([params])`: Draws a circular arc in the direction opposite to `arc()`.
- `curve(m1x, m1y, m2x, m2y, x, y)` or `curve([params])`: Draws a cubic Bézier curve with two control points and an endpoint.
- `polygon([params])`: Draws a closed polygon made up of a sequence of points represented by an array of coordinates `(x1, y1, x2, y2, ...)`.
- `polyline([params])`: Draws an open polyline made up of a sequence of points represented by an array of coordinates `(x1, y1, x2, y2, ...)`.

The angles used by `arc()` and `arcneg()` are expressed in radians. Their direction of rotation follows Cairo's conventions directly.

### 2.2 Path and subpath handling

To build custom vector paths, the engine uses a set of path-control commands:

- `moveto(x, y)`: Moves the starting point of the current path without drawing a line.
- `lineto(x, y)`: Draws a straight line segment from the current position to point `(x, y)`.
- `newpath()`: Discards the current path and creates a new one.
- `newsubpath()`: Starts a new subpath without removing the paths already present.
- `closepath()`: Closes the current subpath by connecting its endpoint to its starting point.
- `getpathextents()`: Returns an array containing the path's bounding rectangle in the form `[x1, y1, x2, y2]`, corresponding to the coordinates of two opposite corners of the rectangle.

### 2.3 Applying rendering operations

Once a path has been defined, various instructions can be used to render it on the canvas.

- `stroke()` / `strokepreserve()`: Strokes the current path, either consuming or preserving it.
- `fill()` / `fillpreserve()`: Fills the interior of the current path, either consuming or preserving it.
- `clip()` / `clippreserve()` / `resetclip()`: Define a clipping region (geometric masking), either consuming or preserving the path, or reset the clipping region.
- `setautofill()` / `setautostroke()` / `setautofillstroke()` / `setautostrokefill()`: Configure how geometric shapes are automatically rendered during auto-draw.

The `stroke()`, `fill()` and `clip()` instructions consume the current path after rendering. Their `preserve` variants retain the path.

The `setauto...()` functions are parameterless instructions that configure the order of operations during auto-draw:

- `setautofill()`: Fill only.
- `setautostroke()`: Stroke only.
- `setautofillstroke()`: Fill, then stroke.
- `setautostrokefill()`: Stroke, then fill.

## Chapter 3: Colors and Styles

This chapter covers color configuration, the background, line width and graphic styles.

### 3.1 Color handling

Colors are handled using values ranging from 0 to 255 for the Red, Green, Blue and Alpha (RGBA) components.

Color-setting instructions accept three forms of arguments:

- Individual parameters: `(r, g, b, a)`
- Array with alpha: `[r, g, b, a]`
- Array without alpha: `[r, g, b]`

When the alpha component is omitted, its value is set to 255 (fully opaque).

- `color(r, g, b, a)`: Sets the current color for stroking and filling. This instruction can be used on its own or as an instruction block (CanvaBloc). This block has the auto-draw mechanism.
- `setbgcolor(r, g, b, a)`: Sets the canvas background color.
- `setautostrokecolor(r, g, b, a)`: Sets the color used for automatic strokes.
- `setautofillcolor(r, g, b, a)`: Sets the color used for automatic fills.
- `settextcolor(r, g, b, a)`: Sets the color used to render text. This color does not apply to `textpath()`, which builds a vector path.

The `setautostrokecolor()` and `setautofillcolor()` functions act on CairoDSL's internal context, not directly on the Cairo graphics context. They configure the colors used during auto-draw and do not directly affect the current graphics state.

### 3.2 Line style

- `setlinewidth(width)`: Changes the line width.
- `setdash(expression)` or `setdash(expression, offset)`: Defines a dashed-line pattern. The first argument is a numeric array that alternately specifies the lengths of drawn segments and gaps. The optional second argument specifies the pattern offset.

The `setlinewidth()` and `setdash()` instructions can also be used as CanvaBloc blocks. They save and restore the graphics context state without triggering auto-draw.

## Chapter 4: Transformations

Geometric transformations make it possible to manipulate the canvas coordinate system to perform translations, rotations, scaling and distortions.

### 4.1 Spatial and matrix transformations

Transformations can be used directly, followed by a semicolon, or associated with an instruction block (CanvaBloc).

- `translate(x, y)`: Translates the coordinate system along the X and Y axes.
- `rotate(angle)`: Rotates the coordinate system by the specified angle, expressed in radians.
- `scale(x, y)`: Scales the coordinate system along the X and Y axes.
- `applymatrix(m1, m2, m3, m4, m5, m6)`: Applies a custom six-parameter transformation matrix. This function directly uses Cairo's `cairo_apply_matrix` mechanism.
- `skewX(angle)` / `skewY(angle)`: Applies a skew along the X or Y axis. The angle is expressed in radians.

### 4.2 Saving and restoring state

- `save()`: Saves the current state of the graphics context.
- `restore()`: Restores the most recently saved state.

These two functions are direct wrappers around the corresponding Cairo functions. They make it possible, in particular, to manage the graphics-state stack explicitly, independently of CanvaBloc usage.

## Chapter 5: Text and Typography

This chapter describes font configuration, string rendering and access to typographic metrics. CairoDSL relies directly on Cairo's text-handling mechanisms.

### 5.1 Font configuration

`setfont("Family", "Slant", "Weight")`: Sets the font family, slant and weight.

- **Family:** Font family name, passed directly to Cairo.
- **Slant:** Text slant. Recognized values: `"normal"`, `"italic"` or `"oblique"`.
- **Weight:** Text weight. Recognized values: `"normal"` or `"bold"`.

Comparisons of the slant and weight parameters are case-insensitive. Any unrecognized value results in the corresponding normal style or normal weight being used.

`setfontsize(size)`: Sets the current font size.

These instructions modify the typographic state of the graphics context. They can be used directly, followed by a semicolon, or associated with an instruction block (CanvaBloc).

### 5.2 Text rendering and path creation

CairoDSL provides two distinct ways to use text:

- `text(expression)`: Draws text directly at the current position of the graphics context. The color previously defined by `settextcolor()` is used. `settextcolor` stores the color in the CairoDSL context and modifies the Cairo context only when `text()` is called; this operation is enclosed by `save` and `restore`.
- `textpath(expression)`: Converts characters into vector paths and adds them to the current path. No direct rendering takes place. The text can then be manipulated like any other path, notably with `fill()`, `stroke()`, their `preserve` variants or clipping mechanisms.

Unlike `text()`, `textpath()` does not take the color defined by `settextcolor()` into account. It uses Cairo's current graphics source.

Both instructions use the current point of the graphics context to position the text, and then move the current point to the end of the text.

**Key point:** `text()` performs direct typographic rendering using the dedicated text color, whereas `textpath()` creates vector geometry that is subject to the usual path-rendering mechanisms.

### 5.3 Typographic metrics

CairoDSL exposes two functions for retrieving numeric information about text dimensions and font characteristics.

`gettextextents(expression)`: Returns the metrics of a string as a six-element numeric array:

| Index | Value | Description |
|---:|---|---|
| 0 | `x_bearing` | Horizontal offset of the text bounding box relative to the current position. |
| 1 | `y_bearing` | Vertical offset of the text bounding box relative to the current position. |
| 2 | `width` | Width of the text bounding box. |
| 3 | `height` | Height of the text bounding box. |
| 4 | `x_advance` | Horizontal advance of the current point after the text. |
| 5 | `y_advance` | Vertical advance of the current point after the text. |

`getfontextents()`: Returns the general metrics of the current font as a five-element numeric array:

| Index | Value | Description |
|---:|---|---|
| 0 | `ascent` | Height above the baseline. |
| 1 | `descent` | Depth below the baseline. |
| 2 | `height` | Recommended line spacing height. |
| 3 | `max_x_advance` | Maximum horizontal character advance. |
| 4 | `max_y_advance` | Maximum vertical character advance. |

The returned values correspond directly to the metrics provided by Cairo, without any particular transformation, apart from their conversion to numeric `double` values.

**Important:** These functions return numeric arrays without named fields. Values must be retrieved by index, starting at zero.

Also, `width` and `height` describe the characters' bounding box, whereas `x_advance` and `y_advance` describe the movement of the current point. These values are therefore not necessarily identical.

Example:

```cairo
setfont("Sans", "normal", "bold");
setfontsize(24);
text("CairoDSL");
metrics = gettextextents("CairoDSL");
largeur = metrics[2];
hauteur = metrics[3];
avancement = metrics[4];
```

## Chapter 6: Groups, Patterns and Masks

CairoDSL can create and manipulate reusable graphics sources from drawing groups and patterns. These objects can be used to apply complex rendering operations, notably gradients or alpha masks.

Groups and patterns are referenced by numeric identifiers internal to CairoDSL, of type `double`. They share the same internal storage space: from Cairo's point of view, all these objects are represented as patterns.

### 6.1 Groups

Groups make it possible to record a sequence of graphics instructions as a reusable source, without immediately drawing it on the canvas.

There are two syntaxes for creating a group.

**Explicit creation:**

```cairo
pushgroup();
// Graphics instructions
rectangle(10, 10, 100, 100);
id = popgroup();
```

- `pushgroup()`: Starts creating a graphics group. This instruction is a wrapper around `cairo_push_group()`.
- `popgroup()`: Ends group creation and returns its internal numeric identifier in CairoDSL.

**Block-based creation:**

```cairo
id = group() {
    rectangle(10, 10, 100, 100);
    rectangle(40, 40, 50, 50);
}
```

`group()` is a CanvaBloc: it automatically saves the graphics context state before executing the block and restores it when the block exits.

The returned identifier can be stored and used again to reuse the group.

**Syntax note:** Unlike a regular instruction, the closing brace of a `group()` block must not currently be followed by a semicolon.

**Using groups**

- `usegroup(id)`: Selects the group as the current graphics source.
- `paint()`: Applies the current graphics source to the entire canvas.
- `paint(alpha)`: Applies the current graphics source with a global alpha value.

`usegroup()` does not render anything by itself. It simply selects the source that will be used by subsequent rendering instructions.

### 6.2 Patterns: linear and radial gradients

Patterns make it possible to define evolving graphics sources, notably in the form of color gradients.

CairoDSL provides two types of patterns:

- `newlinear(x1, y1, x2, y2)`: Creates a linear gradient defined by two points that determine its axis.
- `newradial(x1, y1, r1, x2, y2, r2)`: Creates a radial gradient defined by two circles, each characterized by its center and radius.

These functions return a numeric identifier internal to CairoDSL.

**Adding color stops**

The `addstop()` function adds color stops to the gradient.

Outside a block:

```cairo
p = newlinear(0, 0, 200, 0);
addstop(p, 0, 255, 0, 0, 255);
addstop(p, 1, 0, 0, 255, 255);
```

Syntax:

`addstop(id, position, r, g, b, a)`

- `id`: Pattern identifier.
- `position`: Position of the color stop in the gradient, from 0 to 1.
- `r`, `g`, `b`, `a`: RGBA color components, from 0 to 255.

Inside a creation block:

```cairo
p = newlinear(0, 0, 200, 0) {
    addstop(0, 255, 0, 0, 255);
    addstop(1, 0, 0, 255, 255);
}
```

In this context, the pattern identifier is implicit. The `addstop()` function operates directly on the pattern currently being created.

This syntax is also available with `newradial()`.

**Syntax note:** As with `group()`, the closing brace of a pattern-creation block must not currently be followed by a semicolon.

**Using patterns**

- `usepattern(id)`: Selects a pattern as the current graphics source.
- `paint()`: Applies the selected pattern to the canvas.
- `paint(alpha)`: Applies the selected pattern with a global alpha value.

`usepattern()` and `usegroup()` are aliases recognized by CairoDSL. They perform the same operation: selecting a graphics source.

### 6.3 Masks

The `mask(id)` function uses a group or pattern as an alpha mask.

It retrieves the pattern corresponding to the supplied identifier from CairoDSL's internal storage, then applies Cairo's `cairo_mask()` mechanism.

The mask uses the pattern's alpha channel to determine the transparency applied to the current graphics source.

Syntax:

```cairo
mask(id);
```

The mask is applied to the current graphics source over the area defined by the active clipping region. The current path does not take part in this operation.

This makes it possible, for example, to use a gradient or a group as a transparency mask for another drawing.

### 6.4 Identifier handling

Group and pattern identifiers are numeric values of type `double`, managed by CairoDSL.

They share the same internal storage space. This organization is consistent with Cairo's behavior, which represents both groups and gradients as patterns.

The selection and masking functions can therefore be used with the corresponding identifiers, without distinguishing between categories at the storage level.

## Chapter 7: SVG Import

CairoDSL can import SVG vector files and integrate them directly into CairoDSL rendering. The importer parses the SVG document and interprets the supported graphic elements.

Importing relies on XML document parsing. It makes it possible, in particular, to reuse an existing vector drawing without having to rewrite it in the CairoDSL language.

### 7.1 Importing an SVG file

The `import()` function loads an SVG file.

Two usage modes are available.

**Import and draw immediately**

```cairo
import("dessin.svg");
```

The file is imported and its contents are drawn immediately on the canvas.

**Import and retain an identifier**

```cairo
id = import("dessin.svg");
```

In this case, the import is stored and the function returns a numeric identifier that makes it possible to reuse it later.

The `drawimport()` function can then draw or redraw the import:

```cairo
id = import("dessin.svg");
drawimport(id);
```

The identifier can be stored in a variable and reused as many times as needed.

**Path resolution**

Relative paths are resolved from the current working directory (CWD) containing the CairoDSL source file that was opened.

### 7.2 Supported SVG elements

The importer recognizes several categories of SVG elements.

Supported graphic elements include, among others:

- `<g>`: groups of elements;
- `<rect>`: rectangles;
- `<path>`: vector paths;
- `<line>`: line segments;
- `<circle>`: circles;
- `<ellipse>`: ellipses;
- `<text>`: text.

This list is not exhaustive. However, support for SVG elements does not mean that the entire SVG specification has been implemented.

### 7.3 Transformations

The following SVG transformations are supported:

- `translate`: translation;
- `rotate`: rotation;
- `scale`: scaling;
- `matrix`: matrix transformation;
- `skewX` and `skewY`: horizontal or vertical skew.

Transformations are interpreted according to SVG format conventions. In particular, angles expressed in SVG attributes are converted when necessary for application in Cairo.

Transformations can be combined according to SVG syntax.

### 7.4 Colors and styles

The importer supports several attributes used to define the appearance of elements:

- `fill`: fill color;
- `stroke`: stroke color;
- `fill-opacity`: fill opacity;
- `stroke-opacity`: stroke opacity;
- `style`: style declarations directly in the attribute.

Colors can be defined using the formats recognized by the importer.

Some style properties, such as `stroke-dasharray`, are handled.

**Important limitation:** The importer is not a fully SVG/CSS-compliant rendering engine. Not all CSS properties and not all SVG style cascading and inheritance rules are implemented. A complex SVG file should therefore not be expected to render identically to the way it appears in a web browser.

In particular, style inheritance between elements and the handling of global opacity are not fully supported.

### 7.5 SVG text

`<text>` elements are supported with the following attributes:

- `x` and `y`: text position;
- `font-weight`: font weight;
- `font-size`: font size.

This support remains limited compared with the complete SVG specification.

For example, the `text-anchor` attribute, which aligns text relative to its anchor point, is not currently implemented.

### 7.6 Import dimensions

The `getimportextents(id)` function retrieves the dimensions declared in the `viewBox` of the imported SVG document.

It returns an array of four values:

```cairo
extents = getimportextents(id);
```

The array contains:

| Index | Value |
|---:|---|
| 0 | X coordinate of the `viewBox` |
| 1 | Y coordinate of the `viewBox` |
| 2 | `viewBox` width |
| 3 | `viewBox` height |

The import identifier must be supplied as an argument.

If the SVG document does not contain a `viewBox` attribute, the function returns:

```cairo
[0, 0, 0, 0]
```

These are the dimensions declared in the `viewBox`, not necessarily the actual geometric bounds of the drawn elements.

### 7.7 Key points

The SVG importer makes it possible to integrate external vector resources into a CairoDSL program, reuse them through their identifier and retrieve the dimensions declared in their `viewBox`.

It supports several geometric shapes, transformations and various color and style properties.

However, it is not a complete SVG rendering engine: some specification features, notably gradients, patterns, as well as certain CSS rules, style inheritance and typography features, are outside its current scope.

## Chapter 8: Operators and Mathematical Functions

CairoDSL provides operators and mathematical functions for calculations, comparisons and logical operations. These tools are used in particular to calculate drawing dimensions, positions and geometric metrics dynamically.

### 8.1 Arithmetic operators

Arithmetic operators perform the usual numeric operations.

| Operator / Function | Description |
|---|---|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `power(base, exp)` | Exponentiation |
| `mod(a, b)` | Modulo |

The `power()` and `mod()` functions accept numeric values.

The `mod()` function can notably be used to calculate a remainder with floating-point numbers.

**Handling special values**

Arithmetic operations do not yet have a dedicated mechanism for handling NaN (Not a Number) and Inf (infinity) values.

For example, division by zero may produce an infinite value (Inf) or an indeterminate value (NaN), depending on the operands. These values are then propagated through subsequent calculations according to the usual floating-point operation rules.

Programmers must therefore take these possible values into account in their calculations.

### 8.2 Comparison operators

The available comparison operators are:

| Operator | Meaning |
|---|---|
| `==` | Equality |
| `!=` | Inequality |
| `<` | Less than |
| `<=` | Less than or equal to |
| `>` | Greater than |
| `>=` | Greater than or equal to |

The `==` and `!=` comparisons take the type and structure of the compared values into account.

They work as follows:

- The two values must be of the same type.
- For arrays, they must also contain the same number of elements.
- If these conditions are met, their contents are compared until the first difference.

Thus, two strings are compared by their contents, and two arrays by their respective elements.

The `==` and `!=` operators therefore return false or true depending on whether the values are identical or different, after their structural compatibility has been checked.

Numeric comparisons (`<`, `<=`, `>` and `>=`) apply only to simple numeric values (they do not apply to arrays).

### 8.3 Logical operators

CairoDSL provides four logical operators:

- `and`: logical AND;
- `or`: logical OR;
- `xor`: exclusive OR;
- `not`: logical NOT.

These operators work only on scalar numeric values.

The convention is:

- 0 represents false;
- any other value represents true.

Logical operators return a numeric result corresponding to the outcome of the operation.

They are not comparison operators for strings or arrays.

### 8.4 Trigonometry and angle conversions

CairoDSL provides the usual trigonometric functions.

| Function | Description |
|---|---|
| `cos(rad)` | Cosine |
| `sin(rad)` | Sine |
| `tan(rad)` | Tangent |
| `acos(val)` | Arccosine |
| `asin(val)` | Arcsine |
| `atan(val)` | Arctangent |

Angles used by trigonometric functions are expressed in radians. The inverse functions also return radians.

Two functions convert angles:

- `radians(deg)`: Converts an angle expressed in degrees to radians.
- `degrees(rad)`: Converts an angle expressed in radians to degrees.

These functions can take an array and then return an array.

**Mathematical constants**

Two parameterless functions provide access to mathematical constants:

- `pi()`: Returns π, approximately 3.14159.
- `phi()`: Returns the golden ratio φ, approximately 1.61803.

### 8.5 Rounding and roots

Three functions are available for common rounding and root calculations:

| Function | Description |
|---|---|
| `floor(x)` | Returns the greatest integer less than or equal to `x`. |
| `ceil(x)` | Returns the smallest integer greater than or equal to `x`. |
| `sqrt(x)` | Calculates the square root of `x`. |

These functions can take an array and then return an array.

### 8.6 Random number generation

The `rand()` function generates random values.

Two forms are available:

```cairo
value = rand();
value = rand(max);
```

`rand()` generates a random value between 0 and 1.

`rand(max)` generates a value between 0 and `max`, inclusive.

**Initializing the generator**

The `#randinit` directive initializes the seed of the random-number generator.

With an explicit seed:

```cairo
#randinit(1234);
```

With no argument:

```cairo
#randinit();
```

In the latter case, the seed is built from the system time and the process PID (Process ID).

Using a fixed seed makes it possible to reproduce the same pseudo-random sequence across executions, under the same conditions.

### 8.7 Type conversion

Two functions convert values between strings and numbers.

**Converting a string to a number**

```cairo
value = strtod("12.3");
```

The `strtod()` function directly uses the standard C function of the same name.

It converts the beginning of the string into a floating-point number (`double`).

If the string does not contain a valid numeric representation, `strtod` returns 0.

**Converting a number to a string**

```cairo
text = dtostr(12.3);
```

The `dtostr()` function converts a floating-point number into a string using the `%g` format.

This format automatically adapts the representation of the number, choosing decimal or scientific notation depending on its value.

Both functions can take an array of their input type and then return an array of their output type.

```cairo
valeurs=strtod("12.3", "21", "42");//Values contains [12.3, 21, 42]
```

## Chapter 9: Variables, Arrays and Strings

This chapter presents the different data types handled by CairoDSL, along with the mechanisms for declaring, assigning and manipulating variables, arrays and strings.

### 9.1 Data types and assignment

CairoDSL uses five internal data types:

| Type | Description |
|---|---|
| `NONE` | Uninitialized or cleared variable. |
| `DOUBLE` | Floating-point numeric value (`double`). |
| `DOUBLES` | Array of numeric (`double`) values. |
| `STRING` | String. |
| `STRINGS` | Array of strings. |

Arrays are homogeneous: an array contains either numeric values exclusively or strings exclusively. Mixed arrays and nested arrays are not supported.

**Declaration and assignment**

A variable is created or modified by simple assignment:

```cairo
x = 12.5;
texte = "Bonjour";
nombres = [1, 2, 3];
mots = ["Bonjour", "CairoDSL"];
```

The variable's type is determined automatically by the assigned value.

The `unset()` instruction clears a variable and resets its type:

```cairo
unset(x);
```

After this operation, the variable has type `NONE` again and can be reused to store another category of data.

### 9.2 Generating numeric arrays

Numeric arrays can be built directly using square brackets or generated dynamically from a range.

**Range syntax**

Two syntaxes are available:

```cairo
array = [min:max];
array = [min:max:step];
```

- `min`: Initial value of the range.
- `max`: Target maximum value.
- `step`: Increment applied between two elements.

If the step is omitted, its value is set to 1.

The bounds are inclusive: the maximum value is included in the array if the progression reaches it exactly.

Examples:

```cairo
a = [0:5];       // [0, 1, 2, 3, 4, 5]
b = [0:10:2];    // [0, 2, 4, 6, 8, 10]
c = [0:10:3];    // [0, 3, 6, 9]
d = [5:0:-1];    // [5, 4, 3, 2, 1, 0]
```

A negative step makes it possible to traverse a range in descending order.

**Warning:** The engine does not currently check whether the direction of progression is consistent with the bounds. A range whose step cannot reach the bound in the correct direction, for example `[0:10:-1]`, may cause an attempt to allocate an enormous amount of memory and result in a failure (`malloc failed`).

Always make sure that the sign of the step matches the intended direction of progression.

### 9.3 Operators on numbers, arrays and strings

The `+` operator behaves in several ways depending on the operand types.

**Numeric addition**

Two simple numeric values are added normally:

```cairo
resultat = 10 + 5;
```

**Operations on numeric arrays**

Two numeric arrays of the same size can be added element by element:

```cairo
a = [1, 2, 3];
b = [10, 20, 30];
c = a + b; // [11, 22, 33]
```

Both arrays must contain the same number of elements.

**String concatenation**

Two simple strings can be concatenated:

```cairo
text = "Cairo" + "DSL"; // "CairoDSL"
```

**Operations on arrays of strings**

Two arrays of strings of the same size can be combined element by element. Each element of the first array is concatenated with the corresponding element of the second:

```cairo
a = ["Bon", "Cairo"];
b = ["jour", "DSL"];
c = a + b; // ["Bonjour", "CairoDSL"]
```

A simple string can also be combined with an array of strings:

- `STRING + STRINGS`: Prefixes each element of the array with the string.
- `STRINGS + STRING`: Suffixes each element of the array with the string.

Example:

```cairo
mots = ["Bonjour", "Bonsoir"];
prefixes = "Hello " + mots;
// ["Hello Bonjour", "Hello Bonsoir"]

suffixes = mots + " !";
// ["Bonjour !", "Bonsoir !"]
```

These operations produce a new array and do not directly modify the original operands.

### 9.4 Concatenation and appending elements

CairoDSL provides two equivalent ways to build or extend arrays:

```cairo
concat(array, element);
array << element;
```

The `<<` operator is a syntactic alias for `concat()`.

These operations make it possible to:

- Concatenate two arrays of the same type.
- Append an element to the end of an array.
- Initialize an array progressively when it does not yet exist.

Example:

```cairo
numbers = [1, 2];
numbers = concat(numbers, 3);
numbers = numbers << 4;
// numbers = [1, 2, 3, 4]
```

**Dynamic initialization:**

```cairo
numbers = numbers << 10;
numbers = numbers << 20;
numbers = numbers << 30;
```

The array is automatically created when the first element is appended.

Type compatibility rules apply: numeric arrays and arrays of strings remain distinct.

### 9.5 Extracting and inspecting collections

Several functions make it possible to inspect the contents of arrays and strings.

**`getfirst()` and `getlast()`**

```cairo
premier = getfirst(tableau);
dernier = getlast(tableau);
```

- `getfirst()` returns the first element.
- `getlast()` returns the last element.

When the collection is empty:

- For a numeric array, the result is `NAN`.
- For a string or an array of strings, the result is an empty string `""`.

**`getsize()`**

```cairo
size = getsize(collection);
```

The function returns:

- For an array: the number of elements.
- For a string: the amount of memory allocated to the string, in bytes, including the null terminator.

**`getlen()`**

```cairo
length = getlen(text);
```

This function returns the length of the string, following the behavior of C's `strlen()`.

Unlike `getsize()`, the null terminator is not counted.

### 9.6 Direct index access

Array elements can be accessed directly using their index:

```cairo
tableau = [10, 20, 30];
valeur = tableau[0]; // 10
valeur = tableau[2]; // 30
```

Indexing starts at zero.

The expression used as an index is converted to an unsigned integer (`unsigned int`).

If the resulting index exceeds the array bounds, the engine generates an error.

Make sure to check bounds before accessing an element, particularly when iterating over arrays dynamically.

## Chapter 10: Time and Environment Management

CairoDSL provides functions for accessing the system clock, converting timestamps into calendar information and retrieving parameters from the execution environment.

### 10.1 System clock and timestamps

The `gettimeinsecs()` function retrieves the system time as a UNIX timestamp.

```cairo
timestamp = gettimeinsecs();
```

It returns the number of seconds elapsed since the UNIX epoch (January 1, 1970 at 00:00:00 UTC).

The returned value is a `DOUBLE`.

### 10.2 Converting timestamps into calendar information

Two functions break down a timestamp into calendar information:

- `getgmttime(timestamp)`: Converts the timestamp to UTC (GMT).
- `getlocaltime(timestamp)`: Converts the timestamp to the local time of the execution system.

Both functions return a nine-element numeric array, organized according to the order of the fields in the C language's `struct tm` structure.

| Index | Field | Description | Values |
|---:|---|---|---|
| 0 | `sec` | Seconds | 0 to 60 |
| 1 | `min` | Minutes | 0 to 59 |
| 2 | `hour` | Hours | 0 to 23 |
| 3 | `mday` | Day of the month | 1 to 31 |
| 4 | `mon` | Month of the year | 0 to 11 |
| 5 | `year` | Years since 1900 | Variable |
| 6 | `wday` | Day of the week | 0 to 6 |
| 7 | `yday` | Day of the year | 0 to 365 |
| 8 | `isdst` | Daylight saving time indicator | -1, 0 or 1 |

**Specific characteristics of the returned values**

Some conventions inherited from `struct tm` deserve particular attention:

- **Month (`mon`):** Starts at zero. January is therefore 0 and December is 11.
- **Year (`year`):** Represents the number of years elapsed since 1900. Add 1900 to obtain the calendar year.
- **Day of the week (`wday`):** Sunday is 0, Monday is 1, and so on through Saturday, which is 6.
- **Day of the year (`yday`):** Starts at zero. January 1 is 0.
- **Daylight saving time (`isdst`):**
  - `1`: Daylight saving time is active.
  - `0`: Daylight saving time is not active.
  - `-1`: The information is not available.

**Usage example**

```cairo
now = gettimeinsecs();
utc = getgmttime(now);
local = getlocaltime(now);
heure = local[2];
minute = local[1];
jour = local[3];
mois = local[4] + 1;
annee = local[5] + 1900;
```

Both functions use the same timestamp, but produce different results depending on the time zone applied.

### 10.3 Environment variables

The `getenv()` function retrieves the contents of environment variables from the execution system.

It accepts two forms of call.

**Retrieving one variable**

```cairo
value = getenv("FOO");
```

The function returns the requested environment variable's value as a string (`STRING`).

**Retrieving several variables**

```cairo
values = getenv(["FOO", "BAR"]);
```

The function also accepts an array of variable names and returns an array of strings (`STRINGS`) containing the corresponding values, in the same order as the requested names.

**Handling missing variables**

If a requested environment variable does not exist, the engine generates an error.

When retrieving multiple variables, a single missing variable is enough to trigger an error.

**Example: injecting external parameters**

Combining `getenv()` and `strtod()` makes it easy to pass numeric parameters to the program through the execution environment.

```cairo
angles = strtod(getenv(["angle1", "angle2", "angle3"]));
```

The retrieved string values are converted to numbers and stored in a numeric array.

This technique makes it possible, in particular, to modify drawing parameters without changing the source code directly.

## Chapter 11: Inspection, Utilities and System Configuration

This chapter brings together functions for inspecting certain graphics-context parameters, manipulating predefined colors, sending information to the CairoDSL context and identifying the initial values of the drawing environment.

### 11.1 Inspecting graphics parameters

Several functions retrieve information about the current state of the graphics context.

**`getdash()`**

Returns the numeric array describing the currently defined dashed-line pattern.

When no dashed-line pattern is defined, the function returns `NONE` (uninitialized value).

**`getdashoffset()`**

Returns the current offset of the dashed-line pattern.

When no dashed-line pattern is defined, the function returns 0.

**`getlinewidth()`**

Returns the current line width.

**`getoutputsize()`**

Returns a two-element numeric array containing the dimensions of the output surface:

- Element 0: width.
- Element 1: height.

Example:

```cairo
size = getoutputsize();
width = size[0];
height = size[1];
```

### 11.2 Predefined colors

CairoDSL provides several functions that directly return RGB colors as numeric arrays.

| Function | Returned value |
|---|---|
| `black()` | `[0, 0, 0]` |
| `white()` | `[255, 255, 255]` |
| `red()` | `[255, 0, 0]` |
| `green()` | `[0, 255, 0]` |
| `blue()` | `[0, 0, 255]` |

These functions return only the three RGB components, without an alpha component.

**`getrgb(colorname)`**

Returns the RGB components of a color based on its name.

The function recognizes the 148 CSS/SVG color names supported by CairoDSL. The lookup is case-insensitive.

The result is a three-element numeric array `[r, g, b]`, with components ranging from 0 to 255.

If the color name is not recognized, an error is raised.

The values `transparent` and `currentColor` are not supported.

Example:

```cairo
mycolor = getrgb("CornflowerBlue");
color(mycolor) {
    rectangle(50, 50, 100, 100);
}
```

### 11.3 Sending messages

`message(description, expression)`

Sends information to the user, for example for debugging purposes.

The function takes two arguments:

- `description`: A string describing the information being sent.
- `expression`: An evaluated expression whose value is added to the message.

The second argument can produce any of the five CairoDSL value types:

- `NONE`
- `DOUBLE`
- `DOUBLES`
- `STRING`
- `STRINGS`

The function does not return a usable value.

Example:

```cairo
message("Valeur de phi", phi());
message("Résultat du calcul", phi() * 42);
message("Tableau de valeurs", [1, 2, 3, 4]);
```

Messages are retained in the CairoDSL context, along with errors. They are then transmitted by the interfaces according to how each interface operates.

- **Command-line interface:** Messages and errors are sent to `stderr`, while graphical output is sent to `stdout`.
- **CGI interface:** Messages and errors are included in the XML response.

### 11.4 Initial graphics-environment values

When the graphics context is initialized, CairoDSL uses the following default values:

| Parameter | Initial value |
|---|---|
| Surface width | 640 pixels |
| Surface height | 480 pixels |
| Stroke color | Black `[0, 0, 0, 255]` |
| Fill color | Black `[0, 0, 0, 255]` |
| Initial rendering mode | `stroke` |
| Line width | 5 pixels |

The rendering engine depends on the interface being used:

- **With SDL2:** Rendering is performed in the SDL environment.
- **Without SDL2:** In particular in the CGI, Qt or other interfaces, the default graphical output format is SVG.

These values constitute the initial state of the context. They can subsequently be modified by the appropriate CairoDSL instructions and functions.
