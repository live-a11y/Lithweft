#include <string>
#include <vector>
#include <variant>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include "BlockInterface.h" // Ensure this includes the necessary definitions

class StringBlockDataProcessor {
public:
    static BlockData processStringInput(const std::string& input) {
        std::vector<ListElement> elements;
        std::string token;
        std::string accumulatedToken;
        bool insideQuotes = false;

        std::istringstream stream(input);
        char ch;
        while (stream.get(ch)) {
            if (ch == '"') {
                insideQuotes = !insideQuotes;
                if (!insideQuotes) { // End of quoted string
                    if (!accumulatedToken.empty()) {
                        elements.push_back(Message(accumulatedToken));
                        accumulatedToken.clear();
                    }
                }
                continue; // Skip adding the quote to the token
            }

            if (insideQuotes || !std::isspace(ch)) {
                accumulatedToken += ch; // Accumulate token inside quotes or non-space characters
            } else if (!accumulatedToken.empty() && !insideQuotes) {
                // Token finished, process it
                processToken(accumulatedToken, elements);
                accumulatedToken.clear(); // Reset for the next token
            }
        }

        if (!accumulatedToken.empty()) {
            // Process any remaining token
            processToken(accumulatedToken, elements);
        }

        // Convert elements to BlockData
        if (elements.size() == 1) {
            return std::visit([](const auto& elem) -> BlockData {
                return elem; // Directly return the element, which is now treated as a BlockData type
            }, elements.front());
        } else if (elements.empty()) {
            return Uninitialized{};
        } else {
            return List{elements.begin(), elements.end()};
        }
    }
    
    // New print method for BlockData
       static void printBlockData(const BlockData& blockData) {
           
           std::visit(overloaded{
               [](const Uninitialized& _) { std::cout << "Type: Uninitialized" << std::endl; },
               [](const Number& number) { std::cout << "Type: Number, Value: " << number << std::endl; },
               [](const Message& message) { std::cout << "Type: Message, Value: " << message << std::endl; },
               [](const List& list) {
                   std::cout << "Type: List, Values: ";
                   for (const auto& element : list) {
                       std::visit(overloaded{
                           [](const Number& number) { std::cout << number << " (Number), "; },
                           [](const Message& message) { std::cout << "\"" << message << "\" (Message), "; }
                       }, element);
                   }
                   std::cout << std::endl;
               },
               [](const AudioSignal& signal) {
                   std::cout << "Type: AudioSignal, Sample Count: " << signal.size() << std::endl;
               }
           }, blockData);
           printf("\n");
       }
    
    
    
private:
    
    // Helper struct for overloaded visitation
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    
    
    static void processToken(const std::string& token, std::vector<ListElement>& elements) {
        if (isStrictNumber(token)) {
            elements.push_back(Number(std::stod(token)));
        } else {
            elements.push_back(Message(token));
        }
    }

    static bool isStrictNumber(const std::string& str) {
        char* end;
        std::strtod(str.c_str(), &end);
        return *end == '\0' && end != str.c_str();
    }
};

