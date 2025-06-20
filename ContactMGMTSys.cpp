#include <iostream>
#include <string>
#include <iomanip>
#include "sqlite3.h"
#include <limits> // Required for numeric_limits
#include <list> // Include list for list usage
#include <cstddef> // Include cstddef for NULL
#include <cstdlib> // Required for exit()

using namespace std;

// Hotel structure
struct Hotel {
    int id;
    string name;
    string services;
    string location;
    int roomNumber;
};

// Guest structure
struct Guest {
    int id;
    string name;
    int queuePosition;
};

// Node for Hotel Linked List
struct HotelNode {
    Hotel data;
    HotelNode* next;

    HotelNode(Hotel hotel) : data(hotel), next(NULL) {}
};

// Linked List for Hotels
class HotelLinkedList {
public:
    HotelNode* head;
    HotelLinkedList() : head(NULL) {}

    void addHotel(Hotel hotel) {
        HotelNode* newNode = new HotelNode(hotel);
        if (!head) {
            head = newNode;
        } else {
            HotelNode* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    bool removeHotel(int id) {
        if (!head) return false;
        if (head->data.id == id) {
            HotelNode* temp = head;
            head = head->next;
            delete temp;
            return true;
        }
        HotelNode* current = head;
        while (current->next && current->next->data.id != id) {
            current = current->next;
        }
        if (!current->next) return false;
        HotelNode* temp = current->next;
        current->next = current->next->next;
        delete temp;
        return true;
    }

    HotelNode* findHotel(int id) {
        HotelNode* current = head;
        while (current) {
            if (current->data.id == id) {
                return current;
            }
            current = current->next;
        }
        return NULL;
    }

    void displayHotels() {
        cout << "\n--- Hotel List ---\n";
        HotelNode* current = head;
        while (current) {
            cout << "ID: " << current->data.id << "\nName: " << current->data.name
                 << "\nServices: " << current->data.services << "\nLocation: " << current->data.location
                 << "\nRoom Number: " << current->data.roomNumber << "\n\n";
            current = current->next;
        }
    }

    bool isHotelIdUnique(int id) {
        HotelNode* current = head;
        while (current) {
            if (current->data.id == id) {
                return false; // ID already exists
            }
            current = current->next;
        }
        return true; // ID is unique
    }

    list<Hotel> toList() {
        list<Hotel> hotelList;
        HotelNode* current = head;
        while (current) {
            hotelList.push_back(current->data);
            current = current->next;
        }
        return hotelList;
    }

    int size() {
        int count = 0;
        HotelNode* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }


    void clearList() {
        HotelNode* current = head;
        while (current) {
            HotelNode* next = current->next;
            delete current;
            current = next;
        }
        head = NULL;
    }
};

// Node for Guest Linked List
struct GuestNode {
    Guest data;
    GuestNode* next;

    GuestNode(Guest guest) : data(guest), next(NULL) {}
};

// Linked List for Guests (acting as Queue)
class GuestLinkedList {
public:
    GuestNode* head;
    GuestNode* tail; // To efficiently add to the end (enqueue)
    GuestLinkedList() : head(NULL), tail(NULL) {}

    void addGuest(Guest guest) { // Enqueue
        GuestNode* newNode = new GuestNode(guest);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    Guest serveGuest() { // Dequeue
        if (!head) {
            return Guest(); // Return default Guest if queue is empty
        }
        GuestNode* temp = head;
        Guest guest = head->data;
        head = head->next;
        if (!head) {
            tail = NULL; // Queue becomes empty, update tail
        }
        delete temp;
        return guest;
    }

    void displayGuests() {
        cout << "\n--- Guest Queue ---\n";
        GuestNode* current = head;
        while (current) {
            cout << "ID: " << current->data.id << ", Name: " << current->data.name
                 << ", Position: " << current->data.queuePosition << "\n";
            current = current->next;
        }
    }

    bool isGuestIdUnique(int id) {
        GuestNode* current = head;
        while (current) {
            if (current->data.id == id) {
                return false; // ID already exists
            }
            current = current->next;
        }
        return true; // ID is unique
    }

    list<Guest> toList() {
        list<Guest> guestList;
        GuestNode* current = head;
        while (current) {
            guestList.push_back(current->data);
            current = current->next;
        }
        return guestList;
    }

     int size() {
        int count = 0;
        GuestNode* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }

     void clearList() {
        GuestNode* current = head;
        while (current) {
            GuestNode* next = current->next;
            delete current;
            current = next;
        }
        head = NULL;
        tail = NULL;
    }
};


// Global Data
HotelLinkedList hotelList;
GuestLinkedList guestQueue;
list<string> itinerary;
sqlite3* db;

// Function prototypes
void initializeDatabase();
void closeDatabase();
void adminMenu();
void customerMenu();
bool authenticateAdmin();
void addHotel();
void updateHotel();
void viewHotels();
void deleteHotel();
void saveHotelToDatabase(Hotel hotel);
void loadHotelsFromDatabase();
void addGuest();
void serveGuest();
void displayGuestQueue();
void saveGuestToDatabase(Guest guest);
void loadGuestsFromDatabase();
void addStopToItinerary();
void viewItinerary();
bool isHotelIdUnique(int id);
bool isGuestIdUnique(int id);

// Predefined hotels near Lalibela
void addPredefinedHotels() {
    hotelList.clearList(); // Clear existing list first if needed
    list<Hotel> predefinedHotels; // Initialize as an empty list

    Hotel hotel1 = {1, "Lalibela Lodge", "Free Wi-Fi, Restaurant, Pool", "Lalibela", 20};
    predefinedHotels.push_back(hotel1);
    Hotel hotel2 = {2, "Mountain View Hotel", "Spa, Free Breakfast", "Lalibela", 15};
    predefinedHotels.push_back(hotel2);
    Hotel hotel3 = {3, "Rock-Hewn Inn", "Bar, Room Service", "Lalibela", 25};
    predefinedHotels.push_back(hotel3);
    Hotel hotel4 = {4, "Zion Hotel", "Free Parking, Conference Room", "Lalibela", 30};
    predefinedHotels.push_back(hotel4);
    Hotel hotel5 = {5, "St. George Guesthouse", "Free Wi-Fi, Garden", "Lalibela", 10};
    predefinedHotels.push_back(hotel5);
    Hotel hotel6 = {6, "Axum Hotel", "Restaurant, Free Parking", "Lalibela", 18};
    predefinedHotels.push_back(hotel6);
    Hotel hotel7 = {7, "Queen Sheba Hotel", "Pool, Spa, Gym", "Lalibela", 22};
    predefinedHotels.push_back(hotel7);
    Hotel hotel8 = {8, "Ethiopian Heritage Hotel", "Free Breakfast, Bar", "Lalibela", 12};
    predefinedHotels.push_back(hotel8);
    Hotel hotel9 = {9, "Bete Maryam Hotel", "Free Wi-Fi, Restaurant", "Lalibela", 14};
    predefinedHotels.push_back(hotel9);
    Hotel hotel10 = {10, "Tekle Haymanot Lodge", "Garden, Room Service", "Lalibela", 16};
    predefinedHotels.push_back(hotel10);


    for (list<Hotel>::iterator hotel_iter = predefinedHotels.begin(); hotel_iter != predefinedHotels.end(); ++hotel_iter) {
        hotelList.addHotel(*hotel_iter);
    }
}

// Predefined itineraries
void addPredefinedItinerary() {
    itinerary.clear(); // Clear existing itinerary if needed
    itinerary.push_back("1. Visit the Rock-Hewn Churches of Lalibela");
    itinerary.push_back("2. Explore Asheton Maryam Monastery");
    itinerary.push_back("3. Hike to the top of Mount Abuna Yosef");
    itinerary.push_back("4. Visit the Lalibela Market");
    itinerary.push_back("5. Attend a traditional coffee ceremony");
}

int main() {
    initializeDatabase();
    loadHotelsFromDatabase();
    loadGuestsFromDatabase();
    addPredefinedHotels(); // Call addPredefinedHotels to populate hotel list

    int userType;
    do {
        cout << "\nLalibela Tourism Management\n";
        cout << "1. Admin\n2. Customer\n3. Exit\nChoice: ";
        cin >> userType;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        switch(userType) {
            case 1:
                if(authenticateAdmin()) adminMenu();
                else cout << "Access denied!\n";
                break;
            case 2: customerMenu(); break;
            case 3: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while(userType != 3);

    closeDatabase();
    return 0;
}

// Initialize SQLite Database
void initializeDatabase() {
    if (sqlite3_open("tourism.db", &db)) {
        cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }

    char* createHotelsTable = (char*)
        "CREATE TABLE IF NOT EXISTS Hotels ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "services TEXT, "
        "location TEXT, "
        "roomNumber INTEGER);";

    char* createGuestsTable = (char*)
        "CREATE TABLE IF NOT EXISTS Guests ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT, "
        "queuePosition INTEGER);";

    char* errMsg;
    if (sqlite3_exec(db, createHotelsTable, NULL, NULL, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Hotels table: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }

    if (sqlite3_exec(db, createGuestsTable, NULL, NULL, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Guests table: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }

    cout << "Database initialized successfully.\n";
}

// Close SQLite Database
void closeDatabase() {
    sqlite3_close(db);
    cout << "Database connection closed.\n";
}


// Check if hotel ID is unique
bool isHotelIdUnique(int id) {
    return hotelList.isHotelIdUnique(id);
}

// Check if guest ID is unique
bool isGuestIdUnique(int id) {
    return guestQueue.isGuestIdUnique(id);
}

// Admin authentication
bool authenticateAdmin() {
    string password;
    cout << "Enter admin password: ";
    getline(cin, password);
    return (password == "admin123"); // Simple password check
}

// Admin menu
void adminMenu() {
    int choice;
    do {
        cout << "\nAdmin Menu\n"
             << "1. Add Hotel\n"
             << "2. Update Hotel\n"
             << "3. View Hotels\n"
             << "4. Delete Hotel\n"
             << "5. Back\nChoice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        switch(choice) {
            case 1: addHotel(); break;
            case 2: updateHotel(); break;
            case 3: viewHotels(); break;
            case 4: deleteHotel(); break;
        }
    } while(choice != 5);
}

// Add a new hotel
void addHotel() {
    Hotel hotel;
    cout << "Enter Hotel ID: ";
    cin >> hotel.id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    if (!isHotelIdUnique(hotel.id)) {
        cout << "Error: Hotel ID already exists. Please enter a unique ID.\n";
        return;
    }

    cout << "Enter Hotel Name: ";
    getline(cin, hotel.name);
    cout << "Enter Services: ";
    getline(cin, hotel.services);
    cout << "Enter Location: ";
    getline(cin, hotel.location);
    cout << "Enter Room Number: ";
    cin >> hotel.roomNumber;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    hotelList.addHotel(hotel);
    saveHotelToDatabase(hotel);
    cout << "Hotel added successfully!\n";
}

// Save hotel to database
void saveHotelToDatabase(Hotel hotel) {
    char* sql = (char*) "INSERT INTO Hotels (id, name, services, location, roomNumber) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, hotel.id);
        sqlite3_bind_text(stmt, 2, hotel.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, hotel.services.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, hotel.location.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, hotel.roomNumber);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Error inserting hotel into database: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << endl;
    }
}

// Load hotels from database
void loadHotelsFromDatabase() {
    hotelList.clearList();
    char* sql = (char*) "SELECT * FROM Hotels;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Hotel hotel;
            hotel.id = sqlite3_column_int(stmt, 0);
            hotel.name = (char*)sqlite3_column_text(stmt, 1);
            hotel.services = (char*)sqlite3_column_text(stmt, 2);
            hotel.location = (char*)sqlite3_column_text(stmt, 3);
            hotel.roomNumber = sqlite3_column_int(stmt, 4);
            hotelList.addHotel(hotel);
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error loading hotels from database: " << sqlite3_errmsg(db) << endl;
    }
}

// Update hotel details
void updateHotel() {
    int id;
    cout << "Enter Hotel ID to update: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    HotelNode* hotelNode = hotelList.findHotel(id);
    if (hotelNode) {
        Hotel& hotel = hotelNode->data;
        cout << "New name (" << hotel.name << "): ";
        getline(cin, hotel.name);
        cout << "New services (" << hotel.services << "): ";
        getline(cin, hotel.services);
        cout << "New location (" << hotel.location << "): ";
        getline(cin, hotel.location);
        cout << "New room number (" << hotel.roomNumber << "): ";
        cin >> hotel.roomNumber;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        char* sql = (char*) "UPDATE Hotels SET name=?, services=?, location=?, roomNumber=? WHERE id=?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, hotel.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, hotel.services.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, hotel.location.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 4, hotel.roomNumber);
            sqlite3_bind_int(stmt, 5, hotel.id);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                cerr << "Error updating hotel: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << endl;
        }

        cout << "Hotel updated successfully!\n";
        return;
    }
    cout << "Hotel not found!\n";
}

// View all hotels
void viewHotels() {
    hotelList.displayHotels();
}

// Delete a hotel
void deleteHotel() {
    int id;
    cout << "Enter Hotel ID to delete: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    if (hotelList.removeHotel(id)) {
        char* sql = (char*) "DELETE FROM Hotels WHERE id=?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Hotel deleted successfully!\n";
            } else {
                cerr << "Error deleting hotel from database: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing delete statement: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cout << "Hotel not found!\n";
    }
}

// Customer menu
void customerMenu() {
    int choice;
    do {
        cout << "\nCustomer Menu\n"
             << "1. View Hotels\n"
             << "2. Join Queue\n"
             << "3. View Queue\n"
             << "4. Add Itinerary Stop\n"
             << "5. View Itinerary\n"
             << "6. Back\nChoice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        switch(choice) {
            case 1:
                // DEBUG OUTPUT:
                cout << "Debug: Number of hotels in list: " << hotelList.size() << endl;
                viewHotels();
                break;
            case 2: addGuest(); break;
            case 3: displayGuestQueue(); break;
            case 4: addStopToItinerary(); break;
            case 5: viewItinerary(); break;
        }
    } while(choice != 6);
}

// Add a guest to the queue
void addGuest() {
    Guest guest;
    cout << "Enter Guest ID: ";
    cin >> guest.id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    if (!isGuestIdUnique(guest.id)) {
        cout << "Error: Guest ID already exists. Please enter a unique ID.\n";
        return;
    }

    cout << "Enter Guest Name: ";
    getline(cin, guest.name);

    guest.queuePosition = guestQueue.size() + 1;
    guestQueue.addGuest(guest);
    saveGuestToDatabase(guest);
    cout << "Guest added to queue! Position: " << guest.queuePosition << "\n";
}

// Save guest to database
void saveGuestToDatabase(Guest guest) {
    char* sql = (char*) "INSERT INTO Guests (id, name, queuePosition) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, guest.id);
        sqlite3_bind_text(stmt, 2, guest.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, guest.queuePosition);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cerr << "Error inserting guest into database: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing insert statement: " << sqlite3_errmsg(db) << endl;
    }
}

// Load guests from database
void loadGuestsFromDatabase() {
    guestQueue.clearList();
    char* sql = (char*) "SELECT * FROM Guests;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Guest guest;
            guest.id = sqlite3_column_int(stmt, 0);
            guest.name = (char*)sqlite3_column_text(stmt, 1);
            guest.queuePosition = sqlite3_column_int(stmt, 2);
            guestQueue.addGuest(guest);
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error loading guests from database: " << sqlite3_errmsg(db) << endl;
    }
}

// Serve the next guest in the queue
void serveGuest() {
    Guest servedGuest = guestQueue.serveGuest();
    if (servedGuest.id != 0) { // Check if a guest was actually served (ID will be 0 if default Guest was returned)
        cout << "Serving guest: " << servedGuest.name << " (ID: " << servedGuest.id << ")\n";

        char* sql = (char*) "DELETE FROM Guests WHERE id=?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, servedGuest.id);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Guest removed from database.\n"; //Confirmation message for database deletion
            } else {
                cerr << "Error deleting guest from database: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing delete statement: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cout << "Queue is empty!\n";
    }
}

// Display the guest queue
void displayGuestQueue() {
    guestQueue.displayGuests();
}

// Add a stop to the itinerary
void addStopToItinerary() {
    string stop;
    cout << "Enter stop: ";
    getline(cin, stop);
    itinerary.push_back(stop);
    cout << "Stop added to itinerary!\n";
}

// View the itinerary
void viewItinerary() {
    cout << "\n--- Itinerary ---\n";
    for (list<string>::iterator stop_iter = itinerary.begin(); stop_iter != itinerary.end(); ++stop_iter) {
        cout << *stop_iter << "\n";
    }
}
