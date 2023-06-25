#ifndef __LRC_H_
#define __LRC_H_ 
#include "map.h"
#include "linklist.h"


#define MAX_LINE_LENGTH 1024


/*
 * 功能：读取指定文件中的歌词并将其转换为map
 * 参数：filename - 歌词文件路径
 * 返回值：map表指针
 * 作者：杨皓云
 */
map_t* get_lrc_map( char *filename );
/*
 * 功能：获取歌曲的歌词并处理为链表
 * 参数：file_path - 歌曲文件路径
 * 返回值：无
 * 作者：杨皓云
 */
void GetAndDealLrc(char* file_path);
/*
 * 功能：读取指定文件中的歌词并将其转换为链表
 * 参数：filename - 歌词文件路径
 * 返回值：链表长度
 * 作者：杨皓云
 */
int get_lrc_linklist(char *filename);

#endif

