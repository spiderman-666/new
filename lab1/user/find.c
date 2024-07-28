#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void solve(char *path, char *cur)
{
  char buf[512], *p;//声明文件名缓冲区和指针
  int fd;
  // 声明与文件相关的结构体
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){// open函数打开路径
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){//返回文件结点中的所有信息
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  //不是目录类型
  if (st.type != T_DIR)
  {
    fprintf(2,"%s isn't a directory\n",path);
    return;
  }
  //路径过长放不入缓冲区
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    fprintf(2,"path too long\n");
    close(fd);
    return;
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    //不考虑递归 "." 和 "..."
    if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
      fprintf(2,"find: cannot stat %s\n", buf);
      continue;
    }
    //目录类型递归查找
    if (st.type==T_DIR){
      solve(buf,cur);
    }//文件类型且名称与要查找的文件名相同
    else if(st.type == T_FILE && strcmp(de.name,cur)==0){
      printf("%s\n",buf);
    }
    
  }
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2,"Input Error");
    exit(1);
  }
  solve(argv[1],argv[2]);
  exit(0);
}
