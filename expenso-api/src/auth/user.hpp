#pragma once

class person
{
public:
    long id;
    QString firstName;
    QString lastName;
    QDateTime birthDate;

    person() : id(0) { ; }
    virtual ~person() { ; }
};
