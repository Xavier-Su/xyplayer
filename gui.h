#ifndef __GUI_H_
#define __GUI_H_

#include <gtk/gtk.h>

#include <regex.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gi18n.h>
#include <glib/gspawn.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <time.h>

#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>
// 进程间通信
#include <fcntl.h>
#include "map.h"
#include "lrc.h"
#include "linklist.h"

// 文件描述符
#define PAUSE 1
#define PLAY 0

#define LRC_YES 1
#define LRC_NO 0

#define ORDER_LOOP 0
#define ORDER_ORDER 1
#define ORDER_RANDOM 2

int order_state = ORDER_ORDER;

int IF_LRC=0;

extern LinkList *lrc_node;
extern int song_num;

map_t *map = NULL;


gint fd;
gint pipe_fd[2]; // 一个无名管道（接受mplayer应答）
gint jump_start = 0;
gint jump_end = 0;
gint if_change_song = 0;

gint seconds_old = 0;
gint time_length = 0;
gchar lrc_old[128] = "";

gchar time_now[100] = "";

GdkPixbuf *icon;

GtkAdjustment *adjustment;
GtkWidget *scale;
GtkWidget *progress;

GtkWidget *fixed;
GtkWidget *label_title;
GtkWidget *label_songname;
GtkWidget *label_day_time;
GtkWidget *label_day_progress;
GtkWidget *label_song_playing_time;
GtkWidget *label_song_length_time;

GtkWidget *label_lrc_01;
GtkWidget *label_lrc_02;
GtkWidget *label_lrc_03;
GtkWidget *label_lrc_04;
GtkWidget *label_lrc_05;


GtkWidget *btn_vol_up;
GtkWidget *btn_vol_down;

GtkWidget *btn_open_file;
GtkWidget *btn_play_pause;
GtkWidget *btn_next;
GtkWidget *btn_pre;
GtkWidget *btn_forward;
GtkWidget *btn_rewind;
GtkWidget *btn_gy;
GtkWidget *btn_playlist;
GtkWidget *btn_online;
GtkWidget *btn_theme;
GtkWidget *btn_delete;
GtkWidget *btn_order;



GtkWidget *img_vol_up;
GtkWidget *img_vol_down;

GtkWidget *img_open_file;
GtkWidget *img_forward;
GtkWidget *img_pre;
GtkWidget *img_play_pause;
GtkWidget *img_next;
GtkWidget *img_rewind;
GtkWidget *img_btn_gy;
GtkWidget *img_btn_playlist;
GtkWidget *img_btn_online;
GtkWidget *img_btn_theme;
GtkWidget *img_btn_delete;
GtkWidget *img_btn_order;

// 歌曲列表变量
GtkWidget *window, *treeview, *scrolled_window;
GtkListStore *store;
GtkTreeViewColumn *column;
GtkCellRenderer *renderer;
GtkTreeIter iter;

GtkTreeSelection *selection;

gint play_pause = PLAY;


/*
 * 功能：在新线程中打印当前系统时间
 * 参数：arg - 指向线程参数的指针
 * 返回值：无
 * 作者：苏小峰
 */
void *print_time(void *arg);
/*
 * 功能：每隔一段时间向子进程发送命令
 * 参数：arg - 指向线程参数的指针
 * 返回值：无
 * 作者：苏小峰
 */
void *send_cmd(void *arg);
/*
 * 功能：接收并处理从客户端发来的消息
 * 参数：arg - 指向线程参数的指针
 * 返回值：无
 * 作者：苏小峰
 */
void *recv_msg(void *arg);
/*
 * 功能：分析歌曲文件的信息并加载到列表中
 * 参数：filename - 歌曲文件名
 * 返回值：无
 * 作者：苏小峰
 */
void analyze_songfile(gchar *filename);
/*
 * 功能：处理表格控件的行被双击的事件
 * 参数：treeview - 表格控件
 *       path - 行的路径
 *       col - 列的信息
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);

/*
 * 功能：处理表格控件的行被单击的事件
 * 参数：treeview - 表格控件
 *       path - 行的路径
 *       col - 列的信息
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_row_single(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);

/*
 * 功能：初始化GUI界面
 * 参数：argc - 命令行参数数量
 *       argv - 命令行参数数组
 * 返回值：无
 * 作者：苏小峰
 */
void gui_init(gint argc, gchar *argv[]);

/*
 * 功能：处理滑块控件的值变化事件
 * 参数：range - 滑块控件
 *       user_data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_scale_changed(GtkRange *range, gpointer user_data);

/*
 * 功能：处理文件选择对话框的确认按钮点击事件，用来读取音乐文件
 * 参数：widget - 文件选择对话框
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_file_clicked(GtkWidget *widget, gpointer data);

/*
 * 功能：更改播放器页面的背景图片
 * 参数：widget - 图片控件
 *       w - 控件宽度
 *       h - 控件高度
 *       path - 新图片的文件路径
 * 返回值：无
 * 作者：苏小峰
 */
void chang_background(GtkWidget *widget, gint w, gint h, const gchar *path);

/*
 * 功能：加载图片文件到指定的控件中
 * 参数：image - 图片控件
 *       file_path - 图片文件路径
 *       w - 控件宽度
 *       h - 控件高度
 * 返回值：无
 * 作者：苏小峰
 */
void load_image(GtkWidget *image, const gchar *file_path, const gint w, const gint h);

/*
 * 功能：设置标签控件的字体大小和颜色
 * 参数：label - 标签控件
 *       size - 字体大小
 *       yanse - 字体颜色
 * 返回值：无
 * 作者：苏小峰
 */
void set_label_font_size_and_color(GtkWidget *label, gint size, const gchar *yanse);

/*
 * 功能：处理滑块控件的鼠标松开事件
 * 参数：widget - 滑块控件
 *       event - 鼠标事件对象
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_scale_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data);

/*
 * 功能：处理滑块控件的鼠标按下事件
 * 参数：widget - 滑块控件
 *       event - 鼠标事件对象
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_scale_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);

/*
 * 功能：处理“上一首”按钮的点击事件
 * 参数：button - 按钮控件
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小
 */
void on_pre_clicked(GtkButton *button, gpointer data);

/*
 * 功能：处理“下一首”按钮的点击事件
 * 参数：button - 按钮控件
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void on_next_clicked(GtkButton *button, gpointer data);

/*
 * 功能：删除歌曲列表中选中的歌曲
 * 参数：button - 按钮控件
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void delate_song_list(GtkButton *button, gpointer data);
/*
 * 功能：处理歌曲列表的隐藏与显示事件
 * 参数：widget - 列表控件
 *       data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void toggle_list(GtkWidget* widget, gpointer data);

/*
 * 功能：设置列表中歌曲的播放模式
 * 参数：button - 按钮控件
 * 返回值：无
 * 作者：苏小峰
 */
void order_set(GtkButton *button);

/*
 * 功能：打开在线的音乐下载网站
 * 参数：button - 按钮控件
 * 返回值：无
 * 作者：苏小峰
 */
void online_web(GtkButton *button);

/*
 * 功能：处理页面背景切换事件
 * 参数：button - 按钮控件
 *       user_data - 传递给回调函数的用户数据指针
 * 返回值：无
 * 作者：苏小峰
 */
void change_theme(GtkWidget *button, gpointer user_data);

/*
 * 功能：暂停播放功能
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void do_pause();
/*
 * 功能：退出播放器
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
void mplayer_quit();

/*
 * 功能：切换歌曲
 * 参数：songpath - 歌曲文件路径
 * 返回值：无
 * 作者：苏小峰
 */
void change_song(gchar *songpath);

/*
 * 功能：处理暂停按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_button_pause_clicked();

/*
 * 功能：处理快进按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_forward_clicked();

/*
 * 功能：处理快退按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_rewind_clicked();

/*
 * 功能：处理加音量按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_vol_up_clicked();

/*
 * 功能：处理减音量按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_vol_down_clicked();

/*
 * 功能：处理退出按钮的单击事件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
static void on_quit_clicked();

/*
 * 功能：处理关于按钮的单击事件
 * 参数：button - 按钮控件
 * 返回值：无
 * 作者：苏小峰
 */
void gy(GtkButton *button);
/*
 * 功能：初始化标签控件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
void label_init();
/*
 * 功能：初始化按钮控件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
void button_init();
/*
 * 功能：初始化列表窗口控件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
void list_window_init();
/*
 * 功能：初始化进度条控件
 * 参数：无
 * 返回值：无
 * 作者：苏小峰
 */
void scale_progress_init();


#endif
