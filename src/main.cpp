#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <mongoose.h>

int event_handler(struct mg_connection *conn, enum mg_event ev)
{
    switch (ev)
    {
        case MG_AUTH:
            return MG_TRUE;

        case MG_REQUEST:
            mg_printf_data(conn, "I hate you!!!");
            return MG_TRUE;

        default:
            return MG_FALSE;
    }
}

int main(int argc, char** argv)
{
    ACE_DEBUG((LM_INFO, "New instance of %s is staring...\n", *argv));
    const char *document_root = ".";
    const char *listening_port = "8080";

    struct mg_server *server = mg_create_server(NULL, event_handler);
    mg_set_option(server, "document_root", document_root);
    mg_set_option(server, "listening_port", listening_port);

    while (true)
    {
        mg_poll_server(server, 1000);
    }

    mg_destroy_server(&server);

    return 0;
}
