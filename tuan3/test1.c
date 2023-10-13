#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Hàm callback được gọi khi dữ liệu từ trang web được nhận
size_t write_callback(void *ptr, size_t size, size_t count, void *stream)
{
    size_t data_size = size * count;
    FILE *file = (FILE *)stream;
    return fwrite(ptr, size, count, file);
}

int main()
{
    CURL *curl;
    FILE *links_file, *text_file, *video_file;
    char *url = "https://www.example.com"; // Thay đổi URL mục tiêu
    char *user_agent = "libcurl/1.0";

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);

        // Mở các tệp CSV cho liên kết, văn bản và video
        links_file = fopen("links.csv", "w");
        text_file = fopen("text.csv", "w");
        video_file = fopen("video.csv", "w");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Thiết lập tệp đầu ra cho từng loại dữ liệu
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, links_file);
        curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, text_file);
        curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, video_file);
        curl_easy_perform(curl);

        // Đóng các tệp CSV
        fclose(links_file);
        fclose(text_file);
        fclose(video_file);

        // Dọn dẹp
        curl_easy_cleanup(curl);
    }

    return 0;
}
