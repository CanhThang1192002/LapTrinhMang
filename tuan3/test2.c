#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libjpeg/jpeglib.h>

// Định nghĩa các hàm xử lý dữ liệu
void parse_links(char *url, FILE *fp_links);
void parse_text(char *url, FILE *fp_text);
void parse_videos(char *url, FILE *fp_videos);
void parse_images(char *url, FILE *fp_images);

// Hàm chính
int main(int argc, char *argv[])
{
    // Khởi tạo CURL
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        fprintf(stderr, "Không thể khởi tạo CURL\n");
        return 1;
    }

    // Thiết lập các tùy chọn cho CURL
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.hust.edu.vn/");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    // Tải trang web
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Không thể tải trang web\n");
        return 1;
    }

    // Đóng CURL
    curl_easy_cleanup(curl);

    // Mở các file lưu dữ liệu
    FILE *fp_links = fopen("links.csv", "w");
    FILE *fp_text = fopen("text.csv", "w");
    FILE *fp_videos = fopen("videos.csv", "w");
    FILE *fp_images = fopen("images.csv", "w");

    // Khởi tạo các hàm xử lý dữ liệu
    parse_links("https://www.hust.edu.vn/", fp_links);
    parse_text("https://www.hust.edu.vn/", fp_text);
    parse_videos("https://www.hust.edu.vn/", fp_videos);
    parse_images("https://www.hust.edu.vn/", fp_images);

    // Đóng các file lưu dữ liệu
    fclose(fp_links);
    fclose(fp_text);
    fclose(fp_videos);
    fclose(fp_images);

    return 0;
}

// Hàm xử lý link
void parse_links(char *url, FILE *fp)
{
    // Tạo một đối tượng XML
    xmlDocPtr doc = xmlNewDoc(NULL);
    xmlNodePtr root = xmlNewNode(NULL, "html");
    xmlDocSetRootElement(doc, root);

    // Tải nội dung trang web vào đối tượng XML
    char *html = curl_easy_escape(curl, url, 0);
    xmlParseChunk(doc, html, strlen(html));
    free(html);

    // Tìm tất cả các thẻ <a>
    xmlNodePtr node = root->children;
    while (node != NULL)
    {
