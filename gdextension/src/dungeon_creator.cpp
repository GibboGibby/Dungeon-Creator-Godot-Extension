#include "dungeon_creator.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include "json.hpp"


DungeonCreator::DungeonCreator()
{
    http_request = memnew(HTTPRequest);

    // ADDING THE CHILD CRASHES FOR SOME REASON

    //add_child(http_request, false, INTERNAL_MODE_FRONT);
    http_request->connect("request_completed", Callable(this, "_on_request_completed"));
}

DungeonCreator::~DungeonCreator()
{
    memdelete(http_request);
}

void DungeonCreator::start_test()
{
   //UtilityFunctions::print("Hello world from GDExtension");
   //UtilityFunctions::print("I am now about to ask chat gpt for some information");
   //UtilityFunctions::print("ChatGPT responds to 'Pick me a random theme for a dungeon, preferably something gothic and dark' with - ");
   //std::string gptResponse = GetCompletion("Pick me a random theme for a dungeon, preferably something gothic and dark");
   //UtilityFunctions::print(gptResponse.c_str());

   Variant a;
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

        //UtilityFunctions::print("Gets past easy perform");
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

   ADD_PROPERTY(PropertyInfo(Variant::STRING, "GPTString"), "SetGPTString", "GetGPTString");
   ADD_PROPERTY(PropertyInfo(Variant::STRING, "imageString"), "SetImageString", "GetImageString");
}

String DungeonCreator::DownloadFile(const String& url)
{
    http_request->request(url);
    return String("done");
}

String DungeonCreator::DownloadFileRequest(const String& url, HTTPRequest* request)
{
    // Runs but doesnt do anything. i think I need to test the saving on a random image (DISCORD IMAGE PERHAPS)
    request->connect("request_completed", Callable(this, "_on_request_completed"));
    request->request(url);
    return String("done");
}

void DungeonCreator::_on_request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body)
{
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