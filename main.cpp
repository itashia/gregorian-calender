#include <iostream>
#include <string>
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <conio.h>

class Date {
private:
    int year, month, day;
    
    static constexpr std::array<int, 13> month_days = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static constexpr std::array<std::string_view, 13> month_names = {
        "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static constexpr std::array<std::string_view, 7> day_names = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    bool is_leap_year() const {
        return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
    }

    int days_in_month() const {
        if (month == 2 && is_leap_year()) {
            return 29;
        }
        return month_days[month];
    }

    void validate_date() {
        if (year < 1900) throw std::out_of_range("Year must be 1900 or later");
        if (month < 1 || month > 12) throw std::out_of_range("Invalid month");
        if (day < 1 || day > days_in_month()) throw std::out_of_range("Invalid day for month");
    }

public:
    // Constructors
    Date(int y = 1900, int m = 1, int d = 1) : year(y), month(m), day(d) {
        validate_date();
    }

    // Factory method for current date
    static Date today() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    }

    // Getters
    int get_year() const { return year; }
    int get_month() const { return month; }
    int get_day() const { return day; }

    // Setters with validation
    void set_year(int y) { year = y; validate_date(); }
    void set_month(int m) { month = m; validate_date(); }
    void set_day(int d) { day = d; validate_date(); }

    // Date conversion to days since epoch (1900-01-01)
    int to_days() const {
        int days = day;
        
        // Add days from months in current year
        for (int m = 1; m < month; ++m) {
            days += (m == 2 && is_leap_year()) ? 29 : month_days[m];
        }
        
        // Add days from previous years
        for (int y = 1900; y < year; ++y) {
            days += ((y % 400 == 0) || (y % 100 != 0 && y % 4 == 0)) ? 366 : 365;
        }
        
        return days;
    }

    // Create Date from days since epoch
    static Date from_days(int days) {
        if (days < 0) throw std::out_of_range("Days must be non-negative");
        
        int y = 1900;
        int d = days;
        
        // Calculate years
        while (true) {
            int days_in_year = ((y % 400 == 0) || (y % 100 != 0 && y % 4 == 0)) ? 366 : 365;
            if (d < days_in_year) break;
            d -= days_in_year;
            y++;
        }
        
        // Calculate months
        int m = 1;
        while (true) {
            int days_in_month = (m == 2 && ((y % 400 == 0) || (y % 100 != 0 && y % 4 == 0))) ? 29 : month_days[m];
            if (d < days_in_month) break;
            d -= days_in_month;
            m++;
        }
        
        return Date(y, m, d + 1); // +1 because days start at 1
    }

    // Display formatted date
    void display(int format = 0) const {
        switch (format) {
            case 0: // YYYY/MM/DD
                std::cout << year << "/" 
                          << std::setw(2) << std::setfill('0') << month << "/" 
                          << std::setw(2) << std::setfill('0') << day;
                break;
                
            case 1: // DD MonthName YYYY
                std::cout << day << " " << month_names[month] << " " << year;
                break;
                
            case 2: // DayName DD MonthName YYYY
                std::cout << day_names[to_days() % 7] << "\t" 
                          << day << "\t" << month_names[month] << "\t" << year;
                break;
                
            case 3: // ISO 8601 format
                std::cout << year << "-"
                          << std::setw(2) << std::setfill('0') << month << "-"
                          << std::setw(2) << std::setfill('0') << day;
                break;
                
            default:
                throw std::invalid_argument("Invalid display format");
        }
    }

    // Input date from user
    void input() {
        while (true) {
            try {
                std::cout << "Year (>=1900): ";
                std::cin >> year;
                
                std::cout << "Month (1-12): ";
                std::cin >> month;
                
                std::cout << "Day (1-" << days_in_month() << "): ";
                std::cin >> day;
                
                validate_date();
                break;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\nPlease try again.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    // Date arithmetic
    Date operator+(int days) const {
        return Date::from_days(to_days() + days);
    }
    
    Date operator-(int days) const {
        return Date::from_days(to_days() - days);
    }
    
    int operator-(const Date& other) const {
        return to_days() - other.to_days();
    }

    // Comparison operators
    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
    
    bool operator!=(const Date& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator>(const Date& other) const {
        return other < *this;
    }
    
    bool operator<=(const Date& other) const {
        return !(*this > other);
    }
    
    bool operator>=(const Date& other) const {
        return !(*this < other);
    }

    // Increment/decrement operators
    Date& operator++() { // prefix
        *this = *this + 1;
        return *this;
    }
    
    Date operator++(int) { // postfix
        Date temp = *this;
        ++*this;
        return temp;
    }
    
    Date& operator--() { // prefix
        *this = *this - 1;
        return *this;
    }
    
    Date operator--(int) { // postfix
        Date temp = *this;
        --*this;
        return temp;
    }

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << date.year << '-' 
           << std::setw(2) << std::setfill('0') << date.month << '-'
           << std::setw(2) << std::setfill('0') << date.day;
        return os;
    }
    
    friend std::istream& operator>>(std::istream& is, Date& date) {
        char sep1, sep2;
        int y, m, d;
        
        if (is >> y >> sep1 >> m >> sep2 >> d && sep1 == '-' && sep2 == '-') {
            date = Date(y, m, d);
        } else {
            is.setstate(std::ios::failbit);
        }
        
        return is;
    }
};

int main() {
    try {
        // Demonstrate modern features
        std::cout << "Today's date: " << Date::today() << "\n\n";
        
        Date d1;
        std::cout << "Enter a date (YYYY-MM-DD): ";
        std::cin >> d1;
        
        std::cout << "\nDate in different formats:\n";
        std::cout << "1. Basic: "; d1.display(0); std::cout << "\n";
        std::cout << "2. Textual: "; d1.display(1); std::cout << "\n";
        std::cout << "3. With weekday: "; d1.display(2); std::cout << "\n";
        std::cout << "4. ISO format: "; d1.display(3); std::cout << "\n";
        
        Date d2 = d1 + 30;
        std::cout << "\n30 days later: " << d2 << "\n";
        
        std::cout << "Days between: " << (d2 - d1) << "\n";
        
        if (d1 < d2) {
            std::cout << d1 << " is earlier than " << d2 << "\n";
        }
        
        std::cout << "\nTomorrow's date: " << ++d1 << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    std::cout << "\nPress any key to exit...";
    _getch();
    return 0;
}
