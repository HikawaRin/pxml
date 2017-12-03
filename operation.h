#pragma once

#include "tinyxml.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

class XmlResult
{
public:
	XmlResult(std::string path, TiXmlDocument * doc, std::string location)
	{
		Path = path;
		Doc = doc;
		Location = split(location, ",");
		PRoot = doc->FirstChildElement();
		PLocation = NULL;
	};
	XmlResult(std::string path, TiXmlDocument * doc)
	{
		Path = path;
		Doc = doc;
		Location.push_back("root");
		PRoot = doc->FirstChildElement();
		PLocation = NULL;
	};
	
	//trick
	std::vector<std::string> split(const std::string &s, const std::string &seperator);
	void print_element(TiXmlElement * element);
	//searh all doc to find location, return 0 if location exist 
	int search_doc(std::string location);

	//Operation
	void print_doc();
	//insert text to the location
	void insert(std::string location, std::string text);
	//delete location node
	void delete_node(std::string location);
private:
	//string loation, set by input
	std::vector<std::string> Location;
	//point to the element with the same location 
	TiXmlElement * PLocation;
	//root of the xml tree
	TiXmlElement * PRoot;
	//point to doc
	TiXmlDocument * Doc;
	//path of doc
	std::string Path;
};

std::vector<std::string> XmlResult::split(const std::string &s, const std::string &seperator)
{
	std::vector<std::string> result;
	typedef std::string::size_type string_size;
	string_size i = 0;

	std::string root(PRoot->Value());
	result.push_back(root);
	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
};

void XmlResult::print_element(TiXmlElement * element)
{
	if (element != NULL)
	{
		std::cout << element->Value() << std::endl;
	}
}

//return 0 when find loation
int XmlResult::search_doc(std::string location)
{
	Location = XmlResult::split(location, ".");
	TiXmlElement * ptr = PRoot->FirstChildElement();
	int i = 1, state = 0;
	for (; i != Location.size();)
	{
		PLocation = ptr;
		if (ptr != NULL)
		{
			std::string sptr(ptr->Value());
			if (sptr == Location[i])
			{
				ptr = ptr->FirstChildElement();
				state = 1;
				i = i + 1;
			}else
			{
				ptr = ptr->NextSiblingElement();
				state = 0;
			}
		}else
		{
			break;
		}
	}
	if (i == Location.size())
	{
		std::cout << "find location: ";
		for (int j = 0; j < i; j++)
		{
			std::cout << Location[j] << ".";
		}
		std::cout << std::endl << PLocation->Value();
		return 0;
	}
	else
	{
		PLocation = NULL;
		if (state)
		{
			std::cout << "xml loc not deep enough" << std::endl;
		}
		else
		{
			std::cout << "location not exist" << std::endl;
		}
		//std::cout << i << " " << Location[i];
		return i;
	}
}

void XmlResult::print_doc()
{
	XmlResult::print_element(PRoot);
	TiXmlElement * ptr = PRoot->FirstChildElement();
	while (ptr != NULL)
	{
		XmlResult::print_element(ptr);
		TiXmlElement * childptr = ptr->FirstChildElement();
		TiXmlElement * siblingptr = ptr->NextSiblingElement();
		if (childptr != NULL)
		{
			ptr = childptr;
		}
		else
		{
			ptr = ptr->NextSiblingElement();
		}
	}
}

void XmlResult::insert(std::string location, std::string text)
{
	int sloc(XmlResult::search_doc(location));
	std::cout << sloc;
	//loation already exist
	if (!sloc)
	{

		const char * ctext = text.c_str();
		TiXmlText * etext = new TiXmlText(ctext);
		PLocation->LinkEndChild(etext);
	}
	//loation totally not exist
	else if(sloc == 1)
	{
		Location = XmlResult::split(location, ".");
		const char * eroot = Location[sloc].c_str();
		TiXmlElement * nroot = new TiXmlElement(eroot);
		TiXmlText * etext = new TiXmlText(" ");
		nroot->LinkEndChild(etext);
		PRoot->LinkEndChild(nroot);

		TiXmlNode * ptr(nroot);

		for (int i = sloc + 1; i != Location.size(); i++)
		{
			const char * ele = Location[i].c_str();
			TiXmlElement * element = new TiXmlElement(ele);
			TiXmlText * etext = new TiXmlText(" ");
			element->LinkEndChild(etext);
			ptr->LinkEndChild(element);
			ptr = ptr->LastChild();
		}
		//insert text
		int kick(XmlResult::search_doc(location));
		if (!kick)
		{
			const char * ctext = text.c_str();
			TiXmlText * etext = new TiXmlText(ctext);
			PLocation->LinkEndChild(etext);
		}
	}
	//location not complete exist
	else
	{

		std::string loc;
		for (int i = 1; i < sloc; i++)
		{
			std::string dot = ".";
			loc = loc + Location[i] + dot;
			std::cout << Location[i] << std::endl;
		}
		
		int hloc = XmlResult::search_doc(loc);//PLocation should be in place
		if (hloc == 0)
		{
			Location = XmlResult::split(location, ".");
			const char * eroot = Location[sloc].c_str();
			TiXmlElement * nroot = new TiXmlElement(eroot);
			TiXmlText * etext = new TiXmlText(" ");
			nroot->LinkEndChild(etext);
			PLocation->LinkEndChild(nroot);

			TiXmlNode * ptr(nroot);

			for (int i = sloc + 1; i != Location.size(); i++)
			{
				const char * ele = Location[i].c_str();
				TiXmlElement * element = new TiXmlElement(ele);
				TiXmlText * etext = new TiXmlText(" ");
				element->LinkEndChild(etext);
				ptr->LinkEndChild(element);
				ptr = ptr->LastChild();
			}
		}
		//insert text
		int kick(XmlResult::search_doc(location));
		if (!kick)
		{
			const char * ctext = text.c_str();
			TiXmlText * etext = new TiXmlText(ctext);
			PLocation->LinkEndChild(etext);
		}
	}
	const char * path = Path.c_str();
	Doc->SaveFile(path);
}

void XmlResult::delete_node(std::string location)
{
	int sloc(XmlResult::search_doc(location));
	if (sloc)
	{
		std::cout << "Node " << Location.back() << " doesn't exist";
	}
	else
	{
		TiXmlNode * Pparent = PLocation->Parent();
		Pparent->RemoveChild(PLocation);
		std::cout << "Clear node " << Location.back() << " and it's children";
	}
	const char * path = Path.c_str();
	Doc->SaveFile(path);
}
