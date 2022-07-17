#pragma once
#include "stdafx.h"
#include <base\hook\inline.h>
#include <include\EditorData.h>
#include "WorldEditor.h"


struct ActionInfo
{
	int type_id;
	std::string name;
};


void SetActionToTextBufferSize(int size);


typedef std::vector<ActionInfo> ActionInfoList;
typedef std::map<std::string, ActionInfoList> ActionInfoMap;

extern ActionInfoMap g_actionInfoTable;

extern HMODULE g_hModule;

extern fs::path g_module_path;

extern std::vector<HWND> g_editor_windows;


class Helper
{
public:
	Helper();
	~Helper();
	
	//static Helper* getInstance();

	enum CONFIG :uint32_t {

		ENABLE_PLUGIN = 1 << 1,

		//���ٱ���
		SUPPER_SPEED_SAVE = 1 << 2,

		//������Դ
		INCRE_RESOURCE = 1 << 3,

		//��ʾ����̨
		SHOW_CONSOLE = 1 << 4,
	};

	uint32_t getConfig();

	void setConfig(uint32_t config);

	void updateState();

	void enableConsole();


	void attach();//����

	void detach();//����


	void setMenuEnable(bool is_enable);
private:


	//�����ͼ
	static uintptr_t onSaveMap();

	//ѡ��ת��ģʽ
	int onSelectConvartMode();

	//���Զ���ת������ʱ
	int onConvertTrigger(Trigger* trg);



	uint32_t m_config = 0;
public:
	fs::path ydwe_path;

protected:
	bool m_bAttach;

	//�Զ���jass��������hook
	hook::hook_t* m_hookSaveMap;
	hook::hook_t* m_hookConvertTrigger;

	//��̬�������� �������͵�hook
	hook::hook_t* m_hookCreateUI;
	hook::hook_t* m_hookReturnTypeStrcmp;

	//�Զ��嶯�����hook
	hook::hook_t* m_hookGetChildCount;
	hook::hook_t* m_hookGetString;
	hook::hook_t* m_hookGetActionType;
	
	//�ǶȻ��Ȼ�����hook����
	hook::hook_t* m_hookParamTypeStrncmp1;
	hook::hook_t* m_hookParamTypeStrncmp2;
	hook::hook_t* m_hookGetParamType;

	//hook we�ĵ��� �����ʱ��������в˵�
	uintptr_t m_hookMessageBoxA;

	hook::hook_t* m_hookInsertActionToText;
	hook::hook_t* m_hookInitWindows;

	fs::path m_configPath;
};
extern Helper g_CHelper;
Helper& get_helper();
