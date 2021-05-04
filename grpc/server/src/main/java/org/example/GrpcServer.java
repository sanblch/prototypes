package org.example;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.protobuf.services.ProtoReflectionService;
import io.grpc.stub.StreamObserver;
import org.example.proto.ProjectServiceGrpc;
import org.example.proto.TokenRequest;
import org.example.proto.TokenResponse;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

public class GrpcServer {
    private static final Logger logger = LoggerFactory.getLogger(GrpcServer.class);

    private final int port;
    private final Server server;

    public GrpcServer(int port) {
        this.port = port;
        this.server = ServerBuilder.forPort(port)
                .useTransportSecurity(new File("server.crt"), new File("server.key"))
                .addService(new GrpcService())
                .addService(ProtoReflectionService.newInstance())
                .build();
    }

    /** Start serving requests. */
    public void start() throws IOException {
        server.start();
        logger.info("Server started, listening on " + port);
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            // Use stderr here since the logger may have been reset by its JVM shutdown hook.
            System.err.println("*** shutting down gRPC server since JVM is shutting down");
            try {
                GrpcServer.this.stop();
            } catch (InterruptedException e) {
                e.printStackTrace(System.err);
            }
            System.err.println("*** server shut down");
        }));
    }

    /** Stop serving requests and shutdown resources. */
    public void stop() throws InterruptedException {
        if (server != null) {
            server.shutdown().awaitTermination(30, TimeUnit.SECONDS);
        }
    }

    /**
     * Await termination on the main thread since the grpc library uses daemon threads.
     */
    private void blockUntilShutdown() throws InterruptedException {
        if (server != null) {
            server.awaitTermination();
        }
    }

    public static void main(String[] args) throws Exception {
        GrpcServer server = new GrpcServer(8889);
        server.start();
        server.blockUntilShutdown();
    }

    private static class GrpcService extends ProjectServiceGrpc.ProjectServiceImplBase {
        @Override
        public void getToken(TokenRequest request, StreamObserver<TokenResponse> responseObserver) {
            var token = request.getLogin() + request.getPassword();
            responseObserver.onNext(TokenResponse.newBuilder().setToken(token).build());
            responseObserver.onCompleted();
        }
    }

}
