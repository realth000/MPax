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
            <img src="https://img.shields.io/github/workflow/status/realth000/MPax/CMake?label=Linux&logo=linux"/></a>
        <a href="https://github.com/realth000/MPax/releases">
            <img src="https://img.shields.io/github/release/realth000/MPax"/></a>
        <a href="https://www.qt.io">
            <img src="https://img.shields.io/badge/Qt-≥5.9-blue?logo=qt"/></a>
        <a href="https://github.com/realth000/MPax/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/realth000/MPax"/></a>
        <a href="https://www.codacy.com/">
            <img src="https://app.codacy.com/project/badge/Grade/e84cd3b100f24444ac1ac2ddb8d946c0"/></a>
    </p>
</div>


English|[简体中文](./docs/README_zh_CN.md)

## Introduction

MPax is a simple and easy-to-use music player.

![UI](./docs/images/ui_02.png)

## Usage

* Default keymap
  - Play/Pause：``Ctrl+Meta+B``
  - Previous：``Ctrl+Meta+Left``
  - Next：``Ctrl+Meta+Right``
  - Step backward/forward：``Left``/``Right``
  - Open search window：``Ctrl+F``
* Search syntax（search in metadata）：
  * Title has abc：``title HAS abc``
  * Album name has abc and 123：``album HAS abc 123``

## TODO list

* * [ ] Music management.
  * [x] Basic music playing.
  * [x] Support .mp3/.flac/.wav.
  * [ ] **Support .cue**.
  * [ ] Show lyric.
  * [ ] **Mini window mode**.
  * [x] **Custom global shortcut**.
  * [ ] **Custom global shortcut in UI settings.**
  * [ ] Show music in different groups (artists, albums and so on).
* - [ ] Playlist management.
  - [x] Add/Save/Delete playlist.
  - [x] **Export and import playlist.**
  - [x] Play history.
  - [ ] Custom playlist header.
  - [x] **Sort by custom header**.
  - [ ] **Move audio between playlists**.
  - [x] Manage audio file through playlists.
* - [x] Search.
  - [x] Filter search with syntax statement.
* - [x] Manage id3v2/id3v1 info.
* - [ ] Better encoding (if possible).

......

## Build from source

### Dependencies

* Qt5 >= 5.9
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

### Build steps

1. * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools``(Ubuntu2204)
   * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qt5-default``(Ubuntu2004 or Ubuntu1804)
3. ``mkdir cmake-build-release && cd cmake-build-release``
4. ``cmake ../ && cmake --build . -j $(nproc)``
4. ``mkdir translation && cp -af ../resource/translation/*.qm translation/ # Install translations.``
5. Build *.deb
   * Ubuntu2204: ``cd scripts && ./build_deb.sh -d Ubuntu2204``
   * Ubuntu2004: ``cd scripts && ./build_deb.sh -d Ubuntu2004``
   * Ubuntu1804: ``cd scripts && ./build_deb.sh -d Ubuntu1804``

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
