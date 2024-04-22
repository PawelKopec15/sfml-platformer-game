#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

struct XMLAttribute
{
	std::string name  = "";
	std::string value = "";
};

struct XMLElement
{
	std::string name = "";

	std::vector<XMLAttribute> attributes = {};
	std::vector<XMLElement> children     = {};
};

class XMLParser
{
public:
	XMLParser() = default;
	explicit XMLParser(const std::string& filename) { parseFile(filename); }
	~XMLParser() = default;

	bool parseFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Error opening file: " << filename << std::endl;
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		std::istringstream xmlStream(buffer.str());

		std::string line;
		while (std::getline(xmlStream, line))
		{
			if (!process_line(line))
			{
				std::cerr << "Error parsing line: \"" << line << "\". Parsing stopped." << std::endl;
				return false;
			}
		}

		if (!stack.empty())
		{
			std::cerr << "Error parsing data, stack not empty. Stack size: " << stack.size() << "." << std::endl;
			return false;
		}

		return true;
	}

	XMLElement getRoot() const { return root; }

	void printParsedData() { print(root, ""); }

private:
	std::stack<XMLElement> stack;

	XMLElement root;

	XMLAttribute createNewAttribute(const std::string& word)
	{
		XMLAttribute toRet;

		char targetChar = '=';
		auto foundIndex = word.find(targetChar);

		if (foundIndex != std::string::npos)
		{
			toRet.name  = word.substr(0, foundIndex);
			toRet.value = word.substr(foundIndex + 2, word.length() - foundIndex - 3);
		}
		else
			toRet.value = word;

		return toRet;
	}

	void handlePopping()
	{
		auto child = stack.top();
		stack.pop();
		if (!stack.empty())
		{
			auto parent = stack.top();
			stack.pop();
			parent.children.push_back(child);
			stack.push(parent);
		}
		else
			root = child;
	}

	bool signedLessThanUnsigned(int sig, uint32_t unsig)
	{
		return (sig < 0) ? true : (static_cast<unsigned int>(sig) < unsig);
	}

	bool process_line(const std::string& val)
	{
		auto line = val;

		// Remove leading whitespace
		line.erase(line.begin(),
				   std::find_if_not(line.begin(), line.end(), [](unsigned char ch) { return std::isspace(ch); }));

		// Remove trailing whitespace
		line.erase(
			std::find_if_not(line.rbegin(), line.rend(), [](unsigned char ch) { return std::isspace(ch); }).base(),
			line.end());

		// Skip xml header
		if (line.substr(0, 2) == "<?")
			return true;

		bool openingBracket   = false;
		bool quotesOnlyOpened = false;

		std::string word = "";

		int firstChar = 0;
		int i         = 0;

		for (; signedLessThanUnsigned(i, line.length()); ++i)
		{
			switch (line[i])
			{
				case '<':
					if (!openingBracket)
					{
						openingBracket = true;
						firstChar      = i + 1;
						break;
					}
					return false;

				case '\"':
					quotesOnlyOpened = !quotesOnlyOpened;
					break;

				case ' ':
				case '>':
					if (quotesOnlyOpened)
						break;

					word      = line.substr(firstChar, i - firstChar);
					firstChar = i + 1;

					if (word.length() == 1)
						break;

					{
						bool shouldPopNext = false;
						if (i > 0 && line[i - 1] == '/')
						{
							word          = word.substr(0, word.length() - 1);
							shouldPopNext = true;
						}

						if (openingBracket)
						{
							XMLElement newElement;
							newElement.name = word;
							openingBracket  = false;
							stack.push(newElement);
						}
						else
						{
							auto element = stack.top();
							stack.pop();
							element.attributes.push_back(createNewAttribute(word));
							stack.push(element);
						}

						if (shouldPopNext)
						{
							openingBracket = false;
							handlePopping();
						}
					}

					break;

				case '/':
					if (!(i > 0 && line[i - 1] == '<') ||
						(signedLessThanUnsigned(i, line.length() - 1) && line[i + 1] == '>'))
						break;

					openingBracket = false;
					handlePopping();

					return true;

				default:
					break;
			}
		}

		if (firstChar != i && !stack.empty())
		{
			auto element = stack.top();
			stack.pop();
			element.attributes.push_back(createNewAttribute(line));
			stack.push(element);
		}

		return true;
	}

	void print(const XMLElement& element, const std::string& prefix)
	{
		std::cout << prefix << "-- ELEMENT name = \'" << element.name << "\'--\n"
				  << prefix << "-- attributes --" << std::endl;

		for (auto& at : element.attributes)
		{
			std::cout << prefix << "name = \'" << at.name << "\'";
			for (int i = at.name.length(); i < 16; i++)
				std::cout << " ";

			std::cout << "value = \'" << at.value << "\'" << std::endl;
		}

		std::cout << prefix << "children: ";
		if (element.children.empty())
			std::cout << "{}" << std::endl;
		else
		{
			std::cout << "\n" << prefix << "{" << std::endl;
			for (auto& child : element.children)
			{
				print(child, prefix + "    ");
				std::cout << std::endl;
			}
			std::cout << prefix << "}" << std::endl;
		}
	}
};