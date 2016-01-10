// ConfigFile.h: interface for the ConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGFILE_H__4D165134_14DC_425B_84B6_77963BADF072__INCLUDED_)
#define AFX_CONFIGFILE_H__4D165134_14DC_425B_84B6_77963BADF072__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning (disable: 4503)
#pragma warning (disable: 4786)

#include <string>
#include <map>
#include <stack>

class ConfigFile  
{
public:
	ConfigFile(std::string filename);
	virtual ~ConfigFile();


public: //accessors
  std::string getStr( std::string key);
  int getInt( std::string key);
  float getFloat ( std::string key);
  bool isKey( std::string key );

public: //section modifiers
  void pushs( std::string section );
  std::string tops();
  void pops();

public: //modifiers
  void setValueStr( std::string key, std::string value);
  void setValueInt( std::string key, int value);
  void setValueFlt( std::string key, float value);


  void write();


private:
  void writeMap();

  std::string filename;
  std::map< std::string, std::map< std::string, std::string > > data;
  std::stack< std::string > section;
  std::map< std::string, std::string > *curSec;


};

#endif // !defined(AFX_CONFIGFILE_H__4D165134_14DC_425B_84B6_77963BADF072__INCLUDED_)





