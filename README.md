<div align="center">
    <p>
    <h1>
        <img src="./docs/images/MPax@4x.png"/>
        <br/>
        MPax
    </h1>
    </p>
    <p>
        <a href="https://github.com/realth000/MPax/actions">
            <img src="https://img.shields.io/github/workflow/status/realth000/MPax/CMake?label=Linux"/></a>
        <a href="https://github.com/realth000/MPax/releases">
            <img src="https://img.shields.io/github/release/realth000/MPax"/></a>
        <img src="https://img.shields.io/badge/Qt-5.12+-blue"/>
        <a href="https://github.com/realth000/MPax/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/realth000/MPax"/></a>
    </p>
</div>


English|[简体中文](./docs/README_zh_CN.md)

## Introduction

MPax is a simple and easy-to-use music player.

![UI](./docs/images/ui_02.png)

## TODO list

* * [ ] Music management.
  * [x] Basic music playing.
  * [x] Support .mp3/.flac/.wav.
  * [ ] **Support .cue**.
  * [ ] Show lyric.
  * [ ] **A "small window" mode**.
  * [x] **Custom global shortcut**.
  * [ ] **Custom global shortcut in UI settings.**
  * [ ] Show music in different groups (artists, albums and so on).
* - [ ] Playlist management.
  - [x] Add/Save/Delete playlist.
  - [ ] **Export and import playlist.**
  - [x] Play history.
  - [ ] Custom playlist header.
  - [x] **Sort by custom header**.
  - [ ] **Move audio between playlists**.
  - [x] Manage audio file through playlists.
* - [x] Search.
  - [x] Filter search with syntax statement (title HAS xxx).
* - [ ] Manage id3v2/id3v1 info.
  - [x] **Read id3v2/id3v1 info**.
  - [ ] Modify id3v2/id3v1 info.
* - [ ] Better encoding (if possible).

......

## Build from source

### Dependencies

* Qt5 >= 5.12
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

### Build steps

1. ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qt5-default``
2. ``mkdir build && cd build``
3. ``cmake ../ && cmake --build . --target=all -j $(nproc)``
4. ``mkdir translation && cp -af ../resource/translation/*.qm translation/ # Install translations.``

## Releases

Releases are [here](https://github.com/realth000/MPax/releases).

## License

MPax is licensed under the term of GPL v3 license.

MPax uses the following libraries:

* [Qt](https://www.qt.io)

  Qt is the fastest and smartest way to produce industry-leading software that users love.

  Qt is licensed under GNU (L)GPL.

* [Skycoder42/QHotkey](https://github.com/Skycoder42/QHotkey)

  A global shortcut/hotkey for Desktop Qt-Applications.

  Licensed under the BSD-3-Clause license.

* [TagLib](https://taglib.org/)

  TagLib is a library for reading and editing the meta-data of several popular audio formats.

  TagLib is distributed under the GNU Lesser General Public License.

* [GTRONICK/QSS](https://github.com/GTRONICK/QSS)

  QT Style Sheets templates.

  Licensed under the MIT license.
