// Software Name: D-Red-Bot.cpp (Advanced CLI - Hacker OS Edition)
// Version: Qv1.5.0
// Author: Quick Red Tech (Chisom)
// ⚙️ Smart System CLI + GUI Hybrid

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <limits>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_RESET "\x1b[0m"

struct FileInfo {
    std::string name;
    long size;
    time_t creation_time;
    char *content;
};

bool color_enabled = true;
int max_files_limit = 1024;
bool partial_match_enabled = true;

// ========= COLOR UTIL =========
std::string apply_color(const std::string &text, const std::string &color) {
    return color_enabled ? color + text + COLOR_RESET : text;
}

// ========= FILE READER =========
char* read_file_content(const std::string &filename, long &size) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return nullptr;
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    char *buffer = new char[size];
    file.read(buffer, size);
    file.close();
    return buffer;
}

bool is_regular_file(const std::string &path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) return false;
    return (path_stat.st_mode & S_IFMT) == S_IFREG;
}

time_t get_creation_time(const std::string &filename) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;
    FILETIME ftCreate;
    if (!GetFileTime(hFile, &ftCreate, NULL, NULL)) {
        CloseHandle(hFile);
        return 0;
    }
    ULARGE_INTEGER ull;
    ull.LowPart = ftCreate.dwLowDateTime;
    ull.HighPart = ftCreate.dwHighDateTime;
    CloseHandle(hFile);
    return static_cast<time_t>((ull.QuadPart - 116444736000000000ULL) / 10000000ULL);
#else
    struct stat info;
    if (stat(filename.c_str(), &info) != 0) return 0;
    return info.st_ctime;
#endif
}

// ========= D-RED-BOOT SYSTEM =========
void boot_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    using namespace std::chrono_literals;

    std::string logo = R"(
██████╗     ██████╗ ███████╗██████╗ ██████╗     ██████╗  ██████╗ ████████╗
██╔══██╗   ██╔═══██╗██╔════╝██╔══██╗██╔══██╗   ██╔══██╗██╔═══██╗╚══██╔══╝
██║  ██║   ██║   ██║█████╗  ██║  ██║██║  ██║   ██████╔╝██║   ██║   ██║   
██║  ██║   ██║   ██║██╔══╝  ██║  ██║██║  ██║   ██╔═══╝ ██║   ██║   ██║   
██████╔╝██╗╚██████╔╝███████╗██████╔╝██████╔╝██╗██║     ╚██████╔╝   ██║   
╚═════╝ ╚═╝ ╚═════╝ ╚══════╝╚═════╝ ╚═════╝ ╚═╝╚═╝      ╚═════╝    ╚═╝   

         🔴 D-RED-BOT v1.5.0 — Quick Red Tech
)";
    std::cout << COLOR_RED << logo << COLOR_RESET << "\n";

    std::vector<std::string> boot_lines = {
        "Initializing D-Red memory subsystems",
        "Loading kernel-space hooks",
        "Engaging predictive address resolver",
        "Scanning heap allocations",
        "Compiling telemetry cache",
        "Launching main system daemon",
        "Starting D-Red scheduler",
        "Mounting /core/volumes",
        "Linking /dev/mem -> /dred/memory",
        "Loading virtual device drivers",
        "Checking CPU thermal layer",
        "Starting AI telemetry module",
        "Verifying memory integrity",
        "Authenticating root session",
        "Boot sequence verification complete",
        "D-Red System Online at /dev/redkernel"
    };

    for (auto &line : boot_lines) {
        // Random status type
        std::string status;
        int r = rand() % 100;
        if (r < 70) status = "[ OK ]";
        else if (r < 90) status = "[ INFO ]";
        else status = "[ WARN ]";

        std::string color = (status == "[ OK ]") ? COLOR_GREEN :
                            (status == "[ INFO ]") ? COLOR_CYAN : COLOR_YELLOW;

        std::cout << color << "[BOOT] " << line << "... " << status << COLOR_RESET << "\n";

        // Create corresponding .info file
        std::ofstream f("boot_" + line.substr(0, line.find(' ')) + ".info");
        if (f.is_open()) {
            if (rand() % 2 == 0)
                f << "#include <stdio.h>\nint main(){printf(\"Boot module: " << line << "\\n\");return 0;}";
            else
                f << "print('Initializing module: " << line << "')\nprint('Status: ACTIVE')";
            f.close();
        }

#ifdef _WIN32
        Beep(500 + rand() % 300, 50);
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(250 + rand() % 200));
    }

    std::cout << COLOR_YELLOW << "\n[ SYSTEM ONLINE — D-RED-BOT ACTIVE ]\n" << COLOR_RESET;
    std::this_thread::sleep_for(1s);
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ========= FILE LIST =========
std::vector<FileInfo> get_file_list() {
    DIR *dir = opendir(".");
    if (!dir) {
        std::cout << apply_color("Failed to open directory.\n", COLOR_RED);
        return {};
    }
    std::vector<FileInfo> files;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr && (int)files.size() < max_files_limit) {
        std::string filename = entry->d_name;
        if (is_regular_file(filename)) {
            long size;
            char *content = read_file_content(filename, size);
            if (content != nullptr) {
                time_t ctime = get_creation_time(filename);
                files.push_back({filename, size, ctime, content});
            }
        }
    }
    closedir(dir);
    return files;
}

void list_files() {
    auto files = get_file_list();
    if (files.empty()) {
        std::cout << apply_color("\nNo files found.\n", COLOR_RED);
        return;
    }
    std::cout << apply_color("\n[ Directory Scan Results ]\n\n", COLOR_YELLOW);
    for (auto &f : files) {
        std::tm *tm_info = std::localtime(&f.creation_time);
        char time_buf[26];
        strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        std::cout << apply_color(f.name, COLOR_GREEN)
                  << "  →  " << static_cast<void*>(f.content)
                  << "  [" << f.size << " bytes | " << time_buf << "]\n";
        delete[] f.content;
    }
}

// ========= MEMORY PREDICTOR =========
void predict_memory_addresses() {
    char *test = new char[100];
    uintptr_t base = reinterpret_cast<uintptr_t>(test);
    delete[] test;
    std::cout << apply_color("\n[ Predictive Memory Pattern ]\n", COLOR_YELLOW);
    for (int i = 1; i <= 3; ++i) {
        uintptr_t predicted = base + (i * 0x1000) + (rand() % 256);
        std::cout << apply_color("Prediction ", COLOR_GREEN)
                  << i << ": 0x" << std::hex << predicted << std::dec << "\n";
    }
}

// ========= ESC DETECTION =========
bool esc_pressed() {
#ifdef _WIN32
    if (_kbhit() && _getch() == 27) return true;
#else
    struct termios oldt, newt;
    int ch; int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt; newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch == 27) return true;
#endif
    return false;
}

// ========= LIVE MONITOR =========
void live_address_monitor() {
    std::cout << apply_color("\n[ Live Memory Monitor - Press ESC to stop ]\n", COLOR_YELLOW);
    while (true) {
        if (esc_pressed()) break;
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::cout << apply_color("ACTIVE FILE BUFFERS:\n", COLOR_CYAN);
        auto files = get_file_list();
        for (auto &f : files) {
            std::cout << apply_color(f.name, COLOR_GREEN)
                      << " → " << static_cast<void*>(f.content)
                      << " (" << f.size << " bytes)\n";
            delete[] f.content;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
    }
    std::cout << apply_color("\n[ Monitor Ended ]\n", COLOR_RED);
}

// ========= GUI TEST MODE =========
int main();
void gui_test() {
#ifdef _WIN32
    int msgboxID = MessageBoxA(NULL, "Run GUI progress test?", "D-Red-Bot", MB_YESNO | MB_ICONQUESTION);
    if (msgboxID == IDYES) {
        system("cls");
        std::cout << apply_color("Running GUI simulation...\n\n", COLOR_YELLOW);
        for (int i = 0; i <= 100; i += 5) {
            std::cout << "\r[";
            for (int j = 0; j < i / 5; ++j) std::cout << "#";
            for (int j = i / 5; j < 20; ++j) std::cout << " ";
            std::cout << "] " << i << "%" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
        }
        std::cout << "\n" << apply_color("✅ GUI Test Complete. Restarting...", COLOR_GREEN) << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        system("cls");
        main();
    }
#else
    std::cout << apply_color("[GUI Test] Not supported.\n", COLOR_RED);
#endif
}

// ========= SETTINGS =========
void settings_menu() {
    while (true) {
        std::cout << apply_color("\n[ Settings Menu ]\n", COLOR_YELLOW);
        std::cout << "1> Toggle Colors (" << (color_enabled ? "ON" : "OFF") << ")\n";
        std::cout << "2> Toggle Partial Match (" << (partial_match_enabled ? "ON" : "OFF") << ")\n";
        std::cout << "3> GUI Test Mode\n";
        std::cout << "4> Back\n";
        std::cout << "Select: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        switch (choice) {
            case 1: color_enabled = !color_enabled; break;
            case 2: partial_match_enabled = !partial_match_enabled; break;
            case 3: gui_test(); break;
            case 4: return;
            default: std::cout << apply_color("Invalid option.\n", COLOR_RED);
        }
    }
}

// ========= MAIN MENU =========
void show_menu() {
    while (true) {
        std::cout << COLOR_CYAN << "\nMain Menu:\n" << COLOR_RESET;
        std::cout << "1> Scan Files\n";
        std::cout << "2> Predict Memory\n";
        std::cout << "3> Live Monitor\n";
        std::cout << "4> Settings\n";
        std::cout << "5> Exit\n";
        std::cout << COLOR_YELLOW << "Select: " << COLOR_RESET;
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << COLOR_RED << "Invalid input.\n" << COLOR_RESET;
            continue;
        }
        switch (choice) {
            case 1: list_files(); break;
            case 2: predict_memory_addresses(); break;
            case 3: live_address_monitor(); break;
            case 4: settings_menu(); break;
            case 5:
                std::cout << COLOR_GREEN << "\n[ SYSTEM EXITING... ]\n" << COLOR_RESET;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return;
            default:
                std::cout << COLOR_RED << "Invalid option.\n" << COLOR_RESET;
        }
    }
}

// ========= MAIN =========
int main() {
    srand(static_cast<unsigned>(time(0)));
    boot_screen();
    show_menu();
    return 0;
}