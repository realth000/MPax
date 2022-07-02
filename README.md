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
  * [x] Support .mp3.
  * [ ] Support .flac.
  * [ ] **Support .cue**.
  * [ ] Show lyric.
  * [ ] **A "small window" mode**.
  * [ ] **Custom global shortcut**.
  * [ ] Show music in different groups (artists, albums and so on).

* - [ ] Play list management.
  - [x] Add play list.
  - [x] Save play list.
  - [ ] Delete play list.
  - [ ] **Export and import play lists (in or not in .m3u8)**.
  - [ ] Custom play list header.
  - [x] **Sort by custom header**.
  - [ ] **Move audio between play lists**.
  - [ ] **Groups in play list**.
  - [x] Manage audio file through play list.

* - [ ] Search.
  - [x] Basic search.
  - [ ] **Search with id3v2 info**.
  - [ ] **Search statement (for example, only search in title by "TITLE HAS xxx")**.
* - [ ] Manage id3v2 info.
  - [ ] **Read id3v2 info**.
  - [ ] Modify id3v2 info.
  - [ ] Support id3v1 and id3v2.2 to id3v2.4.
* - [ ] Better encoding (if possible).

......

## Build from source

MPax requires the following dependencies:

* Qt5 >= 5.15.1
* GCC >= 7.3 (C++ 11)
* CMake >= 3.13

## Release

Releases are [here](https://github.com/realth000/MPax/releases).

## License

MPax is licensed under the term of GPL v3.
