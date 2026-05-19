#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <cctype>
#include "table.h"
using namespace std;

#ifndef MILOUTILS_H
#define MILOUTILS_H

namespace MiloUtils {
    void clearScreen() {

    }
    
    void printHeader(const string& title) {
        int width = 39;
        int padding = (width + title.length()) / 2;
        string border = "+=======================================+";
        cout << "\n\033[1;36m" << border << "\033[0m" << endl;
        cout << "\033[1;33m|" << setw(padding) << title << setw(width - padding + 1) << "|\033[0m" << endl;
        cout << "\033[1;36m" << border << "\033[0m" << endl;
        
    }
    
    void printSeparator() {
        cout << "\033[1;36m=========================================\033[0m" << endl;
    }
    
    void printSuccess(const string& message) {
        cout << "\n\033[1;32m[OK] " << message << "\033[0m" << endl;
    }
    
    void printError(const string& message) {
        cout << "\n\033[1;31m[ERROR] " << message << "\033[0m" << endl;
    }
    
    void printWarning(const string& message) {
        cout << "\n\033[1;33m[WARNING] " << message << "\033[0m" << endl;
    }
    
    void pause() {
        cout << "\n\033[1;36mpress enter to continue...\033[0m";
        cin.ignore();
    }
    
    int getValidatedInput(int minVal, int maxVal, const string& prompt) {
        int input;
        while (true) {
            cout << "\033[1;37m" << prompt << "\033[0m";
            if (cin >> input) {
                if (input >= minVal && input <= maxVal) {
                    return input;
                } else {
                    printError("input harus antara " + to_string(minVal) + "-" + to_string(maxVal));
                }
            } else {
                printError("input harus berupa angka!");
                cin.clear();
                cin.ignore(10000, '\n');
            }
        }
    }
}

#endif

#define ASSERT(kondisi, pesan) \
    if (!(kondisi)) { \
        stringstream ss; \
        ss << "[ASSERT FAILED] " << pesan << " | file: " << __FILE__ \
           << " | line: " << __LINE__ << " | function: " << __func__; \
        throw runtime_error(ss.str()); \
    }

struct user {
    string username;
    string password;
    string role;
};

struct infopaket {
    string hari;    
    string jamsesi;
    int bulanMember;
    int harga;
};

struct member {
    int id;
    string nama;
    string username;
    string namapaket;
    infopaket detail;
    bool diskon_aktif = false;
    int nominal_diskon = 0;
};

#define MAX_MEMBER 100
#define MAX_user 20
#define PERTEMUAN_PER_BULAN 4
#define MAX_PER_SESI 10

class PADELEXCEPTION : public exception {
private:
    string message;
public:
    PADELEXCEPTION(const string& msg) : message(msg) {}
    const char* what() const throw() {
        return message.c_str();
    }
};

string getSesiKey(string namapaket, string hari, string jamsesi) {
    return namapaket + "|" + hari + "|" + jamsesi;
}

bool cekKuotaTersedia(string namapaket, string hari, string jamsesi, member members[], int jumlahmember, int excludeId = -1) {
    string key = getSesiKey(namapaket, hari, jamsesi);
    int currentCount = 0;
    for (int i = 0; i < jumlahmember; i++) {
        if (members[i].id == excludeId) continue;
        string memberKey = getSesiKey(members[i].namapaket, members[i].detail.hari, members[i].detail.jamsesi);
        if (memberKey == key) {
            currentCount++;
        }
    }
    return currentCount < MAX_PER_SESI;
}

void tampilkansesitersedia(string namapaket, string hari, member members[], int jumlahmember) {
    cout << "\n\033[1;35m=== SESI TERSEDIA UNTUK HARI " << hari << " ===\033[0m" << endl;
    int maxJam = (namapaket == "SUN") ? 6 : 7;
    vector<pair<int, string>> sesiTersedia;
    
    for (int i = 1; i <= maxJam; i++) {
        string jamsesi;
        if (namapaket == "SUN") {
            int jamMulai = 5 + i;
            jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
        } else {
            int jamMulai = 15 + i;
            jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
        }
        
        if (cekKuotaTersedia(namapaket, hari, jamsesi, members, jumlahmember)) {
            sesiTersedia.push_back({i, jamsesi});
        }
    }
    
    if (sesiTersedia.empty()) {
        cout << "\033[1;33m[WARNING] Tidak ada sesi tersedia untuk hari " << hari << "\033[0m" << endl;
    } else {
        for (auto& sesi : sesiTersedia) {
            int currentCount = 0;
            string key = getSesiKey(namapaket, hari, sesi.second);
            for (int j = 0; j < jumlahmember; j++) {
                string memberKey = getSesiKey(members[j].namapaket, members[j].detail.hari, members[j].detail.jamsesi);
                if (memberKey == key) currentCount++;
            }
            cout << "   \033[1;32m" << sesi.first << ". " << sesi.second << "\033[0m (\033[1;33mkuota: " << currentCount << "/" << MAX_PER_SESI << "\033[0m)" << endl;
        }
    }
}

int hitungpertemuan(int bulan) {
    try {
        ASSERT(bulan >= 0, "bulan tidak boleh negatif");
        if (bulan <= 0) return 0;
        ASSERT(bulan <= 12, "maksimal durasi 12 bulan");
        return PERTEMUAN_PER_BULAN + hitungpertemuan(bulan - 1);
    } catch (const PADELEXCEPTION& e) {
        throw;
    }
}

string infohari(string namapaket) {
    if (namapaket == "SUN") return "Senin-Jumat";
    else if (namapaket == "MOON") return "Senin-Jumat";
    else if (namapaket == "STAR") return "Sabtu-Minggu";
    else return "-";
}

bool validasihari(string hari, string namapaket) {
    if (namapaket == "SUN" || namapaket == "MOON") {
        return (hari == "Senin" || hari == "senin" ||
                hari == "Selasa" || hari == "selasa" ||
                hari == "Rabu" || hari == "rabu" ||
                hari == "Kamis" || hari == "kamis" ||
                hari == "Jumat" || hari == "jumat");
    } else if (namapaket == "STAR") {
        return (hari == "Sabtu" || hari == "sabtu" ||
                hari == "Minggu" || hari == "minggu");
    }
    return false;
}

int hargapaket(string namapaket, int bulan) {
    try {
        ASSERT(bulan >= 0, "durasi tidak boleh negatif");
        int hargaPerSesi;
        if (namapaket == "SUN") hargaPerSesi = 450;
        else if (namapaket == "MOON") hargaPerSesi = 500;
        else if (namapaket == "STAR") hargaPerSesi = 650;
        else throw PADELEXCEPTION("PAKET TIDAK VALID");
        int totalPertemuan = hitungpertemuan(bulan);
        return hargaPerSesi * totalPertemuan;
    } catch (const PADELEXCEPTION& e) {
        throw;
    }
}

int hargapaket(string namapaket) {
    if (namapaket == "SUN") return 450;
    else if (namapaket == "MOON") return 500;
    else if (namapaket == "STAR") return 650;
    else return 0;
}

int hitungdiskon(int bulan) {
    if (bulan >= 6 && bulan <= 8) return 15;
    else if (bulan >= 9 && bulan <= 12) return 30;
    return 0;
}

void tampilkanpilihanpaket() {
    clitable::Table tabel;
    
    tabel.addColumn(clitable::Column("NO", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 5, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("PAKET", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("HARI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 20, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("HARGA/SESI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::RIGHT_ALIGN, 1, 15, clitable::Column::RESIZABLE));
    
    tabel.addTitle("PILIH PAKET MEMBERSHIP");
    
    string row1[] = {"1", "SUN", "Senin-Jumat", "Rp 450.000"};
    string row2[] = {"2", "MOON", "Senin-Jumat", "Rp 500.000"};
    string row3[] = {"3", "STAR", "Sabtu-Minggu", "Rp 650.000"};
    
    tabel.addRow(row1);
    tabel.addRow(row2);
    tabel.addRow(row3);
    
    cout << tabel.draw() << endl;
}

void jamsun() {
    clitable::Table tabel;
    
    tabel.addColumn(clitable::Column("NO", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 5, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("JAM SESI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 20, clitable::Column::RESIZABLE));
    
    tabel.addTitle("PAKET SUN - Pilih Jam Sesi (06.00-12.00)");
    
    for (int i = 1; i <= 6; i++) {
        int jamMulai = 5 + i;
        string jam = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
        string row[] = {to_string(i), jam};
        tabel.addRow(row);
    }
    
    cout << tabel.draw() << endl;
}

void jammoon() {
    clitable::Table tabel;
    
    tabel.addColumn(clitable::Column("NO", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 5, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("JAM SESI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 20, clitable::Column::RESIZABLE));
    
    tabel.addTitle("PAKET MOON - Pilih Jam Sesi (16.00-23.00)");
    
    for (int i = 1; i <= 7; i++) {
        int jamMulai = 15 + i;
        string jam = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
        string row[] = {to_string(i), jam};
        tabel.addRow(row);
    }
    
    cout << tabel.draw() << endl;
}

void jamstar() {
    clitable::Table tabel;
    
    tabel.addColumn(clitable::Column("NO", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 5, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("JAM SESI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 20, clitable::Column::RESIZABLE));
    
    tabel.addTitle("PAKET STAR - Pilih Jam Sesi (16.00-23.00)");
    
    for (int i = 1; i <= 7; i++) {
        int jamMulai = 15 + i;
        string jam = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
        string row[] = {to_string(i), jam};
        tabel.addRow(row);
    }
    
    cout << tabel.draw() << endl;
}

void jamberdasarkanpaket(string namapaket) {
    if (namapaket == "SUN") {
        jamsun();
    } else if (namapaket == "MOON") {
        jammoon();
    } else if (namapaket == "STAR") {
        jamstar();
    }
}

void tampilkaninfopaket() {
    clitable::Table tabel;
    
    tabel.addColumn(clitable::Column("PAKET", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("JADWAL", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::LEFT_ALIGN, 1, 25, clitable::Column::RESIZABLE));
    tabel.addColumn(clitable::Column("HARGA/SESI", clitable::Column::CENTER_ALIGN, 
                    clitable::Column::RIGHT_ALIGN, 1, 15, clitable::Column::RESIZABLE));
    
    tabel.addTitle("PAKET SESI MILO PADEL");
    
    string row1[3] = {"SUN", "weekday, jam 06.00-16.00", "Rp 450.000"};
    string row2[3] = {"MOON", "weekday, jam 16.00-23.00", "Rp 500.000"};
    string row3[3] = {"STAR", "weekend, jam 16.00-23.00", "Rp 650.000"};
    
    tabel.addRow(row1);
    tabel.addRow(row2);
    tabel.addRow(row3);
    
    cout << tabel.draw();   
    cout << "\n\033[1;36m[INFO] 1 bulan = 4 pertemuan (1x seminggu)\033[0m" << endl;
    cout << "\033[1;36m[INFO] Maksimal 10 member per sesi (5 lapangan x 2 orang)\033[0m" << endl;
}

void sortnamadescending(member arr[], int n) {
    sort(arr, arr + n, [](const member& a, const member& b) {
        string namaA = a.nama;
        string namaB = b.nama;
        
        for (char &c : namaA) c = tolower(c);
        for (char &c : namaB) c = tolower(c);
        
        return namaA > namaB;
    });
}

void sorthargadescending(member arr[], int n) {
    sort(arr, arr + n, [](const member& a, const member& b) {
        return a.id < b.id;
    });
}

void sorthargaascending(member arr[], int n) {
    sort(arr, arr + n, [](const member& a, const member& b) {
        int totalA = hargapaket(a.namapaket, a.detail.bulanMember);
        int totalB = hargapaket(b.namapaket, b.detail.bulanMember);
        
        if (a.diskon_aktif) totalA = totalA - (totalA * a.nominal_diskon / 100);
        if (b.diskon_aktif) totalB = totalB - (totalB * b.nominal_diskon / 100);
        
        return totalA < totalB;
    });
}

int binarysearchid(member *arr, int n, int target) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].id == target) return mid;
        else if (arr[mid].id < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

int linearsearchname(member *arr, int n, string target) {
    for (int i = 0; i < n; i++)
        if (arr[i].nama == target) return i;
    return -1;
}

vector<string> logAktivitas;

void catatLog(const string& aktivitas) {
    logAktivitas.push_back(aktivitas);
}

void tampilkanLog() {
    if (logAktivitas.empty()) {
        cout << "\n\033[1;36m[INFO] belum ada aktivitas tercatat.\033[0m" << endl;
        return;
    }
    cout << "\n\033[1;35m=== LOG AKTIVITAS ===\033[0m" << endl;
    for (size_t i = 0; i < logAktivitas.size(); i++) {
        cout << "\033[1;32m[" << (i+1) << "]\033[0m \033[1;37m" << logAktivitas[i] << "\033[0m" << endl;
    }
    cout << "\033[1;35m=====================\033[0m" << endl;
}

const string FILENAME = "data_member.csv";

void loadmembersfromCSV(member members[], int &jumlahmember) {
    ifstream file(FILENAME);
    if (!file.is_open()) {
        return;
    }
    
    string line;
    getline(file, line);
    
    while (getline(file, line) && jumlahmember < MAX_MEMBER) {
        stringstream ss(line);
        string token;
        member m;
        
        getline(ss, token, ','); m.id = stoi(token);
        getline(ss, token, ','); m.nama = token;
        getline(ss, token, ','); m.username = token;
        getline(ss, token, ','); m.namapaket = token;
        getline(ss, token, ','); m.detail.hari = token;
        getline(ss, token, ','); m.detail.jamsesi = token;
        getline(ss, token, ','); m.detail.bulanMember = stoi(token);
        getline(ss, token, ','); m.detail.harga = stoi(token);
        getline(ss, token, ','); m.diskon_aktif = (token == "1");
        getline(ss, token, ','); m.nominal_diskon = stoi(token);
        
        members[jumlahmember++] = m;
    }
    file.close();
}

void savememberstoCSV(member members[], int jumlahmember) {
    ofstream file(FILENAME);
    if (!file.is_open()) {
        cout << "[ERROR] gagal menyimpan ke " << FILENAME << endl;
        return;
    }
    
    file << "id,nama,username,namapaket,hari,jamsesi,bulanMember,harga,diskon_aktif,nominal_diskon\n";
    
    for (int i = 0; i < jumlahmember; i++) {
        file << members[i].id << ","
             << members[i].nama << ","
             << members[i].username << ","
             << members[i].namapaket << ","
             << members[i].detail.hari << ","
             << members[i].detail.jamsesi << ","
             << members[i].detail.bulanMember << ","
             << members[i].detail.harga << ","
             << (members[i].diskon_aktif ? "1" : "0") << ","
             << members[i].nominal_diskon << "\n";
    }
    file.close();
}

void createdatamember(member members[], int &jumlahmember) {
    MiloUtils::clearScreen();
    try {
        ASSERT(jumlahmember < MAX_MEMBER, "kapasitas member penuh");
        
        cout << "\n\033[1;33m--- BUAT AKUN MEMBER ---\033[0m" << endl;
        member baru;
        baru.id = (jumlahmember > 0) ? members[jumlahmember-1].id + 1 : 1;
        baru.diskon_aktif = false;
        baru.nominal_diskon = 0;

        while (true) {
            cout << "\033[1;37mUSERNAME: \033[0m";
            getline(cin, baru.username);
            ASSERT(!baru.username.empty(), "username tidak boleh kosong");
            
            bool usernameada = false;
            for (int i = 0; i < jumlahmember; i++) {
                if (members[i].username == baru.username) {
                    usernameada = true;
                    break;
                }
            }
            if (usernameada) {
                cout << "\033[1;33m[!] Username sudah digunakan! Silakan gunakan username lain.\033[0m" << endl;
                continue;
            }
            break;
        }
        
        cout << "\033[1;37mNAMA: \033[0m";
        getline(cin, baru.nama);
        ASSERT(!baru.nama.empty(), "nama tidak boleh kosong");
        
        tampilkanpilihanpaket();
        
        int pilihanPaket;
        while (true) {
            cout << "\n\033[1;37mPILIH PAKET (1-3): \033[0m";
            cin >> pilihanPaket;
            cin.ignore();
            
            if (pilihanPaket == 1) {
                baru.namapaket = "SUN";
                break;
            } else if (pilihanPaket == 2) {
                baru.namapaket = "MOON";
                break;
            } else if (pilihanPaket == 3) {
                baru.namapaket = "STAR";
                break;
            } else {
                cout << "\033[1;33m[!] Pilihan tidak valid! Masukkan angka 1-3.\033[0m" << endl;
            }
        }
        
        string infoHari = (baru.namapaket == "STAR") ? "Sabtu/Minggu" : "Senin-Jumat";
        while (true) {
            cout << "\n\033[1;37mHARI (" << infoHari << "): \033[0m";
            getline(cin, baru.detail.hari);
            
            if (validasihari(baru.detail.hari, baru.namapaket)) {
                if (baru.detail.hari.length() > 0) {
                    baru.detail.hari[0] = toupper(baru.detail.hari[0]);
                    for (int i = 1; i < baru.detail.hari.length(); i++) {
                        baru.detail.hari[i] = tolower(baru.detail.hari[i]);
                    }
                }
                break;
            } else {
                cout << "\033[1;33m[!] Hari tidak valid untuk paket " << baru.namapaket << "!\033[0m" << endl;
                cout << "    Paket " << baru.namapaket << " hanya bisa: " << infoHari << endl;
            }
        }
        
        tampilkansesitersedia(baru.namapaket, baru.detail.hari, members, jumlahmember);
        
        int maxJam = (baru.namapaket == "SUN") ? 6 : 7;
        int pilihanJam;
        bool sesiValid = false;
        
        while (!sesiValid) {
            cout << "\n\033[1;37mPILIH JAM SESI (1-" << maxJam << "): \033[0m";
            cin >> pilihanJam;
            cin.ignore();
            
            if (pilihanJam >= 1 && pilihanJam <= maxJam) {
                string jamsesi;
                if (baru.namapaket == "SUN") {
                    int jamMulai = 5 + pilihanJam;
                    jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
                } else {
                    int jamMulai = 15 + pilihanJam;
                    jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
                }
                
                if (cekKuotaTersedia(baru.namapaket, baru.detail.hari, jamsesi, members, jumlahmember)) {
                    baru.detail.jamsesi = jamsesi;
                    sesiValid = true;
                } else {
                    cout << "\033[1;33m[!] Sesi sudah penuh! (maksimal " << MAX_PER_SESI << " member)\033[0m" << endl;
                    tampilkansesitersedia(baru.namapaket, baru.detail.hari, members, jumlahmember);
                }
            } else {
                cout << "\033[1;33m[!] Pilihan jam tidak valid! Masukkan angka 1-" << maxJam << ".\033[0m" << endl;
            }
        }
        
        while (true) {
            cout << "\n\033[1;37mMAU JOIN BERAPA BULAN (1-12): \033[0m";
            cin >> baru.detail.bulanMember;
            cin.ignore();
            
            if (baru.detail.bulanMember >= 1 && baru.detail.bulanMember <= 12) {
                break;
            } else {
                cout << "\033[1;33m[!] Durasi harus antara 1-12 bulan! Silakan input kembali.\033[0m" << endl;
            }
        }
        
        baru.detail.harga = hargapaket(baru.namapaket);
        members[jumlahmember] = baru;
        jumlahmember++;
        
        savememberstoCSV(members, jumlahmember);
        
        int totalPertemuan = hitungpertemuan(baru.detail.bulanMember);
        int total = hargapaket(baru.namapaket, baru.detail.bulanMember);
        
        MiloUtils::printSuccess("YEY AKUN MEMBER UDAH ADA!");
        cout << "\033[1;36m      PAKET           : \033[1;37m" << baru.namapaket << "\033[0m" << endl;
        cout << "\033[1;36m      HARI            : \033[1;37m" << baru.detail.hari << "\033[0m" << endl;
        cout << "\033[1;36m      JAM SESI        : \033[1;37m" << baru.detail.jamsesi << "\033[0m" << endl;
        cout << "\033[1;36m      DURASI          : \033[1;37m" << baru.detail.bulanMember << " bulan\033[0m" << endl;
        cout << "\033[1;36m      TOTAL PERTEMUAN : \033[1;37m" << totalPertemuan << "x\033[0m" << endl;
        cout << "\033[1;36m      HARGA /SESI     : \033[1;32mRp " << baru.detail.harga << ".000\033[0m" << endl;
        cout << "\033[1;36m      TOTAL BAYAR     : \033[1;32mRp " << total << ".000\033[0m" << endl;
        catatLog("member baru: " + baru.nama + " (ID: " + to_string(baru.id) + ") - Sesi: " + baru.detail.hari + " " + baru.detail.jamsesi);
    } catch (const PADELEXCEPTION& e) {
        MiloUtils::printError(e.what());
    } catch (const exception& e) {
        MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
    }
}

void menutampildata(member members[], int jumlahmember) {
    MiloUtils::clearScreen();
    if (jumlahmember == 0) {
        MiloUtils::printWarning("data member masih kosong!");
        return;
    }

    int pilihanUtama;
    bool exitMenu = false;

    while (!exitMenu) {
        try {
            MiloUtils::printHeader("MENU TAMPILAN DATA");
            cout << "\n\033[1;32m1. SEARCH\033[0m" << endl;
            cout << "\033[1;34m2. SORT\033[0m" << endl;
            cout << "\033[1;31m3. KEMBALI\033[0m" << endl;
            MiloUtils::printSeparator();
            cout << "\033[1;37mPILIHAN: \033[0m";
            cin >> pilihanUtama;
            cin.ignore();
            ASSERT(pilihanUtama >= 1 && pilihanUtama <= 3, "PILIHAN TIDAK VALID");

            if (pilihanUtama == 1) {
                int pilihanSearch;
                bool exitSearch = false;
                while (!exitSearch) {
                    cout << "\n\033[1;35m--- MENU SEARCH ---\033[0m" << endl;
                    cout << "1. BERDASARKAN ID" << endl;
                    cout << "2. BERDASARKAN NAMA" << endl;
                    cout << "3. KEMBALI" << endl;
                    cout << "\033[1;37mPILIHAN: \033[0m";
                    cin >> pilihanSearch;
                    cin.ignore();
                    ASSERT(pilihanSearch >= 1 && pilihanSearch <= 3, "PILIHAN TIDAK VALID");

                    if (pilihanSearch == 1) {
                        int cariID; cout << "\033[1;37mmasukkan ID: \033[0m"; cin >> cariID; cin.ignore();
                        int idx = -1;
                        for(int i=0; i<jumlahmember; i++) if(members[i].id == cariID) { idx = i; break; }
                        if (idx != -1) {
                            int total = hargapaket(members[idx].namapaket, members[idx].detail.bulanMember);
                            int totalAkhir = members[idx].diskon_aktif ? total - (total * members[idx].nominal_diskon / 100) : total;
                            MiloUtils::printSuccess("DATA DITEMUKAN!");
                            cout << "\033[1;36m  ID: \033[1;37m" << members[idx].id << "\033[1;36m | NAMA: \033[1;37m" << members[idx].nama 
                                 << "\033[1;36m | PAKET: \033[1;37m" << members[idx].namapaket 
                                 << "\033[1;36m | SESI: \033[1;37m" << members[idx].detail.hari << " " << members[idx].detail.jamsesi
                                 << "\033[1;36m | DISKON: \033[1;37m" << (members[idx].diskon_aktif ? to_string(members[idx].nominal_diskon) + "%" : "-")
                                 << "\033[1;36m | TOTAL: \033[1;32mRp " << totalAkhir << "K\033[0m" << endl;
                        } else {
                            MiloUtils::printWarning("ID TIDAK DITEMUKAN.");
                        }
                    } else if (pilihanSearch == 2) {
                        string cariNama; cout << "\033[1;37mmasukkan Nama: \033[0m"; getline(cin, cariNama);
                        int idx = linearsearchname(members, jumlahmember, cariNama);
                        if (idx != -1) {
                            int total = hargapaket(members[idx].namapaket, members[idx].detail.bulanMember);
                            int totalAkhir = members[idx].diskon_aktif ? total - (total * members[idx].nominal_diskon / 100) : total;
                            MiloUtils::printSuccess("DATA DITEMUKAN!");
                            cout << "\033[1;36m  ID: \033[1;37m" << members[idx].id << "\033[1;36m | NAMA: \033[1;37m" << members[idx].nama 
                                 << "\033[1;36m | PAKET: \033[1;37m" << members[idx].namapaket 
                                 << "\033[1;36m | SESI: \033[1;37m" << members[idx].detail.hari << " " << members[idx].detail.jamsesi
                                 << "\033[1;36m | DISKON: \033[1;37m" << (members[idx].diskon_aktif ? to_string(members[idx].nominal_diskon) + "%" : "-")
                                 << "\033[1;36m | TOTAL: \033[1;32mRp " << totalAkhir << "K\033[0m" << endl;
                        } else {
                            MiloUtils::printWarning("NAMA TIDAK DITEMUKAN.");
                        }
                    } else if (pilihanSearch == 3) {
                        exitSearch = true;
                    }
                    if (!exitSearch) MiloUtils::pause();
                }
            } else if (pilihanUtama == 2) {
                int pilihanSort;
                bool exitSort = false;
                while (!exitSort) {
                    cout << "\n\033[1;35m--- MENU SORT ---\033[0m" << endl;
                    cout << "1. NAMA" << endl;
                    cout << "2. ID" << endl;
                    cout << "3. HARGA" << endl;
                    cout << "4. KEMBALI" << endl;
                    cout << "\033[1;37mPILIHAN: \033[0m";
                    cin >> pilihanSort;
                    cin.ignore();
                    ASSERT(pilihanSort >= 1 && pilihanSort <= 4, "PILIHAN TIDAK VALID");

                    if (pilihanSort >= 1 && pilihanSort <= 3) {
                        member temp[MAX_MEMBER];
                        for(int i=0; i<jumlahmember; i++) temp[i] = members[i];

                        if(pilihanSort == 1) sortnamadescending(temp, jumlahmember);
                        else if(pilihanSort == 2) sorthargadescending(temp, jumlahmember);
                        else if(pilihanSort == 3) sorthargaascending(temp, jumlahmember);

                        cout << "\n\033[1;36m========================================================================================\033[0m" << endl;
                        cout << "\033[1;33m" << left << setw(4) << "ID" << setw(15) << "NAMA" << setw(8) << "PAKET" 
                             << setw(12) << "SESI" << setw(8) << "BULAN" << setw(10) << "DISKON" << setw(15) << "TOTAL" << "\033[0m" << endl;
                        cout << "\033[1;36m----------------------------------------------------------------------------------------\033[0m" << endl;
                        for (int i = 0; i < jumlahmember; i++) {
                            int total = hargapaket(temp[i].namapaket, temp[i].detail.bulanMember);
                            int totalAkhir = temp[i].diskon_aktif ? total - (total * temp[i].nominal_diskon / 100) : total;
                            string sesiInfo = temp[i].detail.hari.substr(0,3) + " " + temp[i].detail.jamsesi.substr(0,5);
                            cout << "\033[1;37m" << left << setw(4) << temp[i].id << setw(15) << temp[i].nama 
                                 << setw(8) << temp[i].namapaket << setw(12) << sesiInfo
                                 << setw(8) << temp[i].detail.bulanMember
                                 << setw(10) << (temp[i].diskon_aktif ? to_string(temp[i].nominal_diskon) + "%" : "-")
                                 << setw(15) << ("Rp " + to_string(totalAkhir) + "K") << "\033[0m" << endl;
                        }
                        cout << "\033[1;36m========================================================================================\033[0m" << endl;
                    } else if (pilihanSort == 4) {
                        exitSort = true;
                    }
                    if (!exitSort) MiloUtils::pause();
                }
            } else if (pilihanUtama == 3) {
                exitMenu = true;
            }
        } catch (const PADELEXCEPTION& e) { MiloUtils::printError(e.what()); MiloUtils::pause(); }
        catch (const exception& e) { MiloUtils::printError("ERROR " + string(e.what())); MiloUtils::pause(); }
    }
}

void keloladiskonmembership(member members[], int jumlahmember) {
    MiloUtils::clearScreen();
    try {
        vector<int> eligible;
        for(int i=0; i<jumlahmember; i++) {
            if(members[i].detail.bulanMember >= 6) eligible.push_back(i);
        }
        
        if(eligible.empty()) {
            MiloUtils::printWarning("tidak ada member dengan durasi >= 6 bulan.");
            return;
        }
        
        cout << "\n\033[1;35m=== DISKON MEMBERSHIP ===\033[0m" << endl;
        cout << "\033[1;36m[INFO] KETENTUAN DISKON:\033[0m" << endl;
        cout << "\033[1;33m1. 6-8 bulan --> diskon 15%\033[0m" << endl;
        cout << "\033[1;33m2. 9-12 bulan --> diskon 30%\033[0m" << endl;
        cout << "\n\033[1;37mDAFTAR MEMBER   :\033[0m" << endl;
        cout << "\033[1;36m" << left << setw(4) << "ID" << setw(15) << "NAMA" << setw(8) << "BULAN" 
             << setw(15) << "TOTAL AWAL" << "\033[0m" << endl;
        cout << "\033[1;36m------------------------------------------\033[0m" << endl;
        for(int idx : eligible) {
            int total = hargapaket(members[idx].namapaket, members[idx].detail.bulanMember);
            cout << "\033[1;37m" << left << setw(4) << members[idx].id << setw(15) << members[idx].nama 
                 << setw(8) << members[idx].detail.bulanMember
                 << setw(15) << ("Rp " + to_string(total) + "K") << "\033[0m" << endl;
        }
        cout << "\033[1;36m------------------------------------------\033[0m" << endl;
        
        cout << "\n\033[1;37mmasukkan ID member untuk diberi diskon --> 0 untuk batal: \033[0m";
        int idPilih; cin >> idPilih; cin.ignore();
        if(idPilih == 0) return;
        
        int foundIdx = -1;
        for(int i=0; i<jumlahmember; i++) {
            if(members[i].id == idPilih && members[i].detail.bulanMember >= 6) {
                foundIdx = i;
                break;
            }
        }
        
        ASSERT(foundIdx != -1, "ID tidak valid atau durasi < 6 bulan!");
        
        int diskon = hitungdiskon(members[foundIdx].detail.bulanMember);
        int total = hargapaket(members[foundIdx].namapaket, members[foundIdx].detail.bulanMember);
        int totalAkhir = total - (total * diskon / 100);
        
        cout << "\n\033[1;35m--- KONFIRMASI ---\033[0m" << endl;
        cout << "\033[1;36mMEMBER: \033[1;37m" << members[foundIdx].nama << " (ID: " << members[foundIdx].id << ")\033[0m" << endl;
        cout << "\033[1;36mDURASI: \033[1;37m" << members[foundIdx].detail.bulanMember << " bulan\033[0m" << endl;
        cout << "\033[1;33mklik 1 untuk ACC, angka lain --> batal: \033[0m";
        
        int konfirmasi; cin >> konfirmasi; cin.ignore();
        
        if(konfirmasi == 1) {
            members[foundIdx].diskon_aktif = true;
            members[foundIdx].nominal_diskon = diskon;
            
            savememberstoCSV(members, jumlahmember);
            
            cout << "\n\033[1;32m[OK] DISKON " << diskon << "% BERHASIL DITERAPKAN!\033[0m" << endl;
            cout << "\033[1;36mdata akhir setelah diskon: \033[1;32mRp " << totalAkhir << ".000\033[0m" << endl;
            catatLog("diskon " + to_string(diskon) + "% untuk member ID " + to_string(idPilih));
        } else {
            MiloUtils::printWarning("pemberian diskon dibatalkan");
        }
        
    } catch (const PADELEXCEPTION& e) {
        MiloUtils::printError(e.what());
    } catch (const exception& e) {
        MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
    }
}

void readdatamember(member members[], int jumlahmember, string username) {
    try {
        bool ditemukan = false;
        
        for (int i = 0; i < jumlahmember; i++) {
            if (members[i].username == username) {
                clitable::Table tabel;
                
                tabel.addColumn(clitable::Column("DATA MEMBER", clitable::Column::LEFT_ALIGN, 
                                clitable::Column::LEFT_ALIGN, 1, 20, clitable::Column::RESIZABLE));
                tabel.addColumn(clitable::Column("INFORMASI", clitable::Column::LEFT_ALIGN, 
                                clitable::Column::LEFT_ALIGN, 1, 35, clitable::Column::RESIZABLE));
                
                tabel.addTitle("DATA MEMBER ANDA");
                
                int totalPertemuan = hitungpertemuan(members[i].detail.bulanMember);
                int total = hargapaket(members[i].namapaket, members[i].detail.bulanMember);
                int totalAkhir = members[i].diskon_aktif ? total - (total * members[i].nominal_diskon / 100) : total;
                
                string row1[2] = {"ID", to_string(members[i].id)};
                string row2[2] = {"NAMA", members[i].nama};
                string row3[2] = {"USERNAME", members[i].username};
                string row4[2] = {"PAKET", members[i].namapaket};
                string row5[2] = {"HARI", members[i].detail.hari};
                string row6[2] = {"JAM SESI", members[i].detail.jamsesi};
                string row7[2] = {"DURASI", to_string(members[i].detail.bulanMember) + " bulan"};
                string row8[2] = {"PERTEMUAN /bulan", to_string(PERTEMUAN_PER_BULAN) + "x"};
                string row9[2] = {"TOTAL PERTEMUAN", to_string(totalPertemuan) + "x"};
                string row10[2] = {"HARGA /sesi", "Rp " + to_string(members[i].detail.harga) + ".000"};
                
                if (members[i].diskon_aktif) {
                    string row11[2] = {"DISKON", to_string(members[i].nominal_diskon) + "%"};
                    string row12[2] = {"TOTAL BAYAR", "Rp " + to_string(totalAkhir) + ".000"};
                    tabel.addRow(row1);
                    tabel.addRow(row2);
                    tabel.addRow(row3);
                    tabel.addRow(row4);
                    tabel.addRow(row5);
                    tabel.addRow(row6);
                    tabel.addRow(row7);
                    tabel.addRow(row8);
                    tabel.addRow(row9);
                    tabel.addRow(row10);
                    tabel.addRow(row11);
                    tabel.addRow(row12);
                } else {
                    string row11[2] = {"TOTAL BAYAR", "Rp " + to_string(total) + ".000"};
                    tabel.addRow(row1);
                    tabel.addRow(row2);
                    tabel.addRow(row3);
                    tabel.addRow(row4);
                    tabel.addRow(row5);
                    tabel.addRow(row6);
                    tabel.addRow(row7);
                    tabel.addRow(row8);
                    tabel.addRow(row9);
                    tabel.addRow(row10);
                    tabel.addRow(row11);
                }
                
                cout << tabel.draw();
                ditemukan = true;
                break;
            }
        }
        
        if (!ditemukan) {
            cout << "\n\033[1;31m[ERROR] BELUM JOIN MEMBER! silakan buat akun membership dulu..\033[0m" << endl;
        }
        
    } catch (const PADELEXCEPTION& e) {
        MiloUtils::printError(e.what());
    } catch (const exception& e) {
        MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
    }
}

void updatedatamember(member members[], int jumlahmember) {
    MiloUtils::clearScreen();
    try {
        ASSERT(jumlahmember > 0, "data member masih kosong");
        cout << "\n\033[1;36mDAFTAR MEMBER:\033[0m" << endl;
        for (int i = 0; i < jumlahmember; i++) {
            cout << "\033[1;37m  [" << members[i].id << "] " << members[i].nama << " | PAKET: " << members[i].namapaket << " | SESI: " << members[i].detail.hari << " " << members[i].detail.jamsesi << "\033[0m" << endl;
        }
        
        int index;
        cout << "\n\033[1;37mID yang mau diupdate: \033[0m";
        cin >> index;
        cin.ignore();
        ASSERT(index > 0 && index <= jumlahmember, "ID MEMBER TIDAK VALID");
        
        int idx = index - 1;
        cout << "\n\033[1;33m--- UPDATE DATA MEMBER ---\033[0m" << endl;
        cout << "\033[1;36mNAMA: \033[1;37m" << members[idx].nama << "\033[0m" << endl;
        cout << "\033[1;36mPAKET SEKARANG: \033[1;37m" << members[idx].namapaket << "\033[0m" << endl;
        
        tampilkanpilihanpaket();

        int pilihanPaket;
        while (true) {
            cout << "\n\033[1;37mPILIH PAKET BARU (1-3): \033[0m";
            cin >> pilihanPaket;
            cin.ignore();
            
            if (pilihanPaket == 1) {
                members[idx].namapaket = "SUN";
                break;
            } else if (pilihanPaket == 2) {
                members[idx].namapaket = "MOON";
                break;
            } else if (pilihanPaket == 3) {
                members[idx].namapaket = "STAR";
                break;
            } else {
                cout << "\033[1;33m[!] Pilihan tidak valid! Masukkan angka 1-3.\033[0m" << endl;
            }
        }
        
        string infoHari = (members[idx].namapaket == "STAR") ? "Sabtu/Minggu" : "Senin-Jumat";
        while (true) {
            cout << "\n\033[1;37mHARI (" << infoHari << "): \033[0m";
            getline(cin, members[idx].detail.hari);
            
            if (validasihari(members[idx].detail.hari, members[idx].namapaket)) {
                if (members[idx].detail.hari.length() > 0) {
                    members[idx].detail.hari[0] = toupper(members[idx].detail.hari[0]);
                    for (int i = 1; i < members[idx].detail.hari.length(); i++) {
                        members[idx].detail.hari[i] = tolower(members[idx].detail.hari[i]);
                    }
                }
                break;
            } else {
                cout << "\033[1;33m[!] Hari tidak valid untuk paket " << members[idx].namapaket << "!\033[0m" << endl;
                cout << "    Paket " << members[idx].namapaket << " hanya bisa: " << infoHari << endl;
            }
        }
        
        tampilkansesitersedia(members[idx].namapaket, members[idx].detail.hari, members, jumlahmember);
        
        int maxJam = (members[idx].namapaket == "SUN") ? 6 : 7;
        int pilihanJam;
        bool sesiValid = false;
        
        while (!sesiValid) {
            cout << "\n\033[1;37mPILIH JAM SESI BARU (1-" << maxJam << "): \033[0m";
            cin >> pilihanJam;
            cin.ignore();
            
            if (pilihanJam >= 1 && pilihanJam <= maxJam) {
                string jamsesi;
                if (members[idx].namapaket == "SUN") {
                    int jamMulai = 5 + pilihanJam;
                    jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
                } else {
                    int jamMulai = 15 + pilihanJam;
                    jamsesi = to_string(jamMulai) + ".00-" + to_string(jamMulai + 1) + ".00";
                }
                
                if (cekKuotaTersedia(members[idx].namapaket, members[idx].detail.hari, jamsesi, members, jumlahmember, members[idx].id)) {
                    members[idx].detail.jamsesi = jamsesi;
                    sesiValid = true;
                } else {
                    cout << "\033[1;33m[!] Sesi sudah penuh! (maksimal " << MAX_PER_SESI << " member)\033[0m" << endl;
                    tampilkansesitersedia(members[idx].namapaket, members[idx].detail.hari, members, jumlahmember);
                }
            } else {
                cout << "\033[1;33m[!] Pilihan jam tidak valid! Masukkan angka 1-" << maxJam << ".\033[0m" << endl;
            }
        }
        
        while (true) {
            cout << "\n\033[1;37mMAU JOIN BERAPA BULAN (1-12): \033[0m";
            cin >> members[idx].detail.bulanMember;
            cin.ignore();
            
            if (members[idx].detail.bulanMember >= 1 && members[idx].detail.bulanMember <= 12) {
                break;
            } else {
                cout << "\033[1;33m[!] Durasi harus antara 1-12 bulan! Silakan input kembali.\033[0m" << endl;
            }
        }
        
        members[idx].detail.harga = hargapaket(members[idx].namapaket);
        int totalPertemuan = hitungpertemuan(members[idx].detail.bulanMember);
        int total = hargapaket(members[idx].namapaket, members[idx].detail.bulanMember);
        int totalAkhir = members[idx].diskon_aktif ? total - (total * members[idx].nominal_diskon / 100) : total;
        
        savememberstoCSV(members, jumlahmember);
        
        MiloUtils::printSuccess("UPDATE BERHASIL!");
        cout << "\033[1;36m   TOTAL PERTEMUAN : \033[1;37m" << totalPertemuan << "x\033[0m" << endl;
        if (members[idx].diskon_aktif) {
            cout << "\033[1;36m   DISKON          : \033[1;33m" << members[idx].nominal_diskon << "%\033[0m" << endl;
            cout << "\033[1;36m   TOTAL BAYAR     : \033[1;32mRp " << totalAkhir << ".000 (setelah diskon)\033[0m" << endl;
        } else {
            cout << "\033[1;36m   TOTAL BAYAR     : \033[1;32mRp " << total << ".000\033[0m" << endl;
        }
        catatLog("update member ID " + to_string(members[idx].id));
    } catch (const PADELEXCEPTION& e) {
        MiloUtils::printError(e.what());
    } catch (const exception& e) {
        MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
    }
}

void deletedatamember(member members[], int &jumlahmember) {
    MiloUtils::clearScreen();
    try {
        ASSERT(jumlahmember > 0, "data member masih kosong");
        cout << "\n\033[1;36mDAFTAR MEMBER:\033[0m" << endl;
        for (int i = 0; i < jumlahmember; i++) {
            cout << "\033[1;37m  [" << members[i].id << "] " << members[i].nama << "\033[0m" << endl;
        }
        
        int index;
        cout << "\n\033[1;37mID yang mau dihapus: \033[0m";
        cin >> index;
        cin.ignore();
        ASSERT(index > 0 && index <= jumlahmember, "ID MEMBER TIDAK VALID");
        
        for (int i = index - 1; i < jumlahmember - 1; i++) {
            members[i] = members[i + 1];
        }
        jumlahmember--;
        
        savememberstoCSV(members, jumlahmember);
        
        MiloUtils::printSuccess("MEMBER ID " + to_string(index) + " BERHASIL DIHAPUS!");
        catatLog("hapus member ID " + to_string(index));
    } catch (const PADELEXCEPTION& e) {
        MiloUtils::printError(e.what());
    } catch (const exception& e) {
        MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
    }
}

void menuadmin(member members[], int &jumlahmember, string userLogin) {
    MiloUtils::clearScreen();
    int pilihan; 
    bool menuberjalan = true;
    
    do {
        try {
            cout << endl;
            MiloUtils::printHeader("ADMIN MENU - " + userLogin);
            cout << "\n\033[1;34m1. BUAT AKUN MEMBER\033[0m" << endl;
            cout << "\033[1;32m2. TAMPILKAN DATA\033[0m" << endl;
            cout << "\033[1;33m3. KELOLA DISKON MEMBERSHIP\033[0m" << endl;
            cout << "\033[1;34m4. UPDATE PAKET MEMBER\033[0m" << endl;
            cout << "\033[1;31m5. HAPUS MEMBER\033[0m" << endl;
            cout << "\033[1;35m6. LOG AKTIVITAS\033[0m" << endl;
            cout << "\033[1;36m7. LOGOUT\033[0m" << endl;
            MiloUtils::printSeparator();
            cout << "\033[1;37mPILIHAN (1-7): \033[0m";
            cin >> pilihan;
            cin.ignore();
            ASSERT(pilihan >= 1 && pilihan <= 7, "PILIHAN TIDAK VALID!");
            
            switch (pilihan) {
                case 1: createdatamember(members, jumlahmember); break;
                case 2: menutampildata(members, jumlahmember); break;
                case 3: keloladiskonmembership(members, jumlahmember); break;
                case 4: updatedatamember(members, jumlahmember); break;
                case 5: deletedatamember(members, jumlahmember); break;
                case 6: tampilkanLog(); MiloUtils::pause(); break;
                case 7:
                    MiloUtils::printSuccess("BERHASIL LOGOUT!");
                    menuberjalan = false;
                    break;
            }
        } catch (const PADELEXCEPTION& e) {
            MiloUtils::printError(e.what());
        } catch (const exception& e) {
            MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
        }
    } while (menuberjalan);
}

void menumember(member members[], int &jumlahmember, string userLogin) {
    MiloUtils::clearScreen();
    int pilihan; 
    bool menuberjalan = true;
    
    do {
        try {
            cout << endl;
            MiloUtils::printHeader("MEMBER MENU - " + userLogin);
            cout << "\n\033[1;36m1. LIHAT JADWAL & HARGA PAKET\033[0m" << endl;
            cout << "\033[1;32m2. BUAT AKUN MEMBERSHIP\033[0m" << endl;
            cout << "\033[1;34m3. LIHAT DATA SAYA\033[0m" << endl;
            cout << "\033[1;31m4. LOGOUT\033[0m" << endl;
            MiloUtils::printSeparator();
            cout << "\033[1;37mPILIHAN: \033[0m";
            cin >> pilihan;
            cin.ignore();
            ASSERT(pilihan >= 1 && pilihan <= 4, "PILIHAN TIDAK VALID!");
            
            switch (pilihan) {
                case 1: tampilkaninfopaket(); break;
                case 2: createdatamember(members, jumlahmember); break;
                case 3: readdatamember(members, jumlahmember, userLogin); break;
                case 4:
                    MiloUtils::printSuccess("BERHASIL LOGOUT!");
                    menuberjalan = false;
                    break;
            }
        } catch (const PADELEXCEPTION& e) {
            MiloUtils::printError(e.what());
        } catch (const exception& e) {
            MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
        }
    } while (menuberjalan);
}

int main() {
    try {
        user users[MAX_user] = {
            {"tirizzzmen", "2025", "member"},
            {"admin", "milopadel", "admin"}
        };
        int jumlahuser = 2;
        member members[MAX_MEMBER];
        int jumlahmember = 0;
        string inputnama, inputpw; 
        string userLogin, roleLogin;
        
        loadmembersfromCSV(members, jumlahmember);
        if (jumlahmember > 0) {
            cout << "\033[1;32m[INFO] " << jumlahmember << " data member berhasil dimuat dari " << FILENAME << "\033[0m" << endl;
        }
        
        while (true) {
            try {
                MiloUtils::clearScreen();
                MiloUtils::printHeader("WELCOME TO MILO PADEL");
                cout << "\n\033[1;33m--- MENU UTAMA ---\033[0m" << endl;
                cout << "\033[1;32m1. REGISTRASI AKUN BARU\033[0m" << endl;
                cout << "\033[1;34m2. LOGIN\033[0m" << endl;
                cout << "\033[1;31m3. KELUAR PROGRAM\033[0m" << endl;
                cout << "\033[1;37mPILIHAN: \033[0m";
                int pilihanawal;
                cin >> pilihanawal;
                cin.ignore();
                ASSERT(pilihanawal >= 1 && pilihanawal <= 3, "PILIHAN TIDAK VALID");
                
                if (pilihanawal == 1) {
                    cout << "\n\033[1;33m--- REGISTRASI AKUN ---\033[0m" << endl;
                    cout << "\033[1;37mUSERNAME: \033[0m";
                    getline(cin, inputnama);
                    bool usernameada = false;
                    for (int i = 0; i < jumlahuser; i++) {
                        if (inputnama == users[i].username) {
                            usernameada = true;
                            break;
                        }
                    }
                    ASSERT(!usernameada, "username sudah digunakan!");
                    cout << "\033[1;37mPASSWORD: \033[0m";
                    getline(cin, inputpw);
                    ASSERT(inputpw.length() >= 3, "password minimal 3 karakter!");
                    users[jumlahuser].username = inputnama;
                    users[jumlahuser].password = inputpw;
                    users[jumlahuser].role = "user";
                    jumlahuser++;
                    MiloUtils::printSuccess("REGISTRASI BERHASIL! SILAKAN LOGIN");
                } else if (pilihanawal == 2) {
                    cout << "\n\033[1;33m--- LOGIN ---\033[0m" << endl;
                    bool loginSukses = false;
                    for (int percobaan = 0; percobaan < 3; percobaan++) {
                        cout << "\033[1;37mUSERNAME: \033[0m";
                        getline(cin, inputnama);
                        cout << "\033[1;37mPASSWORD: \033[0m";
                        getline(cin, inputpw);
                        bool usnbenar = false, pwbenar = false;
                        for (int i = 0; i < jumlahuser; i++) {
                            if (inputnama == users[i].username) {
                                usnbenar = true;
                                if (inputpw == users[i].password) {
                                    pwbenar = true;
                                    userLogin = inputnama;
                                    roleLogin = users[i].role;
                                    break;
                                }
                            }
                        }
                        if (usnbenar && pwbenar) {
                            loginSukses = true;
                            break;
                        }
                        MiloUtils::printError("LOGIN GAGAL!");
                        if (!usnbenar) cout << "\033[1;33m  username tidak ditemukan!\033[0m" << endl;
                        else cout << "\033[1;33m  password salah!\033[0m" << endl;
                        ASSERT(percobaan < 2, "AKSES DITOLAK! GAGAL LOGIN 3x");
                        cout << "\033[1;36m  sisa percobaan: " << (2 - percobaan) << "\033[0m" << endl;
                    }
                    ASSERT(loginSukses, "LOGIN GAGAL");
                    if (roleLogin == "admin") menuadmin(members, jumlahmember, userLogin);
                    else menumember(members, jumlahmember, userLogin);
                } else if (pilihanawal == 3) {
                    savememberstoCSV(members, jumlahmember);
                    MiloUtils::printSuccess("TERIMA KASIH MILOVERS! SEE YOU...");
                    break;
                }
            } catch (const PADELEXCEPTION& e) {
                MiloUtils::printError(e.what());
                MiloUtils::pause();
            } catch (const exception& e) {
                MiloUtils::printError("TERJADI KESALAHAN: " + string(e.what()));
                MiloUtils::pause();
            }
        }
    } catch (const exception& e) {
        MiloUtils::printError("FATAL ERROR: " + string(e.what()));
        return 1;
    }
    return 0;
}