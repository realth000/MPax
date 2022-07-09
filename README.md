[中文文档](./docs/README_zh_CN.md)

![MPax](./docs/images/MPax@4x.png)

---

## Introduction

MPax is a simple music player aims to achieve smooth experience like foobar2000 on Linux.

MPax mainly works on managing large mount of music on disk including play lists, searching, converting music formats and an easy-to-use experience.

MPax is available on Linux and Windows now (though recommend to use foobar2000 on Windows). 

![UI](./docs/images/ui_02.png)

## TODO List

* * [ ] Music management.
  * [x] Basic music playing.
  * [x] Support .mp3/.flac/.wav.
  * [ ] **Support .cue**.
  * [ ] Show lyric.
  * [ ] **A "small window" mode**.
  * [x] **Custom global shortcut**.
  * [ ] **Custom global shortcut in UI settings.**
  * [ ] Show music in different groups (artists, albums and so on).
* - [ ] Play list management.
  - [x] Add/Save/Delete play list.
  - [ ] **Export and import play lists (in or not in .m3u8)**.
  - [x] Play history.
  - [ ] Custom play list header.
  - [x] **Sort by custom header**.
  - [ ] **Move audio between play lists**.
  - [x] Manage audio file through play list.
* - [ ] Search.
  - [x] Basic search.
  - [ ] **Search with id3v2 info**.
  - [ ] **Search statement (for example, only search in title by "TITLE HAS xxx")**.
* - [ ] Manage id3v2/id3v1 info.
  - [x] **Read id3v2/id3v1 info**.
  - [ ] Modify id3v2/id3v1 info.
* - [ ] Better encoding (if possible).

......

## Build from source

MPax requires the following dependencies:

* Qt5 >= 5.15.1
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

## Release

Releases are [here](https://github.com/realth000/MPax/releases).

## License

MPax is licensed under the term of GPL v3.

MPax uses the following libries:

* [Qt](https://www.qt.io)：Main framework. [GPL/LGPL]
* [Skycoder42/QHotkey](https://github.com/Skycoder42/QHotkey)：Global shortcut. [BSD-3-clause]
* [TagLib](https://taglib.org/)：Read and manage file metadata. [LGPL v2.1]
