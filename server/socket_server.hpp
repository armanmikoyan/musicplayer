#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>

class socket_server
{
public:
   socket_server()
   {
      create_socket();
      set_socket_option();
      bind_socket();
      listen_connections();
   }

private:
   void create_socket()
   {
      server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (server_fd < 0)
      {
         std::cerr << "fail creating socket\n";
         exit(1);
      }
   }

   void set_socket_option()
   {
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
      {
         std::cerr << "fail setting option\n";
         exit(1);
      }
   }

   void bind_socket()
   {
      address.sin_family = AF_INET;
      address.sin_port = htons(PORT);
      address.sin_addr.s_addr = INADDR_ANY;

      if (bind(server_fd, reinterpret_cast<sockaddr *>(&address), addr_len) < 0)
      {
         std::cerr << "fail binding socket\n";
         exit(1);
      }
   }

   void listen_connections()
   {
      if (listen(server_fd, 10) < 0)
      {
         std::cerr << "fail listenning conntection\n";
         exit(1);
      }
      std::cout << "Server is running, listenning to connections on Port: " << PORT << "\n";
   }

public:
   int accept_connection()
   {
      int client_socket = accept(server_fd, reinterpret_cast<sockaddr *>(&address), reinterpret_cast<socklen_t *>(&addr_len));
      if (client_socket < 0)
      {
         std::cerr << "fail acception conntection\n";
         exit(1);
      }
      std::cout << "connection\n";
      send(client_socket, message.c_str(), message.length(), 10);
      return client_socket;
   }

   std::string request(int client_socket)
   {
      char buff[256];
      int recived = read(client_socket, buff, 256);
      if (recived < 0)
      {
         std::cerr << "fail reciving data\n";
         response(client_socket, "no request");
         exit(1);
      }
      buff[recived] = '\0';
      return static_cast<std::string>(buff);
   }

   void response(int client_socket, const char *resp)
   {
      int sended = send(client_socket, resp, strlen(resp), 0);
      if (sended < 0)
      {
         std::cerr << "fail sending data\n";
         exit(1);
      }
   }

   void stream_music(int client_socket, const std::string &fileName)
   {
      std::string filePath = "/Users/armanmikoyan/Music/" + fileName;

      std::ifstream musicFile(filePath, std::ios::binary);
      if (!musicFile)
      {
         std::cerr << "Error opening music file: " << filePath << std::endl;
         response(client_socket, "Error opening music file");
         return;
      }
      const int bufferSize = 1024;
      char buffer[bufferSize];
      while (!musicFile.eof())
      {
         musicFile.read(buffer, bufferSize);
         int bytesRead = musicFile.gcount();

         if (bytesRead > 0)
         {
            send(client_socket, buffer, bytesRead, 0);
         }
      }
      musicFile.close();
   }

private:
   sockaddr_in address;
   int addr_len = sizeof(address);
   int server_fd{};
   const int PORT = 8080;
   std::string message = "connection succeded";
   int opt = 1; // if connection ended free port

   // serving music_player
private:
   std::vector<std::string> get_music_names(const std::string &directory)
   {
      std::vector<std::string> fileNames;

      for (const auto &entry : std::filesystem::directory_iterator(directory))
      {
         if (entry.is_regular_file())
         {
            // Check if the file has a supported extension
            std::string extension = entry.path().extension().string();
            if (extension == ".mp3" || extension == ".wav")
            {
               fileNames.push_back(entry.path().filename().string());
            }
         }
      }

      return fileNames;
   }

public:
   std::string send_music_list()
   {
      std::vector<std::string> musicList = get_music_names("/Users/armanmikoyan/Music");

      std::string musicData;
      for (const auto &song : musicList)
      {
         musicData += song + "\n";
      }

      return musicData;
   }
};