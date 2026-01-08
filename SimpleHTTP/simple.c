#include "simple_http.h"
#include "../SimpleTools/simple_tools.h"
#include "../SimpleFS/simple_fs.h"
int main(void){
    http_enable_verbose(1);
    http_host(8080);
    char *indexfile = "502 Backend cannot get hi.txt!";
    if(fs_exists("hi.txt")){
    indexfile = fs_read("hi.txt");
    }
    while(1){
        if(http_reqready()){
    if(http_is("/")){
        http_reply(indexfile);
    } else if(http_is("/stall")){
        http_stall(1);
        sleep(4);
        http_stall(0);
        http_reply("stalled!");
    } else if(http_is("/api")){
      char *yourdata = http_param("data");
      if(yourdata){
        char *yourmodifieddata = str_replace_all(yourdata, "%20", " ");
        http_reply(yourmodifieddata);
        free(yourmodifieddata);
      }  else {
      http_reply("No data or malformed data.");
      }
    } 
    else{
        http_reply("404");
        }
        }
    }
}