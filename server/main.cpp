#include "socket_server.hpp"

int main()
{
   socket_server server;
   while (1)
   {
      int client = server.accept_connection();
      std::string req = server.request(client);
      if (req == "meta_music")
      {
         std::cout << req << "\n";
         server.response(client, server.send_music_list().c_str());
         std::cout << "sended music" << req
                   << "\n";
      }
      else
      {
         std::cout << "sended music :" << req << "\n";
         server.stream_music(client, req);
      }
   }
}