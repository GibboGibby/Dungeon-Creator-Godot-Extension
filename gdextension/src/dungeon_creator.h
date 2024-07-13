#ifndef DUNGEON_CREATOR_H
#define DUNGEON_CREATOR_H

#define CURL_STATICLIB

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <libcurl/curl/curl.h>
#include <string>



using namespace godot;

class DungeonCreator : public Node2D{
   GDCLASS(DungeonCreator, Node2D);

   public:

   DungeonCreator();
   ~DungeonCreator();

   String gpt_complete();
   String gpt_image();
   void start_test();
   static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
   std::string GetCompletion(const std::string& prompt, const std::string& model = "gpt-3.5-turbo");
   std::string GetImage(const std::string& prompt);

   String GetImageGodotOnly(String prompt);

   String DownloadFile(const String& url);

   bool UserHasConnection();
   String DungeonCreator::DownloadFileRequest(const String& url, HTTPRequest* request);

   //Downloading Image
   void _on_request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body);

   private:
   String GPTString = "Empty";
   String imageString = "Empty";

   HTTPRequest* http_request;

   public:
   String GetGPTString();
   void SetGPTString(String input);
   String GetImageString();
   void SetImageString(String input);

   protected:
   static void _bind_methods();
};


#endif