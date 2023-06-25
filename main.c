#include "gui.h"

gint main(gint argc, gchar *argv[])
{
    pid_t pid;
    int result = unlink("fifo_cmd");
    if (result == 0)
    {

        mkfifo("fifo_cmd", 0777); // 管道
        printf("通信初始化成功\n");
    }
    else
    {
        mkfifo("fifo_cmd", 0777);
        perror("创建通信渠道\n");
    }

    pipe(pipe_fd);

    play_pause = PAUSE;
    gchar *args[] = {"mplayer", "-idle", "-quiet", "-slave", "-nolirc", "-input", "file=./fifo_cmd", NULL};

    pid = fork();
    if (pid == 0)
    {
        // 将标准输出设备1 重定向fd[1]
        dup2(pipe_fd[1], 1); // close(1), 重定向
        // 子进程 启动音乐播放器
        execvp("mplayer", args);
        _exit(0);
    }
    else if (pid > 0)
    {
        // 父进程 启动页面
        // 打开有名管道
        fd = open("fifo_cmd", O_WRONLY); // 只写打开
        gui_init(argc, argv);
    }
    else
    {
        // 错误处理
        fprintf(stderr, "Fork failed\n");
        return 1;
    }

    return 0;
}

void gui_init(gint argc, gchar *argv[])
{

    // 第一步：环境初始化
    gtk_init(&argc, &argv);

    // 第二步：创建窗口
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, 800, 500);
    gtk_window_set_title(GTK_WINDOW(window), "玄音播放器");
    chang_background(window, 800, 500, "bg0.jpg"); // 修改背景

    GError *error = NULL;
    icon = gdk_pixbuf_new_from_file("./img/play.png", &error);
    if (error != NULL)
    {
        g_error("Error loading icon file: %s", error->message);
    }

    // 第三步：创建 GtkFixed 容器
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // 滑动条进度条初始化
    scale_progress_init();

    // 标签控件初始化
    label_init();

    // 列表窗口初始化
    list_window_init();

    // 按钮初始化
    button_init();

    // 第六步：显示窗口
    gtk_widget_show_all(window);

    // 释放图标资源
    g_object_unref(icon);

    // 第七步：信号连接函数
    g_signal_connect(window, "destroy", G_CALLBACK(mplayer_quit), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_play_pause, "pressed", G_CALLBACK(do_pause), NULL);
    g_signal_connect(btn_forward, "pressed", G_CALLBACK(on_forward_clicked), NULL);
    g_signal_connect(btn_rewind, "pressed", G_CALLBACK(on_rewind_clicked), NULL);
    g_signal_connect(btn_pre, "pressed", G_CALLBACK(on_pre_clicked), NULL);
    g_signal_connect(btn_next, "pressed", G_CALLBACK(on_next_clicked), NULL);
    g_signal_connect(btn_vol_up, "pressed", G_CALLBACK(on_vol_up_clicked), NULL);
    g_signal_connect(btn_vol_down, "pressed", G_CALLBACK(on_vol_down_clicked), NULL);

    g_signal_connect(btn_open_file, "clicked", G_CALLBACK(on_file_clicked), NULL);

    g_signal_connect(scale, "button-press-event", G_CALLBACK(on_scale_button_press), NULL);
    g_signal_connect(scale, "button-release-event", G_CALLBACK(on_scale_button_release), NULL);

    //g_signal_connect(treeview, "button-press-event", G_CALLBACK(on_row_single), NULL);
    // 将树视图的"row-activated"事件与回调函数on_row_activated关联
    g_signal_connect(treeview, "row-activated", G_CALLBACK(on_row_activated), NULL);

    g_signal_connect(btn_gy, "pressed", G_CALLBACK(gy), "press");
    g_signal_connect(btn_playlist, "clicked", G_CALLBACK(toggle_list), (gpointer)treeview);
    g_signal_connect(btn_online, "clicked", G_CALLBACK(online_web), NULL);
    g_signal_connect(btn_theme, "clicked", G_CALLBACK(change_theme), window);
    g_signal_connect(btn_delete, "clicked", G_CALLBACK(delate_song_list), NULL);
    g_signal_connect(btn_order, "clicked", G_CALLBACK(order_set), NULL);
    // 创建线程号
    pthread_t th;
    // 创建线程
    pthread_create(&th, NULL, &print_time, NULL);

    pthread_t tid1;
    pthread_create(&tid1, NULL, &send_cmd, NULL);
    pthread_t tid2;
    pthread_create(&tid2, NULL, &recv_msg, NULL);

    // 第八步：主事件循环
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    // return 0;
}

static void do_pause()
{

    if (play_pause == PLAY)
    {
        play_pause = PAUSE;
        load_image(img_play_pause, "./img/play.png", 50, 50);
        gtk_label_set_text(GTK_LABEL(label_title), "-暂停播放-");
        g_printf("暂停\n");
        on_button_pause_clicked();
    }
    else
    {
        play_pause = PLAY;
        load_image(img_play_pause, "./img/pause.png", 50, 50);
        gtk_label_set_text(GTK_LABEL(label_title), "-正在播放-");
        g_printf("播放\n");
        on_button_pause_clicked();
    }
}

void load_image(GtkWidget *image, const gchar *file_path, const gint w, const gint h)
{
    gtk_image_clear(GTK_IMAGE(image));                                                       // 清除图像
    GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(file_path, NULL);                       // 创建图片资源
    GdkPixbuf *dest_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h, GDK_INTERP_BILINEAR); // 指定大小
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), dest_pixbuf);                                // 图片控件重新设置一张图片(pixbuf)
    g_object_unref(src_pixbuf);                                                              // 释放资源
    g_object_unref(dest_pixbuf);                                                             // 释放资源
}

void set_label_font_size_and_color(GtkWidget *label, gint size, const gchar *yanse)
{
    GdkColor color;
    gdk_color_parse(yanse, &color);
    gtk_widget_modify_fg(label, GTK_STATE_NORMAL, &color);

    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, size * PANGO_SCALE);
    gtk_widget_modify_font(label, font_desc);
    pango_font_description_free(font_desc);
}

void on_file_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileFilter *filter;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "audio/mpeg");
    gtk_file_filter_add_mime_type(filter, "audio/m4a");
    gtk_file_filter_add_mime_type(filter, "audio/ogg");
    gtk_file_filter_add_mime_type(filter, "audio/wav");
    gtk_file_filter_add_mime_type(filter, "audio/flac");
    gtk_file_filter_add_mime_type(filter, "audio/x-ms-wma");
    gtk_file_filter_set_name(filter, "Music Files");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename = NULL;
        gchar *songname = NULL;

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        g_print("选择歌曲: %s\n", filename);

        
        g_print("解析歌曲开始\n");
        analyze_songfile(filename);

        // 使用正则表达式从文件路径中提取文件名
        regex_t re;
        regmatch_t matches[2];
        if (regcomp(&re, "/([^/]+)\\.[^\\.]+$", REG_EXTENDED) != 0)
        {
            g_print("Error: Failed to compile regular expression.\n");
            return;
        }
        if (regexec(&re, filename, 2, matches, 0) == 0)
        {
            songname = g_strndup(filename + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
            g_print("Selected file RE: %s\n", songname);
            gtk_label_set_text(GTK_LABEL(label_songname), songname);
        }
        else
        {
            g_print("Error: Failed to match regular expression.\n");
        }
        regfree(&re);

        init_database();

        // 读取数据库列表
        // get_all_songName(store, iter);
        if (insert_song(1, songname, filename, "nourl") == 0)
        {
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, songname, -1);
            song_num++;
        }

        g_free(songname);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}
void chang_background(GtkWidget *widget, gint w, gint h, const gchar *path)
{

    gtk_widget_set_app_paintable(widget, TRUE); // 以TRUE标准，设置APP允许窗口可以绘图
    gtk_widget_realize(widget);

    /* 更改背景图时，图片会重叠
     * 这时要手动调用下面的函数，让窗口绘图区域失效，产生窗口重绘制事件（即 expose 事件）。
     */
    gtk_widget_queue_draw(widget); // 等待重绘制

    GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path, NULL); // 创建图片资源对象
    // w, h是指定图片的宽度和高度
    GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h, GDK_INTERP_BILINEAR); // 修改并保存获取的图片

    GdkPixmap *pixmap = NULL;

    /* 创建pixmap图像;
     * NULL：不需要蒙版;
     * 123： 0~255，透明到不透明
     */
    gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
    // 通过pixmap给widget设置一张背景图，最后一个参数必须为: FASLE
    gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);

    // 释放资源
    g_object_unref(src_pixbuf);
    g_object_unref(dst_pixbuf);
    g_object_unref(pixmap);
}
void *print_time(void *arg)
{
    time_t t;
    struct tm *timeinfo;

    while (1)
    {
        time(&t);
        timeinfo = localtime(&t);
        gint hour = timeinfo->tm_hour;
        gint min = timeinfo->tm_min;
        gdouble time_progress = ((hour * 60) + min) / 1440.0;

        sprintf(time_now, "%d年%d月%d日-%d:%d:%d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, hour, min, timeinfo->tm_sec);
        // printf("年 : 月 : 日 : 时 : 分 : 秒\n");
        // printf("%s\n",time_now);
        gdk_threads_enter();
        gtk_label_set_text(GTK_LABEL(label_day_time), time_now);
        gdk_threads_leave();
        gdk_threads_enter();
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), time_progress);
        gdk_threads_leave();
        sleep(1);
    }

    // return 0;
}

void *send_cmd(void *arg)
{
    while (1)
    {
        if (play_pause == PLAY)
        {
            
            if (if_change_song == 1)
            {
                write(fd, "get_time_length\n", strlen("get_time_length\n"));
                usleep(10000);
            }
            // write(fd, "get_percent_pos\n", strlen("get_percent_pos\n"));
            write(fd, "get_time_pos\n", strlen("get_time_pos\n"));
            usleep(10000);

            // write(fd, "get_percent_pos\n", strlen("get_percent_pos\n"));

            // "get_time_length\n"
            // "get_time_pos\n"
            // "get_percent_pos\n"
        }
        usleep(250000);
    }
}

void *recv_msg(void *arg)
{
    while (1)
    {

        gchar buf[128] = "";
        gint len = read(pipe_fd[0], buf, sizeof(buf) - 1); // 读取数据前检查数据是否超过缓冲区大小
        if (len > 0)
        {
            buf[len] = '\0'; // 确保字符串结尾
        }
        else
        {
            continue; // 没有数据则继续循环
        }

        if (if_change_song == 1)
        {
            if (strncmp(buf, "ANS_LENGTH", strlen("ANS_LENGTH")) == 0)
            {

                sscanf(buf, "ANS_LENGTH=%d", &time_length);
                // g_printf("time_length=%d\n", time_length);
                gdk_threads_enter();
                gtk_range_set_range(GTK_RANGE(scale), 0, time_length);
                gdk_threads_leave();
                gchar time_str[10] = "";
                sprintf(time_str, "%02d:%02d", time_length / 60, time_length % 60);
                gdk_threads_enter();
                gtk_label_set_text(GTK_LABEL(label_song_length_time), time_str);
                gdk_threads_leave();
                if_change_song = 0;
            }
        }

        if (strncmp(buf, "ANS_TIME_POSITION", strlen("ANS_TIME_POSITION")) == 0)
        {
            gint seconds = 0;
            sscanf(buf, "ANS_TIME_POSITION=%d", &seconds);

            if (seconds != seconds_old)//区别时间不重复
            {
                seconds_old = seconds;
                gdk_threads_enter();
                gtk_range_set_value(GTK_RANGE(scale), (gdouble)seconds);
                gdk_threads_leave();

                if (seconds == time_length - 1)//下一首跳转
                {
                    gpointer data = NULL;
                    on_next_clicked(GTK_BUTTON(btn_next), data);
                }

                gchar time_str[16] = "";
                sprintf(time_str, "%02d:%02d", seconds / 60, seconds % 60);
                gdk_threads_enter();
                gtk_label_set_text(GTK_LABEL(label_song_playing_time), time_str);
                gdk_threads_leave();

                if (IF_LRC)//有歌词
                {
                    const gchar *lrc_now = FindTextByTime(lrc_node, seconds);

                    if ((lrc_now == NULL) || (strcmp(lrc_now, lrc_old) == 0) || (strlen(lrc_now) < 1))
                    {
                        continue;
                    }

                    if (strcmp(lrc_now, "no_lrc") == 0)
                    {
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_03), "-无歌词-");
                        gdk_threads_leave();
                        continue;
                    }

                    strcpy(lrc_old, lrc_now);

                    
                    if (GetLrcNowRow() == 1)
                    {
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_02), SeekStepText(lrc_node, GetLrcNowRow() - 1));
                        gdk_threads_leave();
                    }
                    if (GetLrcNowRow() >= 2)
                    {
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_01), SeekStepText(lrc_node, GetLrcNowRow() - 2));
                        gdk_threads_leave();
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_02), SeekStepText(lrc_node, GetLrcNowRow() - 1));
                        gdk_threads_leave();
                    }
                    gdk_threads_enter();
                    gtk_label_set_text(GTK_LABEL(label_lrc_03), lrc_now);
                    gdk_threads_leave();
                    const gchar *lrc_next = SeekStepText(lrc_node, GetLrcNowRow() + 1);
                    if (lrc_next != NULL)
                    {
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_04), lrc_next);
                        gdk_threads_leave();

                        const gchar *lrc_next_next = SeekStepText(lrc_node, GetLrcNowRow() + 2);
                        if (lrc_next_next != NULL)
                        {
                            gdk_threads_enter();
                            gtk_label_set_text(GTK_LABEL(label_lrc_05), lrc_next_next);
                            gdk_threads_leave();
                        }
                        else
                        {
                            gdk_threads_enter();
                            gtk_label_set_text(GTK_LABEL(label_lrc_05), "");
                            gdk_threads_leave();
                        }
                    }
                    else
                    {
                        gdk_threads_enter();
                        gtk_label_set_text(GTK_LABEL(label_lrc_04), "");
                        gdk_threads_leave();
                    }
                }
            }
            if (IF_LRC == 0)//无歌词
            {
                gdk_threads_enter();
                gtk_label_set_text(GTK_LABEL(label_lrc_01), "");
                gtk_label_set_text(GTK_LABEL(label_lrc_02), "");
                gtk_label_set_text(GTK_LABEL(label_lrc_03), "-无歌词-");
                gtk_label_set_text(GTK_LABEL(label_lrc_04), "");
                gtk_label_set_text(GTK_LABEL(label_lrc_05), "");
                gdk_threads_leave();
            }
        }

    }
}

void on_scale_changed(GtkRange *range, gpointer user_data)
{
    gdouble value = gtk_range_get_value(range);
    g_printf("滑动条的值为：%.0f\n", value);
}

void on_scale_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS && event->button == 1)
    {
  
        // 按下左键时，记录当前值

        play_pause = PAUSE;
        write(fd, "pause\n", strlen("pause\n"));
        // 按下左键，记录当前滑动条的值
        if (event->button == 1)
        {
            jump_start = gtk_range_get_value(GTK_RANGE(widget));
            g_print("\n");
        }
    }
}

void on_scale_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE && event->button == 1)
    {

        jump_end = gtk_range_get_value(GTK_RANGE(widget));
        gchar seekcmd[20] = "";
        sprintf(seekcmd, "seek %d\n", (jump_end - jump_start));

        write(fd, seekcmd, strlen(seekcmd));
        play_pause = PLAY;

        g_print("\n");
    }
}

void analyze_songfile(gchar *filename)
{

    write(fd, "pause\n", strlen("pause\n"));
    play_pause = PAUSE;

    if (map != NULL)
    {
        map_destroy(map);
    }

    if (get_lrc_linklist(filename) == LRC_YES)//判断有无歌词
    {
        IF_LRC = 1;
    }
    else
        IF_LRC = 0;

    change_song(filename);
}

// 回调函数，用于处理条目的单击事件
void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    // 获取选中的条目的迭代器
    model = gtk_tree_view_get_model(treeview);
    gtk_tree_model_get_iter(model, &iter, path);

    // 获取迭代器所指向的条目的值（即歌曲名）
    gchar *song = NULL;
    gtk_tree_model_get(model, &iter, 0, &song, -1);

    // 打印歌曲名
    g_print("准备播放歌曲: %s\n", song);

    gtk_label_set_text(GTK_LABEL(label_songname), song);
    analyze_songfile(get_songpath(song));

    g_free(song);
}

void on_row_single(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
    g_print("你点了一下是吧\n");
}

void on_pre_clicked(GtkButton *button, gpointer data)
{
    // 获取当前选中行
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeViewColumn *column = NULL;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *song_name = NULL;
        gint row = 0;
        // g_print("当前选中的歌曲是：%s\n", song_name);

        gint row_num = gtk_tree_path_get_indices(gtk_tree_model_get_path(model, &iter))[0];
        column = gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 0);

        // printf("当前选中的行数是: %d\n", row_num+1);
        g_print("上一曲\n");
        if (order_state == ORDER_ORDER)
        {
            g_printf("顺序模式\n");
            if (row_num == 0)
            {
                row_num = song_num - 1;
            }
            else
                row_num--;
        }
        if (order_state == ORDER_LOOP)
        {
            g_printf("单曲模式\n");
            row_num = row_num;
        }
        if (order_state == ORDER_RANDOM)
        {
            g_printf("随机模式\n");
            row_num = rand() % (song_num - 1);
        }

        // 自动点击上一行
        GtkTreePath *path = gtk_tree_path_new_from_indices(row_num, -1);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, FALSE);

        // 获取迭代器并进行双击
        if (gtk_tree_model_get_iter(model, &iter, path))
        {
            gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(treeview), path, column, renderer, TRUE);
            gtk_tree_view_row_activated(GTK_TREE_VIEW(treeview), path, NULL);
        }

        // 获取当前光标所在位置的内容

        gtk_tree_model_get(model, &iter, 0, &song_name, -1);
        // g_print("当前选中的歌曲为：%s\n", song_name);

        gtk_tree_path_free(path);

        g_free(song_name);
    }
}
void on_next_clicked(GtkButton *button, gpointer data)
{

    // 获取当前选中行
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeViewColumn *column = NULL;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *song_name = NULL;
        gint row = 0;
        // g_print("当前选中的歌曲是：%s\n", song_name);

        gint row_num = gtk_tree_path_get_indices(gtk_tree_model_get_path(model, &iter))[0];
        column = gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 0);

        g_print("下一曲\n");

        if (order_state == ORDER_ORDER)
        {
            g_printf("顺序模式\n");
            if (row_num == song_num - 1)
            {
                row_num = 0;
            }
            else
                row_num++;
        }
        if (order_state == ORDER_LOOP)
        {
            g_printf("单曲模式\n");
            row_num = row_num;
        }
        if (order_state == ORDER_RANDOM)
        {
            g_printf("随机模式\n");
            row_num = rand() % (song_num - 1);
        }

        // 自动点击下一行
        GtkTreePath *path = gtk_tree_path_new_from_indices(row_num, -1);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, FALSE);

        // 获取迭代器并进行双击
        if (gtk_tree_model_get_iter(model, &iter, path))
        {
            gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(treeview), path, column, renderer, TRUE);
            gtk_tree_view_row_activated(GTK_TREE_VIEW(treeview), path, NULL);
        }

        gtk_tree_model_get(model, &iter, 0, &song_name, -1);
        // g_print("当前选中的歌曲是为：%s\n", song_name);

        gtk_tree_path_free(path);

        g_free(song_name);
    }

}

void delate_song_list(GtkButton *button, gpointer data)
{
    g_print("移除歌曲开始\n");

    // 获取当前选中行
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *song_name = NULL;

        gtk_tree_model_get(model, &iter, 0, &song_name, -1);
        g_print("当前选中的歌曲是为：%s\n", song_name);
        delete_song(song_name);

        // 清除列表模型
        gtk_list_store_clear(store);

        // 重新获取所有歌曲名称
        get_all_songName(store, iter);

        // 刷新树视图
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

        g_free(song_name);
    }
}

static void on_button_pause_clicked()
{
    if (fd != -1)
    {

        gchar *command = "pause\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

static void on_forward_clicked()
{
    // 向标准输入发送快进命令
    if (fd != -1)
    {
        gchar *command = "seek +10 0\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

static void on_rewind_clicked()
{
    // 向标准输入发送快退命令
    if (fd != -1)
    {
        gchar *command = "seek -5 0\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

static void on_vol_up_clicked()
{
    // 向标准输入发送快退命令
    if (fd != -1)
    {
        gchar *command = "volume +10\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

static void on_vol_down_clicked()
{
    // 向标准输入发送快退命令
    if (fd != -1)
    {
        gchar *command = "volume -5\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

static void on_quit_clicked()
{
    // 向标准输入发送退出命令
    if (fd != -1)
    {
        gchar *command = "quit\n";
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
    }
}

void change_song(gchar *songpath)
{
    // 文件描述符
    // 要写入的内容
    // 写入的大小
    g_printf("加载歌曲中...\n");
    gchar songcmd[400] = "";
    sprintf(songcmd, "loadfile %s\n", songpath);
    if (fd != -1)
    {
        gchar *command = songcmd;
        if (write(fd, command, strlen(command)) != strlen(command))
        {
            perror("Error writing to pipe");
        }
        command = "";
        if_change_song = 1;
        play_pause = PLAY;
        load_image(img_play_pause, "./img/pause.png", 50, 50);
        gtk_label_set_text(GTK_LABEL(label_title), "-正在播放-");
    }
}

void mplayer_quit()
{

    write(fd, "quit\n", strlen("quit\n"));
    close(fd);
}

void gy(GtkButton *button)
{

    const gchar *authors[] = {
        "-（拉普拉斯喵队员组成）-",
        "苏小峰",
        "蒋伟伦",
        "杨皓云",
        "朱睿欣",
        "任倚霖（线上）",
        "何波（线上）",
        NULL};

    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "玄音播放器");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "\nv1.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(c)2023 拉普拉斯喵（队）");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("./img/gy.png", NULL);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);

    g_object_unref(pixbuf), pixbuf = NULL;
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void online_web(GtkButton *button)
{

    const gchar *url = "https://tools.liumingye.cn/music/#/";
    gtk_show_uri(NULL, url, GDK_CURRENT_TIME, NULL);
}

void change_theme(GtkWidget *button, gpointer user_data)
{
    GtkWidget *dialog;
    GtkFileFilter *filter;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    filter = gtk_file_filter_new();
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/gif");
    gtk_file_filter_add_mime_type(filter, "image/bmp");
    gtk_file_filter_add_mime_type(filter, "image/tiff");
    gtk_file_filter_set_name(filter, "Image Files");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename = NULL;
        gchar *songname = NULL;

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        GtkWidget *window = user_data;
        g_print("选择图片: %s\n", filename);
        g_print("更换主题\n");

        chang_background(window, 800, 500, filename); // 修改背景
        g_print("更换主题成功\n");
    }
    gtk_widget_destroy(dialog);
}

void label_init()
{
    label_title = gtk_label_new("-正在播放-");
    set_label_font_size_and_color(label_title, 12, "ForestGreen");
    label_songname = gtk_label_new("-曲名-");
    set_label_font_size_and_color(label_songname, 15, "ForestGreen");
    label_day_time = gtk_label_new("-系统时间-");
    set_label_font_size_and_color(label_day_time, 10, "DimGray");
    label_day_progress = gtk_label_new("每日进度");
    set_label_font_size_and_color(label_day_progress, 10, "DimGray");

    label_song_playing_time = gtk_label_new("00:00");
    set_label_font_size_and_color(label_song_playing_time, 10, "DimGray");
    label_song_length_time = gtk_label_new("00:00");
    set_label_font_size_and_color(label_song_length_time, 10, "DimGray");

    label_lrc_01 = gtk_label_new("-");
    set_label_font_size_and_color(label_lrc_01, 12, "OliveDrab");
    label_lrc_02 = gtk_label_new("-");
    set_label_font_size_and_color(label_lrc_02, 12, "OliveDrab");
    label_lrc_03 = gtk_label_new("-歌词未加载-");
    set_label_font_size_and_color(label_lrc_03, 14, "DarkTurquoise");
    label_lrc_04 = gtk_label_new("-");
    set_label_font_size_and_color(label_lrc_04, 12, "OliveDrab");
    label_lrc_05 = gtk_label_new("-");
    set_label_font_size_and_color(label_lrc_05, 12, "OliveDrab");

    // 设置Label控件的字字符长度
    gtk_label_set_width_chars(GTK_LABEL(label_title), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_songname), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_day_time), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_day_progress), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_song_playing_time), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_song_length_time), 128);

    gtk_label_set_width_chars(GTK_LABEL(label_lrc_01), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_lrc_02), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_lrc_03), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_lrc_04), 128);
    gtk_label_set_width_chars(GTK_LABEL(label_lrc_05), 128);

    // 设置Label控件的宽度
    gtk_widget_set_size_request(label_title, 200, 20);
    gtk_widget_set_size_request(label_songname, 300, 20);
    gtk_widget_set_size_request(label_day_time, 150, 15);
    gtk_widget_set_size_request(label_day_progress, 55, 15);
    gtk_widget_set_size_request(label_song_playing_time, 50, 15);
    gtk_widget_set_size_request(label_song_length_time, 50, 15);

    gtk_widget_set_size_request(label_lrc_01, 400, 40);
    gtk_widget_set_size_request(label_lrc_02, 400, 40);
    gtk_widget_set_size_request(label_lrc_03, 400, 40);
    gtk_widget_set_size_request(label_lrc_04, 400, 40);
    gtk_widget_set_size_request(label_lrc_05, 400, 40);

    gtk_fixed_put(GTK_FIXED(fixed), label_title, 300, 20);
    gtk_fixed_put(GTK_FIXED(fixed), label_songname, 250, 50);
    gtk_fixed_put(GTK_FIXED(fixed), label_day_time, 15, 460);
    gtk_fixed_put(GTK_FIXED(fixed), label_day_progress, 15, 483);

    gtk_fixed_put(GTK_FIXED(fixed), label_song_playing_time, 145, 405);
    gtk_fixed_put(GTK_FIXED(fixed), label_song_length_time, 605, 405);

    gtk_fixed_put(GTK_FIXED(fixed), label_lrc_01, 200, 180);
    gtk_fixed_put(GTK_FIXED(fixed), label_lrc_02, 200, 220);
    gtk_fixed_put(GTK_FIXED(fixed), label_lrc_03, 200, 260);
    gtk_fixed_put(GTK_FIXED(fixed), label_lrc_04, 200, 300);
    gtk_fixed_put(GTK_FIXED(fixed), label_lrc_05, 200, 340);
}

void button_init()
{
    btn_vol_up = gtk_button_new();
    btn_vol_down = gtk_button_new();

    btn_open_file = gtk_button_new();
    btn_play_pause = gtk_button_new();
    btn_next = gtk_button_new();
    btn_pre = gtk_button_new();
    btn_forward = gtk_button_new();
    btn_rewind = gtk_button_new();

    btn_gy = gtk_button_new();
    btn_playlist = gtk_button_new();
    btn_online = gtk_button_new();
    btn_theme = gtk_button_new();
    btn_delete = gtk_button_new();
    btn_order = gtk_button_new();

    // 设置按钮为透明
    gtk_button_set_relief(GTK_BUTTON(btn_vol_up), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_vol_down), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_gy), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_playlist), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_online), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_theme), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_delete), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_order), GTK_RELIEF_NONE);

    gtk_button_set_relief(GTK_BUTTON(btn_open_file), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_play_pause), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_next), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_pre), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_forward), GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(btn_rewind), GTK_RELIEF_NONE);

    gtk_widget_set_size_request(btn_vol_up, 50, 50);
    gtk_widget_set_size_request(btn_vol_down, 50, 50);
    gtk_widget_set_size_request(btn_gy, 50, 50);
    gtk_widget_set_size_request(btn_playlist, 50, 50);
    gtk_widget_set_size_request(btn_online, 50, 50);
    gtk_widget_set_size_request(btn_theme, 50, 50);
    gtk_widget_set_size_request(btn_delete, 50, 50);
    gtk_widget_set_size_request(btn_order, 50, 50);

    gtk_widget_set_size_request(btn_open_file, 50, 50);

    gtk_widget_set_size_request(btn_play_pause, 50, 50);
    gtk_widget_set_size_request(btn_next, 50, 50);
    gtk_widget_set_size_request(btn_pre, 50, 50);
    gtk_widget_set_size_request(btn_forward, 50, 50);
    gtk_widget_set_size_request(btn_rewind, 50, 50);

    gtk_fixed_put(GTK_FIXED(fixed), btn_vol_up, 650, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_vol_down, 720, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_gy, 700, 25);
    gtk_fixed_put(GTK_FIXED(fixed), btn_playlist, 165, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_online, 700, 75);
    gtk_fixed_put(GTK_FIXED(fixed), btn_theme, 700, 125);
    gtk_fixed_put(GTK_FIXED(fixed), btn_delete, 200, 25);
    gtk_fixed_put(GTK_FIXED(fixed), btn_order, 585, 430);

    gtk_fixed_put(GTK_FIXED(fixed), btn_open_file, 50, 25);

    gtk_fixed_put(GTK_FIXED(fixed), btn_rewind, 235, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_pre, 305, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_play_pause, 375, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_next, 445, 430);
    gtk_fixed_put(GTK_FIXED(fixed), btn_forward, 515, 430);

    // // 第一步：给按钮设置图片的时候只能创建一个空白的按钮
    // GtkWidget *gtk_button_new();
    // 第二步：创建一个新的空白图片控件
    img_vol_up = gtk_image_new_from_pixbuf(NULL);
    img_vol_down = gtk_image_new_from_pixbuf(NULL);
    img_btn_gy = gtk_image_new_from_pixbuf(NULL);
    img_btn_playlist = gtk_image_new_from_pixbuf(NULL);
    img_btn_online = gtk_image_new_from_pixbuf(NULL);
    img_btn_theme = gtk_image_new_from_pixbuf(NULL);
    img_btn_delete = gtk_image_new_from_pixbuf(NULL);
    img_btn_order = gtk_image_new_from_pixbuf(NULL);

    img_open_file = gtk_image_new_from_pixbuf(NULL);
    img_forward = gtk_image_new_from_pixbuf(NULL);
    img_pre = gtk_image_new_from_pixbuf(NULL);
    img_play_pause = gtk_image_new_from_pixbuf(NULL);
    img_next = gtk_image_new_from_pixbuf(NULL);
    img_rewind = gtk_image_new_from_pixbuf(NULL);
    // 第三步：给图片控件加载图片
    load_image(img_vol_up, "./img/volume-up.png", 45, 45);
    load_image(img_vol_down, "./img/volume-down.png", 45, 45);
    load_image(img_btn_gy, "./img/gy.png", 45, 45);
    load_image(img_btn_playlist, "./img/playlist.png", 45, 45);
    load_image(img_btn_online, "./img/online.png", 45, 45);
    load_image(img_btn_theme, "./img/theme.png", 45, 45);
    load_image(img_btn_delete, "./img/delete.png", 45, 45);
    load_image(img_btn_order, "./img/order.png", 45, 45);

    load_image(img_open_file, "./img/add.png", 45, 45);
    load_image(img_forward, "./img/fast-forward.png", 45, 45);
    load_image(img_pre, "./img/previous.png", 45, 45);
    load_image(img_play_pause, "./img/play.png", 45, 45);
    load_image(img_next, "./img/next.png", 45, 45);
    load_image(img_rewind, "./img/rewind.png", 45, 45);

    // 第四步：将图片控件包含到按钮上

    gtk_container_add(GTK_CONTAINER(btn_vol_up), img_vol_up);
    gtk_container_add(GTK_CONTAINER(btn_vol_down), img_vol_down);
    gtk_container_add(GTK_CONTAINER(btn_gy), img_btn_gy);
    gtk_container_add(GTK_CONTAINER(btn_playlist), img_btn_playlist);
    gtk_container_add(GTK_CONTAINER(btn_online), img_btn_online);
    gtk_container_add(GTK_CONTAINER(btn_theme), img_btn_theme);
    gtk_container_add(GTK_CONTAINER(btn_delete), img_btn_delete);
    gtk_container_add(GTK_CONTAINER(btn_order), img_btn_order);

    gtk_container_add(GTK_CONTAINER(btn_open_file), img_open_file);
    gtk_container_add(GTK_CONTAINER(btn_forward), img_forward);
    gtk_container_add(GTK_CONTAINER(btn_pre), img_pre);
    gtk_container_add(GTK_CONTAINER(btn_play_pause), img_play_pause);
    gtk_container_add(GTK_CONTAINER(btn_next), img_next);
    gtk_container_add(GTK_CONTAINER(btn_rewind), img_rewind);
}

void list_window_init()
{
    // 创建一个列表存储模型
    store = gtk_list_store_new(1, G_TYPE_STRING); // 每个条目只有一个字符串属性（歌曲名）

    // 创建一个滚动窗口，用于包含列表视图
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // 创建一个树视图，并将其添加到滚动窗口中
    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    // 创建一个单元格渲染器，并将其添加到树视图列中
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("播放列表", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    gtk_container_add(GTK_CONTAINER(scrolled_window), treeview);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    gtk_widget_set_size_request(scrolled_window, 200, 280);
    gtk_fixed_put(GTK_FIXED(fixed), scrolled_window, 50, 80);

    // 读取数据库列表
    get_all_songName(store, iter);
}
void scale_progress_init()
{
    // 第四步：创建控件
    // 创建 GtkScale 对象
    scale = gtk_hscale_new_with_range((gdouble)0, (gdouble)100, (gdouble)1);
    gtk_range_set_value(GTK_RANGE(scale), (gdouble)0);

    // 设置 GtkScale 的属性
    gtk_scale_set_digits(GTK_SCALE(scale), 0);
    // gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    gtk_scale_set_draw_value(GTK_SCALE(scale), FALSE);
    gtk_widget_set_events(scale, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    progress = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(progress), GTK_PROGRESS_LEFT_TO_RIGHT);

    // 设置控件大小

    gtk_widget_set_size_request(scale, 400, 20);
    gtk_widget_set_size_request(progress, 640, 5);

    // 第五步：添加控件到布局

    gtk_fixed_put(GTK_FIXED(fixed), scale, 200, 400);
    gtk_fixed_put(GTK_FIXED(fixed), progress, 70, 490);
}
void toggle_list(GtkWidget *widget, gpointer data)
{
    // 获取列表控件
    GtkWidget *list = GTK_WIDGET(data);

    // 判断列表控件是否可见
    gboolean visible = gtk_widget_get_visible(list);

    // 隐藏或显示列表控件
    if (visible)
    {
        gtk_widget_hide(list);
        gtk_widget_hide(btn_delete);
    }
    else
    {
        gtk_widget_show(list);
        gtk_widget_show(btn_delete);
    }
}

/*（函数注解部分在gui.h头文件中）
功能：
参数：
返回值：
作者：
*/


void order_set(GtkButton *button)
{

    if (order_state == ORDER_ORDER)
    {
        // 单曲循环
        order_state = ORDER_LOOP;
        load_image(img_btn_order, "./img/loop.png", 50, 50);
        g_printf("单曲循环\n");
    }
    else if (order_state == ORDER_LOOP)
    {
        // 随机播放
        order_state = ORDER_RANDOM;
        load_image(img_btn_order, "./img/random.png", 50, 50);
        g_printf("随机播放\n");
    }
    else if (order_state == ORDER_RANDOM)
    {
        // 顺序播放
        order_state = ORDER_ORDER;
        load_image(img_btn_order, "./img/order.png", 50, 50);
        g_printf("顺序播放\n");
    }
}
