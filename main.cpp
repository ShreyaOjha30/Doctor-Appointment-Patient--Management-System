// Doctor Appointment & Patient Management System
// Tools: C++, File Handling, STL, Role-Based Menus

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;

struct Appointment {
    string patientName;
    string gender;
    int age;
    string doctorName;
    string date;  // YYYY-MM-DD
    string time;  // HH:MM
};

struct User {
    string username, password, role;
};

vector<Appointment> appointments;
vector<User> users;
string currentDoctor = "";
const string appointmentsFile = "appointments.txt";
const string usersFile = "users.txt";

void loadingDots(string msg) {
    cout << msg;
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(400));
    }
    cout << endl;
}

void saveAppointmentsToFile() {
    ofstream file(appointmentsFile);
    for (const auto &a : appointments) {
        file << a.patientName << "," << a.gender << "," << a.age << "," << a.doctorName << "," << a.date << "," << a.time << "\n";
    }
    file.close();
}

void loadAppointmentsFromFile() {
    appointments.clear();
    ifstream file(appointmentsFile);
    string line;
    while (getline(file, line)) {
        Appointment a;
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        size_t p3 = line.find(',', p2 + 1);
        size_t p4 = line.find(',', p3 + 1);
        size_t p5 = line.find(',', p4 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos || p4 == string::npos || p5 == string::npos) continue;
        a.patientName = line.substr(0, p1);
        a.gender = line.substr(p1 + 1, p2 - p1 - 1);
        a.age = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        a.doctorName = line.substr(p3 + 1, p4 - p3 - 1);
        a.date = line.substr(p4 + 1, p5 - p4 - 1);
        a.time = line.substr(p5 + 1);
        appointments.push_back(a);
    }
    file.close();
}

void loadUsersFromFile() {
    users.clear();
    ifstream file(usersFile);
    string uname, pass, role;
    while (file >> uname >> pass >> role) {
        users.push_back({uname, pass, role});
    }
    file.close();
}

bool login(string &role) {
    string uname, pass;
    cout << "\n===== LOGIN =====\n";
    cout << "Username: "; cin >> uname;
    cout << "Password: "; cin >> pass;
    for (const auto &u : users) {
        if (u.username == uname && u.password == pass) {
            role = u.role;
            if (role == "doctor") currentDoctor = uname;
            return true;
        }
    }
    cout << "\n❌ Login failed.\n";
    return false;
}

void bookAppointment() {
    Appointment a;
    cout << "\nEnter Patient Name: ";
    getline(cin >> ws, a.patientName);
    cout << "Enter Gender (M/F): ";
    getline(cin >> ws, a.gender);
    cout << "Enter Age: ";
    cin >> a.age;
    cout << "Enter Doctor Name: ";
    getline(cin >> ws, a.doctorName);
    cout << "Enter Date (YYYY-MM-DD): ";
    getline(cin >> ws, a.date);
    cout << "Enter Time (HH:MM): ";
    getline(cin >> ws, a.time);

    for (const auto &app : appointments) {
        if (app.date == a.date && app.time == a.time && app.doctorName == a.doctorName) {
            cout << "\n❌ Another appointment already exists with Dr. " << app.doctorName << " at this time.\n";
            return;
        }
    }
    appointments.push_back(a);
    saveAppointmentsToFile();
    loadingDots("\nSaving appointment");
    cout << "✅ Appointment booked successfully!\n";
}

void displayAppointments(const string &filterDoctor = "") {
    bool found = false;
    for (const auto &a : appointments) {
        if (filterDoctor.empty() || a.doctorName == filterDoctor) {
            if (!found) {
                cout << "\n+----------------+--------+-----+----------------+------------+--------+\n";
                cout << "| Patient Name   | Gender | Age | Doctor         | Date       | Time   |\n";
                cout << "+----------------+--------+-----+----------------+------------+--------+\n";
                found = true;
            }
            cout << "| " << setw(14) << left << a.patientName
                 << "| " << setw(6) << left << a.gender
                 << "| " << setw(3) << left << a.age
                 << "| " << setw(14) << left << a.doctorName
                 << "| " << setw(10) << left << a.date
                 << "| " << setw(6) << left << a.time << " |\n";
        }
    }
    if (!found) cout << "\n📂 No appointments found.\n";
    else cout << "+----------------+--------+-----+----------------+------------+--------+\n";
}

void searchPatients(const string &doctorFilter = "") {
    string name;
    cout << "\nEnter patient name to search: ";
    getline(cin >> ws, name);
    bool found = false;
    for (const auto &a : appointments) {
        if ((doctorFilter.empty() || a.doctorName == doctorFilter) && a.patientName.find(name) != string::npos) {
            cout << "- " << a.patientName << " | " << a.gender << " | Age: " << a.age << " | Dr. " << a.doctorName << " | " << a.date << " | " << a.time << endl;
            found = true;
        }
    }
    if (!found) cout << "❌ No matching records found.\n";
}

void removeAppointment() {
    string name, date;
    cout << "\nEnter patient name to remove: "; getline(cin >> ws, name);
    cout << "Enter date (YYYY-MM-DD): "; getline(cin >> ws, date);
    auto it = remove_if(appointments.begin(), appointments.end(), [&](Appointment a) {
        return a.patientName == name && a.date == date;
    });
    if (it != appointments.end()) {
        appointments.erase(it, appointments.end());
        saveAppointmentsToFile();
        cout << "\n✅ Appointment removed.\n";
    } else cout << "\n❌ No matching appointment found.\n";
}

void doctorDashboard() {
    int choice;
    do {
        cout << "\n========== DOCTOR DASHBOARD ==========\n";
        cout << "1. View My Appointments\n2. Search My Patients\n3. Logout\n";
        cout << "Enter choice: "; cin >> choice; cin.ignore();
        switch (choice) {
            case 1: displayAppointments(currentDoctor); break;
            case 2: searchPatients(currentDoctor); break;
            case 3: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 3);
}

void receptionDashboard() {
    int choice;
    do {
        cout << "\n========== RECEPTION DASHBOARD ==========\n";
        cout << "1. Book Appointment\n2. View Appointments\n3. Search Patients\n4. Logout\n";
        cout << "Enter choice: "; cin >> choice; cin.ignore();
        switch (choice) {
            case 1: bookAppointment(); break;
            case 2: displayAppointments(); break;
            case 3: searchPatients(); break;
            case 4: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 4);
}

void adminDashboard() {
    int choice;
    do {
        cout << "\n========== ADMIN DASHBOARD ==========\n";
        cout << "1. View All Appointments\n2. Remove Appointment\n3. Logout\n";
        cout << "Enter choice: "; cin >> choice; cin.ignore();
        switch (choice) {
            case 1: displayAppointments(); break;
            case 2: removeAppointment(); break;
            case 3: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 3);
}

int main() {
    loadUsersFromFile();
    loadAppointmentsFromFile();
    string role;
    if (login(role)) {
        if (role == "doctor") doctorDashboard();
        else if (role == "reception") receptionDashboard();
        else if (role == "admin") adminDashboard();
    }
    return 0;
}
