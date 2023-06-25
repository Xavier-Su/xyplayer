#ifndef __DATABASE_H_
#define __DATABASE_H_ 
#include "sqlite3.h"


/*
 * 功能：初始化数据库连接
 * 参数：无
 * 返回值：0表示成功，-1表示失败
 * 作者：朱睿欣
 */
int init_database();
/*
 * 功能：检查歌曲路径是否存在
 * 参数：歌曲路径字符串
 * 返回值：0表示路径不存在，1表示路径存在
 * 作者：朱睿欣
 */
int check_songpath(char *songpath);

/*
 * 功能：根据歌曲名获取歌曲路径
 * 参数：歌曲名字符串
 * 返回值：歌曲路径字符串
 * 作者：朱睿欣
 */
char* get_songpath(char *songname);

/*
 * 功能：获取数据库中所有歌曲名
 * 参数：GtkListStore控件指针，GtkTreeIter结构体指针
 * 返回值：0表示成功，-1表示失败
 * 作者：朱睿欣
 */
int get_all_songName(GtkListStore *store,GtkTreeIter iter);

/*
 * 功能：插入新的歌曲信息到数据库
 * 参数：歌曲ID，歌曲名，歌曲路径，歌曲URL
 * 返回值：0表示成功，1表示失败
 * 作者：朱睿欣
 */
int insert_song(int id, char *songname, char *songpath, char *songurl);
/*
 * 功能：从数据库中删除指定歌曲信息
 * 参数：歌曲名
 * 返回值：0表示成功，1表示失败
 * 作者：朱睿欣
 */
int delete_song(char *songname);

#endif

