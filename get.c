#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

// Callback function to capture header data
size_t headerCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Print the header data
  char formattedString[9999];  // Adjust the size accordingly
    snprintf(formattedString, sizeof(formattedString), "%.*s", (int)(size * nmemb), (char*)contents);

    // Print the string
    printf("%s\n", formattedString);
    
    // Search for "Server:" in the string
    const char *serverStart = strstr(formattedString, "Server:");
    
    if (serverStart != NULL) {
        // Move the pointer to the beginning of the server detail
        serverStart += strlen("Server:");

        // Extract the server detail using sscanf
        char serverDetail[100]; // Adjust the size as needed
        sscanf(serverStart, " %49s", serverDetail);

        // Print the extracted server detail
        printf("Server Detail: %s\n", serverDetail);
        // Check if "Apache" or "apache" is present in serverDetail
    if (strstr(serverDetail, "Apache") != NULL || strstr(serverDetail, "apache") != NULL) {
        // Check if "2.4.49" is also present
        if (strstr(serverDetail, "2.4.49") != NULL) {
            // Perform actions if both conditions are met
            printf("Server details match: Apache and version 2.4.49\n");

            // Your code here

        } else {
            // Print a message if "2.4.49" is not found
            printf("Server version does not match (expected: 2.4.49)\n");
        }
    } else {
        // Print a message if "Apache" or "apache" is not found
        printf("Server type does not match (expected: Apache)\n");
    } 
    } else {
        printf("\n");
    }
    
    return size * nmemb;
}

// Callback function to ignore response body
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Just return the size without storing the body
    return size * nmemb;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }
    fprintf(stderr, "%s:%s\n", argv[1],argv[2]);
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();
    if (curl) {
        // Set the URL with IP and PORT
        char url[256];
        snprintf(url, sizeof(url), "http://%s:%s", argv[1], argv[2]);
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set timeout (in seconds)
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

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

        // Set the write callback function to ignore response body
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

        // Perform the HTTP GET request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Cleanup libcurl
    curl_global_cleanup();

    return 0;
}

