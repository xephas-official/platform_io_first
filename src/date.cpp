#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <Arduino.h>
#include <ESP8266WiFi.h>

std::string getTodayFormatted() {
  // Get current time
  time_t now = time(0);

  // Convert time to tm structure for date manipulation
  tm *timeinfo = localtime(&now);

  // Create a string stream for formatted date
  std::stringstream ss;

  // Use setw to ensure consistent width for day, month, and year
  ss << std::setfill('0') << std::setw(2) << timeinfo->tm_mday << "-";
  ss << std::setfill('0') << std::setw(2) << timeinfo->tm_mon + 1 << "-";
  ss << timeinfo->tm_year + 1900;

  return ss.str();
}

// a function to return getTodayFormatted as a String
String getTodayFormattedString()
{
    return getTodayFormatted().c_str();
}

// int main() {
//   std::string today = getTodayFormatted();
//   std::cout << "Today's date: " << today << std::endl;
//   return 0;
// }
