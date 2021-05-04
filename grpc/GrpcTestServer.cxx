#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/grpc++.h>
#include <grpc++/health_check_service_interface.h>

#include "plotter.grpc.pb.h"
#include <fstream>

class ProjectServiceImpl final : public geomech::ProjectService::Service {
public:
  grpc::Status getToken(grpc::ServerContext *context, const geomech::TokenRequest *request,
                  geomech::TokenResponse *response) override {
    response->set_token(request->login() + request->password());
    response->set_error("");
    return grpc::Status::OK;
  }
};

int main() {
  std::ifstream fcrt("server.crt");
  std::string crt((std::istreambuf_iterator<char>(fcrt)),
                  std::istreambuf_iterator<char>());
  std::ifstream fkey("server.key");
  std::string key((std::istreambuf_iterator<char>(fkey)),
                  std::istreambuf_iterator<char>());

  std::string serverAddress("localhost:8888");
  ProjectServiceImpl service;  
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  grpc::SslServerCredentialsOptions ssl_opts;
  ssl_opts.pem_key_cert_pairs.push_back({key, crt});

  std::shared_ptr<grpc::ServerCredentials> creds(
      grpc::SslServerCredentials(ssl_opts));

  builder.AddListeningPort(serverAddress, creds);
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if(server == nullptr) {
    std::cout << "Server not started on " << serverAddress
              << ". Please check if the port is free.";
  }
  else {
    std::cout << "Start listening on " << serverAddress << std::endl;
    server->Wait();
  }

  return 0;
}
