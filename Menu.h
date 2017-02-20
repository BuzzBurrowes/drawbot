#pragma once

class UiElement {
public:
   UiElement(const char* name);
   UiElement(const char* name, std::function<void()> drawContentFunc,
      std::function<void()> pollFunc);
   void DrawContent() { mDrawContentFunc(); }
   void Poll()        { mPollFunc();        }
   void SetNext(UiElement* next) { mNext = next; }

private:
   const char* mName = nullptr;
   std::function<void()> mDrawContentFunc;
   std::function<void()> mPollFunc;  
   UiElement* mNext = nullptr;
};

class UiMenu : public UiElement {
public:
   UiMenu(const char* name) 
      : UiElement(name) {}
   void Add(UiElement& element);

private:
   uint8_t mCurrentItem = 0;
   uint8_t mNumItems = 0;
   UiElement *mFirstChild = nullptr;
};

class UiMaster {
public:
   UiMaster();
   void Setup();
   void Poll();
   void DrawContent(float& content);
   void FloatEditor(float& content);
};

extern UiMaster theUiMaster;