#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>
#include <variant>
#include <cstdint>

// Define conditional compilation macros
#ifdef SERVER
#define SERVER_CODE
const int CONSOLE_MAX_MESSAGES = 20;
#else
#define CLIENT_CODE
const int CONSOLE_MAX_MESSAGES = 255;
#endif

// Forward declarations for dependencies
class TConsole;

// Using declarations for WideString and other types
using WideString = std::wstring;
using AnsiString = std::string;
using LongWord = uint32_t;

struct TConsole {
    std::vector<WideString> TextMessage;
    std::vector<LongWord> TextMessageColor;
    std::vector<int> NumMessage;
    int Count;
    int CountMax;
    int ScrollTick;
    int ScrollTickMax;  // how long the scroll count down is before it
                        // scrolls - in ticks 60=1 sec
    int NewMessageWait; // how long it waits after a new message before
                        // resuming the scroll count down
    uint8_t AlphaCount;
#ifdef SERVER_CODE
    bool TerminalColors; // use terminal colors
#endif

    // Constructor
    TConsole() : TextMessage(CONSOLE_MAX_MESSAGES), 
                 TextMessageColor(CONSOLE_MAX_MESSAGES), 
                 NumMessage(CONSOLE_MAX_MESSAGES),
                 Count(0), 
                 CountMax(CONSOLE_MAX_MESSAGES), 
                 ScrollTick(0), 
                 ScrollTickMax(0),
                 NewMessageWait(0),
                 AlphaCount(0)
#ifdef SERVER_CODE
                 , TerminalColors(false)
#endif
    {}

    void ScrollConsole();
#ifdef SERVER_CODE
    void Console(const std::variant<int, float, std::string, WideString>& What, 
                 uint32_t Col, 
                 uint8_t Sender); // overload
#endif
    void Console(const WideString& What, uint32_t Col); // overload
    void Console(const AnsiString& What, uint32_t Col); // overload
    void Console(const std::variant<int, float, std::string, WideString>& What, 
                 uint32_t Col); // overload
    void ConsoleAdd(const WideString& What, uint32_t Col);
    void ConsoleNum(const WideString& What, uint32_t Col, int Num);
};

namespace ConsoleImpl {
    inline void TConsole::ScrollConsole() {
        if (Count > 0) {
            for (int X = 0; X < Count - 1; X++) {
                TextMessageColor[X] = TextMessageColor[X + 1];
                TextMessage[X] = TextMessage[X + 1];
                NumMessage[X] = NumMessage[X + 1];  // scroll the messages up 1
                AlphaCount = 255;
            }
            TextMessage[Count - 1] = L"";  // blank the last message
            NumMessage[Count - 1] = 0;
            Count--;
        }
        ScrollTick = 0;
    }

    inline void TConsole::ConsoleAdd(const WideString& What, uint32_t Col) {
        // adds a new message
        if (Count < CONSOLE_MAX_MESSAGES) {
            Count++;
            ScrollTick = -NewMessageWait;
            TextMessage[Count - 1] = What;
            TextMessageColor[Count - 1] = Col;
            NumMessage[Count - 1] = -255;
            if (Count == 1)
                AlphaCount = 255;
            if (Count == CountMax)
                ScrollConsole();
        }
    }

    inline void TConsole::ConsoleNum(const WideString& What, uint32_t Col, int Num) {
        // adds a new message
        if (Count < CONSOLE_MAX_MESSAGES) {
            Count++;
            ScrollTick = -NewMessageWait;
            TextMessage[Count - 1] = What;
            TextMessageColor[Count - 1] = Col;
            NumMessage[Count - 1] = Num;
            if (Count == CountMax)
                ScrollConsole();
        }
    }

    inline void TConsole::Console(const WideString& What, uint32_t Col) {
        if (What.empty())
            return;

        // AddLineToLogFile equivalent would go here
        // AddLineToLogFile(GameLog, AnsiString(What), ConsoleLogFileName);

#ifdef SERVER_CODE
        if (TerminalColors) {
            // Print with terminal colors
            // Implementation would depend on platform
            printf("\033[38;2;%d;%d;%dm%ls\033[0m\n",
                   (Col & 0x00FF0000) >> 16, // r
                   (Col & 0x0000FF00) >> 8,  // g
                   (Col & 0x000000FF),       // b
                   What.c_str());
        } else {
            wprintf(L"%ls\n", What.c_str());
        }
        
        // Broadcast message equivalent
        // BroadCastMsg(AnsiString(What));

        // adds a new message
        // NOTE: not thread safe!
        // added mod to prevent AVs
        Count++;
        if (Count >= CountMax) 
            Count = 1;

        ScrollTick = -NewMessageWait;
        TextMessage[Count - 1] = What;
        TextMessageColor[Count - 1] = Col;
        NumMessage[Count - 1] = -255;
        if (Count == 1)
            AlphaCount = 255;
        if (Count == CountMax)
            ScrollConsole();
#else
        // MainConsole.ConsoleAdd(What, Col);
        // BigConsole.ConsoleAdd(What, Col);
        ConsoleAdd(What, Col);
#endif
    }

    inline void TConsole::Console(const AnsiString& What, uint32_t Col) {
        WideString wideStr(What.begin(), What.end());
        Console(wideStr, Col);
    }

    inline void TConsole::Console(const std::variant<int, float, std::string, WideString>& What, uint32_t Col) {
        WideString result;
        if (std::holds_alternative<int>(What)) {
            result = std::to_wstring(std::get<int>(What));
        } else if (std::holds_alternative<float>(What)) {
            result = std::to_wstring(std::get<float>(What));
        } else if (std::holds_alternative<std::string>(What)) {
            const std::string& str = std::get<std::string>(What);
            result = WideString(str.begin(), str.end());
        } else if (std::holds_alternative<WideString>(What)) {
            result = std::get<WideString>(What);
        }
        Console(result, Col);
    }

#ifdef SERVER_CODE
    inline void TConsole::Console(const std::variant<int, float, std::string, WideString>& What, 
                                  uint32_t Col, 
                                  uint8_t Sender) {
        Console(What, Col);
        // Implementation for ServerSendStringMessage would go here
        // if ((Sender > 0) && (Sender < MAX_PLAYERS + 1))
        //    ServerSendStringMessage(What, Sender, 255, MSGTYPE_PUB);
    }
#endif
}

// Bring the methods into the global scope for direct access on the struct
using ConsoleImpl::TConsole;

#endif // CONSOLE_H