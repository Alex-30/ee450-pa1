#include <iostream>
#include <fstream>
#include <string>

using namespace std;


void binary_sum(string &sum, string &tmp) {
	int carry = false;
	int len = int(sum.size());

	for (int i = (len - 1); i >= 0; i--) {
		if (sum[i] == '0' && tmp[i] == '0') {
			sum[i] = char(int('0') + carry);
			carry = false;
		} else if (sum[i] == '0' && tmp[i] == '1' || sum[i] == '1' && tmp[i] == '0') {
			if (carry) sum[i] = '0';
			else sum[i] = '1';
		} else {
			sum[i] = char(int('0') + carry);
			carry = true;
		}
	}

	if (carry) sum = '1' + sum;
}


void ones_complement(string &str) {
	for (int i = 0; i < int(str.size()); i++) {
		if (str[i] == '1') str[i] = '0';
		else str[i] = '1';
	}
}


void carry_wrap(string &sum) {
	string tmp;
	int len = int(sum.size()) - 16;
	
	tmp.clear();
	
	for (int i = 0; i < len; i++) tmp = tmp + sum[i];
	for (int i = 0; i < (16 - len); i++) tmp = '0' + tmp;

	sum = sum.substr(len, 16);
	binary_sum(sum, tmp);
}


string xor_opr(string data, string error, string sum) {
	string trans = data + sum;

	for (int i = 0; i < int(error.size()); i++) {
		if (error[i] != trans[i]) trans[i] = '1';
		else trans[i] = '0';
	}

	return trans;
}


string check_sum(const string data, const string error, string &result) {
	string sum;
	string tmp;
	string check_sum;
	string received;

	sum.clear();
	tmp.clear();
	received.clear();
	check_sum.clear();

	sum = "0000000000000000";

	for (int i = 0; i < int(data.size()); i++) {
		tmp = tmp + data[i];

		if ((i + 1) % 16 == 0) {
			for (int i = 0; i < int(sum.size()) - 16; i++) tmp = '0' + tmp;

			binary_sum(sum, tmp);
			tmp.clear();
		}
	}

	carry_wrap(sum);
	ones_complement(sum);

	check_sum = sum;

	received = xor_opr(data, error, sum);

	sum.clear();
	tmp.clear();

	sum = "0000000000000000";

	for (int i = 0; i < int(received.size()); i++) {
		tmp = tmp + received[i];

		if ((i + 1) % 16 == 0) {
			for (int i = 0; i < int(sum.size()) - 16; i++) tmp = '0' + tmp;

			binary_sum(sum, tmp);
			tmp.clear();
		}
	}

	carry_wrap(sum);

	if (sum != "1111111111111111") result = "Not Pass";
	else result = "Pass";

	return check_sum;
}


void xor_division(string &tmp, string genr) {
	int check = false;

	for (int i = 0; i < int(tmp.size()); i++) {
		if (tmp[i] != genr[i]) tmp[i] = '1';
		else tmp[i] = '0';
	}

	for (int i = 0; i < int(tmp.size()); i++) {
		if (tmp[i] != '0') {
			tmp = tmp.substr(i, int(tmp.size()) - i);
			check = true;
			break;
		}
	}

	if (!check) tmp = "";
}


string CRC_16(const string data, const string error, string &result) {
	string received;
	string remainder;
	string trans = data + "0000000000000000";
	string genr = "10000010000010001";
	string crc;

	int num = 0;
	for (num = 0; num < int(trans.size()); num++) if (trans[num] != '0') break;

	if (num + 16 < int(trans.size())) crc = trans.substr(num, 16);
	else crc = trans.substr(num, int(trans.size()) - num);

	for (int i = num + 16; i < int(trans.size()); i++) {
		crc = crc + trans[i];

		if (int(crc.size()) == int(genr.size())) xor_division(crc, genr);
	}

	for (int i = 0; i < 16 - int(crc.size()); i++) crc = '0' + crc;

	received = xor_opr(data, error, crc);
	remainder = received.substr(0, 16);

	for (int i = 16; i < int(received.size()); i++) {
		remainder = remainder + received[i];

		if (int(remainder.size()) == int(genr.size())) xor_division(remainder, genr);
	}

	result = "Pass";
	for (int i = 0; i < int(remainder.size()); i++) if (remainder[i] == '1') result = "Not Pass";

	return crc;
}


int main() {
	string data;
	string error;

	ifstream file("data.txt");

	if (!file.is_open()) {
		cout << "error to open the file!\n";
		return 0;
	}

	while (file >> data >> error) {
		string result;

		cout << "=================================\n";
		cout << "Data:" << data << '\n';
		cout << "Error:" << error << '\n';

		cout << "CRC-16\n";
		cout << "CRC:" << CRC_16(data, error, result) << ";\t\tResult: " << result << '\n';

		cout << "Checksum\n";
		cout << "Checksum:" << check_sum(data, error, result) << ";\tResult: " << result << '\n';
	}

	file.close();

	return 0;
}


