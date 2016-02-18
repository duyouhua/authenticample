/**
 * \file server/main.cpp
 * \author Anton Gorev aka Veei
 * \date 2015/08/18
 */
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

int main(int argc, char* argv[])
{
    const char *document_root = ".";
    const char *listening_port = argv[1];

    ACE_DEBUG((LM_INFO, "Server is starting on port %s...\n", listening_port));

    struct mg_server *server = mg_create_server(NULL, event_handler);

    bool fail = false;
    const char *error;
    if ((error = mg_set_option(server, "document_root", document_root)))
    {
        ACE_DEBUG((LM_ERROR, "Set option error: %s\n", error));
        fail = true;
    }
    if ((error = mg_set_option(server, "listening_port", listening_port)))
    {
        ACE_DEBUG((LM_ERROR, "Set option error: %s\n", error));
        fail = true;
    }
    if (fail)
    {
        ACE_DEBUG((LM_ERROR, "Error while configuring Mongoose server. Exiting...\n"));
    }

    while (true)
    {
        mg_poll_server(server, 1000);
    }

    mg_destroy_server(&server);

    return 0;
}
