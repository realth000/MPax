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
        <img src="https://img.shields.io/badge/Qt-5.9+-blue"/>
        <a href="https://github.com/realth000/MPax/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/realth000/MPax"/></a>
    </p>
</div>


English|[简体中文](./docs/README_zh_CN.md)

## Introduction

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/268156a9ba91463d92835bb2f0f84cb4)](https://app.codacy.com/gh/realth000/MPax?utm_source=github.com&utm_medium=referral&utm_content=realth000/MPax&utm_campaign=Badge_Grade_Settings)

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
* - [x] Manage id3v2/id3v1 info.
* - [ ] Better encoding (if possible).

......

## Build from source

### Dependencies

* Qt5 >= 5.9
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

### Build steps

1. * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools``(Ubuntu2204 or Debian11)
   * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qt5-default``(Ubuntu2004 or Ubuntu1804 or Debian10)
3. ``mkdir cmake-build-release && cd cmake-build-release``
4. ``cmake ../ && cmake --build . -j $(nproc)``
5. ``mkdir translation && cp -af ../resource/translation/*.qm translation/ # Install translations.``

### Build package
* Ubuntu2204: ``cd scripts && sudo ./build_deb.sh -d Ubuntu2204``

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
