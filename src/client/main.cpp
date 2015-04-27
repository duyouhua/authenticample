#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <curl/curl.h>

static size_t CURL_WriteFunctionCallback(void *contents, size_t size, size_t nmemb, void *userp); // to make possible describe as friend

class CMemoryChunk
{
    friend size_t CURL_WriteFunctionCallback(void *contents, size_t size, size_t nmemb, void *userp);

    public:
        CMemoryChunk(): m_size(0), m_memory(static_cast<char *>(malloc(1))) {}

        const char *data() const { return m_memory; }

        ~CMemoryChunk() { free(m_memory); }

    private:
        size_t m_size;
        char *m_memory;
};

static size_t CURL_WriteFunctionCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size*nmemb;
    CMemoryChunk *chunk = static_cast<CMemoryChunk*>(userp);
    chunk->m_memory = static_cast<char *>(realloc(chunk->m_memory, chunk->m_size + realsize + 1));
    if (NULL == chunk->m_memory)
    {
        ACE_DEBUG((LM_ERROR, "Reallocation failed. Not enough memory.\n"));
        return 0;
    }

    memcpy(&(chunk->m_memory[chunk->m_size]), contents, realsize);
    chunk->m_size += realsize;
    chunk->m_memory[chunk->m_size] = '\0';

    return realsize;
}

bool get_url(const char *url, const char *cert_file, const char *key_file, const char *cacerts_file)
{
    CURL *curl;
    curl = curl_easy_init();
    CMemoryChunk chunk;
    
    if (!curl)
    {
        ACE_DEBUG((LM_WARNING, "Cannot initialize CURL session.\n"));
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURL_WriteFunctionCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);

    // SSL options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L); // Always check host name of remote server (compare it with CN in the server's certificate)
    // Set client's certificate and private key file names and their format
    if (NULL != cert_file)
    {
        curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
        curl_easy_setopt(curl, CURLOPT_SSLCERT, cert_file);
    }
    if (NULL != key_file)
    {
        curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
        curl_easy_setopt(curl, CURLOPT_SSLKEY, key_file);
    }
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    if (NULL != cacerts_file)
    {
        // Use specified CA certs file
        curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_file);
        // Explicitly set peer verification
    }

    CURLcode result;
    result = curl_easy_perform(curl);
    if (CURLE_OK != result)
    {
        ACE_DEBUG((LM_WARNING, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result)));
        curl_easy_cleanup(curl);
        return false;
    }

    ACE_DEBUG((LM_INFO, "%s\n", chunk.data()));
    curl_easy_cleanup(curl);

    return true;
}

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);

    const char *url = "http://veei.ru:8080";
    const char *cert_file = NULL;
    const char *key_file = NULL;
    const char *cacerts_file = NULL;

    int opt;
    while (-1 != (opt = getopt(argc, argv, "u:c:k:r:")))
    {
        switch (opt)
        {
            case 'u':
                url = optarg;
                break;

            case 'c':
                cert_file = optarg;
                break;

            case 'k':
                key_file = optarg;
                break;

            case 'r':
                cacerts_file = optarg;
                break;

            default:
                ACE_DEBUG((LM_ERROR, "Undefined command line option has been used.\n"
                            "Usage: %s [-u <URL>] [-c <client's cert file> -k <client's private key file>] [-r <CA certs file>]\n",
                            argv[0]));
                return -2;
        }
    }

    ACE_DEBUG((LM_INFO, "Client starting...\n"));

    if (!get_url(url, cert_file, key_file, cacerts_file))
    {
        ACE_DEBUG((LM_ERROR, "Error while retrieving URL. Exiting...\n"));
    }

    curl_global_cleanup();

    return 0;
}
