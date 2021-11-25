#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
using namespace std;

class Calculator {
public:
	stack<double> stackForOperand;

	virtual double evaluate(const string& postfixExpression) = 0;
	virtual bool executeOperator(const char& anOperator) = 0;
};
//연산
class PostfixCalculator : public Calculator {
private:
	stack<char> stackForTemp;
public:
	double evaluate(const string& postfixExpression) {
		if (postfixExpression.size() == 0) {
			cout << "Non of Expression" << endl;
			throw "Error";
		}
		const char* expression = postfixExpression.c_str();
		char token;
		for (int current = 0; current < postfixExpression.size(); current++) {
			token = expression[current];
			if (token == '\0') {
				break;
			}
			if (token == ' ') { //두자리 이상 연산
				if (!stackForTemp.empty()) {
					vector<char> tempValue(this->stackForTemp.size());
					for (int i = tempValue.size() - 1; i >= 0; i--) {
						tempValue[i] = this->stackForTemp.top();
						this->stackForTemp.pop();
					}
					string temp(tempValue.begin(), tempValue.end());
					int tokenvalue = stoi(temp);
					this->stackForOperand.push(tokenvalue);
				}
				continue;
			}
			if (isdigit(token)) {//숫자는 무조건 push
				this->stackForTemp.push(token);
			}
			else {//연산자는 
				bool check = this->executeOperator(token);
				if (!check) {
					throw "Error";
				}
			}
		}
		if (this->stackForOperand.size() == 1) {
			double result = this->stackForOperand.top();
			this->stackForOperand.pop();
			return result;
		}
		else {
			throw "Error";
		}
	}
	bool executeOperator(const char& anOperator) {
		if (this->stackForOperand.size() < 2) {
			throw "Error";
		}
		double operand1 = this->stackForOperand.top();
		this->stackForOperand.pop();
		double operand2 = this->stackForOperand.top();
		this->stackForOperand.pop();
		double calculated = 0;
		switch (anOperator) {
		case '*':
			calculated = operand2 * operand1;
			break;
		case '/':
			if (operand1 == 0) {
				cout << "DIVIDEDBYZERO" << endl;
				return false;
			}
			else {
				calculated = operand2 / operand1;
			}
			break;
		case '+':
			calculated = operand2 + operand1;
			break;
		case '-':
			calculated = operand2 - operand1;
			break;
		default:
			return false;
			break;
		}

		this->stackForOperand.push(calculated);
		return true;
	}
};
//변환
class Infix2Postfix {//후위연산으로 변환
public:
	stack<char> operatorStack;
	string infixExpression;
	string postfixExpression;
	PostfixCalculator* postfixCalculator = new PostfixCalculator();
	//식에서의 우선순위
	int inComingPrecedence(const char& aToken) {
		switch (aToken) {
		case '(':
			return 20;
		case ')':
			return 19;
		case '*':
			return 13;
		case '/':
			return 13;
		case '+':
			return 12;
		case '-':
			return 12;
		default:
			return -1;
		}
	}
	/*
	 * 스택 안에서 '('는 우선순위가 가장 낮아 연산자를 모두 스택에 포함 ')'는 가장 우선순위가 높아 
	 * 그 안을 모두 pop함 스택에 넣어지기위해서 연산자 '^'는 우선순위가 다르다.
	 */
	int inStackPrecedence(const char& aToken) {
		switch (aToken) {
		case '(':
			return 0;
		case ')':
			return 19;
		case '*':
			return 13;
		case '/':
			return 13;
		case '+':
			return 12;
		case '-':
			return 12;
		default:
			return -1;
		}
	}
	virtual bool infixToPostfix() = 0;
	double evaluates(const string& infixExpression) {
		this->infixExpression = infixExpression;
		if (infixExpression.size() == 0) {
			cout << "NOEXPRESSION" << endl;
			throw "error";
		}
		bool infixError = this->infixToPostfix();
		if (infixError) {
			return this->postfixCalculator->evaluate(this->postfixExpression);
		}
		else {
			throw "error";
		}
	}
};
//변환 세부
class Conversion : public Infix2Postfix {
	string binaryToDecimal(const string& binaryV) {
		const char* binary = binaryV.c_str();
		int resultInt = 0;
		for (int i = 0; i < binaryV.size(); i++) {
			if (binary[i] > '1') {
				cout << "INVALIDBINARYNUMBER" << endl;
				throw "error";
			}
			else {
				int n = binary[i] - '0';
				int k = binaryV.size() - (i + 1);
				resultInt += n * pow(2, k);

			}
		}
		string result = to_string(resultInt);
		return result;
	}

	//16진수를 10진수로 바꾸기
	string hexToDecimal(const string& hexV) {
		const char* hex = hexV.c_str();
		int resultInt = 0;
		for (int i = 0; i < hexV.size(); i++) {
			int k = hexV.size() - (i + 1);
			if (hex[i] >= '0' && hex[i] <= '9') {
				int n = (hex[i] - '0');
				resultInt += n * pow(16, k);
			}
			else if (hex[i] >= 'a' && hex[i] <= 'f') {
				int n = hex[i] - ('a' - 10);
				resultInt += n * pow(16, k);
			}
			else {
				cout << "INVALIDHEXADECIMAL" << endl;
				throw "error";
			}
		}
		string result = to_string(resultInt);
		return result;
	}

	bool infixToPostfix() {
		int sizeV = infixExpression.size();
		vector<char> postfixVector(sizeV);

		char currentToken, poppedToken, topToken;
		const char* expression = infixExpression.c_str();
		int p = 0;
		for (int i = 0; i < infixExpression.size(); i++) {
			currentToken = expression[i];
			if (isdigit(currentToken)) {//숫자일 경우
				if (currentToken == '0') {
					//2진수
					if (i != infixExpression.size() - 1 && (expression[i + 1] == 'b' || expression[i + 1] == 'B')) {
						i = i + 2;
						vector<char> binaryVactor(0);
						while (isdigit(expression[i])) {
							int k = binaryVactor.size();
							binaryVactor.resize(++k);
							currentToken = expression[i];
							binaryVactor[k - 1] = currentToken;
							i++;
						}
						string binaryV(binaryVactor.begin(), binaryVactor.end());
						string decimals = binaryToDecimal(binaryV);
						const char* decimal = decimals.c_str();
						for (int j = 0; j < decimals.size(); j++) {
							postfixVector[p++] = decimal[j];
						}
						i--;

					}
					//16진수
					else if (i != infixExpression.size() - 1 && (expression[i + 1] == 'x' || expression[i + 1] == 'B')) {
						i = i + 2;
						vector<char> hexVactor(0);
						while ((expression[i] >= 'a' && expression[i] <= 'f') || isdigit(expression[i])) {
							int k = hexVactor.size();
							hexVactor.resize(++k);
							currentToken = expression[i];
							hexVactor[k - 1] = currentToken;
							i++;
						}
						string binaryV(hexVactor.begin(), hexVactor.end());
						string hexs = hexToDecimal(binaryV);
						const char* hex = hexs.c_str();
						for (int j = 0; j < hexs.size(); j++) {
							postfixVector[p++] = hex[j];
						}
						i--;
					}
					else {//0이지만 10진수
						postfixVector[p++] = currentToken;
					}
				}
				else {//일반 숫자
					postfixVector[p++] = currentToken;
				}
			}
			else {//연산자의 경우
				postfixVector.resize(++sizeV);
				postfixVector[p++] = ' ';

				if (currentToken == ')') {//괄호 있을때
					poppedToken = operatorStack.top();
					operatorStack.pop();
					while (!operatorStack.empty() && poppedToken != '(') {
						postfixVector[p++] = poppedToken;
						poppedToken = operatorStack.top();
						operatorStack.pop();
					}
				}
				else {
					int inComing = this->inComingPrecedence(currentToken);
					if (inComing < 0) {
						cout << "UNKNOWNOPERATOR" << endl;
						return false;
					}

					if (!operatorStack.empty()) {
						topToken = operatorStack.top();
						while (!operatorStack.empty() && inStackPrecedence(topToken) >= inComing) {
							poppedToken = operatorStack.top();
							operatorStack.pop();
							postfixVector[p++] = poppedToken;
						}
					}

					operatorStack.push(currentToken);
				}
			}
		}
		while (!operatorStack.empty()) {
			poppedToken = operatorStack.top();
			operatorStack.pop();
			if (poppedToken == '(') {
				cout << "MISSINGRIGHTPAREN" << endl;
				return false;
			}
			postfixVector.resize(++sizeV);
			postfixVector[p++] = ' ';
			postfixVector[p++] = poppedToken;
		}
		string temp(postfixVector.begin(), postfixVector.end());
		this->postfixExpression = temp;
		return true;
	}

};
//실행
int main() {
	string input;
	cout << "수식 입력 ('!'입력시 종료) : ";
	getline(cin, input);

	Infix2Postfix* calculator = new Conversion();
	while (input != "!") {
		try {
			double result = calculator->evaluates(input);
			cout << result << endl;
		}
		catch (...) {
			//cout << "오류발생" << endl;
		}
		cout << "수식 입력 ('!'입력시 종료) : ";
		getline(cin, input);
	}
	return 0;
}