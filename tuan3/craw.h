#include <curl/curl.h>
#include <libxml/HTMLparser.h>

// hàm lấy HTML từ trang web và lưu vào tệp
void get_html(const char *url, const char *html_file)
{
    CURL *curl;   // con trỏ tới cURL
    CURLcode res; // mã trả về từ cURL

    curl = curl_easy_init(); // khởi tạo cURL

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);           // thiết lập URL
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // cho phép chuyển hướng

        FILE *htmlfile = fopen(html_file, "wb");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, htmlfile); // thiết lập tệp HTML để ghi
        res = curl_easy_perform(curl);                       // thực hiện yêu cầu

        fclose(htmlfile);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res)); // in lỗi
        }

        curl_easy_cleanup(curl); // giải phóng cURL
    }
}

void error_handler(void *userData, const char *msg, ...)
{
    // Bỏ qua thông báo lỗi, không thực hiện bất kỳ xử lý cụ thể nào
}

// lấy các link từ tệp HTML
void get_links_from_html(xmlNode *node, FILE *file)
{
    for (; node; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE) // nếu là một thẻ HTML
        {
            if (xmlStrcmp(node->name, (const xmlChar *)"a") == 0 || xmlStrcmp(node->name, (const xmlChar *)"link") == 0 || xmlStrcmp(node->name, (const xmlChar *)"img") == 0)
            {
                xmlChar *href = xmlGetProp(node, (const xmlChar *)"href"); // lấy thuộc tính href
                if (href)
                {
                    fprintf(file, "%s\n", href);
                    xmlFree(href);
                }
            }
            get_links_from_html(node->children, file); // xử lý các nút con
        }
    }
}

// Hàm lấy text từ trang HTML và lưu vào tệp CSV
void get_text_from_html(xmlNode *node, FILE *file)
{
    for (; node; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE) // nếu là một thẻ HTML
        {
            get_text_from_html(node->children, file); // xử lý các nút con
        }
        else if (node->type == XML_TEXT_NODE)
        {
            const char *text = (const char *)xmlNodeGetContent(node); // lấy nội dung
            if (text)
            {
                fprintf(file, "%s\n", text);
                xmlFree((void *)text); // giải phóng bộ nhớ
            }
        }
    }
}

// Hàm lấy thông tin về video va anh từ trang HTML và lưu vào tệp CSV
void get_videos_from_html(xmlNode *node, FILE *file)
{
    for (; node; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE) // nếu là một thẻ HTML
        {
            if (xmlStrcmp(node->name, (const xmlChar *)"video") == 0 || xmlStrcmp(node->name, (const xmlChar *)"source") == 0)
            {
                xmlChar *src = xmlGetProp(node, (const xmlChar *)"src"); // lấy thuộc tính src
                if (src)
                {
                    fprintf(file, "%s\n", src);
                    xmlFree(src);
                }
            }
            get_videos_from_html(node->children, file); // xử lý các nút con
        }
    }
}

void craw(const char *url)
{
    // const char *url = "lichsu247.com";      // địa chỉ trang web
    const char *html_file = "tailieu.html"; // tên tệp HTML
    get_html(url, html_file);               // lấy HTML từ trang web

    xmlSetGenericErrorFunc(NULL, error_handler); // bỏ qua thông báo lỗi
    htmlDocPtr doc;                              // con trỏ tới tài liệu HTML
    xmlNode *root;                               // con trỏ tới nút gốc
    xmlKeepBlanksDefault(0);                     // không giữ khoảng trắng
    doc = htmlParseFile(html_file, NULL);        // phân tích tệp HTML
    if (doc == NULL)
    {
        fprintf(stderr, "Unable to parse HTML file.\n");
        return;
    }
    root = xmlDocGetRootElement(doc); // lấy nút gốc

    FILE *link = fopen("link.csv", "a+");
    if (link == NULL)
    {
        printf("Cannot open file.\n");
        return;
    }

    FILE *text = fopen("text.csv", "a+");
    if (text == NULL)
    {
        printf("Cannot open file.\n");
        return;
    }

    FILE *video = fopen("video_img.csv", "a+");
    if (video == NULL)
    {
        printf("Cannot open file.\n");
        return;
    }

    get_links_from_html(root, link);   // lấy các liên kết từ tệp HTML
    get_text_from_html(root, text);    // lấy văn bản từ tệp HTML
    get_videos_from_html(root, video); // lấy video từ tệp HTML

    fclose(link);
    fclose(text);
    fclose(video);
    xmlFreeDoc(doc);    // giải phóng tài liệu
    xmlCleanupParser(); // giải phóng bộ nhớ
}