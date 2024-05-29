// Определение препроцессорной макроса для оптимизации кода
#define WIN32_LEAN_AND_MEAN

// Подключение необходимых библиотек
#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Использование пространства имен std
using namespace std;

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        // Вывод сообщения об ошибке при неудачной инициализации Winsock
        cout << "WSAStartup failed with result: " << WSAGetLastError() << endl;
        return 1;
    }

    // Заполнение структуры с параметрами сокета
    ADDRINFO hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // Адресное семейство IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получение информации об удаленном хосте и порте
    ADDRINFO* addrResult;
    int result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        // Вывод сообщения об ошибке при неудачном получении информации об адресе
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка Winsock
        return 1;
    }

    // Создание сокета
    SOCKET ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        // Вывод сообщения об ошибке при неудачном создании сокета
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка Winsock
        return 1;
    }

    // Установление соединения с удаленным хостом
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        // Вывод сообщения об ошибке при неудачном установлении соединения
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult
        WSACleanup(); // Очистка Winsock
        return 1;
    }

    // Освобождение памяти, выделенной для addrResult
    freeaddrinfo(addrResult);

    // Буферы для отправки и приема данных
    const char* sendBuffer1 = "Hello from client 1"; // Первое сообщение для отправки
    const char* sendBuffer2 = "Hello from client 2"; // Второе сообщение для отправки
    char recvBuffer[512]; // Буфер для приема данных

    // Отправка первого сообщения
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        // Вывод сообщения об ошибке при неудачной отправке первого сообщения
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        WSACleanup(); // Очистка Winsock
        return 1;
    }
    // Вывод сообщения об успешной отправке первого сообщения
    cout << "Sent: " << result << " bytes" << endl;

    // Отправка второго сообщения
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        // Вывод сообщения об ошибке при неудачной отправке второго сообщения
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        WSACleanup(); // Очистка Winsock
        return 1;
    }
    // Вывод сообщения об успешной отправке второго сообщения
    cout << "Sent: " << result << " bytes" << endl;

    // Закрытие сокета для отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        // Вывод сообщения об ошибке при неудачном закрытии сокета для отправки данных
        cout << "Shutdown failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket); // Закрытие сокета
        WSACleanup(); // Очистка Winsock
        return 1;
    }

    // Прием данных от сервера
    do {
        // Очистка буфера приема
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            // Вывод сообщения о количестве принятых байт и самих принятых данных
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            // Вывод сообщения о закрытии соединения
            cout << "Connection closed" << endl;
        }
        else {
            // Вывод сообщения об ошибке при неудачном приеме данных
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0); // Цикл приема данных продолжается, пока не будет принят 0 байт

    // Закрытие сокета
    closesocket(ConnectSocket);

    // Очистка Winsock
    WSACleanup();

    return 0; // Возврат 0, что означает успешное завершение программы
}