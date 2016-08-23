
/************************* Prototype Object *********************************/

#ifndef Object_H
#define Object_H
#include "Arduino.h"

typedef void (*GeneralBoolFunction) (int id, bool value);
typedef void (*GeneralMessageFunction) (String message);

class Object {
  public:
    Object();
//    void setCallback(GeneralMessageFunction _callback);
    virtual void processMessage(char *message);
    void tempCallback();
  private:
    GeneralMessageFunction callback;
};
#endif
