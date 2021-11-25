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
//����
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
			if (token == ' ') { //���ڸ� �̻� ����
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
			if (isdigit(token)) {//���ڴ� ������ push
				this->stackForTemp.push(token);
			}
			else {//�����ڴ� 
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
//��ȯ
class Infix2Postfix {//������������ ��ȯ
public:
	stack<char> operatorStack;
	string infixExpression;
	string postfixExpression;
	PostfixCalculator* postfixCalculator = new PostfixCalculator();
	//�Ŀ����� �켱����
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
	 * ���� �ȿ��� '('�� �켱������ ���� ���� �����ڸ� ��� ���ÿ� ���� ')'�� ���� �켱������ ���� 
	 * �� ���� ��� pop�� ���ÿ� �־��������ؼ� ������ '^'�� �켱������ �ٸ���.
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
//��ȯ ����
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

	//16������ 10������ �ٲٱ�
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
			if (isdigit(currentToken)) {//������ ���
				if (currentToken == '0') {
					//2����
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
					//16����
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
					else {//0������ 10����
						postfixVector[p++] = currentToken;
					}
				}
				else {//�Ϲ� ����
					postfixVector[p++] = currentToken;
				}
			}
			else {//�������� ���
				postfixVector.resize(++sizeV);
				postfixVector[p++] = ' ';

				if (currentToken == ')') {//��ȣ ������
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
//����
int main() {
	string input;
	cout << "���� �Է� ('!'�Է½� ����) : ";
	getline(cin, input);

	Infix2Postfix* calculator = new Conversion();
	while (input != "!") {
		try {
			double result = calculator->evaluates(input);
			cout << result << endl;
		}
		catch (...) {
			//cout << "�����߻�" << endl;
		}
		cout << "���� �Է� ('!'�Է½� ����) : ";
		getline(cin, input);
	}
	return 0;
}