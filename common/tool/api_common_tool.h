#pragma once
#include "base/memory/singleton.h"
#include <vector>

class ApiCommonTool
{
public:
	SINGLETON_DEFINE(ApiCommonTool);
	ApiCommonTool(){}
	~ApiCommonTool(){}

	std::string ComposeParamArray(std::string key, std::vector<std::string> value);  //��ϲ�ѯ����

};
