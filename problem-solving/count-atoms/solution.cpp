/*
________________________________________________________

Problem description:

Given a chemical formula as a string, the task is to get the count of atoms in this chemical formula.

1. No need to verify if the input is a valid chemical formula in all aspects.
2. However, please validate if basic structure is followed. For example, an atom can be represented using:
	a. A capital letter. (Eg: O)
	b. A capital letter followed by a number (eg: H2)
	c. Two letters where capital letter is followed by a small letter (eg: Na)
	d. Two letters where a capital letter is followed by a small letter and number (eg: Fe2)
3. A formula can have one or more atoms represented in it.


Test case 0:

Input:
Fe2H3OH

Ouput:
Fe 	2
H 	4
O 	1


Test case 1:

Input:
NaCl2NaO2

Output:
Na 	2
Cl 	2
O 	2

____________________________________________________________		*/

#include <iostream>
#include <map> //Use unordered_map if ordering in o/p doesn't matter.
#include <ctype.h>

unsigned int count_atoms(std::string s_formula, std::map<std::string, int> &map) {

	unsigned int
		total_counts = 0,
		count = 1,
		temp = 0; //Using `temp` variable in case if i/p is given as 'X1' where X is an atom.
	std::string buffer(s_formula.begin(), s_formula.begin() + 1); //starts from the first atom.

	for(std::string::iterator i = s_formula.begin() + 1;; ++i) {

		if(i == s_formula.end() || isupper(*i)) {

			//Either increment existing atom count, or initialize it to value `count`.
			map[buffer] = (map.find(buffer) != map.end()) ? (map[buffer] + count) : (count);
			total_counts += count;
			count = 1;
			temp = 0;
			buffer.clear();

			if(i == s_formula.end()) //loop exit condition.
				break;
			else buffer += *i;
		}
		else if(isdigit(*i)) {

			count = (temp = (temp * 10) + (*i - '0')); //To support multi-digit numbers.
		} else if(islower(*i))
			buffer += *i;
		else
			return -1;
	}
	return total_counts;
}

int main() {

	std::string formula;
	std::map<std::string, int> map;

	std::cin >> formula;
	std::cout << "Total atom counts = " << count_atoms(formula, map) << std::endl;
	for(auto i : map) {

		std::cout << i.first << "\t" << i.second << "\n";
	}
}
