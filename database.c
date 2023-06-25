#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "sqlite3.h"
#include "database.h"

int song_num = 0;

int init_database()
{
    // 定义一个句柄
    sqlite3 *db;
    int ret = sqlite3_open("songdata.db", &db);
    if (ret != SQLITE_OK)
    {
        printf("打开数据库错误\n");
        return -1;
    }
    else
        printf("打开数据库成功\n");

    char *sql1 = "create table songtable (id int ,songName text, songPath text, songUrl text);";

    char *errmsg = NULL;
    sqlite3_exec(db, sql1, NULL, NULL, &errmsg);
    if (errmsg != NULL)
        printf("表已存在：%s\n", errmsg);
    else
        printf("表创建成功\n");
    sqlite3_close(db);

    return 0;
}

int insert_song(int id, char *songname, char *songpath, char *songurl)
{

    if (check_songpath(songpath) == 1)
    {
        printf("歌曲已存在\n");
        return 1;
    }
    // 定义一个句柄
    sqlite3 *db;
    int ret = sqlite3_open("songdata.db", &db);
    if (ret != SQLITE_OK)
        printf("打开数据库错误\n");
    else
        printf("打开数据库成功\n");

    // 表操作

    char sql2[300] = "";

    sprintf(sql2, "insert into songtable (songName, songPath, songUrl) values (\'%s\',\'%s\',\'%s\');", songname, songpath, songurl);
    char *errmsg = NULL;
    printf("开始加入歌曲库\n");

    sqlite3_exec(db, sql2, NULL, NULL, &errmsg);

    if (errmsg != NULL)
    {
        printf("errmsg-insert:%s\n", errmsg);
    }
    else
        printf("加入成功\n");

    sqlite3_close(db);

    return 0;
}

int check_songpath(char *songpath)
{
    printf("检查歌曲路径是否存在库中...\n");
    sqlite3 *db;
    sqlite3_open("songdata.db", &db);

    char sql_check_path[300] = "";
    sprintf(sql_check_path, "select songName from \'%s\'where songPath=\'%s\';", "songtable", songpath);
    char **table = NULL;
    char *errmsg = NULL;
    int r = 0, c = 0;
    sqlite3_get_table(db, sql_check_path, &table, &r, &c, &errmsg);
    if (errmsg != NULL)
    {
        printf("检查出错：%s\n", errmsg);
        return 0;
    }

    if (r != 0)
    {
        printf("存在歌曲:%s\n", table[1]);

        sqlite3_free_table(table);
        sqlite3_close(db);
        return 1;
    }
    if (r == 0)
    {
        printf("检查是否存在失败...\n");
        sqlite3_free_table(table);
        sqlite3_close(db);
        return 0;
    }
}

char *get_songpath(char *songname)
{
    printf("检查歌曲路径是否存在库中...\n");
    sqlite3 *db;
    sqlite3_open("songdata.db", &db);

    char sql_check_path[300] = "";
    sprintf(sql_check_path, "select songPath from \'%s\'where songName=\'%s\';", "songtable", songname);
    char **table = NULL;
    char *errmsg = NULL;
    int r = 0, c = 0;
    sqlite3_get_table(db, sql_check_path, &table, &r, &c, &errmsg);
    if (errmsg != NULL)
    {
        printf("检查出错：%s\n", errmsg);
        return NULL;
    }

    if (r != 0)
    {
        char *songpath = table[1];

    

        sqlite3_close(db);
        return songpath;
    }
    if (r == 0)
    {
        printf("检查是否存在失败...\n");
        sqlite3_free_table(table);
        sqlite3_close(db);
        return NULL;
    }
}

int get_all_songName(GtkListStore *store, GtkTreeIter iter)
{
    // 定义一个句柄
    sqlite3 *db;
    int ret = sqlite3_open("songdata.db", &db);
    if (ret != SQLITE_OK)
    {
        printf("打开数据库错误\n");
        return -1;
    }

    else
        printf("打开数据库成功\n");

    // 表操作
    char sql3[100] = "select songName from songtable;";
    char **table = NULL;
    int nrow = 0, ncolumn = 0;
    char *errmsg = NULL;
    sqlite3_get_table(db, sql3, &table, &nrow, &ncolumn, &errmsg);

    if (errmsg != NULL)
    {
        printf("errmsg-traverse:%s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    else
    {
        printf("遍历歌曲名中...\n");
        int i;

        song_num = nrow;
        printf("歌曲数量: %d\n", song_num);
        for (i = 1; i <= nrow; i++)
        {

            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, table[i], -1);
        }
    }
    sqlite3_free_table(table);
    sqlite3_close(db);

    return 0;
}

int delete_song(char *songname)
{
    // 定义一个句柄
    sqlite3 *db;
    int ret = sqlite3_open("songdata.db", &db);
    if (ret != SQLITE_OK)
    {
        printf("打开数据库错误\n");
        return 1;
    }
    else
    {
        printf("打开数据库成功\n");
    }

    // 删除操作
    char sql[300] = "";
    sprintf(sql, "delete from songtable where songName = '%s'", songname);
    char *errmsg = NULL;
    printf("开始删除歌曲 %s ...\n", songname);
    printf("%s\n", sql);
    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    if (ret != SQLITE_OK)
    {
        printf("errmsg-delete: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 1;
    }
    else
    {
        printf("删除歌曲 %s 成功\n", songname);
        sqlite3_close(db);
        return 0;
    }
}
