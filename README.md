# <h1 align="center"><img src="pirlogo.png" alt="Pir Browser Logo" width="32" align="center" /> Pir Browser</h1>

## Overview
Pir Browser is a lightweight, minimalist web browser built in C++ using the Qt framework (`QtWebEngine`), crafted to help keep the small internet fast, accessible, and free from unnecessary bloat. Styled with custom Qt Style Sheets for a clean, distraction-free interface, it focuses on essential functionality without heavy resource overhead.

The browser supports full multi-tabbed navigation, allowing users to open, reorder, switch, and close tabs that dynamically update with website favicons and truncated page titles. Alongside standard back, forward, and reload controls, it features a smart address bar that navigates directly to `http://` or `https://` URLs while automatically routing plain text queries through DuckDuckGo for private, straightforward searching. For quick privacy management, a dedicated **Clear cache** button is built right into the toolbar to wipe the HTTP cache and delete all stored cookies with a single click.

In most cases, everyday browsing relies on mainstream, resource-heavy browsers, but for circumstances where you want a transparent, lightweight alternative you control from source, Pir Browser is there for you. To compile and run the project, clone the repository from `https://github.com/alexbennett2007/pir` and build it with `qmake` or `CMake` using a C++ compiler and a Qt 5 or Qt 6 installation that includes the `QtWebEngineWidgets` module.
