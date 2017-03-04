#pragma once

typedef void (*UiCallbackFunc)();
class UiElement {
public:
   UiElement(const char* name);
   UiElement(const char* name, UiCallbackFunc drawContentFunc,
      UiCallbackFunc pollFunc);

   const char* GetName()         { return mName;       }
   void DrawContent()            { if (mDrawContentFunc) mDrawContentFunc(); }
   void Poll()                   { if (mPollFunc) mPollFunc();        }
   void SetNext(UiElement* next) { mNext = next;       }
   UiElement* GetNext()          { return mNext;       }

private:
   const char* mName = nullptr;
   UiCallbackFunc mDrawContentFunc;
   UiCallbackFunc mPollFunc;  
   UiElement* mNext = nullptr;
};

class UiMenu : public UiElement {
public:
   UiMenu(const char* name) 
      : UiElement(name) {}
   void Add(UiElement& element);

private:
   UiElement* mCurrentItem = nullptr;
   UiElement* mFirstChild = nullptr;
};

class UiMaster {
public:
   UiMaster();
   void Setup();
   void Poll();
   void DrawContent(float& content);
   void FloatEditor(float& content);
   void SetCurrentUiElement(UiElement* el);
private:
   UiElement* mCurrentUiElement = nullptr;
};

extern UiMaster theUiMaster;
