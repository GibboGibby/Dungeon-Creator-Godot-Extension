#ifndef DUNGEON_CREATOR_H
#define DUNGEON_CREATOR_H

#define CURL_STATICLIB

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <map>
#include <libcurl/curl/curl.h>
#include <string>



using namespace godot;



struct DungeonLevel
{
   DungeonLevel()
   {
      memset(roomLayout, -1, sizeof(int) * 4 * 4);
      startingPosition = Vector2i(-1,-1);
      endingPosition = Vector2i(-1,-1);
   }
   int roomLayout[4][4];
   Vector2i startingPosition;
   Vector2i endingPosition;
   std::string roomString[4][4];
};

class DungeonCreator : public Node2D{
   GDCLASS(DungeonCreator, Node2D);

   

   public:
   DungeonLevel level;

   std::map<std::string, Vector2i> blocks;

   DungeonCreator();
   ~DungeonCreator();

   String gpt_complete();
   String gpt_image();
   void start_test();
   static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
   std::string GetCompletion(const std::string& prompt, const std::string& model = "gpt-3.5-turbo");
   std::string GetImage(const std::string& prompt, const std::string& model = "dall-e-2");

   String GetImageGodotOnly(String prompt);
   String GetImageGodotOnly3(String prompt);

   String DownloadFile(const String& url);

   bool UserHasConnection();
   String DungeonCreator::DownloadFileRequest(const String& url, HTTPRequest* request);

   //Downloading Image
   void _on_request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body);

   void GenerateDungeon();

   void Display4x4(int grid[4][4]);
   void Display4x4(std::string grid[4][4]);
   
   int GibRand(RandomNumberGenerator* rng, int x, int y);
   std::string GetRoomLayout(int x, int y);

   void GenerateTiles(TileMap* tilemap);
   void GenerateChunk(TileMap* tilemap, int x, int y);
   void AddTilemapEdge(TileMap* tilemap);
   Vector2i GetBlockAtlasPos(RandomNumberGenerator* rng, char type, bool startRoom = false);

   std::string string_insert(std::string toInsert, std::string original, int pos);
   std::string string_delete(std::string string, int pos, int amount);

   std::string AddObstacles(std::string strTemp, RandomNumberGenerator* rng);


   private:
   String GPTString = "Empty";
   String imageString = "Empty";

   

   HTTPRequest* http_request;

   int NOTIFICATION_POSTINITIALIZE();

   public:
   String GetGPTString();
   void SetGPTString(String input);
   String GetImageString();
   void SetImageString(String input);

   protected:
   static void _bind_methods();
   
};


#endif