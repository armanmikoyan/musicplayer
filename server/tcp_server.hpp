#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

class tcp_server
{
public:
   tcp_server()
      : socket_fd{}
      , PORT{8080}
      , opt{1}
      , size{socklen_t(sizeof(address))}
   {
      address.sin_port = htons(PORT);
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      
      create_socket();
      setting_option(); 
      bind_socket(); 
      listenning_connetions();
   }

private:
   void create_socket()
   {
      socket_fd = socket(AF_INET, SOCK_STREAM, 0);
      
      if (socket_fd < 0)
      {
         throw std::runtime_error("Error creating socket\n");
      }
   }

   void setting_option()
   {
      if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
      {
         throw std::runtime_error("Error seting option for reusing port\n");
      }
   }

   void bind_socket()
   {
      if (bind(socket_fd, reinterpret_cast<sockaddr*>(&address), size) < 0)
      {
         throw std::runtime_error("Error binding socket\n");
      }
   }

   void listenning_connetions()
   {
      if (listen(socket_fd, BACK_LOG) < 0)
      {
         throw std::runtime_error("Error listening connections");
      }

      std::cout << "Server is running on Port: " 
                << PORT
                << ". Listenning to connections\n";
   }

   int accept_connection()
   {
      int socket_for_accepted_client = accept(socket_fd, reinterpret_cast<sockaddr*>(&address), &size);
         
      if (socket_for_accepted_client < 0)
      {  
         throw std::runtime_error("Error accepting connection\n");
      }
         
      std::cout << "Accepted connection\n";
      return socket_for_accepted_client;
   }

   std::string response_music_names()
   {
      std::string file_names;
      std::string directory = "/Users/armanmikoyan/Music";
      for (const auto &file : std::filesystem::directory_iterator(directory))
      {
         if (file.is_regular_file())
         { 
            std::string extension = file.path().extension().string();
            if (extension == ".mp3" || extension == ".wav")
            {
               file_names += file.path().filename().string() + "\n";
            }
         }
      }
      return file_names;
   }

   void stream_music(int client_socket, const std::string& music_name)
   {
      std::string music_path = "/Users/armanmikoyan/Music/" + music_name;

      std::ifstream music_file(music_path, std::ios::binary);

      char buffer[1026];
      while (!music_file.eof())
      {
         music_file.read(buffer, 1026);
         int bytesRead = music_file.gcount();
         if (bytesRead > 0)
         {
            send(client_socket, buffer, bytesRead, 0);
         }
      }  
   }

   int handle_client_request(int client)
   {
      char buff[2048];
      size_t received_bytes = recv(client, buff, sizeof(buff), 0);

      if (received_bytes <= 0)
      {
         if (received_bytes == 0)
         {
            std::cout << "connections closed for client: " << client << std::endl;
            close(client);
            return -99;   // Signal for break loop. connecton closed
         }
         else
         {
            throw std::runtime_error("Error reading request from client");
         }
      }


      // proccess request
      buff[received_bytes] = '\0';
      std::string req = buff;
      size_t sent_bytes = 0; 

      if (req == "get_meta_music")
      {
         std::string response = response_music_names();
         sent_bytes = send(client, response.c_str(), response.length(), 0);
         sent_bytes = 0;
      }
      else // stream music
      {
         std::cout << "Streaming music: " << req << "\n";
         stream_music(client, req);
      }

      if (sent_bytes < 0)
      {
         throw std::runtime_error("Error sending response to client"); 
      }
      else
      {
         std::cout << req << "  sent" << "\n";
      }

      return 1;  // connection is ont closed
   }

public:
   void run()
   {
      while (true)
      {
         try
         {
            int connection = accept_connection();

            // thread for for handle more then 1 request from client
            std::thread([&](){
               while (handle_client_request(connection) != -99);  // if client closed socket connection function returns -99 and loop breaks
            }).detach();    
         }
         catch (const std::exception& e)
         {
            std::cerr << e.what() << '\n';
         }        
      } 
   }

private:
   int socket_fd;
   int PORT;
   int opt;
   sockaddr_in address;
   socklen_t size;
   static constexpr int BACK_LOG = 5; // CONNENCTIONS
};