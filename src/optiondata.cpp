#include "optiondata.h"

//
// Function implementation of the ListOptionObjects class declared in the header.

// 'Setter' method to add an OptionObject to the list at the tail end (append).
void OptionData::addObj(OptionObject optionobj)
{
    this->listofobjs.append(optionobj);
}

// Getter method to obtain the QList of OptionObject structs.
QList<OptionObject> OptionData::getListOfObjs()
{
    return this->listofobjs;
}

// 'Getter' method to obtain a QList where the OptionObject.optiontype equals some value passed as a QString objtype.
QList<OptionObject> OptionData::getListByOptionType(QString objtype)
{
    // In order to implement the 'search function', we must create a new QList of OptionObject structs, and go
    // through all the elements in the main list, check if their option type matches the parameter passed to the
    // method, and append only those that do match to the new list.
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.optiontype==objtype)
            newlist.append(optionobj);
    }
    // Return the new list we created which only contains the OptionObject struct who had matching objtypes to the
    // objtype parameter passed to the method.
    return newlist;
}

// 'Getter' method to obtain a QList where the pathtype of the OptionObject structs is a directory (i.e. == "dir").
// Same paradigm as previous function, except now we use pathtype instead of optiontype, and the query string is
// hardcoded as 'dir'.
QList<OptionObject> OptionData::getListOfDirs()
{
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.pathtype=="dir")
            newlist.append(optionobj);
    }
    // See getListByOptionType method.
    return newlist;
}

// 'Getter' method to obtain a QList where the pathtype of the OptionObject structs is a file (i.e. == "file").
// Same paradigm as previous function, only with 'file' instead of 'dir' as query string.
QList<OptionObject> OptionData::getListOfFiles()
{
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.pathtype=="file")
            newlist.append(optionobj);
    }
    // See getListByOptionType method.
    return newlist;
}

// 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is EDU (i.e. == "edu").
// Same paradigm as previous function, except now we use optiontype instead of pathtype, and the query string is
// 'edu' instead of 'file'.
QList<OptionObject> OptionData::getListOfEdus()
{
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.optiontype=="edu")
            newlist.append(optionobj);
    }
    // See getListByOptionType method.
    return newlist;
}

// 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is campaign (i.e. == "camp").
// Same paradigm as previous function, only with 'camp' instead of 'edu' as query string.
QList<OptionObject> OptionData::getListOfCamps()
{
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.optiontype=="camp")
            newlist.append(optionobj);
    }
    // See getListByOptionType method.
    return newlist;
}

// 'Getter' method to obtain a QList where the optiontype of the OptionObject structs is trees (i.e. == "trees").
// Same paradigm as previous function, only with 'trees' instead of 'camp' as query string.
QList<OptionObject> OptionData::getListOfTrees()
{
    QList<OptionObject> newlist;
    foreach (OptionObject optionobj, this->listofobjs)
    {
        if (optionobj.optiontype=="trees")
            newlist.append(optionobj);
    }
    // See getListByOptionType method.
    return newlist;
}
