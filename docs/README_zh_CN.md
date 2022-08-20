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
            <img src="https://img.shields.io/github/workflow/status/realth000/MPax/CMake?label=Linux"/></a>
        <a href="https://github.com/realth000/MPax/releases">
            <img src="https://img.shields.io/github/release/realth000/MPax"/></a>
        <img src="https://img.shields.io/badge/Qt-5.12+-blue"/>
        <a href="https://github.com/realth000/MPax/blob/master/LICENSE">
            <img src="https://img.shields.io/github/license/realth000/MPax"/></a>
    </p>
</div>

[English](../README.md)|简体中文

## 介绍

MPax是一款简约易用的音乐播放器。

![UI](./images/ui_01.png)

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
  - [ ] **导出和导入播放列表。**
  - [ ] 自定义播放列表行标题。
  - [x] **播放历史。**
  - [x] **根据播放列表标题排序。**
  - [ ] **在播放列表之间移动文件。**
  - [ ] **播放列表中分组。**
  - [x] 在播放列表中管理文件。
* - [x] 搜索。
  - [x] 通过语句筛选搜索（title HAS xxx）。
* - [x] 管理id3v2/id3v1标签。
* - [ ] 更好的解码（咕咕咕）。

......

## 从源码构建

### 依赖

* Qt5 >= 5.12
* GCC >= 4.7 (C++ 11)
* CMake >= 3.13

### 编译步骤

1. ``sudo apt install g++ cmake qtbase5-dev qttools5-dev qtmultimedia5-dev libqt5x11extras5-dev``
2. ``mkdir build && cd build``
3. ``cmake ../ && cmake --build . --target=all -j $(nproc)``
4. ``mkdir translation && cp -af ../resource/translation/*.qm translation/ # 拷贝翻译``

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
