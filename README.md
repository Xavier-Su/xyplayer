# 项目名称

### 玄音播放器v1.0

## 项目简介

玄音播放器是一款基于GTK2.0+Sqlite3+Mplayer编写而成的本地音乐播放器，本软件诞生于毕业实习实践期间，由唐帅思老师指导，拉普拉斯喵组编写完成。

- 支持歌曲列表、播放进度条、进程控制、常用控制栏、sqlite3数据库、歌词显示、系统时间等功能。

- 播放列表使用树视图控件进行编写，支持文件管理器对话窗口进行文件过滤和读取，支持正则表达式规范路径后写入数据库。

- 播放进度条使用scale控件编写，支持按键跳转播放进度，同时显示时长和播放时间。

- 进程控制使用父子进程进行通信。

- 常用控制栏支持播放暂停、快进快退、音量加减、手动或自动切换歌曲等功能。

- 歌词显示支持颜色、字号、有无歌词的情况下对应显示，歌词存储由自封装链表功能实现。

- 系统时间支持格式化显示和一天的时间进度显示。附加功能包括关于按钮、网络曲库、背景图片等，可以个性化装扮播放器。

## 如何安装和使用

项目包含Makefile，在终端中执行make即可，前提电脑环境已经安装了GTK2.0开发环境和Mplayer软件。理论上可适用于其他嵌入式系统，请注意交叉编译链的更换。

## 版权和许可证

目前还没有选择许可证，如果后续进一步优化了软件，且这个软件有存在的价值，可能会选择许可证并发布开源。

## 联系方式

留个邮箱显得太过于专业，而本项目质量也不高，还是留个可用于交流的QQ吧；QQ：1525891375

## 常见问题

注意：本项目在Deepin20.3与Deepin20.8系统上开发完成，所安装的GTK版本（v2.24.32）与实践教学提供的Ubuntu环境中的GTK有细微版本差别，可运行，但是不保证一定没问题，在Ubuntu环境中运行可能存在滑动条鼠标拖不动的问题，不过不影响正常使用。

## 致谢

在这里，特此感谢唐帅思老师的指导，感谢组内成员提供的帮助。感谢所有为我提供了帮助的人士，谢谢你们。


