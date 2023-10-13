#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <csv.h>

// Hàm ghi dữ liệu vào tệp CSV
void write_csv(const char *file_name, const char *data)
{
    FILE *file = fopen(file_name, "a");
    if (file)
    {
        fprintf(file, "%s\n", data);
        fclose(file);
    }
}

// Hàm lấy tất cả các liên kết từ một tệp HTML và lưu vào tệp CSV
void get_links_from_html(const char *html_file)
{
    htmlDocPtr doc; // con trỏ tới tài liệu HTML
    xmlNode *cur;   // con trỏ tới nút HTML

    doc = htmlParseFile(html_file, NULL); // phân tích tệp HTML
    if (doc == NULL)                      // kiểm tra lỗi
    {
        fprintf(stderr, "Could not parse HTML document\n"); // in ra thông báo lỗi
        return;
    }

    cur = xmlDocGetRootElement(doc); // lấy nút gốc của tài liệu HTML
    if (cur == NULL)
    {
        fprintf(stderr, "Empty document\n"); // kiểm tra lỗi
        xmlFreeDoc(doc);                     // giải phóng tài liệu HTML
        return;
    }

    for (cur = cur->children; cur != NULL; cur = cur->next) // duyệt qua tất cả các nút HTML
    {
        if (cur->type == XML_ELEMENT_NODE && xmlStrcmp(cur->name, (const xmlChar *)"a") == 0) // kiểm tra nút có phải là thẻ <a> hay không
        {
            xmlChar *href = xmlGetProp(cur, (const xmlChar *)"href"); // lấy thuộc tính href của thẻ <a>
            if (href)
            {
                write_csv("links.csv", (const char *)href); // ghi vào tệp CSV
                xmlFree(href);                              // giải phóng bộ nhớ
            }
        }
    }

    xmlFreeDoc(doc); // giải phóng tài liệu HTML
}

// Hàm lấy thông tin văn bản từ trang HTML và lưu vào tệp CSV
void get_text_from_html(const char *html_file)
{
    htmlDocPtr doc = htmlParseFile(html_file, NULL); // Phân tích tệp HTML
    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse HTML document\n"); // Kiểm tra lỗi
        return;
    }

    // Tìm toàn bộ văn bản trong tài liệu HTML
    xmlNodePtr cur = xmlDocGetRootElement(doc); // Lấy nút gốc của tài liệu HTML
    while (cur != NULL)
    {
        if (cur->type == XML_TEXT_NODE) // Kiểm tra nút có phải là nút văn bản hay không
        {
            // Lấy nội dung văn bản và loại bỏ khoảng trắng thừa
            const char *text = (const char *)xmlNodeGetContent(cur); // Lấy nội dung văn bản
            if (text)                                                // Kiểm tra nội dung văn bản có khác NULL hay không
            {
                char *trimmed_text = trim_whitespace(text); // Loại bỏ khoảng trắng thừa
                if (strlen(trimmed_text) > 0)               // Kiểm tra văn bản có khác rỗng hay không
                {
                    write_csv("text.csv", trimmed_text); // Ghi vào tệp CSV
                }
                xmlFree(text); // Giải phóng bộ nhớ
            }
        }
        cur = cur->next; // Chuyển sang nút tiếp theo
    }

    xmlFreeDoc(doc); // Giải phóng tài liệu HTML
}

// Hàm lấy thông tin về video từ trang HTML và lưu vào tệp CSV
void get_videos_from_html(const char *html_file)
{
    // Mở tệp HTML và tạo một cấu trúc tài liệu HTML
    htmlDocPtr doc = htmlParseFile(html_file, NULL); // Phân tích tệp HTML
    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse HTML document\n");
        return;
    }

    // Tìm toàn bộ phần tử chứa link video trong tài liệu HTML
    xmlNodePtr cur = xmlDocGetRootElement(doc); // Lấy nút gốc của tài liệu HTML
    while (cur != NULL)
    {
        if (cur->type == XML_ELEMENT_NODE && xmlStrcmp(cur->name, (const xmlChar *)"video") == 0) // Kiểm tra nút có phải là thẻ <video> hay không
        {
            xmlNodePtr source = cur->children; // Lấy nút con đầu tiên của thẻ <video>
            while (source != NULL)             // Duyệt qua tất cả các nút con của thẻ <video>
            {
                if (source->type == XML_ELEMENT_NODE && xmlStrcmp(source->name, (const xmlChar *)"source") == 0) // Kiểm tra nút có phải là thẻ <source> hay không
                {
                    xmlChar *src = xmlGetProp(source, (const xmlChar *)"src"); // Lấy thuộc tính src của thẻ <source>
                    if (src)                                                   // Kiểm tra thuộc tính src có khác NULL hay không
                    {
                        write_csv("videos.csv", (const char *)src); // Ghi vào tệp CSV
                        xmlFree(src);                               // Giải phóng bộ nhớ
                    }
                }
                source = source->next; // Chuyển sang nút con tiếp theo
            }
        }
        cur = cur->next; // Chuyển sang nút tiếp theo
    }

    // Giải phóng tài liệu HTML
    xmlFreeDoc(doc);
}

int main()
{
    const char *url = "https://www.hust.edu.vn";
    const char *html_file = "hust.html";

    CURL *curl;   // con trỏ tới đối tượng CURL
    CURLcode res; // mã trả về của CURL

    curl = curl_easy_init(); // khởi tạo đối tượng CURL

    if (curl) // kiểm tra lỗi khi khởi tạo đối tượng CURL
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);           // thiết lập URL cần tải
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // cho phép điều hướng khi gặp mã trả về 3xx

        FILE *htmlfile = fopen(html_file, "wb");                   // mở tệp HTML để ghi dữ liệu
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, htmlfile);       // thiết lập tệp HTML để ghi dữ liệu
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // thiết lập hàm ghi dữ liệu

        res = curl_easy_perform(curl); // thực hiện tải trang web

        fclose(htmlfile); // đóng tệp HTML

        if (res != CURLE_OK) // kiểm tra lỗi khi tải trang web
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res)); // in ra thông báo lỗi
        }

        curl_easy_cleanup(curl); // giải phóng đối tượng CURL
    }

    // Lấy tất cả các liên kết từ tệp HTML và lưu vào tệp CSV
    get_links_from_html(html_file);

    // Lấy thông tin văn bản từ tệp HTML và lưu vào tệp CSV
    get_text_from_html(html_file);

    // Lấy thông tin về video từ tệp HTML và lưu vào tệp CSV
    get_videos_from_html(html_file);

    return 0;
}
