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




Register_Class(LookupResponseMsg);

LookupResponseMsg::LookupResponseMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->requestID_var = 0;
    this->managerID_var = 0;
    this->error_var = false;
    this->x_var = 0;
    this->hops_var = 0;
    this->specialization_var = 0;
}

LookupResponseMsg::LookupResponseMsg(const LookupResponseMsg& other) : cPacket(other)
{
    copy(other);
}

LookupResponseMsg::~LookupResponseMsg()
{
}

LookupResponseMsg& LookupResponseMsg::operator=(const LookupResponseMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void LookupResponseMsg::copy(const LookupResponseMsg& other)
{
    this->requestID_var = other.requestID_var;
    this->managerID_var = other.managerID_var;
    this->error_var = other.error_var;
    this->x_var = other.x_var;
    this->hops_var = other.hops_var;
    this->specialization_var = other.specialization_var;
}

void LookupResponseMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->requestID_var);
    doPacking(b,this->managerID_var);
    doPacking(b,this->error_var);
    doPacking(b,this->x_var);
    doPacking(b,this->hops_var);
    doPacking(b,this->specialization_var);
}

void LookupResponseMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->requestID_var);
    doUnpacking(b,this->managerID_var);
    doUnpacking(b,this->error_var);
    doUnpacking(b,this->x_var);
    doUnpacking(b,this->hops_var);
    doUnpacking(b,this->specialization_var);
}

unsigned long LookupResponseMsg::getRequestID() const
{
    return requestID_var;
}

void LookupResponseMsg::setRequestID(unsigned long requestID)
{
    this->requestID_var = requestID;
}

int LookupResponseMsg::getManagerID() const
{
    return managerID_var;
}

void LookupResponseMsg::setManagerID(int managerID)
{
    this->managerID_var = managerID;
}

bool LookupResponseMsg::getError() const
{
    return error_var;
}

void LookupResponseMsg::setError(bool error)
{
    this->error_var = error;
}

double LookupResponseMsg::getX() const
{
    return x_var;
}

void LookupResponseMsg::setX(double x)
{
    this->x_var = x;
}

int LookupResponseMsg::getHops() const
{
    return hops_var;
}

void LookupResponseMsg::setHops(int hops)
{
    this->hops_var = hops;
}

int LookupResponseMsg::getSpecialization() const
{
    return specialization_var;
}

void LookupResponseMsg::setSpecialization(int specialization)
{
    this->specialization_var = specialization;
}

class LookupResponseMsgDescriptor : public cClassDescriptor
{
  public:
    LookupResponseMsgDescriptor();
    virtual ~LookupResponseMsgDescriptor();

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

Register_ClassDescriptor(LookupResponseMsgDescriptor);

LookupResponseMsgDescriptor::LookupResponseMsgDescriptor() : cClassDescriptor("LookupResponseMsg", "cPacket")
{
}

LookupResponseMsgDescriptor::~LookupResponseMsgDescriptor()
{
}

bool LookupResponseMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LookupResponseMsg *>(obj)!=NULL;
}

const char *LookupResponseMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LookupResponseMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int LookupResponseMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *LookupResponseMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "requestID",
        "managerID",
        "error",
        "x",
        "hops",
        "specialization",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int LookupResponseMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestID")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "managerID")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "error")==0) return base+2;
    if (fieldName[0]=='x' && strcmp(fieldName, "x")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "hops")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "specialization")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LookupResponseMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "unsigned long",
        "int",
        "bool",
        "double",
        "int",
        "int",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *LookupResponseMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int LookupResponseMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LookupResponseMsg *pp = (LookupResponseMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LookupResponseMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LookupResponseMsg *pp = (LookupResponseMsg *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getRequestID());
        case 1: return long2string(pp->getManagerID());
        case 2: return bool2string(pp->getError());
        case 3: return double2string(pp->getX());
        case 4: return long2string(pp->getHops());
        case 5: return long2string(pp->getSpecialization());
        default: return "";
    }
}

bool LookupResponseMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LookupResponseMsg *pp = (LookupResponseMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setRequestID(string2ulong(value)); return true;
        case 1: pp->setManagerID(string2long(value)); return true;
        case 2: pp->setError(string2bool(value)); return true;
        case 3: pp->setX(string2double(value)); return true;
        case 4: pp->setHops(string2long(value)); return true;
        case 5: pp->setSpecialization(string2long(value)); return true;
        default: return false;
    }
}

const char *LookupResponseMsgDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *LookupResponseMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LookupResponseMsg *pp = (LookupResponseMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(LookupMsg);

LookupMsg::LookupMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->requestID_var = 0;
    this->senderID_var = 0;
    this->x_var = 0;
    this->hops_var = 0;
    this->specialization_var = 0;
}

LookupMsg::LookupMsg(const LookupMsg& other) : cPacket(other)
{
    copy(other);
}

LookupMsg::~LookupMsg()
{
}

LookupMsg& LookupMsg::operator=(const LookupMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void LookupMsg::copy(const LookupMsg& other)
{
    this->requestID_var = other.requestID_var;
    this->senderID_var = other.senderID_var;
    this->x_var = other.x_var;
    this->hops_var = other.hops_var;
    this->specialization_var = other.specialization_var;
}

void LookupMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->requestID_var);
    doPacking(b,this->senderID_var);
    doPacking(b,this->x_var);
    doPacking(b,this->hops_var);
    doPacking(b,this->specialization_var);
}

void LookupMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->requestID_var);
    doUnpacking(b,this->senderID_var);
    doUnpacking(b,this->x_var);
    doUnpacking(b,this->hops_var);
    doUnpacking(b,this->specialization_var);
}

unsigned long LookupMsg::getRequestID() const
{
    return requestID_var;
}

void LookupMsg::setRequestID(unsigned long requestID)
{
    this->requestID_var = requestID;
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

int LookupMsg::getSpecialization() const
{
    return specialization_var;
}

void LookupMsg::setSpecialization(int specialization)
{
    this->specialization_var = specialization;
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

LookupMsgDescriptor::LookupMsgDescriptor() : cClassDescriptor("LookupMsg", "cPacket")
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
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
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
        "requestID",
        "senderID",
        "x",
        "hops",
        "specialization",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int LookupMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestID")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderID")==0) return base+1;
    if (fieldName[0]=='x' && strcmp(fieldName, "x")==0) return base+2;
    if (fieldName[0]=='h' && strcmp(fieldName, "hops")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "specialization")==0) return base+4;
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
        "unsigned long",
        "int",
        "double",
        "int",
        "int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
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
        case 0: return ulong2string(pp->getRequestID());
        case 1: return long2string(pp->getSenderID());
        case 2: return double2string(pp->getX());
        case 3: return long2string(pp->getHops());
        case 4: return long2string(pp->getSpecialization());
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
        case 0: pp->setRequestID(string2ulong(value)); return true;
        case 1: pp->setSenderID(string2long(value)); return true;
        case 2: pp->setX(string2double(value)); return true;
        case 3: pp->setHops(string2long(value)); return true;
        case 4: pp->setSpecialization(string2long(value)); return true;
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
        NULL,
        NULL,
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
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

Register_Class(NEstimationMsg);

NEstimationMsg::NEstimationMsg(const char *name, int kind) : cPacket(name,kind)
{
    this->n_var = 0;
}

NEstimationMsg::NEstimationMsg(const NEstimationMsg& other) : cPacket(other)
{
    copy(other);
}

NEstimationMsg::~NEstimationMsg()
{
}

NEstimationMsg& NEstimationMsg::operator=(const NEstimationMsg& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void NEstimationMsg::copy(const NEstimationMsg& other)
{
    this->n_var = other.n_var;
}

void NEstimationMsg::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->n_var);
}

void NEstimationMsg::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->n_var);
}

unsigned long NEstimationMsg::getN() const
{
    return n_var;
}

void NEstimationMsg::setN(unsigned long n)
{
    this->n_var = n;
}

class NEstimationMsgDescriptor : public cClassDescriptor
{
  public:
    NEstimationMsgDescriptor();
    virtual ~NEstimationMsgDescriptor();

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

Register_ClassDescriptor(NEstimationMsgDescriptor);

NEstimationMsgDescriptor::NEstimationMsgDescriptor() : cClassDescriptor("NEstimationMsg", "cPacket")
{
}

NEstimationMsgDescriptor::~NEstimationMsgDescriptor()
{
}

bool NEstimationMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<NEstimationMsg *>(obj)!=NULL;
}

const char *NEstimationMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int NEstimationMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int NEstimationMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *NEstimationMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "n",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int NEstimationMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "n")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *NEstimationMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "unsigned long",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *NEstimationMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int NEstimationMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    NEstimationMsg *pp = (NEstimationMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string NEstimationMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    NEstimationMsg *pp = (NEstimationMsg *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getN());
        default: return "";
    }
}

bool NEstimationMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    NEstimationMsg *pp = (NEstimationMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setN(string2ulong(value)); return true;
        default: return false;
    }
}

const char *NEstimationMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *NEstimationMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    NEstimationMsg *pp = (NEstimationMsg *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


