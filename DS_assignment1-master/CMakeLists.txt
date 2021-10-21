cmake_minimum_required(VERSION 2.8.11)
project(Assignment1 C)

set(CMAKE_C_STANDARD 99)

INCLUDE_DIRECTORIES("include")

add_executable(server server/server_main/server.c lib/client_connector.c server/server_main/linkedList_clients.c lib/socketio.c server/server_main/rpc_message_storage_clnt.c lib/rpc_message_storage_xdr.c)
add_executable(server_storage server/server_storage/rpc_message_storage_svc.c server/server_storage/rpc_message_storage_server.c lib/rpc_message_storage_xdr.c server/server_storage/linkedList_users.c server/server_storage/linkedList_messages.c)

find_package(Threads)
#Compile server and server_storage with rpc in archlinux using TIRPC.
#target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} tirpc)
#target_link_libraries(server_storage ${CMAKE_THREAD_LIBS_INIT} tirpc)


#Compile server and server_storage witn rpc in ubuntu using NSL.
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} nsl) #nsl
target_link_libraries(server_storage ${CMAKE_THREAD_LIBS_INIT} nsl) #nsl

find_package(Java REQUIRED)
include(UseJava)

# Copy needed jars into the build directory.
file(COPY libjars/ DESTINATION ${CMAKE_BINARY_DIR})

#Create capitalization webService as a jar
add_jar(webServiceServer web_service/capitalizationwebservice/WebPublisher.java web_service/capitalizationwebservice/CapitalizationService.java INCLUDE_JARS ${CMAKE_BINARY_DIR}/jaxws-api-2.3.1.jar MANIFEST web_service/capitalizationwebservice/manifest.mf)
add_jar(jclient client/client.java INCLUDE_JARS ${CMAKE_BINARY_DIR}/java-getopt-1.0.14.jar ${CMAKE_BINARY_DIR}/webServiceClient.jar MANIFEST manifest.mf)

