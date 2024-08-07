#include "dungeon_creator.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include "json.hpp"


DungeonCreator::DungeonCreator()
{
    http_request = memnew(HTTPRequest);

    // ADDING THE CHILD CRASHES FOR SOME REASON

    //add_child(http_request, false, INTERNAL_MODE_FRONT);
    //add_child(http_request);

    blocks = std::map<std::string, Vector2i>{
        {"dirt", Vector2i(0,1)},
        {"stone", Vector2i(0,0)},
        {"wall", Vector2i(28,19)},
        {"ladder", Vector2i(0,1)},
        {"ladder_top", Vector2i(1,1)},
        {"entrance", Vector2i(6,7)},
        {"exit", Vector2i(7,7)},
        {"empty", Vector2i(-1,-1)}
    };
    
    tiles = std::map<std::string, Vector2i>
    {
        {"dirt", Vector2i(0,0)},
        {"stone", Vector2i(1,0)},
        {"ladder", Vector2i(0,1)},
        {"ladder_top", Vector2i(1,1)},
        {"exit", Vector2i(2,0)},
        {"entrance", Vector2i(2,1)},
        {"spike", Vector2i(1,2)},
        {"dirt_variant", Vector2i(2,2)},
        {"wall", Vector2i(0,2)},
        {"empty", Vector2i(-1,-1)}
    };
}

DungeonCreator::~DungeonCreator()
{
    memdelete(http_request);
}

int DungeonCreator::NOTIFICATION_POSTINITIALIZE()
{
    UtilityFunctions::print("post init");
    return 0;
}



void DungeonCreator::start_test()
{
   //UtilityFunctions::print("Hello world from GDExtension");
   //UtilityFunctions::print("I am now about to ask chat gpt for some information");
   //UtilityFunctions::print("ChatGPT responds to 'Pick me a random theme for a dungeon, preferably something gothic and dark' with - ");
   //std::string gptResponse = GetCompletion("Pick me a random theme for a dungeon, preferably something gothic and dark");
   //UtilityFunctions::print(gptResponse.c_str());

   //Variant a;
}

size_t DungeonCreator::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

String DungeonCreator::gpt_complete()
{
    if (GPTString.is_empty() || GPTString == "Empty") return String("No prompt detected");
    std::string str = GPTString.utf8().get_data();
    std::string temp = GetCompletion(str);
    String newString(temp.c_str());
    return newString;
}

std::string DungeonCreator::GetImage(const std::string& prompt, const std::string& model)
{
    std::string apiKey = "sk-proj-QP7gwmynJNQGPhWlwXCXT3BlbkFJz0ridvHNwxDLxobZy5pZ";
    std::string baseUrl = "https://api.openai.com/v1/images/generations";

    std::string readBuffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl) std::cerr << "Curl not working" << std::endl;

    struct curl_slist* headers = NULL;
    
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    nlohmann::json jsonData;
    UtilityFunctions::print(("generating image using " + model + " model").c_str());
    jsonData["model"] = model;
    jsonData["prompt"] = prompt;
    jsonData["size"] = "256x256";
    jsonData["n"] = 1;
    //jsonData["size"] = "1024x1024";

    std::string jsonString = jsonData.dump();

    curl_easy_setopt(curl, CURLOPT_URL, baseUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonString.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &DungeonCreator::WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform() failed with code: " << curl_easy_strerror(res) << std::endl;

    std::cout << "Response: " << readBuffer << std::endl;
    

    nlohmann::json responseJson = nlohmann::json::parse(readBuffer);
    std::string imageUrl = responseJson["data"][0]["url"];
    return imageUrl;
}

bool DungeonCreator::UserHasConnection()
{
    std::string url = "https://www.google.com";
    bool bConnect = false;
    CURL* curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK)
            bConnect = true;

        curl_easy_cleanup(curl);
    }
    return bConnect;
}

String DungeonCreator::gpt_image()
{
    UtilityFunctions::print("It actually enters the gpt image function");
    if (imageString.is_empty() || imageString == "Empty") return String("No prompt detected");
    std::string str = imageString.utf8().get_data();
    UtilityFunctions::print("Just before get image");
    std::string temp = GetImage(str);
    UtilityFunctions::print("Just after get image");
    String newString(temp.c_str());
    return newString;
}


std::string DungeonCreator::GetCompletion(const std::string& prompt, const std::string& model)
{
    std::string apiKey = "sk-proj-QP7gwmynJNQGPhWlwXCXT3BlbkFJz0ridvHNwxDLxobZy5pZ";
    std::string baseUrl = "https://api.openai.com/v1/chat/completions";
    std::string response;
    CURL* curl = curl_easy_init();

    if (curl) {
        //UtilityFunctions::print("Curl if statement after");
        std::cout << "Curl if statement after" << std::endl;
        nlohmann::json requestData;
        requestData["model"] = model;
        requestData["messages"][0]["role"] = "system";
        requestData["messages"][0]["content"] = "You are a dungeon creation assisstant specializing in generating genres/themes and description for tiles in a 2D side-scrolling dungeon game. The genres/themes should be concise, creative and interesting. Consider the following genres: fantasy, medieval, dark, mystical, ancient ruins, steampunk, dark gothic, wasteland, hell, fairy garden. Each tile description should be very generic as the tiles will be used everywhere and will be tiling with eachother. Be creative and ensure the descriptions are cohesive and engaging.";
        requestData["messages"][1]["role"] = "user";
        requestData["messages"][1]["content"] = prompt;
        requestData["temperature"] = 1;

        std::cout << "Gets past setting request data stuff" << std::endl;

        std::string requestDataStr = requestData.dump().c_str();

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, baseUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestDataStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, requestDataStr.length());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &DungeonCreator::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);

        //UtilityFunctions::print("Gets past easy perform");d 
        std::cout << "Gets past easy perform" << std::endl;
        if (res != CURLE_OK)
        {
            //UtilityFunctions::print("Cerr is happening");
            std::cout << "Cerr is happening" << std::endl;
            std::cerr << "Curl request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    nlohmann::json jresponse = nlohmann::json::parse(response);
    return jresponse["choices"][0]["message"]["content"].get<std::string>();
}

String DungeonCreator::GetImageGodotOnly(String prompt)
{
    // Get image URL
    std::string str = prompt.utf8().get_data();
    std::string newStr = GetImage(str);

    // Download Image
    return String(newStr.c_str());
}

String DungeonCreator::GetImageGodotOnly3(String prompt)
{
    // Get image URL
    std::string str = prompt.utf8().get_data();
    std::string model = "dall-e-2";
    std::string newStr = GetImage(str, model);

    // Download Image
    return String(newStr.c_str());
}

void DungeonCreator::_bind_methods()
{
   ClassDB::bind_method(D_METHOD("start_test"), &DungeonCreator::start_test);
   ClassDB::bind_method(D_METHOD("gpt_complete"), &DungeonCreator::gpt_complete);
   ClassDB::bind_method(D_METHOD("gpt_image"), &DungeonCreator::gpt_image);

   ClassDB::bind_method(D_METHOD("GetGPTString"), &DungeonCreator::GetGPTString);
   ClassDB::bind_method(D_METHOD("SetGPTString", "input"), &DungeonCreator::SetGPTString);

   ClassDB::bind_method(D_METHOD("GetImageString"), &DungeonCreator::GetImageString);
   ClassDB::bind_method(D_METHOD("SetImageString", "input"), &DungeonCreator::SetImageString);

   ClassDB::bind_method(D_METHOD("GetThemeStrings"), &DungeonCreator::GetThemeStrings);
   ClassDB::bind_method(D_METHOD("SetThemeStrings", "strings"), &DungeonCreator::SetThemeStrings);

   ClassDB::bind_method(D_METHOD("create_gpt_image", "prompt"), &DungeonCreator::GetImageGodotOnly);
   ClassDB::bind_method(D_METHOD("create_gpt_image_3", "prompt"), &DungeonCreator::GetImageGodotOnly3);

   ClassDB::bind_method(D_METHOD("has_internet_connection"), &DungeonCreator::UserHasConnection);

   ClassDB::bind_method(D_METHOD("download_from_url", "url"), &DungeonCreator::DownloadFile);
   ClassDB::bind_method(D_METHOD("download_from_url_request", "url", "http_request"), &DungeonCreator::DownloadFileRequest);

   ClassDB::bind_method(D_METHOD("generate_dungeon"), &DungeonCreator::GenerateDungeon);
   ClassDB::bind_method(D_METHOD("generate_tiles", "tilemap"), &DungeonCreator::GenerateTiles);
   ClassDB::bind_method(D_METHOD("generate_tilemap_edge", "tilemap"), &DungeonCreator::AddTilemapEdge);

   ClassDB::bind_method(D_METHOD("get_starting_room_position"), &DungeonCreator::GetStartingRoomPosition);

   ClassDB::bind_method(D_METHOD("RunSDGen", "prompt"), &DungeonCreator::RunSDGen);
   ClassDB::bind_method(D_METHOD("generate_images"), &DungeonCreator::GenerateImages);

   ClassDB::bind_method(D_METHOD("regenerate_tileset_texture"), &DungeonCreator::UpdateTileset);

   ADD_PROPERTY(PropertyInfo(Variant::STRING, "GPTString"), "SetGPTString", "GetGPTString");
   ADD_PROPERTY(PropertyInfo(Variant::STRING, "imageString"), "SetImageString", "GetImageString");

   ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "m_Themes"), "SetThemeStrings", "GetThemeStrings");
}

String DungeonCreator::DownloadFile(const String& url)
{
    http_request->connect("request_completed", Callable(this, "_on_Wherequest_completed"));
    http_request->request(url);
    return String("done");
}

String DungeonCreator::DownloadFileRequest(const String& url, HTTPRequest* request)
{
    UtilityFunctions::print("Actually runs this function seemingly");
    // Runs but doesnt do anything. i think I need to test the saving on a random image (DISCORD IMAGE PERHAPS)
    request->connect("request_completed", Callable(this, "_on_request_completed"));
    UtilityFunctions::print("Function has been connected");
    request->request(url);
    UtilityFunctions::print("Request completed");
    //request->get_download_file();
    return String("done");
}

void DungeonCreator::_on_request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body)
{
    UtilityFunctions::print("Entered the on request completed func");
    if (response_code == 200)
    {
        Ref<Image> image = memnew(Image);
        image->load_jpg_from_buffer(body);

        //Ref<ImageTexture> texture = memnew(ImageTexture);
        //texture->create_from_image(image);

        Ref<FileAccess> file = FileAccess::open("res://downloaded_image.png", FileAccess::WRITE);
        file->store_buffer(body);
        file->close();
    }
}


String DungeonCreator::GetGPTString()
{
    return GPTString;
}

void DungeonCreator::SetGPTString(String input)
{
    GPTString = input;
}

String DungeonCreator::GetImageString()
{
    return imageString;
}

void DungeonCreator::SetImageString(String input)
{
    imageString = input;
}

PackedStringArray DungeonCreator::GetThemeStrings() const
{
    return m_Themes;
}

void DungeonCreator::SetThemeStrings(const PackedStringArray& strings)
{
    m_Themes = strings;
}

void DungeonCreator::GenerateDungeon()
{
    // Create Grid for Rooms
    int rooms[4][4];
    memset(rooms, 0, sizeof(int) * 4 * 4);

    UtilityFunctions::print("After memset");
    RandomNumberGenerator* rng = memnew(RandomNumberGenerator);
    UtilityFunctions::print("After rng");
    // Pick room to be starting room
    int startingX = rng->randi_range(0,3);
    Vector2i startingPos(startingX,3);

    Vector2i exitRoom(-1,-1);
    
    Vector2i currentPos(startingX,3);

    int lastRoom = 0;
    bool generating = true;
    while (generating)
    {
        
        int direction = 0; // 1 - left, 2 - right, 3 - down
        // Set room type
        int whereToGoNext = rng->randi_range(1,7);
        UtilityFunctions::print("current position - ", currentPos);
        UtilityFunctions::print(whereToGoNext);
        if (lastRoom == 1)
        {
            rooms[currentPos.y][currentPos.x] = 1;
        }
        else if (lastRoom == 2)
        {
            rooms[currentPos.y][currentPos.x] = 2;
        }
        if (whereToGoNext == 1 || whereToGoNext == 2 || whereToGoNext == 3)
        {
            // Go Left
            // Check if hitting wall go down
            if (currentPos.x == 0) 
            {
                direction = 3;
            }
            else
            {
                direction = 1;
            }
        }
        else if (whereToGoNext == 4 || whereToGoNext == 5 || whereToGoNext == 6)
        {
            // Go right
            // Check if hitting wall
            if (currentPos.x == 3)
            {
                direction = 3;
            }
            else
            {
                direction = 2;
            }

        }
        else
        {
            // Go down
            direction = 3;
        }

        if (lastRoom == 2)
        {
            if (direction == 1 || direction == 2)
            {
                rooms[currentPos.y][currentPos.x] = 3;
            }
        }
        switch (direction)
        {
        case 1:
            currentPos.x -= 1;
            lastRoom = 1;
            break;
        case 2:
            currentPos.x += 1;
            lastRoom = 1;
            break;
        case 3:
            if (currentPos.y == 0)
            {
                // Place Exit Room
                exitRoom.x = currentPos.x;
                exitRoom.y = currentPos.y;

                //Set room type
                if (lastRoom == 2)
                    rooms[currentPos.y][currentPos.x] = 3;
                else
                    rooms[currentPos.y][currentPos.x] = 1;

                generating = false;
                break;
            }
            rooms[currentPos.y][currentPos.x] = 2;
            currentPos.y -= 1;
            lastRoom = 2;
            break;
        default:
            break;
        }
    }


    UtilityFunctions::print("Finished Generating");
    UtilityFunctions::print("Starting room is at - ", startingPos);
    UtilityFunctions::print("Ending room is at - ", exitRoom);
    Display4x4(rooms);
    level.endingPosition = exitRoom;
    level.startingPosition = startingPos;
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            level.roomLayout[y][x] = rooms[y][x];
            level.roomString[y][x] = GetRoomLayout(x,y);
        }
    }

    Display4x4(level.roomString);
    memdelete(rng);
}

void DungeonCreator::Display4x4(int grid[4][4])
{
    for (int y = 0; y < 4; y++)
    {
        std::string temp = "";
        int tempY = 3-y;
        for (int x = 0; x < 4; x++)
        {
            temp += std::to_string(grid[tempY][x]) + ", ";
        }
        UtilityFunctions::print(temp.c_str());
    }
}

void DungeonCreator::Display4x4(std::string grid[4][4])
{
    for (int y = 0; y < 4; y++)
    {
        std::string temp = "";
        int tempY = 3-y;
        for (int x = 0; x < 4; x++)
        {
            temp += grid[tempY][x] + ", ";
        }
        UtilityFunctions::print(temp.c_str());
    }
}


int DungeonCreator::GibRand(RandomNumberGenerator* rng, int x, int y)
{
    int randNum = y - x + 1;
    return floor(rng->randf_range(0,randNum)) + x;
}

std::string DungeonCreator::GetRoomLayout(int x, int y)
{
    RandomNumberGenerator* rng = memnew(RandomNumberGenerator);
    

    std::string strTemp = "00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    int type = level.roomLayout[y][x];
    int roomPathAbove = -1;
    
    // Check if it is the starting room
    
    if (x == level.startingPosition.x && y == level.startingPosition.y)
    {
        //int n = (type == 2) ? GibRand(rng,5,8) : GibRand(rng,1,4);
        int n;
        if (type == 2) 
        {
            n = GibRand(rng,5,8);
        }
        else
        {
            n = GibRand(rng,1,4);
        }
        switch(n)
	    {
	        case 1: { strTemp = "60000600000000000000000000000000000000000008000000000000000000000000001111111111"; break; }
	        case 2: { strTemp = "11111111112222222222000000000000000000000008000000000000000000000000001111111111"; break; }
	        case 3: { strTemp = "00000000000008000000000000000000L000000000P111111000L111111000L00111111111111111"; break; }
	        case 4: { strTemp = "0000000000008000000000000000000000000L000111111P000111111L001111100L001111111111"; break; }
	        // hole
	        case 5: { strTemp = "60000600000000000000000000000000000000000008000000000000000000000000002021111120"; break; }
	        case 6: { strTemp = "11111111112222222222000000000000000000000008000000000000000000000000002021111120"; break; }
	        case 7: { strTemp = "00000000000008000000000000000000L000000000P111111000L111111000L00011111111101111"; break; }
	        case 8: { strTemp = "0000000000008000000000000000000000000L000111111P000111111L001111000L001111011111"; break; }
	    }
    }
    else if (x == level.endingPosition.x && y == level.endingPosition.y)
    {
        int typeAbove = level.roomLayout[y+1][x];
        //int n = (typeAbove == 2) ? GibRand(rng,2,4) : GibRand(rng,3,6);
        int n;
        if (typeAbove == 2) 
        {
            n = GibRand(rng,2,4);
        }
        else
        {
            n = GibRand(rng,3,6);
        }
        switch(n)
	    {
	        case 1: { strTemp = "00000000006000060000000000000000000000000008000000000000000000000000001119111111"; break; }
	        case 2: { strTemp = "00000000000000000000000000000000000000000008000000000000000000000000001111119111"; break; }
	        case 3: { strTemp = "00000000000010021110001001111000110111129012000000111111111021111111201111111111"; break; }
	        case 4: { strTemp = "00000000000111200100011110010021111011000000002109011111111102111111121111111111"; break; }
	        // no drop
	        case 5: { strTemp = "60000600000000000000000000000000000000000008000000000000000000000000001111111191"; break; }
	        case 6: { strTemp = "11111111112222222222000000000000000000000008000000000000000000000000001911111111"; break; }
	    }
    }
    else if (type == 0)
    {
        int n = 0;
        if (y == 3)
        {
            n = GibRand(rng,1,9);
        }
        else
        {
            n = GibRand(rng,1,10);
        }

        switch(n)
	    {
	        // upper plats
	        case 1: { strTemp = "00000000000010111100000000000000011010000050000000000000000000000000001111111111"; break; }
	        // high walls
	        case 2: { strTemp = "110000000040L600000011P000000011L000000011L5000000110000000011000000001111111111"; break; }
	        case 3: { strTemp = "00000000110060000L040000000P110000000L110050000L11000000001100000000111111111111"; break; }
	        case 4: { strTemp = "110000000040L600000011P000000011L000000011L0000000110000000011000000001112222111"; break; }
	        case 5: { strTemp = "00000000110060000L040000000P110000000L110000000L11000000001100000000111112222111"; break; }
	        case 6: { strTemp = "11111111110221111220002111120000022220000002222000002111120002211112201111111111"; break; }
	        case 7: { strTemp = "11111111111112222111112000021111102201111120000211111022011111200002111112222111"; break; }
	        case 8: { strTemp = "11111111110000000000110000001111222222111111111111112222221122000000221100000011"; break; }
	        case 9: { strTemp = "121111112100L2112L0011P1111P1111L2112L1111L1111L1111L1221L1100L0000L001111221111"; break; }
	        // idols
	        case 10: { strTemp = "22000000220000B0000000000000000000000000000000000000000000000000I000001111A01111"; break; }
	        // altars
	        case 11: { strTemp = "220000002200000000000000000000000000000000000000000000x0000002211112201111111111"; break; }
	    }
    }
    else if (type == 0 || type == 1)
    {
        switch(GibRand(rng,1,12))
	    {
    
	        // basic rooms
	        case 1: { strTemp = "60000600000000000000000000000000000000000050000000000000000000000000001111111111"; break; }
	        case 2: { strTemp = "60000600000000000000000000000000000000005000050000000000000000000000001111111111"; break; }
	        case 3: { strTemp = "60000600000000000000000000000000050000000000000000000000000011111111111111111111"; break; }
	        case 4: { strTemp = "60000600000000000000000600000000000000000000000000000222220000111111001111111111"; break; }
	        case 5: { strTemp = "11111111112222222222000000000000000000000050000000000000000000000000001111111111"; break; }
	        case 6: { strTemp = "11111111112111111112022222222000000000000050000000000000000000000000001111111111"; break; }
	        // low ceiling
	        case 7: { strTemp = "11111111112111111112211111111221111111120111111110022222222000000000001111111111"; break; }
	        // ladders
	        case 8: {
	            if (GibRand(rng,1,2) == 1) strTemp = "1111111111000000000L111111111P000000000L5000050000000000000000000000001111111111";
	            else strTemp = "1111111111L000000000P111111111L0000000005000050000000000000000000000001111111111"; break;
	        }
	        case 9: { strTemp = "000000000000L0000L0000P1111P0000L0000L0000P1111P0000L1111L0000L1111L001111111111"; break; }
	        // upper plats
	        case 10: { strTemp = "00000000000111111110001111110000000000005000050000000000000000000000001111111111"; break; }
	        case 11: { strTemp = "00000000000000000000000000000000000000000021111200021111112021111111121111111111"; break; }
	        // treasure below
	        case 12: {
	            if (GibRand(rng,1,2) == 1) strTemp = "2222222222000000000000000000L001111111P001050000L011000000L010000000L01111111111";
	            else strTemp = "222222222200000000000L000000000P111111100L500000100L000000110L000000011111111111"; break;
	        }
	    }
    }
    else if (type == 3)
    {
        switch(GibRand(rng,1,8))
	    {
    
	        // basic rooms
	        case 1: { strTemp = "00000000000000000000000000000000000000000050000000000000000000000000001111111111"; break; }
	        case 2: { strTemp = "00000000000000000000000000000000000000005000050000000000000000000000001111111111"; break; }
	        case 3: { strTemp = "00000000000000000000000000000050000500000000000000000000000011111111111111111111"; break; }
	        case 4: { strTemp = "00000000000000000000000600000000000000000000000000000111110000111111001111111111"; break; }
	        // upper plats
	        case 5: { strTemp = "00000000000111111110001111110000000000005000050000000000000000000000001111111111"; break; }
	        case 6: { strTemp = "00000000000000000000000000000000000000000021111200021111112021111111121111111111"; break; }
	        case 7: { strTemp = "10000000011112002111111200211100000000000022222000111111111111111111111111111111"; break; }
	        // treasure below
	        case 8: {
	            if (GibRand(rng,1,2) == 1) strTemp = "0000000000000000000000000000L001111111P001050000L011000000L010000000L01111111111";
	            else strTemp = "000000000000000000000L000000000P111111100L500000100L000000110L000000011111111111"; break;
	        }
	    }
    }
    else // drop
	{
        int typeAbove = level.roomLayout[y+1][x];
        int n = 0;
	    if (typeAbove != 2) n = GibRand(rng,1,12);
	    else n = GibRand(rng,1,8);
	    switch(n)
	    {
	        case 1: { strTemp = "00000000006000060000000000000000000000006000060000000000000000000000000000000000"; break; }
	        case 2: { strTemp = "00000000006000060000000000000000000000000000050000000000000000000000001202111111"; break; }
	        case 3: { strTemp = "00000000006000060000000000000000000000050000000000000000000000000000001111112021"; break; }
	        case 4: { strTemp = "00000000006000060000000000000000000000000000000000000000000002200002201112002111"; break; }
	        case 5: { strTemp = "00000000000000220000000000000000200002000112002110011100111012000000211111001111"; break; }
	        case 6: { strTemp = "00000000000060000000000000000000000000000000000000001112220002100000001110111111"; break; }
	        case 7: { strTemp = "00000000000060000000000000000000000000000000000000002221110000000001201111110111"; break; }
	        case 8: { strTemp = "00000000000060000000000000000000000000000000000000002022020000100001001111001111"; break; }
	        case 9: { strTemp = "11111111112222222222000000000000000000000000000000000000000000000000001120000211"; break; }
	        case 10: { strTemp = "11111111112222111111000002211100000002110000000000200000000000000000211120000211"; break; }
	        case 11: { strTemp = "11111111111111112222111220000011200000000000000000000000000012000000001120000211"; break; }
	        case 12: { strTemp = "11111111112111111112021111112000211112000002112000000022000002200002201111001111"; break; }
	    }
        
	}

    /*

    if (type == 0)
    {
        strTemp = "00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    }
    else
    {
        strTemp = "11111111111111111111111111111111111111111111111111111111111111111111111111111111";
    }
    */

   if (x == level.startingPosition.x && y == level.startingPosition.y)
   {
        //strTemp = "00000000000111111110011000000001111111100110000110011000011001111111100000000000";
   }
    std::string finalOutput = strTemp;
    finalOutput = AddObstacles(strTemp, rng);

    memdelete(rng);
    return finalOutput;
}

std::string DungeonCreator::AddObstacles(std::string strTemp, RandomNumberGenerator* rng)
{
    // Remove once function works and stuff
    return strTemp;
    for (int i = 1; i < 81; i += 1)
	{
	    int j = i;
  
	    std::string strObs1 = "00000";
	    std::string strObs2 = "00000";
	    std::string strObs3 = "00000";
	    //char tile = string_char_at(strTemp, i);
        char tile = strTemp.at(i);
    
	    if (tile == '8')
	    {
	        switch(GibRand(rng, 1,8))
	        {
	            case 1: { strObs1 = "00900"; strObs2 = "01110"; strObs3 = "11111"; break; }
	            case 2: { strObs1 = "00900"; strObs2 = "02120"; strObs3 = "02120"; break; }
	            case 3: { strObs1 = "00000"; strObs2 = "00000"; strObs3 = "92222"; break; }
	            case 4: { strObs1 = "00000"; strObs2 = "00000"; strObs3 = "22229"; break; }
	            case 5: { strObs1 = "00000"; strObs2 = "11001"; strObs3 = "19001"; break; }
	            case 6: { strObs1 = "00000"; strObs2 = "10011"; strObs3 = "10091"; break; }
	            case 7: { strObs1 = "11111"; strObs2 = "10001"; strObs3 = "40094"; break; }
	            case 8: { strObs1 = "00000"; strObs2 = "12021"; strObs3 = "12921"; break; }
	        }
	    }
	    else if (tile == '5') // ground
	    {
	        switch(GibRand(rng, 1,16))
	        {
	            case 1: { strObs1 = "11111"; strObs2 = "00000"; strObs3 = "00000"; break; }
	            case 2: { strObs1 = "00000"; strObs2 = "11110"; strObs3 = "00000"; break; }
	            case 3: { strObs1 = "00000"; strObs2 = "01111"; strObs3 = "00000"; break; }
	            case 4: { strObs1 = "00000"; strObs2 = "00000"; strObs3 = "11111"; break; }
	            case 5: { strObs1 = "00000"; strObs2 = "20200"; strObs3 = "17177"; break; }
	            case 6: { strObs1 = "00000"; strObs2 = "02020"; strObs3 = "71717"; break; }
	            case 7: { strObs1 = "00000"; strObs2 = "00202"; strObs3 = "77171"; break; }
	            case 8: { strObs1 = "00000"; strObs2 = "22200"; strObs3 = "11100"; break; }
	            case 9: { strObs1 = "00000"; strObs2 = "02220"; strObs3 = "01110"; break; }
	            case 10: { strObs1 = "00000"; strObs2 = "00222"; strObs3 = "00111"; break; }
	            case 11: { strObs1 = "11100"; strObs2 = "22200"; strObs3 = "00000"; break; }
	            case 12: { strObs1 = "01110"; strObs2 = "02220"; strObs3 = "00000"; break; }
	            case 13: { strObs1 = "00111"; strObs2 = "00222"; strObs3 = "00000"; break; }
	            case 14: { strObs1 = "00000"; strObs2 = "02220"; strObs3 = "21112"; break; }
	            case 15: { strObs1 = "00000"; strObs2 = "20100"; strObs3 = "77117"; break; }
	            case 16: { strObs1 = "00000"; strObs2 = "00102"; strObs3 = "71177"; break; }
	        }
	    }
	    else if (tile == '6') // air
	    {
	        switch(GibRand(rng, 1,10))
	        {
	            case 1: { strObs1 = "11111"; strObs2 = "00000"; strObs3 = "00000"; break; }
	            case 2: { strObs1 = "22222"; strObs2 = "00000"; strObs3 = "00000"; break; }
	            case 3: { strObs1 = "11100"; strObs2 = "22200"; strObs3 = "00000"; break; }
	            case 4: { strObs1 = "01110"; strObs2 = "02220"; strObs3 = "00000"; break; }
	            case 5: { strObs1 = "00111"; strObs2 = "00222"; strObs3 = "00000"; break; }
	            case 6: { strObs1 = "00000"; strObs2 = "01110"; strObs3 = "00000"; break; }
	            case 7: { strObs1 = "00000"; strObs2 = "01110"; strObs3 = "02220"; break; }
	            case 8: { strObs1 = "00000"; strObs2 = "02220"; strObs3 = "01110"; break; }
	            case 9: { strObs1 = "00000"; strObs2 = "00220"; strObs3 = "01111"; break; }
	            case 10: { strObs1 = "00000"; strObs2 = "22200"; strObs3 = "11100"; break; }
	        }
	    }
    
	    if (tile == '5' || tile == '6' || tile == '8')
	    {
            
            // Delete 5 chars from strTemp at j
	        strTemp = string_delete(strTemp, j, 5);
            // Insert from strObs into strTemp at j
	        strTemp = string_insert(strObs1, strTemp, j);
	        j += 10;
	        strTemp = string_delete(strTemp, j, 5);
	        strTemp = string_insert(strObs2, strTemp, j);
	        j += 10;
	        strTemp = string_delete(strTemp, j, 5);
	        strTemp = string_insert(strObs3, strTemp, j);
            
	    }
	}
}

std::string DungeonCreator::string_insert(std::string toInsert, std::string original, int pos)
{
    pos = pos - 1;
    original.insert(pos, toInsert);
    return original;
}

std::string DungeonCreator::string_delete(std::string string, int pos, int amount)
{
    pos = pos;
    string.erase(pos, amount);
    return string;
}

cv::Mat DungeonCreator::GetAndResizeTo64(std::string file)
{
    cv::Mat base = cv::imread(file);
    cv::Mat resized;
    cv::resize(base, resized, cv::Size(64,64), 0,0,cv::INTER_NEAREST);
    return resized;
}

void DungeonCreator::RunGen(std::string prompt, std::string fileName)
{
    std::string systemString = CreateSystemString(prompt, fileName);
    system(systemString.c_str());
}

void DungeonCreator::RunSDGen(String prompt)
{
    std::string promptStr = prompt.utf8().get_data();
    RunGen("seamless interesting red hell dirt", "dirt.png");

    RunGen("seamless interesting red decayed mossy stone bricks from hell", "stone.png");

    RunGen("red/brown ladder from hell with black background", "ladder.png");

    RunGen("red/brown ladder top with platform from hell with black background", "ladder_top.png");

    RunGen("open door with hellish doorframe", "exit.png");
    RunGen("closed door with hellish doorframe", "entrance.png");
    RunGen("seamless hell background wall dark", "outer_wall.png");
    RunGen("spike with black background", "spike.png");
    RunGen("seamless interesting red hell dirt", "dirt_variant.png");
    
    // Tileset structure is as follows:
    // Dirt, Stone, Exit
    // Ladder, Ladder Top, Entrance
    // Outer Wall, Spike, Dirt Variant

    CombineImages();
    

    //Node* tileMapNode = find_child("TileMap");
    //TileMap* tilemap = dynamic_cast<TileMap*>(tileMapNode);
    //tilemap->get_tileset()->get_source(1).;
}

void DungeonCreator::GenerateImages()
{
    // Generate theme: From list or completely random
    std::string themeString;
    if (m_Themes.size() == 0)
    {
        // Pick completely random theme
        std::string prompt = "Generate a very broad and simple aesthetic genre for a game level in around 5 words";
        themeString = GetCompletion(prompt);
    }
    else if (m_Themes.size() == 1)
    {
        // Pick theme adjacent to the theme selected
        // That or just not pick a new theme?
        std::string prompt = "Generate a very broad and simple aesthetic genre for a game level in around 5 words that is the same, similar, or adjacent to: " + std::string(m_Themes[0].utf8().get_data());
        themeString = GetCompletion(prompt);
    }
    else
    {
        // Pick either one of the themes or a combination
        std::string listOfThemes;
        for (String str : m_Themes)
        {
            listOfThemes += std::string(str.utf8().get_data()) + ", ";
        }
        listOfThemes.pop_back();
        listOfThemes.pop_back();
        std::string prompt = "Generate a very broad and simple aesthetic genre for a game level in around 3 words not including the world 'level', picking one from this list and either picking something similar or adjacent: " + listOfThemes;
        UtilityFunctions::print("This is the prompt");
        UtilityFunctions::print(prompt.c_str());
        themeString = GetCompletion(prompt);
        
    }

    UtilityFunctions::print("This is the theme picked");
    UtilityFunctions::print(themeString.c_str());
    // Uses theme to generate the rest


    // Generate dirt
    // Example dirt gpt string
    // describe a block texture for a game similar in role to dirt (such as sand, soil, gravel etc) that has a dark gothic theme in around 10 words with the first two being seamless and interesting
    std::string basicDirtString = "describe a tile for a 2D side-scroller dungeon game similar in role to dirt (such as dirt, sand, soil, gravel etc) with the genre: '" + themeString + "' in around 7 words";
    std::string dirtOutput = GetCompletion(basicDirtString);
    UtilityFunctions::print(("dirt output - " + dirtOutput).c_str());
    
    // Generate Stone
    std::string basicStoneString = "describe a tile for a 2D side-scroller dungeon game similar in role to stone (such as stone, stone bricks, cobblestone, concrete etc) with the genre: '" + themeString + "' in around 7 words";
    std::string stoneOutput = GetCompletion(basicStoneString);
    UtilityFunctions::print(("stone output - " + stoneOutput).c_str());

    std::string exitString = "describe a tile for a 2D side-scroller dungeon game that is the exit of a level, resembling an open door with the theme: '" + themeString + "' in around 7 words";
    std::string exitOutput = GetCompletion(exitString);
    UtilityFunctions::print(("exit output - " + exitOutput).c_str());

    std::string ladderString = "describe a tile for a 2D side-scroller dungeon game that is a ladder with the theme: '" + themeString + "' in around 7 words";
    std::string ladderOutput = GetCompletion(ladderString);
    UtilityFunctions::print(("ladder output - " + ladderOutput).c_str());
    
    std::string ladderTopString = "describe a tile for a 2D side-scroller dungeon game that is a ladder with a platform to stand on with the theme: '" + themeString + "' in around 7 words";
    std::string ladderTopOutput = GetCompletion(ladderTopString);
    UtilityFunctions::print(("ladder top output - " + ladderTopOutput).c_str());
    
    std::string entranceString = "describe a tile for a 2D side-scroller dungeon game that is the entrance of a level, resembling an closed door with the theme: '" + themeString + "' in around 7 words";
    std::string entranceOutput = GetCompletion(entranceString);
    UtilityFunctions::print(("entrance output - " + entranceOutput).c_str());
    
    std::string outerWallString = "describe a tile for a 2D side-scroller dungeon game which is the outer wall, which uses dark colours with the theme: '" + themeString + "' in around 7 words";
    std::string outerWallOutput = GetCompletion(outerWallString);
    UtilityFunctions::print(("outer wall output - " + outerWallOutput).c_str());

    std::string spikeString = "describe the tile for a 2D side-scroller dungeon game spikes with the theme: '" + themeString + "' in around 8 words";
    std::string spikeOutput = GetCompletion(spikeString);
    UtilityFunctions::print(("spike output - " + spikeOutput).c_str());

    RunGen(dirtOutput, "dirt.png");
    RunGen(dirtOutput, "dirt_variant.png");

    RunGen("seamless and interesting " + stoneOutput, "stone.png");
    RunGen(exitOutput, "exit.png");
    RunGen(ladderOutput, "ladder.png");
    RunGen(ladderTopOutput, "ladder_top.png");
    RunGen(entranceOutput, "entrance.png");
    RunGen(outerWallOutput, "outer_wall.png");
    RunGen(spikeOutput, "spike.png");

    CombineImages();
}

Vector2i DungeonCreator::GetStartingRoomPosition()
{
    return level.startingPosition;
}

void DungeonCreator::CombineImages()
{
    cv::Mat topMats[3];
    cv::Mat midMats[3];
    cv::Mat botMats[3];
    
    // Combine images
    cv::Mat dirt = GetAndResizeTo64("ai_images/dirt.png");
    cv::Mat stone = GetAndResizeTo64("ai_images/stone.png");
    cv::Mat ladder = GetAndResizeTo64("ai_images/ladder.png");
    cv::Mat ladderTop = GetAndResizeTo64("ai_images/ladder_top.png");
    cv::Mat exit = GetAndResizeTo64("ai_images/exit.png");
    cv::Mat entrance = GetAndResizeTo64("ai_images/entrance.png");
    cv::Mat outerWall = GetAndResizeTo64("ai_images/outer_wall.png");
    cv::Mat spike = GetAndResizeTo64("ai_images/spike.png");
    cv::Mat dirtVariant = GetAndResizeTo64("ai_images/dirt_variant.png");

    topMats[0] = dirt;
    topMats[1] = stone;
    topMats[2] = exit;

    midMats[0] = ladder;
    midMats[1] = ladderTop;
    midMats[2] = entrance;

    botMats[0] = outerWall;
    botMats[1] = spike;
    botMats[2] = dirtVariant;

    cv::Mat top;
    cv::Mat mid;
    cv::Mat bottom;
    cv::Mat final;
    cv::Mat verticalMats[3];

    cv::hconcat(&topMats[0], 3, top);
    cv::hconcat(&midMats[0], 3, mid);
    cv::hconcat(&botMats[0], 3, bottom);

    verticalMats[0] = top;
    verticalMats[1] = mid;
    verticalMats[2] = bottom;

    cv::vconcat(&verticalMats[0], 3, final);

    //cv::hconcat(dirtMatResized, stoneMatResized, top);
    //cv::hconcat(ladderMatResized, ladderTopMatResized, bottom);
    //cv::vconcat(top, bottom, final);

    
    
    String string = OS::get_singleton()->get_user_data_dir();
    std::string str = string.utf8().get_data();

    cv::imwrite("ai_images/combined_tileset.png", final);
    cv::imwrite((str + "/combined_tileset.png").c_str(), final);
}

std::string DungeonCreator::CreateSystemString(std::string prompt, std::string outputFile, int steps, int cfgScale, int seed)
{
    std::string outputFilename = "output.png";

    std::string sdPath = "sd\\sd.exe";
    std::string baseFolder = "\"C:\\Users\\james\\Downloads\\downloaded_models\"";
    std::string dreamshaper = "\"C:\\Users\\james\\Downloads\\downloaded_models\\dreamshaper_8.safetensors\"";
     
    std::string finalPrompt = "\"" + prompt + "<lora:faithful32:1>\"";
    std::string extraParams = "--steps " + std::to_string(steps) + " --cfg-scale " + std::to_string(cfgScale) + " -s " + std::to_string(seed);
    std::string savedFile = "ai_images/";

    std::string systemString = sdPath + " -m " + dreamshaper + " -p " + finalPrompt + " --lora-model-dir " + baseFolder + " " + extraParams + " -o " + savedFile + outputFile;
    return systemString;
}

void DungeonCreator::GenerateTiles(TileMap* tilemap)
{
    
    //GenerateChunk(tilemap, 0,0);
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            GenerateChunk(tilemap, x, y);
        }
    }
}

void DungeonCreator::GenerateChunk(TileMap* tilemap, int x, int y)
{
    bool isStartRoom = false;
    Vector2i block(0,1);
    if (x == level.startingPosition.x && y == level.startingPosition.y)
    {
        isStartRoom = true;
        block.x = 0;
        block.y = 1;
    }
    else if (x == level.endingPosition.x && y == level.endingPosition.y)
    {
        
        block.x = 0;
        block.y = 1;
    }

    std::string levelStr = level.roomString[y][x]; 
    //UtilityFunctions::print("This is the level string: ");
    //UtilityFunctions::print(level.roomString[y][x].c_str());
    Vector2i offsetAmount(x * 10, y * -8);
    std::string levelRows[8] = {"","","","","","","",""};

    
    int index = 0;
    int offset = 0;

    while (index < 8)
    {
        levelRows[index] = levelStr.substr(offset, 10);
        offset += 10;
        //index--;
        index++;
    }

    if (x == 0 && y == 0)
    {
        UtilityFunctions::print("(0,0) row is: ");
        for (int i = 0; i < 8; i++)
        {
            UtilityFunctions::print(levelRows[i].c_str());
        }
    }

    RandomNumberGenerator* rng = memnew(RandomNumberGenerator);
    
    
    //UtilityFunctions::print("printing new order: ");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            std::pair<Vector2i, int> atlasPosAndID = GetBlockAtlasPos(rng, levelRows[i][j], isStartRoom);
            Vector2i atlasPos = atlasPosAndID.first;
            
            
            if (atlasPos.x == -1 && atlasPos.y == -1)
            {
                tilemap->erase_cell(0, Vector2i(j,i) + offsetAmount);
            }
            else
            {
                tilemap->set_cell(0, Vector2i(j,i) + offsetAmount, atlasPosAndID.second, atlasPos);
                /*
                if (atlasPos == tiles["ladder"] || atlasPos == tiles["ladder_top"])
                {
                    //tilemap->set_cell(1,Vector2i(j,i) + offsetAmount, atlasPosAndID.second, atlasPos);
                }
                else
                {
                    tilemap->set_cell(0, Vector2i(j,i) + offsetAmount, atlasPosAndID.second, atlasPos);
                }
                */
            }
            /*
            if (levelRows[i][j] == '1')
            {
                if (GibRand(rng, 1,10) == 1) block = Vector2i(0,0);
                else block = Vector2i(0,1);
                Vector2i pos(j,i);
                tilemap->set_cell(0, pos + offsetAmount, 0, block);
            }
            else if (levelRows[i][j] == '2' && GibRand(rng, 1, 2) == 1)
            {
                if (GibRand(rng, 1,10) == 1) block = Vector2i(0,0);
                else block = Vector2i(0,1);
                Vector2i pos(j,i);
                tilemap->set_cell(0, pos + offsetAmount, 0, block);
            }
            else
            {
                Vector2i pos(j,i);
                tilemap->erase_cell(0, pos + offsetAmount);
            }
            */
        }
        //UtilityFunctions::print(levelRows[i].c_str());
        
    }

    memdelete(rng);
}

void DungeonCreator::UpdateTileset()
{
    Ref<Image> image = Image::load_from_file("user://combined_tileset.png");
    Ref<ImageTexture> texture = ImageTexture::create_from_image(image);
    /*

    Ref<TileSetAtlasSource> atlasSource = memnew(TileSetAtlasSource);
    atlasSource.ptr()->set_texture(texture);
    atlasSource.ptr()->set_texture_region_size(Vector2i(64,64));
    atlasSource.ptr()->set_separation(Vector2i(0,0));
    atlasSource.ptr()->set_margins(Vector2i(0,0));

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            atlasSource.ptr()->create_tile(Vector2i(i,j));
        }
    }
    */

    Node* tileMapNode = find_child("TileMap");
    TileMap* tileMap = dynamic_cast<TileMap*>(tileMapNode);
    Ref<TileSet> tileSet = tileMap->get_tileset();

    dynamic_cast<TileSetAtlasSource*>(tileSet.ptr()->get_source(1).ptr())->set_texture(texture);

    //tileSet.ptr()->remove_source(1);
    //tileSet.ptr()->add_source(atlasSource, 1);
    

}

// This should set all to a base edge
// And then in the generation, setting the edges to the correct facing edge
void DungeonCreator::AddTilemapEdge(TileMap* tilemap)
{
    for (int i = -74; i < 75; i += 1)
    {
        for (int j = -75; j < 74; j += 1)
        {
            Vector2i pos(i,j);
            
            tilemap->set_cell(0, pos, 1, tiles["wall"]);
        }
    }
}
 
// Returns (-1,-1) if the block should not be placed
std::pair<Vector2i, int> DungeonCreator::GetBlockAtlasPos(RandomNumberGenerator* rng, char type, bool startRoom)
{
    int atlasId = 1;
    std::string block = "empty";
    if (type == '1')
    {
        atlasId = 1;
        if (GibRand(rng, 1, 10) == 1) 
        {
            block = "stone";
        }
        else
        {
            block = "dirt";
        }
    }
    else if (type == '2' && GibRand(rng,1,2) == 1)
    {
        atlasId = 1;
        if (GibRand(rng, 1, 10) == 1) 
        {
            block = "stone";
        }
        else
        {
            block = "dirt";
        }
    }
    else if (type == 'L')
    {
        atlasId = 1;
        block = "ladder";
    }
    else if(type == 'P')
    {
        atlasId = 1;
        block = "ladder_top";
    }
    else if (type == '9')
    {
        UtilityFunctions::print("found a number 9");
        if (startRoom)
        {
            block = "entrance";
            UtilityFunctions::print("It was a starting room");
        }
        else
        {
            block = "exit";
            UtilityFunctions::print("it was an exit room");
        }
    }
    else
    {
        block = "empty";
    }

    if (block == "dirt")
    {
        if (GibRand(rng, 1,10) == 1)
        {
            block = "dirt_variant";
        }
    }

    if (atlasId == 1)
    {
        return std::make_pair(tiles[block], 1);
    }

    return std::make_pair(blocks[block], 0);
    //return Vector2i(0,1);
}