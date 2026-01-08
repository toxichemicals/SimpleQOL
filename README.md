# QOL.h — The "Quality of Life" C Library
**A single-header, cross-platform framework for rapid C development.**

`QOL.h` combines essential utilities for File Systems, HTTP Servers/Clients, Low-level Networking, and String Manipulation into one portable file. It is designed to work natively on **Windows (WinSock2)** and **Linux (POSIX)** with zero-configuration for basic tasks and optional power-ups via `libcurl` and `OpenSSL`.

---

## 🚀 Features
* **Zero-Config Amalgamation**: Just drop `QOL.h` into your project.
* **Modular Power**: Enable SSL or libcurl only when you need them via macros.
* **Cross-Platform**: Unified API for path normalization, sleeping, and networking.
* **Built-in JSON**: Lightweight non-recursive JSON object builder.
