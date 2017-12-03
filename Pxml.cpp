// Pxml.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <string>
#include "tinyxml.h"
#include "operation.h"
using namespace std;

#define src "D://Test/test.xml"

TiXmlDocument Load(const char* pFilename)
{
	TiXmlDocument doc(pFilename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		printf("Load file \"%s\"\n", pFilename);
	}
	else
	{
		TiXmlDocument doc;
		TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
		TiXmlElement * element = new TiXmlElement("root");
		TiXmlText * text = new TiXmlText(" ");
		element->LinkEndChild(text);
		doc.LinkEndChild(decl);
		doc.LinkEndChild(element);
		doc.SaveFile(pFilename);
		printf("Creat file \"%s\"\n", pFilename);
	}
	return doc;
}

//insert only element will make an error but with a " " text can work well
int main()
{
	TiXmlDocument doc = Load(src);
	//search finished
	XmlResult result(src, &doc);
	string loc("First.data.");
	int r;
	r = result.search_doc(loc);
	//result.insert("First.data.sam", "right");
	result.delete_node("First.data.sam");
	/*
	TiXmlElement * element = new TiXmlElement("first");
	TiXmlElement * p = doc.FirstChildElement();
	TiXmlText * text = new TiXmlText(" ");
	element->LinkEndChild(text);
	p->LinkEndChild(element);
	doc.SaveFile(src);
	TiXmlElement * pe = doc.FirstChildElement();
	pe = pe->FirstChildElement();
	TiXmlElement * p = pe->NextSiblingElement();
	cout << pe->Value() << p->Value();
	*/
	getchar();
	return 0;
}
