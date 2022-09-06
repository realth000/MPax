<div align="center">
    <p>
    <h1>
        <img src="./images/MPax@4x.png"/>
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

[English](../README.md)|简体中文

## 介绍

MPax是一款简约易用的音乐播放器。

![UI](./images/ui_01.png)

## 功能

* 默认快捷键（Meta指开始菜单键、Win键）
  - 播放/暂停：``Ctrl+Meta+B``
  - 上一首：``Ctrl+Meta+Left``
  - 下一首：``Ctrl+Meta+Right``
  - 后退/前进：``Left``/``Right``
  - 打开搜索窗口：``Ctrl+F``
* 搜索语法（按音乐标签信息搜索）：
  * 标题含有abc：``title HAS abc``
  * 专辑含有abc和123：``album HAS abc 123``

## 计划要做的

* * [ ] 音乐管理。
  * [x] 基本的音乐播放。
  * [x] 支持.mp3/.flac/.wav。
  * [ ] **支持.cue。**
  * [ ] 显示歌词。
  * [ ] **小窗模式。**
  * [x] **自定义全局快捷键。**
  * [ ] **图形设置中修改全局快捷键。**
  * [ ] 按组分组音乐（比如按歌手和专辑分类）。
* - [ ] 播放列表管理。
  - [x] 添加/保存/删除播放列表。
  - [x] **导出和导入播放列表。**
  - [ ] 自定义播放列表行标题。
  - [x] **播放历史。**
  - [x] **根据播放列表标题排序。**
  - [ ] **在播放列表之间移动文件。**
  - [ ] **播放列表中分组。**
  - [x] 在播放列表中管理文件。
* - [x] 搜索。
  - [x] 筛选搜索语法。
* - [x] 管理id3v2/id3v1标签。
* - [ ] 更好的解码（咕咕咕）。

......

## 从源码构建

### 依赖

* Qt5 >= 5.9
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

### 编译步骤

1. * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools``(Ubuntu2204)
   * ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev qt5-default``(Ubuntu2004 or Ubuntu1804)
2. ``mkdir cmake-build-release && cd cmake-build-release``
3. ``cmake ../ && cmake --build . -j $(nproc)``
4. ``mkdir translation && cp -af ../resource/translation/*.qm translation/ # 拷贝翻译``
5. 打包成*.deb
   * Ubuntu2204: ``cd scripts && sudo ./build_deb.sh -d Ubuntu2204``
   * Ubuntu2004: ``cd scripts && sudo ./build_deb.sh -d Ubuntu2004``
   * Ubuntu1804: ``cd scripts && sudo ./build_deb.sh -d Ubuntu1804``

## 版本发布

 [在这里](https://github.com/realth000/MPax/releases)。

## 开源许可

MPax在GPL v3协议下分发。

MPax使用了以下第三方库：

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
