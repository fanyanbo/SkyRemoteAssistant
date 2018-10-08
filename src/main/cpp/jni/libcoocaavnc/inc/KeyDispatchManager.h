/**************************************************************
 * Project			TVDoctor
 * (c) Copyright	2016
 * Company			SKYWORTH GROUP R&D ACADEMY SOFTWARE RESEARCH TEAM			
 *					All rights reserved
 **************************************************************/
 /**
 * @file			KeyDispatchManager.h
 * @ingroup			
 * @author			fanyanbo
 * @data            2016/3/1
 *
 */
#ifndef SKY_KEYDISPATCH_MANAGER
#define SKY_KEYDISPATCH_MANAGER

class KeyDispatchManager
{
public:
	virtual ~KeyDispatchManager();
	static KeyDispatchManager * getInstance();
	void init();
	
private:
	KeyDispatchManager();
	static KeyDispatchManager* m_instance;

	class Garbo
	{
	public:
		~Garbo()
		{
			if (KeyDispatchManager::m_instance != nullptr)
			{
				delete KeyDispatchManager::m_instance;
				KeyDispatchManager::m_instance = nullptr;
			}
		}
	};
	static Garbo garbo;
};

#endif