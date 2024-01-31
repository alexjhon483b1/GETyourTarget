#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

// Structure to store response body data
struct MemoryStruct {
    char* memory;
    size_t size;
};

// Callback function to capture header data
size_t headerCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Print the header data
    printf("%.*s", (int)(size * nmemb), (char*)contents);
    return size * nmemb;
}

// Callback function to store and print response body
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    // Reallocate memory to store the response body
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        // Out of memory
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }

    // Copy response body data to the allocated memory
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0; // Null-terminate the string

    return realsize;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP>\n", argv[0]);
        return 1;
    }
    fprintf(stderr, "%s\n", argv[1]);
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();
    if (curl) {
        // Set the URL with IP and PORT
        char url[256];
        snprintf(url, sizeof(url), "http://%s", argv[1]);
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set timeout (in seconds)
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);

        // Set custom headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate");
        headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Upgrade-Insecure-Requests: 1");
        headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (X11; Fedora; Linux x86_64; rv:100.0) Gecko/20100101 Firefox/100.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the header callback function
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);

        // Set the write callback function to store and print response body
        struct MemoryStruct chunk;
        chunk.memory = malloc(1);  // Initial size
        chunk.size = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        // Perform the HTTP GET request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            // Print the entire response body
            printf("\n\nResponse Body:\n%s\n", chunk.memory);

        // Cleanup
        free(chunk.memory);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}

