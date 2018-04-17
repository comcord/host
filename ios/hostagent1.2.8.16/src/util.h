#pragma once
#include "json.h"

bool Json_Util_Write(Json::Value &root,std::string &str);
bool Json_Util_Open(Json::Value &root,const char * buff);
