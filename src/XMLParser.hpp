#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

struct XMLAttribute
{
	std::string name;
	std::string value;
};

struct XMLElement
{
	std::string name;
	std::vector<XMLAttribute> attributes;
	std::vector<XMLElement> children;
};

class XMLParser
{
public:
	XMLParser() = default;

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

		return parse(buffer.str());
	}

	void printXMLTree() const { printXMLElement(rootElement); }

	XMLElement& getRootElement() { return rootElement; }

private:
	bool parse(const std::string& xmlString)
	{
		std::istringstream xmlStream(xmlString);

		std::stack<XMLElement*> elementStack;
		XMLElement* currentElement = nullptr;

		std::string line;
		while (std::getline(xmlStream, line))
		{
			processLine(line, elementStack, currentElement);
		}

		return true;
	}

	void processLine(const std::string& line, std::stack<XMLElement*>& elementStack, XMLElement*& currentElement)
	{
		// Parse XML elements and attributes here
		// This is a basic example and may not handle all cases

		// Example: Detecting an XML element
		size_t startTagPos = line.find('<');
		if (startTagPos != std::string::npos)
		{
			size_t endTagPos = line.find('>', startTagPos);
			if (endTagPos != std::string::npos)
			{
				std::string tagContent = line.substr(startTagPos + 1, endTagPos - startTagPos - 1);

				if (tagContent[0] == '/')
				{
					// Closing tag
					elementStack.pop();
				}
				else
				{
					// Opening tag
					XMLElement newElement;
					parseElementTag(tagContent, newElement);

					if (!elementStack.empty())
						currentElement = &elementStack.top()->children.emplace_back(newElement);

					else
					{
						rootElement    = newElement;
						currentElement = &rootElement;
					}

					elementStack.push(currentElement);

					// Check for <chunk> element and add "CSV" attribute
					if (newElement.name == "chunk")
						currentElement->attributes.push_back({"CSV", ""});
				}
			}
		}
		else if (currentElement->name == "chunk")
			for (auto&& at : currentElement->attributes)
			{
				if (at.name == "CSV")
					at.value.append(line);
			}
	}

	void parseElementTag(const std::string& tagContent, XMLElement& element)
	{
		std::istringstream tagStream(tagContent);

		// Parse element name
		std::getline(tagStream, element.name, ' ');

		// Parse attributes
		std::string attribute;
		while (std::getline(tagStream, attribute, ' '))
		{
			if (!attribute.empty())
			{
				size_t equalPos = attribute.find('=');
				if (equalPos != std::string::npos)
				{
					XMLAttribute attr;
					attr.name  = attribute.substr(0, equalPos);
					attr.value = attribute.substr(equalPos + 2, attribute.length() - equalPos - 3);

					// Rough " char fix
					if (attr.name.back() == '\"')
						attr.name.pop_back();

					element.attributes.push_back(attr);
				}
			}
		}
	}

	void printXMLElement(const XMLElement& element, int depth = 0) const
	{
		for (int i = 0; i < depth; ++i)
			std::cout << "  ";

		std::cout << "Element: " << element.name << std::endl;

		for (const auto& attribute : element.attributes)
		{
			for (int i = 0; i < depth + 1; ++i)
			{
				std::cout << "  ";
			}
			std::cout << "Attribute: " << attribute.name << " = " << attribute.value << std::endl;
		}

		for (const auto& child : element.children)
			printXMLElement(child, depth + 1);
	}

private:
	XMLElement rootElement;
};
