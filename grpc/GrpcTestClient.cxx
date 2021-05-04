#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/grpc++.h>
#include <grpc++/health_check_service_interface.h>

#include "plotter.grpc.pb.h"

#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <iterator>
#include <thread>

class ProjectServiceClient {
 public:
  ProjectServiceClient(std::shared_ptr<grpc::Channel> channel)
      : stub_(geomech::ProjectService::NewStub(channel)) {}
  std::string getToken(const std::string& login, const std::string& password) {
    geomech::TokenRequest request;
    geomech::TokenResponse response;
    grpc::ClientContext context;
    grpc::Status status;
    
    request.set_login(login);
    request.set_password(password); 
    status = stub_->getToken(&context, request, &response);

    if (status.ok()) {
      return response.token();
    }
    else {
      return status.error_message();
    }
  }

  void removeDataObject(const std::string& uuid) {
    geomech::DataRequest request;
    geomech::DataObjectResponse response;
    grpc::ClientContext context;
    grpc::Status status;

    request.set_uuid(uuid);
    status = stub_->removeDataObject(&context, request, &response);
  }

private:
  std::unique_ptr<geomech::ProjectService::Stub> stub_;
};

class GrpcTestClient : public ::testing::Test
{
protected:
  void SetUp()
  {
// #ifdef __unix__
//     std::system("./GrpcTestServer > /dev/null 2>&1 &");
// #else
//     std::system("PowerShell -Command Start-Process ./GrpcTestServer.exe -NoNewWindow");
// #endif
//     std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  void TearDown()
  {
// #ifdef __unix__
//     std::system("killall GrpcTestServer");
// #else
//     std::system("PowerShell -Command Stop-Process -Name GrpcTestServer -Force");
// #endif
  }
};

TEST_F(GrpcTestClient, SSL) {
  std::ifstream f("server.crt");
  std::string crt((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

  grpc::SslCredentialsOptions ssl_opts;
  ssl_opts.pem_root_certs = crt;

  std::shared_ptr<grpc::ChannelCredentials> creds = grpc::SslCredentials(ssl_opts);
  // std::shared_ptr<grpc::ChannelCredentials> creds =
  //     grpc::InsecureChannelCredentials();
  ProjectServiceClient client(grpc::CreateChannel("localhost:8889", creds));
  std::string response = client.getToken("login", "password");
  ASSERT_EQ(response, "loginpassword");
  client.removeDataObject("baldjfksjs");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
