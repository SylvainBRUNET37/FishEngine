#ifndef EATABLE_H
#define EATABLE_H

class Eatable
{
public:
    
    float mass;
    bool isApex = false;

    [[nodiscard]] bool CanBeEatenBy(Eatable other) const;

};


#endif