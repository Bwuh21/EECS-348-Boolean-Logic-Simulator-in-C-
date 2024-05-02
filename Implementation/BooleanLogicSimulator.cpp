#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>
#include <cctype>

using namespace std;

// Function checks if the character is an operator
bool isOperator(char c) {
	return c == '&' || c == '|' || c == '!' || c == '@' || c == '$';
}

// Function performs the logical operations and operators
// Takes in two or three arguments depending on which operator is used
// Two arguments for NOT, ! and a, b is set to false to stay out of the way
// Three arguments for everything else, operator, a and b, b is set to whatever
// is passed in and overrides the false set
// Switch cases for each of the operators needed
bool performOperation(char op, bool a, bool b = false) {
	switch (op) {
	case '&':
    	return a && b;
	case '|':
    	return a || b;
	case '!':
    	return !a;
	case '@':
    	return !(a && b);
	case '$':
    	return (a || b) && !(a && b);
	default:
    	return false; // Invalid operator
	}
}

// Function to evaluate the Boolean expression
// Creates the expression string and unordered map
// expression string is just the user input
// The unordered map just maps the chars T and F, to the bools true and false respectively
bool evaluateExpression(const string& expression, const unordered_map<char, bool>& truthValues) {
	// Creates two stacks, one for operands and the other for operators
	stack<bool> operands;
	stack<char> operators;
	// Tells the program to expect a Unary operator like NOT; more on this later
	bool expectUnary = true;

	// Initial for loop iterating through the expression
	for (char c : expression) {
    	if (c == ' ') // Skips whitespace
        	continue;

    	if (c == '(') {
        	// Pushes opening parentheses onto the operators stack
        	operators.push(c);
        	expectUnary = true; // Resets the unary flag inside parentheses
    	}
    	else if (c == ')') {
        	// Basically most things get pushed onto the stack until we reach an ending parenthesis
        	// When we reach an ending parenthesis we work our way backwards through the stack
        	// evaluating as we go until we reach the starting parenthesis
        	// So while the operators stack is not empty and the top is not starting parenthesis
        	while (!operators.empty() && operators.top() != '(') {
            	// op is operator we are evaluating
            	// set that equal to the top then pop it off the stack
            	char op = operators.top();
            	operators.pop();

            	if (op == '!') {
                	// a argument is set to new top of stack then popped
                	bool a = operands.top();
                	operands.pop();
                	// Result of performOperation is pushed to top of the stack
                	// NOT function is our only unary operator so it needs its own special
                	// chunk to push just the operator and a as arguments
                	operands.push(performOperation(op, a));
            	}
            	else {
                	// Every other operator is binary so it will push the operator, a and b
                	// again sets the top to be b then pops it
                	bool b = operands.top();
                	operands.pop();

                	// sets top to be a then pops it
                	bool a = operands.top();
                	operands.pop();

                	// pushes the two bools and the op to perform the operation and then
                	// pushes the result to the top of the stack
                	operands.push(performOperation(op, a, b));
            	}
        	}
        	operators.pop(); // Pops starting parenthesis
        	expectUnary = false; // Resets the unary flag after parentheses
    	}
    	else if (isOperator(c)) {
        	// If our expect Unary flag is true and the operator is NOT we push it like normal
        	if (expectUnary && c == '!') {
            	operators.push(c);
            	expectUnary = true; // Reset the unary flag after unary operator
        	}
        	else {
            	// So while our operators stack is not empty and the top is not (, we check for | and & which have higher
            	// precedence than $ and @
            	while (!operators.empty() && operators.top() != '(' && ((c != '!') || (operators.top() == '|' || operators.top() == '&'))) {
                	char op = operators.top();
                	operators.pop();

                	// same functionality as above with the ), this just executes if we have OR or AND because they need to be
                	// evaluated before XOR and NAND
                	if (op == '!') {
                    	bool a = operands.top();
                    	operands.pop();
                    	operands.push(performOperation(op, a));
                	}
                	else {
                    	bool b = operands.top();
                    	operands.pop();

                    	bool a = operands.top();
                    	operands.pop();

                    	operands.push(performOperation(op, a, b));
                	}
            	}
            	operators.push(c);
            	expectUnary = true; // Set the unary flag after binary operator
        	}
    	}
    	// If character is a variable and not an operator
    	else if (isalpha(c)) {
        	// Find its truth value from the unordered map, so T to true and F to false
        	// then push the resulting bool to the operands stack
        	if (truthValues.find(c) != truthValues.end()) {
            	operands.push(truthValues.at(c)); // Push its truth value
            	expectUnary = false; // Reset the unary flag after variable
        	}
        	else {
            	cout << "Error: Unknown variable '" << c << "'" << endl;
            	return false; // Unknown variable
        	}
    	}
    	else {
        	cout << "Error: Unknown operator '" << c << "'" << endl;
        	return false; // Unknown operator
    	}
	}
	// This is the same operations as with the ) chunk and the & and | chunk
	// This just cleans up whatever is left of the expression and pops everything till the operators stack is empty
	while (!operators.empty()) {
    	char op = operators.top();
    	operators.pop();

    	if (op == '!') {
        	bool a = operands.top();
        	operands.pop();
        	operands.push(performOperation(op, a));
    	}
    	else {
        	bool b = operands.top();
        	operands.pop();

        	bool a = operands.top();
        	operands.pop();

        	operands.push(performOperation(op, a, b));
    	}
	}
	if (operands.empty()) {
    	cerr << "Error: Empty expression." << endl;
    	return false;
	}

	return operands.top();
}

int main() {
	unordered_map<char, bool> truthValues;

	// Unordered map
	// Sets T to true and F to False
	truthValues['T'] = true;
	truthValues['F'] = false;
	truthValues['f'] = false;
	truthValues['t'] = true;

	// This is where we gather the input from the user
	// Set up so that it will continue to ask the user for input until they type quit to exit
	string expression;
	while (true) {
    	cout << "Enter a Boolean expression (or type 'quit' to exit): ";
    	getline(cin, expression);

    	if (expression == "quit")
        	break;
    	// variable to evaluate if the expression is valid or not
    	bool isValid = true;

    	// Evaluates if expression is valid or not
    	// Iterates over the expression and basically if they input the right letters or not
    	// For example they can only use t or f and not x and y
    	for (char c : expression) {
        	if (isalpha(c) && truthValues.find(c) == truthValues.end()) {
            	isValid = false;
            	break;
        	}
    	}
    	// resulting output if they input wrong characters
    	if (!isValid) {
        	cout << "Error: Invalid input. Please use T for True and F for False." << endl;
        	continue;
    	}

    	bool result = evaluateExpression(expression, truthValues);

    	cout << "Result: " << (result ? "True" : "False") << endl;
	}

	return 0;
}
