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
        {"wall", Vector2i(15,23)},
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

std::string DungeonCreator::GetImage(const std::string& prompt)
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
    jsonData["prompt"] = prompt;
    jsonData["n"] = 1;
    jsonData["size"] = "256x256";
    //jsonData["size"] = "1024x1024";
    //jsonData["model"] = "dall-e-2";

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
        requestData["messages"][0]["role"] = "user";
        requestData["messages"][0]["content"] = prompt;
        requestData["temperature"] = 0;

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


void DungeonCreator::_bind_methods()
{
   ClassDB::bind_method(D_METHOD("start_test"), &DungeonCreator::start_test);
   ClassDB::bind_method(D_METHOD("gpt_complete"), &DungeonCreator::gpt_complete);
   ClassDB::bind_method(D_METHOD("gpt_image"), &DungeonCreator::gpt_image);

   ClassDB::bind_method(D_METHOD("GetGPTString"), &DungeonCreator::GetGPTString);
   ClassDB::bind_method(D_METHOD("SetGPTString", "input"), &DungeonCreator::SetGPTString);

   ClassDB::bind_method(D_METHOD("GetImageString"), &DungeonCreator::GetImageString);
   ClassDB::bind_method(D_METHOD("SetImageString", "input"), &DungeonCreator::SetImageString);

   ClassDB::bind_method(D_METHOD("create_gpt_image", "prompt"), &DungeonCreator::GetImageGodotOnly);

   ClassDB::bind_method(D_METHOD("has_internet_connection"), &DungeonCreator::UserHasConnection);

   ClassDB::bind_method(D_METHOD("download_from_url", "url"), &DungeonCreator::DownloadFile);
   ClassDB::bind_method(D_METHOD("download_from_url_request", "url", "http_request"), &DungeonCreator::DownloadFileRequest);

   ClassDB::bind_method(D_METHOD("generate_dungeon"), &DungeonCreator::GenerateDungeon);
   ClassDB::bind_method(D_METHOD("generate_tiles", "tilemap"), &DungeonCreator::GenerateTiles);
   ClassDB::bind_method(D_METHOD("generate_tilemap_edge", "tilemap"), &DungeonCreator::AddTilemapEdge);

   ADD_PROPERTY(PropertyInfo(Variant::STRING, "GPTString"), "SetGPTString", "GetGPTString");
   ADD_PROPERTY(PropertyInfo(Variant::STRING, "imageString"), "SetImageString", "GetImageString");
}

String DungeonCreator::DownloadFile(const String& url)
{
    http_request->connect("request_completed", Callable(this, "_on_request_completed"));
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
        if (type == 2) 
        {
            n = GibRand(rng,2,4);
        }
        else
        {
            n = GibRand(rng,3,6);
        }
        switch(n)
	    {
	        case 1: { strTemp = "00000000006000060000000000000000000000000008000000000000000000000000001111111111"; break; }
	        case 2: { strTemp = "00000000000000000000000000000000000000000008000000000000000000000000001111111111"; break; }
	        case 3: { strTemp = "00000000000010021110001001111000110111129012000000111111111021111111201111111111"; break; }
	        case 4: { strTemp = "00000000000111200100011110010021111011000000002109011111111102111111121111111111"; break; }
	        // no drop
	        case 5: { strTemp = "60000600000000000000000000000000000000000008000000000000000000000000001111111111"; break; }
	        case 6: { strTemp = "11111111112222222222000000000000000000000008000000000000000000000000001111111111"; break; }
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
        strTemp = "00000000000000000000000000000000000000000000000000000000000000000000000000000000";
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
    
    return strTemp;
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
    
    Vector2i block(0,1);
    if (x == level.startingPosition.x && y == level.startingPosition.y)
    {
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
            Vector2i atlasPos = GetBlockAtlasPos(rng, levelRows[i][j]);
            if (atlasPos.x == -1 && atlasPos.y == -1)
            {
                tilemap->erase_cell(0, Vector2i(j,i) + offsetAmount);
            }
            else
            {
                tilemap->set_cell(0, Vector2i(j,i) + offsetAmount, 0, atlasPos);
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


}

void DungeonCreator::AddTilemapEdge(TileMap* tilemap)
{
    for (int i = -75; i < 75; i++)
    {
        for (int j = -75; j < 75; j++)
        {
            Vector2i pos(i,j);
            tilemap->set_cell(0, pos, 0, Vector2i(15,23));
        }
    }
}
 
// Returns (-1,-1) if the block should not be placed
Vector2i DungeonCreator::GetBlockAtlasPos(RandomNumberGenerator* rng, char type)
{
    std::string block = "empty";
    if (type == '1')
    {
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
        if (GibRand(rng, 1, 10) == 1) 
        {
            block = "stone";
        }
        else
        {
            block = "dirt";
        }
    }
    else
    {
        block = "empty";
    }
    return blocks[block];
    //return Vector2i(0,1);
}