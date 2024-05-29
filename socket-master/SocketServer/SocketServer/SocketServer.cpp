// Определение макроса WIN32_LEAN_AND_MEAN для уменьшения размера заголовочного файла Windows
#define WIN32_LEAN_AND_MEAN

// Включение необходимых заголовочных файлов
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Использование пространства имен std
using namespace std;

int main() {
    // Инициализация библиотеки Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        // Вывод ошибки и завершение программы в случае неудачной инициализации
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Определение параметров сокета
    ADDRINFO hints;
    ADDRINFO* addrResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ConnectSocket = INVALID_SOCKET;
    char recvBuffer[512];

    // Буфер для отправки данных
    const char* sendBuffer = "Hello from server";

    // Очистка структуры hints
    ZeroMemory(&hints, sizeof(hints));

    // Настройка параметров сокета: IPv4, TCP, протокол потока, флаг пассивного режима
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Получение адреса для связывания сокета с помощью функции getaddrinfo
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для прослушивания входящих соединений
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Связывание сокета с локальным адресом и портом с помощью функции bind
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "Bind failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Прослушивание входящих соединений с помощью функции listen
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "Listen failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие входящего соединения с помощью функции accept
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "Accept failed, error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета для прослушивания
    closesocket(ListenSocket);

    // Цикл обмена данными между клиентом и сервером
    do {
        // Очистка буфера для приема данных
        ZeroMemory(recvBuffer, 512);

        // Прием данных от клиента с помощью функции recv
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            // Вывод полученных данных
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;

            // Отправка данных клиенту с помощью функции send
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                // Вывод ошибки и завершение программы в случае неудачи
                cout << "Send failed, error: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            // Вывод сообщения о закрытии соединения
            cout << "Connection closing" << endl;
        }
        else {
            // Вывод ошибки и завершение программы в случае неудачи
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Завершение отправки данных с помощью функции shutdown
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        // Вывод ошибки и завершение программы в случае неудачи
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета и очистка памяти
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();

    // Возврат кода успешного завершения программы
    return 0;
}