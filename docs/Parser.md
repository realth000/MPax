# Parser

## Foobar

### 功能

1. Text Expressions

   * 基本的文字搜索（不是正则，不含通配符）

2. Numeric Expressions

   * 数值型表达式

   * *<field> GEATER <number>*

     *<field> LESS <number>*

     *<field> EQUAL <number>*

   * ``%rating% GEATER 3``

3. Metadata Expressions

   * *<field> MISSING*

     *<field> PRESENT*

   * ``genre MISSING``

4. Time Expressions

   * *<time1> BEFORE <time2>*

     *<time1> AFTER <time2>*

     *<time1> SINCE <time2>*

     *<time1> DURING <time2>*

5. Composed Queries

   * *<expression1> AND <expression2>*

     *<expression1> OR <expression2>*

     *NOT<expression>*

6. Sort result

   * *SORT BY <sort-pattern>*

     *SORT DESCENDING BY <sort-pattern>*

### Operator Summary

| Operator      | Syntax                                                       | Comment                          |
| ------------- | ------------------------------------------------------------ | -------------------------------- |
| *AFTER*       | *<time1> AFTER <time2>*                                      |                                  |
| *AND*         | *<expression1> AND <expression2>*                            |                                  |
| *BEFORE*      | *<time1> BEFORE <time2>*                                     |                                  |
| *DURING*      | *<time1> DURING <time2>*                                     |                                  |
| *DURING LAST* | *<time> DURING LAST <number> SECONDS/MINUTES/HOURS/DAYS/WEEKS* *<time> DURING LAST SECOND/MINUTE/HOUR/DAY/WEEK* |                                  |
| *EQUAL*       | *<field> EQUAL <number>*                                     |                                  |
| *GREATER*     | *<field> GREATER <number>*                                   |                                  |
| *HAS*         | *<field> HAS <string>* ** HAS <string>*                      |                                  |
| *IS*          | *<field> IS <string>*                                        |                                  |
| *LESS*        | *<field> LESS <number>*                                      |                                  |
| *MISSING*     | *<field> MISSING*                                            |                                  |
| *NOT*         | *NOT <expression>*                                           |                                  |
| *OR*          | *<expression1> OR <expression2>*                             |                                  |
| *PRESENT*     | *<field> PRESENT*                                            |                                  |
| *SINCE*       | *<time1> SINCE <time2>*                                      |                                  |
| *SORT BY*     | *SORT BY <sort-pattern>* *SORT DESCENDING BY <sort-pattern>* | Must be at the end of the query. |

### Generic fields

| Syntax                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| %title%                                                      | Title of the track. If “title” metadata field is missing, file name is used instead. |
| %artist%                                                     | Name of the artist of the track. Checks following metadata fields, in this order: “artist”, “album artist”, “composer”, “performer”. |
| %album artist%                                               | Name of the artist of the album specified track belongs to. Checks following metadata fields, in this order: “album artist”, “artist”, “composer”, “performer”. The difference between this and %artist% is that %album artist% is intended for use where consistent value across entire album is needed even when per-track artists values vary. |
| %track artist%                                               | Name of the artist of the track; present only if %album artist% is different than %artist% for specific track. Intended for use together with %album artist%, to indicate track-specific artist info, e.g. ”%album artist% - %title%[ '//' %track artist%]” - in this case, last part will be displayed only when track-specific artist info is present. |
| %album%                                                      | Name of the album specified track belongs to. Checks following metadata fields, in this order: “album”, “venue”. |
| %date%                                                       | Recording date (year) of the album specified track belongs to, using “date” metadata field. |
| %genre%                                                      | Genre of the album specified track belongs to, using “genre” metadata field. |
| %tracknumber%                                                | Two-digit index of specified track within the album. Available only when “tracknumber” field is present in track’s metadata. |
| %totaltracks%                                                | Two-digit number of tracks within the album specified track belongs to. Available only when “totaltracks” field is present in track’s metadata. |
| %discnumber%                                                 | Index of disc specified track belongs to, within the album. Available only when “discnumber”/“disc” field is present in track’s metadata. |
| %totaldiscs%                                                 | Number of discs within the album specified track belongs to. Available only when “totaldiscs” field is present in track’s metadata. |
| %codec%                                                      | Name of codec used to encode the track. If exact codec name is not available, file extension is used. |
| %codec_profile%                                              | Additional information about encoding settings used. Not always available. |
| %filename%                                                   | Name of file, excluding directory path and extension.        |
| %filename_ext%                                               | Name of the file, including extension but excluding directory path. |
| %directoryname%                                              | Name of the directory the file is in.                        |
| %path%                                                       | Full path of the file. Note that %path_sort% should be use for sorting instead of %path%. |
| %subsong%                                                    | Index of the track within the file, for use with file formats that support multiple tracks within single physical file (e.g. chapters or a cuesheet). |
| %path_sort%                                                  | Path of the file; for use with sorting, where order of subsongs within a file needs to be preserved; exact value returned by it is undefined and should not be relied on. |
| %length%                                                     | Length of the track, formatted as *[HH:]MM:SS*.              |
| %length_seconds%                                             | Length of the track, in seconds, formatted as an integer.    |
| %bitrate%                                                    | Bitrate of the track in kilobits per second (kbps); includes dynamic VBR bitrate display for currently played track. |
| %channels%                                                   | Number of channels in the track (mono/stereo/etc).           |
| %samplerate%                                                 | Sample rate of the track, in Hz.                             |
| %replaygain_track_gain%, %replaygain_album_gain%, %replaygain_track_peak%, %replaygain_album_peak%, %replaygain_track_peak_db%, %replaygain_album_peak_db% | ReplayGain information.                                      |
| %filesize%                                                   | Size of the file, in bytes.                                  |
| %filesize_natural%                                           | Size of the file, automatically formatted in appropriate units such as megabytes or kilobytes. |
| %last_modified%                                              | Last modification time of the file, formatted as *YYYY-MM-DD HH:MM:SS*. |



## 语法设计

### 关键字

1. 用于匹配字符串的
   * HAS
   * IS
2. 用于可比较的数字的：
   * GEATER
   * LESSER
   * EQUAL
3. 用于检查metadata是否存在的：
   * MISSING
   * PRESENT
4. 用于比较时间的：
   * BEFORE
   * AFTER
   * SINCE
   * DURING
   * DURING LAST
5. 用于连接表达式的：
   * AND
   * OR
   * NOT

6. 用于设置排序顺序的（**暂不实现**）：
   * SORT BY
   * SORT DESCENDING BY

### 关键字的作用范围

#### 空格

1. 空格代表**“与”**，延长前一个条件的范围，如：
   * ``title HAS c c``表示标题中含有两个c。
   * ``title HAS c c c AND * HAS dance``表示标题含有三个c，且任意部分含有dance。
   * 因此，如果表达式的值不支持多个值，只解析到第一个值，如：
     - ``totaltracks GREATER 50 100 200``表示总音轨数大于50，而且任何位置包含100和200。
2. 连续多个空格视为一个空格（不要增加复杂度）。

#### 匹配字符串

* ``\* HAS xxx``等价于直接搜索xxx。
* 可以将数字转化为字符串搜索：
  - ``totaltracks HAS 2``表示总音轨数的数量中包含2这个数字。

#### 比较数字

* 可以将字符串中的数字单独拿出来比较，如：
  - ``title GREATER 2000``表示标题中包含大于2000的数字。

#### 检查meatadata

* 无特别注意

#### 比较时间

* 无特别注意

#### 连接表达式

* 无特别注意

### 其他事项

* metadata字段可以用%括起来，也可以不括，如：%title%，title
* 文件基本信息必须用%括起来，如：%last_modified%
* 多个表达式之间连接时，单个表达式可以用()括起来，也可以不括，如：(%last_modified% HAS 2) AND title HAS b
* 如果我要匹配大写的关键字，如何对敌？
  - 暂时采取单词之前加转义的方式，加一个符号认为整个单词都是普通文本，而非关键词

#### 与foobar2000语法不同的地方

主要是foobar2000认为语法错误的地方做一些宽松处理：

* 如果值只能有一个，但是写了多个，后续多个值不认为在本表达式之内。
  - 还需商榷，**暂不实现，认为非法**。
* ~~如果()和%括起来的地方不匹配，认为是普通文本，不包含语法。~~
  - 还是不要这么做的好，如果需要匹配普通文本中的括号或者百分号，建议转义。
  - 如果真这么做的话，一些输入错误的语法会被认为是普通文本，不会产生错误提示，有误导成分，长的语句更容易被认为没有匹配，而不是输入格式错误。

#### 转义

* 暂定用``\``放在前面转义。
* 暂定允许文件信息中包含``\``，``\\``被认为是匹配``\``（尽管``\``出现在文件名中是非常不标准的）。
* 转义用来转义大写的关键字，以及``()``和``%``。
* 转义其他符号会被认定为无效转义。

## 实现步骤

大致步骤为：

1. 判断语句是否有效。
   * 但是经过以上与foobar2000不同的宽松处理，具体语句无效的场景是？
     - 括号不匹配，%不匹配
     - 无效转义
     - 无效关键字，指用括号括起来强制解析为关键字的
   * 任一字表达式无效会导致整个表达式无效
2. 表达式拆分：
   * 按照括号拆分，并记录表达式之间的关系
   * 一直拆分，拆分到无法再次拆分
3. 表达式转化为操作：
   * 根据表达式的内容转化为操作，append进操作list
   * 操作具体是指：
     - 检查某些字段转化为字符串后是否包含某些文本
     - 检查某些数字字段是否大于/等于/小于某些数字
     - 检查某些字段是否为空
4. 按照操作list依次检查每一行的内容，任一操作返回不符合则不符合，进行下一条。