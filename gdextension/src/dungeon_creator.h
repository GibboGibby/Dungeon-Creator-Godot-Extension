#ifndef DUNGEON_CREATOR_H
#define DUNGEON_CREATOR_H

#define CURL_STATICLIB

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/tile_map.hpp>
#include <godot_cpp/classes/tile_set_atlas_source.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/os.hpp>
#include <map>
#include <libcurl/curl/curl.h>
#include <string>


#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "opencv2/img_hash/average_hash.hpp"
#include "opencv2/imgproc/imgproc.hpp"


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
   Vector2 actualStartingPosition;
   Vector2i endingPosition;
   std::string roomString[4][4];
};

class DungeonCreator : public Node2D{
   GDCLASS(DungeonCreator, Node2D);

   private:

   std::string string_insert(std::string toInsert, std::string original, int pos);
   std::string string_delete(std::string string, int pos, int amount);
   std::string AddObstacles(std::string strTemp, RandomNumberGenerator* rng);
   std::string CreateSystemString(std::string prompt, std::string outputFile = "output.png", int steps = 40, int cfgScale = 7, int seed = -1, bool includeFaithful = true);

   public:
   DungeonLevel level;

   std::map<std::string, Vector2i> blocks;
   std::map<std::string, Vector2i> tiles;

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
   std::pair<Vector2i, int> GetBlockAtlasPos(RandomNumberGenerator* rng, char type, bool startRoom = false);



   void RunSDGen(String prompt);
   void RunGen(std::string prompt, std::string fileName);
   void CreateBackground(std::string prompt, std::string fileName);
   void CombineImages();
   cv::Mat GetAndResizeTo64(std::string file);

   void GenerateImages();

   void UpdateTileset();

   private:
   String GPTString = "Empty";
   String imageString = "Empty";
   PackedStringArray m_Themes;
   
   

   HTTPRequest* http_request;

   int NOTIFICATION_POSTINITIALIZE();

   public:
   // Getters and Setters
   String GetGPTString();
   void SetGPTString(String input);
   String GetImageString();
   void SetImageString(String input);

   Vector2i GetStartingRoomPosition();

   PackedStringArray GetThemeStrings() const;
   void SetThemeStrings(const PackedStringArray& strings);

   protected:
   static void _bind_methods();
   
};


#endif