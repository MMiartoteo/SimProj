//
// Generated file, do not edit! Created by opp_msgc 4.2 from Msgs.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Msgs_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(ManagerMsg);

ManagerMsg::ManagerMsg(const char *name, int kind) : cMessage(name,kind)
{
    this->managerID_var = 0;
    this->x_var = 0;
    this->hops_var = 0;
}

ManagerMsg::ManagerMsg(const ManagerMsg& other) : cMessage(other)
{
    copy(other);
}

ManagerMsg::~ManagerMsg()
{
}

ManagerMsg& ManagerMsg::operator=(const ManagerMsg& other)
{
    if (this==&other) return *this;
    cMessage::operator=(other);
    copy(other);
    return *this;
}

void ManagerMsg::copy(const ManagerMsg& other)
{
    this->managerID_var = other.managerID_var;
    this->x_var = other.x_var;
    this->hops_var = other.hops_var;
}

void ManagerMsg::parsimPack(cCommBuffer *b)
{
    cMessage::parsimPack(b);
    doPacking(b,this->managerID_var);
    doPacking(b,this->x_var);
    doPacking(b,this->hops_var);
}

void ManagerMsg::parsimUnpack(cCommBuffer *b)
{
    cMessage::parsimUnpack(b);
    doUnpacking(b,this->managerID_var);
    doUnpacking(b,this->x_var);
    doUnpacking(b,this->hops_var);
}

int ManagerMsg::getManagerID() const
{
    return managerID_var;
}

void ManagerMsg::setManagerID(int managerID)
{
    this->managerID_var = managerID;
}

double ManagerMsg::getX() const
{
    return x_var;
}

void ManagerMsg::setX(double x)
{
    this->x_var = x;
}

int ManagerMsg::getHops() const
{
    return hops_var;
}

void ManagerMsg::setHops(int hops)
{
    this->hops_var = hops;
}

class ManagerMsgDescriptor : public cClassDescriptor
{
  public:
    ManagerMsgDescriptor();
    virtual ~ManagerMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ManagerMsgDescriptor);

ManagerMsgDescriptor::ManagerMsgDescriptor() : cClassDescriptor("ManagerMsg", "cMessage")
{
}

ManagerMsgDescriptor::~ManagerMsgDescriptor()
{
}

bool ManagerMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ManagerMsg *>(obj)!=NULL;
}

const char *ManagerMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ManagerMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int ManagerMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *ManagerMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "managerID",
        "x",
        "hops",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int ManagerMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "managerID")==0) return base+0;
    if (fieldName[0]=='x' && strcmp(fieldName, "x")==0) return base+1;
    if (fieldName[0]=='h' && strcmp(fieldName, "hops")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ManagerMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *ManagerMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ManagerMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ManagerMsg *pp = (ManagerMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ManagerMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ManagerMsg *pp = (ManagerMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getManagerID());
        case 1: return double2string(pp->getX());
        case 2: return long2string(pp->getHops());
        default: return "";
    }
}

bool ManagerMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ManagerMsg *pp = (ManagerMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setManagerID(string2long(value)); return true;
        case 1: pp->setX(string2double(value)); return true;
        case 2: pp->setHops(string2long(value)); return true;
        default: return false;
    }
}

const char *ManagerMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *ManagerMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ManagerMsg *pp = (ManagerMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(LookupMsg);

LookupMsg::LookupMsg(const char *name, int kind) : cMessage(name,kind)
{
    this->senderID_var = 0;
    this->x_var = 0;
    this->hops_var = 0;
}

LookupMsg::LookupMsg(const LookupMsg& other) : cMessage(other)
{
    copy(other);
}

LookupMsg::~LookupMsg()
{
}

LookupMsg& LookupMsg::operator=(const LookupMsg& other)
{
    if (this==&other) return *this;
    cMessage::operator=(other);
    copy(other);
    return *this;
}

void LookupMsg::copy(const LookupMsg& other)
{
    this->senderID_var = other.senderID_var;
    this->x_var = other.x_var;
    this->hops_var = other.hops_var;
}

void LookupMsg::parsimPack(cCommBuffer *b)
{
    cMessage::parsimPack(b);
    doPacking(b,this->senderID_var);
    doPacking(b,this->x_var);
    doPacking(b,this->hops_var);
}

void LookupMsg::parsimUnpack(cCommBuffer *b)
{
    cMessage::parsimUnpack(b);
    doUnpacking(b,this->senderID_var);
    doUnpacking(b,this->x_var);
    doUnpacking(b,this->hops_var);
}

int LookupMsg::getSenderID() const
{
    return senderID_var;
}

void LookupMsg::setSenderID(int senderID)
{
    this->senderID_var = senderID;
}

double LookupMsg::getX() const
{
    return x_var;
}

void LookupMsg::setX(double x)
{
    this->x_var = x;
}

int LookupMsg::getHops() const
{
    return hops_var;
}

void LookupMsg::setHops(int hops)
{
    this->hops_var = hops;
}

class LookupMsgDescriptor : public cClassDescriptor
{
  public:
    LookupMsgDescriptor();
    virtual ~LookupMsgDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(LookupMsgDescriptor);

LookupMsgDescriptor::LookupMsgDescriptor() : cClassDescriptor("LookupMsg", "cMessage")
{
}

LookupMsgDescriptor::~LookupMsgDescriptor()
{
}

bool LookupMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LookupMsg *>(obj)!=NULL;
}

const char *LookupMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LookupMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int LookupMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *LookupMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "senderID",
        "x",
        "hops",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int LookupMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderID")==0) return base+0;
    if (fieldName[0]=='x' && strcmp(fieldName, "x")==0) return base+1;
    if (fieldName[0]=='h' && strcmp(fieldName, "hops")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LookupMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *LookupMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int LookupMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LookupMsg *pp = (LookupMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LookupMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LookupMsg *pp = (LookupMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSenderID());
        case 1: return double2string(pp->getX());
        case 2: return long2string(pp->getHops());
        default: return "";
    }
}

bool LookupMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LookupMsg *pp = (LookupMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenderID(string2long(value)); return true;
        case 1: pp->setX(string2double(value)); return true;
        case 2: pp->setHops(string2long(value)); return true;
        default: return false;
    }
}

const char *LookupMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *LookupMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LookupMsg *pp = (LookupMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


