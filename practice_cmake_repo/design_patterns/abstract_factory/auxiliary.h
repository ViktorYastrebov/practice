#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <map>

//http://stackoverflow.com/questions/11796121/implementing-the-visitor-pattern-using-c-templates
//the main point is - create factory for the classified object set
/* template<class ActionType>
class IObject {
  public:
  //  virtual void accept(ActionType & ac) = 0;
    virtual ~IObject(){}
};

template<class ActionType>
class RealData : public IObject<ActionType> {
public:
  /*virtual void accept(ActionType  &ac) {
    typename ac.visit(*this);
  }*/
  /*
  std::string  &name() {
    return name_;
  };

  const std::string& name() const {
    return name_;
  }
  */
  //virtual ~RealData() {}
/*private:
  std::string name_;
*/
//};
/*
class Serializer
{
public:
  virtual void visit(RealData<Serializer>  &v) {
    std::cout << v.name() << std::endl;
  }
  virtual ~Serializer() {}
};
*/

class IButton {
public:
  virtual std::string GetName() const = 0;
  virtual ~IButton(){}
};

class WinButton : public IButton {
public:
  std::string GetName() const {
    return std::string("WinButton");
  }
  ~WinButton() {}
};

class MacButton : public IButton {
public:
  virtual std::string GetName() const {
    return std::string("MacButton");
  }
  ~MacButton(){}
};

class IPicture {
public:
  virtual std::string GetPictureName() const = 0;
  virtual ~IPicture() {}
};

class WinPicture : public IPicture {
public:
  virtual std::string GetPictureName() const {
    return std::string("WinPicture");
  }
  ~WinPicture() {}
};

class MacPicture : public IPicture {
public:
  virtual std::string GetPictureName() const {
    return std::string("MacPicture");
  }
};


class GuiFactory {
public:
  virtual std::shared_ptr<IButton> CreateButton() const = 0;
  virtual std::shared_ptr<IPicture> CreatePicture() const = 0;
  virtual ~GuiFactory() {}
};


class WinGuiFactory : public GuiFactory {
public:
  std::shared_ptr<IButton> CreateButton() const {
    return std::make_shared<WinButton>();
  }
  std::shared_ptr<IPicture> CreatePicture() const {
    return std::make_shared<WinPicture>();
  }
};

class MacGuiFactory : public GuiFactory {
public:
  std::shared_ptr<IButton> CreateButton() const {
    return std::make_shared<MacButton>();
  }
  std::shared_ptr<IPicture> CreatePicture() const {
    return std::make_shared<MacPicture>();
  }
};


class GuiSelector {
public:
  GuiSelector()
  {
    buildMap_["Window"] = std::make_shared<WinGuiFactory>();
    buildMap_["Mac"] = std::make_shared<MacGuiFactory>();
  }

  std::shared_ptr<GuiFactory> getByName(const std::string  &name) const {
    std::shared_ptr<GuiFactory> ret;
    std::map<std::string, std::shared_ptr<GuiFactory>>::const_iterator i = buildMap_.find(name);
    if (i != buildMap_.end()) {
      ret = i->second;
    }
    return ret;
  }

private:
  std::map<std::string, std::shared_ptr<GuiFactory> > buildMap_;
};