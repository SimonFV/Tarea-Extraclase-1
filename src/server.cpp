#include "/home/simon/Cpp/TE_1/TE_1_server/src/include/grafo.h"

/**
 * @brief Inicia el servidor tipo socket
 * 
 * El servidor esperara a que un cliente se conecte, analizara los mensajes recibidos
 * y devolvera la respuesta correspondiente
 * @param g1 Puntero al grafo
 * @return Resultado de la operacion
 */
int run_server(grafo *g1){
    /**
     * @note Crear el socket
     */
    
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socket == -1){
        cerr<<"No se pudo crear el socket, cerrando..."<<endl;
        return -1;
    }

    /**
     * @note Enlaza una ip y un numero de puerto al socket
     */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    bind(listen_socket, (sockaddr*)&hint, sizeof(hint));
    
    /**
     * @note Dice Winsock el socket es para listening
     */
    listen(listen_socket, SOMAXCONN);
 
    /**
     * @note Espera una conexion
     */
    cout<<"Esperando cliente..."<<endl;
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
 
    int clientSocket = accept(listen_socket, (sockaddr*)&client, &clientSize);
 
    char host[NI_MAXHOST]; //Nombre remoto del cliente
    char service[NI_MAXSERV]; //Donde se conecta el cliente
 
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
 
    if(getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) ==0){
        cout<<host<<" conectado en el puerto "<<service<<endl;
    }else{
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout<<host<<" conectado en el puerto " <<ntohs(client.sin_port)<<endl;
    }
    
    /**
     * @note Cierra el socket para listening
     */
    close(listen_socket);

    char buf[4096];

    /**
     * @note Envia la informacion del grafo al cliente
     */
    string message = "";
    message = message + to_string(g1->tamano())+",";
    vertice *temp = g1->first;
    while(temp!=NULL){
        message = message + temp->name + ",";
        temp = temp->next;
    }
    temp = g1->first;
    arista *aux;
    while(temp!=NULL){
        aux = temp->ady;
        while(aux!=NULL){
            message = message + to_string(g1->get_position(temp->name)) + ",";
            message = message + to_string(g1->get_position(aux->ady->name)) + ",";
            message = message + to_string(aux->peso) + ",";
            aux = aux->next;
        }
        temp = temp->next;
    }
    send(clientSocket, message.c_str(), message.size() + 1, 0);
    
    /**
     * @note Espera que el cliente envie un dato
     */
    while(true){
        memset(buf, 0, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1){
            cerr<<"Error al recibir el mensaje"<<endl;
            break;
        }
 
        if (bytesReceived == 0){
            cout<<"Cliente desconectado"<<endl;
            break;
        }
        string start = "";
        string end = "";
        string temp_s;
        temp_s = string(buf, 0, bytesReceived);
        int i = 0;
        while(temp_s[i]!=','){
            start = start + temp_s[i];
            i++;
        }
        i++;
        while(temp_s[i]!=','){
            end = end + temp_s[i];
            i++;
        }

        string ruta;
        if(g1->exists(start) && g1->exists(end)){
            ruta = g1->dijk(g1, start, end);
        }else{
            ruta = "Vertices incorrectos";
        }
        cout<<string(buf, 0, bytesReceived)<<endl;
        
        send(clientSocket, ruta.c_str(), ruta.size() + 1, 0);
    }
    
    /**
     * @note Cierra el socket
     */
    close(clientSocket);
    
    return 0;
}