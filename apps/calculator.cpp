#include <iostream>
#include <string>
using namespace std;

int main() {
    string operation;
    double num1, num2;

    cout << "Calculator Task Started\n";
    cout << "Instructions:\n";
    cout << "  1. Enter an operation (+, -, *, /) and press Enter.\n";
    cout << "  2. Enter two numbers separated by a space and press Enter.\n";
    cout << "Available operations: +, -, *, /, exit\n";

    while (true) {
        cout << "Calculator :> ";
        cin >> operation;

        if (operation == "exit") {
            cout << "Closing calculator...\n";
            break;
        }

        if (operation == "+" || operation == "-" || operation == "*" || operation == "/") {
            cout << "Enter two numbers: ";
            if (cin >> num1 >> num2) {
                if (operation == "+") cout << "Result: " << (num1 + num2) << "\n\n";
                else if (operation == "-") cout << "Result: " << (num1 - num2) << "\n\n";
                else if (operation == "*") cout << "Result: " << (num1 * num2) << "\n\n";
                else if (operation == "/") {
                    if (num2 != 0) cout << "Result: " << (num1 / num2) << "\n\n";
                    else cout << "Error: Division by zero\n\n";
                }
            } else {
                cout << "Error: Invalid input. Please enter numbers.\n";
                cin.clear();
                string dummy;
                getline(cin, dummy);
            }
        } else {
            cout << "Error: Invalid operation. Use +, -, *, / or type exit.\n";
            cin.clear();
            string dummy;
            getline(cin, dummy);
        }
    }

    return 0;
}
