package org.example;

import io.grpc.stub.StreamObserver;
import org.example.proto.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@net.devh.boot.grpc.server.service.GrpcService
public class GrpcService extends ProjectServiceGrpc.ProjectServiceImplBase {
    private static final Logger logger = LoggerFactory.getLogger(GrpcService.class);

    @Override
    public void getToken(TokenRequest request, StreamObserver<TokenResponse> responseObserver) {
        var token = request.getLogin() + request.getPassword();
        responseObserver.onNext(TokenResponse.newBuilder().setToken(token).build());
        responseObserver.onCompleted();
    }
}
