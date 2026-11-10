# CairoInterpreter

[English](README.md) | [Français](README.fr.md)

**CairoInterpreter** is a lightweight scripting language interpreter built on top of the [Cairo](https://www.cairographics.org/) graphics library.

It allows users to describe graphical operations using a dedicated scripting language and generate graphical output, including SVG and PNG formats.

The project supports several execution modes:

- **Bash**: command-line execution.
- **SDL2**: interactive graphical output.
- **Qt**: graphical interface for editing and running scripts.
- **CGI**: execution through a web server, with XML output designed for client-side interfaces.

---

## 1. Dependencies

### Core dependencies

The project requires:

- **Cairo**: graphics library.
- **Flex** and **Bison**: lexical and syntax parser generation.
- **libxml2**: XML processing and SVG import.
- **uthash**: hash table library.
- **SDL2**: graphical output (optional).
- **libb64**: Base64 encoding of PNG images in server mode.
- **Python 3**: used by some project generation tools.
- **CMake** and a C compiler: build tools.

The [log.c](https://github.com/rxi/log.c) library is used for logging.

### Installing dependencies on Ubuntu / Debian

Install the main dependencies with:

```bash
sudo apt install build-essential cmake bison flex python3 \
    libcairo2-dev libxml2-dev libsdl2-dev libb64-dev uthash-dev
```

### Additional Qt dependencies

The Qt graphical interface requires Qt 6 and some additional libraries:

```bash
sudo apt install qt6-base-dev qt6-svg-dev \
    libqt6widgets6 libqt6xml6 libqt6svgwidgets6 \
    qt6-tools-dev linguist-qt6 librsvg2-dev
```

---

## 2. Build options

Several CMake options allow you to adapt the build to different environments.

| Option | Description |
|---|---|
| `NOSDL` | Disables SDL2 support for graphical output. The interpreter can then run without SDL2, including in Bash and CGI environments. |
| `CGIMODE` | Enables CGI server mode: generates a complete XML response including HTTP headers and handles input data using `CONTENT_LENGTH`. |
| `QTGUI` | Builds the Qt graphical interface. Currently incompatible with SDL2 builds. |
| `BUILD_UNITTESTS` | Enables or disables unit test compilation. |
| `UPDISTANT` | Additional project build option. |

---

## 3. Building the project

### Building with the Qt interface

The following configuration builds both the interpreter and its Qt graphical interface:

```bash
mkdir build
cd build

cmake ../ \
    -DCMAKE_VERBOSE_MAKEFILE=false \
    -DBUILD_UNITTESTS=false \
    -DUPDISTANT=true \
    -DNOSDL=false \
    -DCGIMODE=false \
    -DQTGUI=true

make
```

This generates, among other files:

- `CairoInterpreter`: the interpreter.
- `CairoDSLEditor`: the Qt graphical editor.

To build without the Qt interface, simply set `QTGUI` to `false`.

---

## 4. Usage

### 4.1. Bash mode

The interpreter can be used directly from a terminal.

**With SDL2:**

```bash
./CairoInterpreter < script
```

**Without SDL2:**

```bash
./CairoInterpreter < script > output
```

In the latter case, graphical output is redirected to the `output` file.

### 4.2. Qt graphical interface

The Qt editor provides a graphical environment for editing and executing CairoDSL scripts.

To launch it:

```bash
./CairoDSLEditor
```

The initial version of the graphical interface was generated with AI assistance and subsequently integrated into the project.

### 4.3. CGI server mode

In CGI mode, the interpreter uses the `CONTENT_LENGTH` environment variable to determine the size of the incoming request data.

To test it from a terminal:

```bash
export CONTENT_LENGTH=$(wc -c < script)
./CairoInterpreter < script
```

CGI mode generates a complete XML response, including the HTTP headers required for use with a web server.

---

## 5. Web interface

The project also includes a web interface located at:

```text
Gui/Editor.html
```

This interface can communicate with a CGI version of CairoInterpreter installed on a remote server.

To configure it, simply edit the `URL_DE_LA_CGI` variable in `Editor.html` and set it to the URL of your CGI script.

### Testing the web interface locally on Linux

You can test the web interface locally without setting up a full web server.

1. Build CairoInterpreter with `CGIMODE=true` and SDL2 disabled (`NOSDL=true`).
2. Copy the executable into the CGI directory:

```bash
cp build/CairoInterpreter Gui/cgi-bin/cairo.cgi
```

3. Edit `URL_DE_LA_CGI` in `Gui/Editor.html`:

```javascript
URL_DE_LA_CGI = "http://localhost:8000/cgi-bin/cairo.cgi";
```

4. From the `Gui` directory, start a local HTTP server:

```bash
python3 -m http.server --cgi 8000
```

5. Open the following URL in your browser:

```text
http://localhost:8000/Editor.html
```

**Note:** Python's built-in CGI support has been deprecated and removed in recent Python versions. An alternative CGI-compatible server may be required.

---

## 6. Documentation

The `doc.odt` file contains the CairoDSL language documentation.

It describes the main language instructions, available features and their usage.

---

## 7. Languages

The graphical interface supports two languages:

- **French** when the system language is French.
- **English** for all other system languages.
