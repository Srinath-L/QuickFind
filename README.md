# 🚀 QuickFind — Fast File Search in C++

**QuickFind** is a high-performance **CLI (Command Line Interface)** file search tool written in **C++**, designed primarily for **Windows systems**, where built-in file search can feel slow on large drives.  
It provides **fast indexing** and **near-instant searching**, and can also be used on **Linux systems**.

---

## ✨ Features

- ⚡ Fast file search optimized for Windows
- 🌲 Trie-based prefix search
- 🔍 Supports:
  - Exact filename search
  - Prefix-based search
  - Substring-based search
- 🧵 Multithreaded directory indexing
- 🚫 Automatically skips heavy system directories:
  - `Windows`
  - `Program Files`
  - `Program Files (x86)`
  - `.git`
  - `node_modules`
- 🔡 Case-insensitive searching
- ⏱ Displays indexing time and total result count

---

## 🧠 How It Works

1. Recursively scans directories from a user-specified root path
2. Stores:
   - `filename → full path(s)` mapping using `unordered_map`
   - Filenames inside a **Trie** for fast prefix lookup
3. Indexing is parallelized using `std::thread`
4. Search is executed in three stages:
   - **Exact match** — `O(1)`
   - **Prefix match** — Trie traversal
   - **Substring match** — linear scan

---

## 🧰 Requirements

- Windows 10 or later  
- C++17 compatible compiler (**MinGW / MSVC**)  
- `<filesystem>` support enabled  

---

## 🛠 Build Instructions

### Windows (MinGW / MSVC)

```bash
g++ main.cpp -o quickfind.exe
quickfind.exe

```

---

## 📖 Usage

1. Enter the directory to index  
2. Wait for indexing to complete  
3. Search files by name  
4. Type `exit` to quit  

---

## 📤 Output

- Displays matching file paths  
- Shows total result count  
- Prints indexing time  

---

## ⚡ Performance Notes

- Designed for large Windows file systems  
- Search is near-instant after indexing  
- Indexing performance depends on disk speed and file count  

---

## ⚠️ Limitations

- Searches filenames only  
- Command-line interface  
- No persistent index storage yet  

---

## 📜 License

MIT License  

---

## 🤝 Contributing

Contributions and optimizations are welcome.

QuickFind is intended for **learning systems programming, concurrency, and high-performance C++ on Windows**.
