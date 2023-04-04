#ifndef ProgressBarUtils_HH
#define ProgressBarUtils_HH

#include <iostream>
#include <string>

class ProgressBarUtils {
public:
	const int DisplayWidth = 50;
	const int To = 1;

	ProgressBarUtils(const int to, std::string desc) : To(to), StepValue((double)DisplayWidth / ((double)to)) {
		if (desc != "")
			std::cout << desc << std::endl;
		std::cout << "|";
		for (int i = 0; i < DisplayWidth; i++)
			std::cout << " ";
		std::cout << "|" << std::endl;
	};

	void SetTo(int value) {
		if (value > Value)
			Update(value - Value);
	}

	void Update(int by = 1) {
		if (Value > To)
			return;
		Value += by;
		int newIndex = Value * StepValue;
		for (int i = CurrentDisplayIndex; i < newIndex; i++) {
			std::cout << "X";
		}
		if (Value >= To)
			std::cout << std::endl;
		else
			std::cout.flush();
		CurrentDisplayIndex = newIndex;
	}
	void End() {
		Update(To - Value);
	}

private:
	const int Indent = 0;
	int Value = 0;

	const double StepValue = 0;
	int CurrentDisplayIndex = 0;
};

#endif