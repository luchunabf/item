#include <signal.h>
#include "db.hpp"
#include <stdio.h>
#include <fstream>
#include "httplib.h" 
#include <jsoncpp/json/json.h>
#include <sys/stat.h>

class FileUtil
{
public:
  //写文件
  static bool Write(const std::string& file_name,
      const std::string& content)//将content写入到文件file_name中去
  {
    std::ofstream file(file_name.c_str()); //打开文件
    if(!file.is_open())
    {
      return false;
    }
    file.write(content.c_str(), content.length());//将content写入文件
    file.close();//关闭文件
    return true;
  }

//读文件
static bool Read(const std:: string& file_name, std::string* content)//将file_name文件读入到content中去
{
  std::ifstream file(file_name.c_str());
  if(!file.is_open())
  {
    return false;
  }
  struct stat st;
  stat(file_name.c_str(), &st);////////////////////////////计算file_name文件大小
  content->resize(st.st_size);//将content扩容到要读取文件file_name的大小
  //一口气把整个文件读完
  //需要先知道文件的大小
  //char* 缓冲区长度
  //int 读取多长
  file.read((char*)content->c_str(), content->size());
  file.close();
  return true;
}
};


//回调函数
/*void Hello(const httplib::Request& req, httplib::Response& resp)
  //HTTP Content-Type
{
  resp.set_content("<h1>hello</h1>", "text/html");
}*/

MYSQL* mysql = NULL;

int main()
{
  using namespace httplib;
  mysql = image_system::MySQLInit();//这里连接数据库并设置编码格式
  image_system::ImageTable image_table(mysql);//创建一个（已经封装好）数据库类的对象，用对象来操作数据库
  signal(SIGINT,[](int){//处理一下信号处理函数，按ctrl+c的时候退出进程并且释放数据库
      image_system::MySQLRelease(mysql);
      exit(0);
      });

  Server server;////////////////////////////
  //客户端请求 /hello 路径的时候，执行一个特定的函数
  //制定不同的路径对应到不同的函数上，这个过程
  //称为“设置路由”
  //服务器中有两个重要的概念
  //1.请求(Request)
  //2.响应(Response)
  //[&image_table] 这是lambda 的重要特性， 捕获变量
  //
  server.Post("/image",[&image_table](const Request& req, Response& resp){
        Json::FastWriter writer;//将Json格式化输出
        Json::Value resp_json;//创建Jaon类对象，使用类似std::map
        printf("上传图片\n");
        //auto size = req.files.size();//图片的个数
        //1.对参数进行校验
        auto ret = req.has_file("upload");//参数必须传upload请求
        if(!ret)
        {
          printf("文件上传出错！\n");
          resp.status = 404;//返回响应参数的状态
          //可以使用Json 格式组织一个返回结果
          resp_json["ok"] = false;
          resp_json["reason"] = "上传文件出错，没有需要的upload字段";
          resp.set_content(writer.write(resp_json), "application/json");//将Json封装好的响应格式化写入到响应参数resp中去
          return;
        }
        //2.根据文件名称获取到文件数据file对象
        const auto& file = req.get_file_value("upload");////////////////////////
          // file.filename;
          // file.content_type;
          // 图片内容
       // auto body = req.body.substr(file.offset, file.length);
        //3.把文件属性信息插入到数据库中
        Json::Value image;//用Json封装参数出阿奴数据库类函数中
        image["image_name"]= file.filename;
        image["size"] = (int)file.length;
        image["upload_time"] = "2019/08/29";
        image["md5"] = "aaaaaa";
        image["type"] = file.content_type;
        image["path"] = "./data/" + file.filename;
        ret = image_table.Insert(image);//将封装好的Json对象传入数据库的Insert函数
        if(!ret)
        {
          printf("image_table Insert failed!\n");
          resp_json["ok"] = false;
          resp_json["reason"] = "数据库插入失败！";
          resp.status = 500;
          resp.set_content(writer.write(resp_json), "application/json");
          return;
        }

        //4.把图片保存到指定的磁盘目录中
        auto body = req.body.substr(file.offset, file.length);//将图片文件中的内容读到body中（文件的偏移量，文件的长度）
        FileUtil::Write(image["path"].asString(), body);//将body写入文件名为test.png的文件中 

        //5.构造一个响应数据通知客户端上传成功
        resp_json["ok"] = true;
        resp.status = 200;
        //printf("size = %lu ret = %d filename = %s content-type = %s\n", size, ret, file.filename.c_str(),  file.content_type.c_str());//本来应该这样输出，但是太乱太长了，所以用Json封装并格式化输出
        resp.set_content(writer.write(resp_json),"application/json");//【问题】不该加双银好的位置不要加双引号
        return;
      });



  server.Get("/image", [&image_table](const Request& req, Response& resp)
    {
      //void(req);//没有任何实际的效果
      printf("获取所有图片信息\n");
      Json::Value resp_json;
      Json::FastWriter writer;
      //1.调用数据库接口来获取数据
      bool ret = image_table.SelectAll(&resp_json);//参数是输出参数，将结果放入resp_json中
      if(!ret)
      {
        printf("查询数据库失败！\n");
        resp_json["ok"] = false;
        resp_json["reason"] = "查询数据库失败！";//失败才需要reason
        resp.status = 500;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      //2.构造响应结果返回给客户端
      resp.status =  200;
      resp.set_content(writer.write(resp_json), "application/json");//数据库中已将查询结果用Json封装好放于resp_json中，这里直接格式化输出就行
      return;
      });

  server.Get(R"(/image/(\d+))", [&image_table](const Request& req, Response& resp)
    {
      printf("获取单个图片信息\n");
      Json::Value resp_json;
      Json::FastWriter writer;
      //1. 先获取图片id
      int image_id = std::stoi(req.matches[1]);
      printf("获取id为%d 的图片信息！\n", image_id);
      //2.根据图片信息查询数据库
      bool ret = image_table.SelectOne(image_id, &resp_json);
      if(!ret)
      {
        printf("查询数据库出错！\n");
        resp_json["ok"] = false;
        resp_json["reason"] = "查询数据库出错！";
        resp.status = 404;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      //3. 把查询结果返回给客户端
      resp_json["ok"] = true;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
      });

  server.Get(R"(/show/(\d+))", [&image_table](const Request& req, Response& resp){
     printf("获取指定图片内容\n"); //这一步是查看图片，其他的是查信息，用Json封装响应，这里用string接收文件路径，从文件中读取内容
     Json::Value resp_json;
     Json::FastWriter writer;
     //1. 先获取图片id
     int image_id = std::stoi(req.matches[1]);
     printf("获取id为%d 的图片内容！\n", image_id);
     Json::Value image;
     bool ret = image_table.SelectOne(image_id, &image);//将查询到的该条结果返回到参数image中
     if(!ret)
     {
     printf("读取数据库失败！\n");
     resp_json["ok"] = false;
     resp_json["reason"] = "查询数据库出错！";                                                        
     resp.status = 404;
     resp.set_content(writer.write(resp_json), "application/json");
     return;
     }
      //2.根据目录找到文件内容，读取文件内容
     std::string image_body;
     printf("%s\n", image["path"].asCString());//打印一下文件路径（在前面通过数据库获得）
     ret = FileUtil::Read(image["path"].asString(), & image_body);//从image["path"].asString()中读取内容到image_body中
     if(!ret)
     {
       printf("读取图片文件失败！\n");
       resp_json["ok"] = false;
       resp_json["reason"] = "读取图片文件失败";
       resp.status = 500;
       resp.set_content(writer.write(resp_json), "application/json");
       return;
     }
      //3.把文件内容构造成一个响应
      resp.status = 200;
      //不同的图片， 设置的content type 是不一样的
      //如果是png 应该设为image/png
      //如果是jpg 应该设为image/jpg
      resp.set_content(image_body, image["type"].asCString());//（输出格式，类型）//这里不用Json封装，因为是直接查看图片内容
      });
    
  server.Delete(R"(/image/(\d+))", [&image_table](const Request& req, Response& resp){
      Json::Value resp_json;                                                                             
      Json::FastWriter writer;
      //1.根据图片id去数据库中查找对应的目录
      int image_id = std::stoi(req.matches[1]);
      printf("删除id为%d 的图片！\n", image_id);
      //2. 查找到对应文件的路径
      Json::Value image;
      bool ret = image_table.SelectOne(image_id, & image);
      if(!ret)
      {
      printf("删除图片文件失败！\n");
      resp_json["ok"] = false;
      resp_json["reason"] = "删除图片文件失败";
      resp.status = 404;
      resp.set_content(writer.write(resp_json), "application/json");
      return;
      }
      //3.调用数据库进行删除
      ret = image_table.Delete(image_id);
      if(!ret)
      {
        printf("读取图片文件失败！\n");
        resp_json["ok"] = false;
        resp_json["reason"] = "读取文件失败";
        resp.status = 404;
        resp.set_content(writer.write(resp_json), "application/json");
        return;
      }
      //4.删除磁盘上的文件
      //C++标准库中， 没有删除文件的方法
      //C++17标准里是有的
      //此处只能使用操作系统提供的函数了
      unlink(image["path"].asCString());

      //5.构造响应
      resp_json["ok"] = true;
      resp.status = 200;
      resp.set_content(writer.write(resp_json), "application/json");
      });


  server.set_base_dir("./wwwroot");// 设置静态文件目录
  server.listen("0.0.0.0", 9094);
  return 0;
}
