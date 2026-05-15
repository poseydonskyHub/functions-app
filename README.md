# Function Graph Plotter

A simple desktop application written in C++ with Qt for plotting mathematical functions on a coordinate plane.

This project was created as a learning project for working with Qt Widgets, custom painting, and basic mathematical expression parsing.

## Supported examples

The current version supports expressions such as:

- `y = x`
- `y = 2x + 3`
- `y = x^2`
- `y = x^2 + x`
- `y = 1/x`
- `y = |x|`
- `y = |x - 1| - x`
- `y = (x^2 - 1) / (x - 1)`

## Technologies

- C++
- Qt Widgets
- QPainter
- CMake

## Project status

This is an early version of the project.

Planned improvements:
- mouse wheel zoom
- trigonometric functions
- better expression parser
- improved UI
- more accurate handling of discontinuities

## License

This project is licensed under the MIT License.

## Build

You can build the project in Qt Creator or with CMake.

Example:

```bash
cmake -B build
cmake --build build 
